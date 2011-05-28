
#include "main.h"

nPlane::nPlane(Vec3f sPos, Quat4f sRot, objectType Type, objectController* c):controlledObject(sPos, sRot, Type, c), maxHealth(100), explode(NULL), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0)
{
	spawn();
}
nPlane::nPlane(Vec3f sPos, Quat4f sRot, objectType Type):controlledObject(sPos, sRot, Type, CONTROL_COMPUTER), maxHealth(100), explode(NULL), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0)
{
	spawn();
}

void nPlane::update(double time, double ms)
{
	control->update();
	controlState controller=control->getControlState();

	//static bool followingPath = false;
	//if(time < planePath.endTime())
	//{
	//	objectPath::point p=planePath(time);
	//	rotation = p.rotation;
	//	pos = p.position;
	//	followingPath = true;
	//	return;
	//}
	//if(followingPath)
	//{
	//	objectPath::point p=planePath(time);
	//	rotation = p.rotation;
	//	pos = p.position;
	//	ms -= (time - planePath.endTime());
	//	followingPath = false;
	//}

	if(respawning)
	{
		altitude=world.altitude(position);
		Vec3f lastPos=position;
		Quat4f lastRotation=rotation;
		Vec3f lastFwd = rotation * Vec3f(0,0,1), fwd;

		speed += 9.8 * (ms/1000) * -sin(climb);//gravity


	 	if(altitude<3.0||hitGround)
		{
			position.y=position.y-altitude+3;
			hitGround=true;
		}
		else
		{

			rotation = Quat4f(0,0,0,1);
			rotation = Quat4f(Vec3f(0,0,1),turn) * rotation;
			rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;
			position += rotation * Vec3f(0,0,1) * (ms/1000);
			fwd = rotation * Vec3f(0,0,1);

			position += rotation * Vec3f(0,0,1) * speed * (ms/1000);

			int sgn=turn/abs(turn);
			turn-=0.1*(ms/1000)*sgn;
			if(turn/abs(turn)!=sgn)
				turn=0;

			climb -= 0.3 * (ms/1000);
			if(climb < -PI/2)
				climb = -PI/2;
		}
	}
	else
	{	
		/////update rockets/////////////////////
		if(rockets.left<rockets.max)
			rockets.rechargeLeft-=ms;
		if(rockets.rechargeLeft<0)
		{
			rockets.left++;
			if(rockets.left==rockets.max)
				rockets.rechargeLeft=rockets.rechargeTime;
			else
				rockets.rechargeLeft+=rockets.rechargeTime;
		}
		rockets.coolDownLeft-=ms;
		/////update machine gun/////////////////
		if(machineGun.roundsLeft==0)
			machineGun.rechargeLeft-=ms;
		if(machineGun.rechargeLeft<0)
		{
			machineGun.left+=machineGun.roundsMax;
			machineGun.roundsLeft=machineGun.roundsMax;
			machineGun.rechargeLeft=machineGun.rechargeTime;
			extraShootTime = 0.0;
		}
		machineGun.coolDownLeft-=ms;

		if(controled)
		{
			if(wayPoints.size()>0)
				autoPilotUpdate(ms);
			else
				controled=false;
		}
		else
		{
			Vec3f lastPos=position;
			Quat4f lastRotation=rotation;
			Vec3f lastFwd = rotation * Vec3f(0,0,1), fwd;

			altitude=world.altitude(position);
			if(altitude<3.0)
			{
				hitGround=true;
				position.y-=altitude-3;
				particleManager.addEmitter(new particle::blackSmoke(id));
				die();
			}
			
			speed = max(speed,clamp(speed + 10.0f*controller.accelerate*ms - 10.0f*controller.brake*ms,25.0,69.0));
			climb = clamp(climb + 1.0*controller.climb*(ms/1000) - 1.0*controller.dive*(ms/1000),-PI/3,PI/4);
			turn  = clamp(turn  + 1.5*controller.right*(ms/1000) - 1.5*controller.left*(ms/1000),-1.0,1.0);
			direction -= turn*ms/3000.0;//needs to be adjusted to be continious
			speed += 9.8 * (ms/1000) * -sin(climb);//gravity

			rotation = Quat4f(0,0,0,1);
			rotation = Quat4f(Vec3f(0,0,1),turn) * rotation;
			rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;

			fwd = rotation * Vec3f(0,0,1);
			//////////////////////move//////////////////////////////////////
			if(ms>0)
			{
				position += rotation * Vec3f(0,0,1) * speed * (ms/1000);
			}
			////////////////////end move////////////////////////////////////
			//bool setAutoPilot=false;
			//if(position.x > settings.MAX_X * size && !controled){position.x = settings.MAX_X * size-6;setAutoPilot=true;}
			//if(position.x < settings.MIN_X * size && !controled){position.x = settings.MIN_X * size+6;setAutoPilot=true;}
			//if(position.z > settings.MAX_Y * size && !controled){position.z = settings.MAX_Y * size-6;setAutoPilot=true;}
			//if(position.z < settings.MIN_Y * size && !controled){position.z = settings.MIN_Y * size+6;setAutoPilot=true;}
			//if(setAutoPilot)
			//{
			//	returnToBattle();
			//}

			//if(pos.y>300 && velocity.y>0)
			//	velocity.y=clamp(velocity.y-ms/2500,0,velocity.y);

			//if(pos.y>250)	pos.y=250;
			//roll=turn*PI/180;
			level(ms);

			if(controller.shoot1 <= 0.75)
				extraShootTime=0.0;
			else if(controller.shoot1 > 0.75)
			{
				extraShootTime+=ms;
				while(extraShootTime > machineGun.coolDown && machineGun.roundsLeft > 0)
				{
					extraShootTime-=machineGun.coolDown;
					machineGun.roundsLeft--;
					Vec3f o=rotation*(settings.planeStats[type].machineGuns[shotsFired%settings.planeStats[type].machineGuns.size()]);
					Vec3f l=position*(1.0-extraShootTime/ms) + lastPos*extraShootTime/ms;
					Vec3f t=((fwd.normalize()*(1.0-extraShootTime/ms) + lastFwd.normalize()*extraShootTime/ms)*1000-o).normalize() + random<Vec3f>()*0.0025;
					
					shotsFired++;
					world.bullets.push_back(bullet(o + l,t,id,time-extraShootTime-machineGun.coolDown));
				}
			}
			if(controller.shoot2>0.75)	ShootMissile();
			
			planePath.currentPoint(position,rotation);
			//static double lastWaypoint = 0.0;
			//lastWaypoint += ms;

			//if(lastWaypoint >= 100.0)
			//{
			//	lastWaypoint -= 1000.0;
			//	objectPath::point p;
			//	p.position = position;
			//	p.rotation = rotation;
			//	p.time = world.time();
			//	planePath << p;
			//}
		}
		//if(angle > 360)						angle -= 360;
		//if(angle < 0)						angle += 360;
		//Vec3f v(sin(float(angle * PI / 180)),climb,cos(float(angle * PI / 180)));
		//Vec3f o(sin(float((angle+90) * PI / 180)),cos((turn+270)/180*3.1415926535),cos(float((angle+90) * PI / 180)));
		//v=v.normalize();
		//o=o.normalize();                         MUST BE REPLACED!
		//normal = v.cross(o);
		//forward=Vec3f(sin(angle*DegToRad)*acceleration,cos(turn*DegToRad)*climb,cos(angle *DegToRad)*acceleration).normalize();
		
		//normal=Vec3f(0,1,0);
		findTargetVector();
	}
}
void nPlane::autoPilotUpdate(float value)
{
	wayPoint w1;
	wayPoint w2;
	double time=world.time();
	float t;
	if(time>(wayPoints.back()).time)
	{
		controled=false;
		Vec3f fwd = wayPoints.back().rotation * Vec3f(0,0,1);

		rotation=		wayPoints.back().rotation;
		position =		wayPoints.back().position;
		direction =		atan2A(fwd.x,fwd.z);
		climb =			asin(fwd.y/fwd.magnitude());
		turn =			0;//to hard and rather useless to find

		wayPoints.clear();
		return;
	}
	else if(time<wayPoints.begin()->time)
	{
		return;// should not occur
	}

	for(int i=1;i<wayPoints.size();i++)
	{
		if(wayPoints[i-1].time<time && wayPoints[i].time>time)
		{
			w1=wayPoints[i-1];
			w2=wayPoints[i];
		}
	}

	t=1.0-(w2.time-time)/(w2.time-w1.time);

	position = w1.position*(1.0-t)+w2.position*t;
	rotation = slerp(w1.rotation,w2.rotation,t);
	center.x = position.x;
	center.z = position.z;
	//Vec3f fwd = rotation * Vec3f(0,0,1);
	//direction =	atan2A(fwd.x,fwd.z);
	//climb =		asin(fwd.y/fwd.magnitude());
	//turn =		0;

}
void nPlane::exitAutoPilot()
{
	wayPoint w1;
	wayPoint w2;
	double time=world.time();
	float t;
	if(wayPoints.size()==0)//no waypoints
	{
		turn=0;
		controled=false;
		return;
	}
	if(time>(wayPoints.back()).time)//beyond last waypoint
	{
		controled=false;
		Vec3f fwd = wayPoints.back().rotation * Vec3f(0,0,1);

		rotation=		wayPoints.back().rotation;
		position =		wayPoints.back().position;
		direction =		atan2A(fwd.x,fwd.z);
		climb =			asin(fwd.y/fwd.magnitude());
		turn =			0;//to hard and rather useless to find

		wayPoints.clear();
		return;
	}
	else if(time<wayPoints.begin()->time)//autopilot not yet started
	{
		return;// should not occur
	}

	for(int i=1;i<wayPoints.size();i++)
	{
		if(wayPoints[i-1].time<time && wayPoints[i].time>time)
		{
			w1=wayPoints[i-1];
			w2=wayPoints[i];
		}
	}

	t=(w2.time-time)/(w2.time-w1.time);

	position =	w1.position*t+w2.position*(1.0-t);
	rotation =	slerp(w1.rotation,w2.rotation,t);
	Vec3f fwd = rotation * Vec3f(0,0,1);
	direction =	atan2A(fwd.x,fwd.z);
	climb =		asin(fwd.y/fwd.magnitude());
	turn =		0;

	controled=false;
	wayPoints.clear();
}
void nPlane::returnToBattle()
{

	wayPoints.clear();
	controled =true;
	double time=world.time();

	Vec3f fwd	= rotation * Vec3f(0,0,1);
	Vec3f up	= rotation * Vec3f(0,1,0);
	Vec3f right	= rotation * Vec3f(1,0,0);
	fwd.y=0; fwd=fwd.normalize();

	Quat4f newRot(Vec3f(0,1,0),atan2A(position.x-size*64,position.z-size*64));
	Vec3f newFwd = newRot * Vec3f(0,0,1);

	camera=Vec3f(position.x - fwd.x*20, position.y + sin(45.0)*20,	 position.z - fwd.z*20);
	center=Vec3f(position.x + fwd.x*35, position.y, position.z + fwd.z*35);
	

	wayPoints.push_back(wayPoint(time,				position,								rotation								));
	wayPoints.push_back(wayPoint(time+3000.0,		position+newFwd*2500,					newRot									));
	wayPoints.push_back(wayPoint(time+3610.0,		position+newFwd*2875+Vec3f(0,375,0),	newRot * Quat4f(Vec3f(-1,0,0),PI*0.25)	));
	wayPoints.push_back(wayPoint(time+4220.0,		position+newFwd*2500+Vec3f(0,750,0),	newRot * Quat4f(Vec3f(-1,0,0),PI*0.5)	));
	wayPoints.push_back(wayPoint(time+4460.0,		position+newFwd*2309+Vec3f(0,588,0),	newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5300.0,		position+newFwd*1743+Vec3f(0,176,0),	newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5660.0,		position+newFwd*1500,					newRot * Quat4f(Vec3f(-1,0,0),PI)		));
	wayPoints.push_back(wayPoint(time+7000.0,		position-newFwd*5,						newRot * Quat4f(Vec3f(0,0,1),PI) * Quat4f(Vec3f(-1,0,0),PI) 	));

	////wayPoints.push_back(wayPoint(time+7000.0,	pos,
	////							0,				acceleration,
	////							angle+180,		0));


}

void nPlane::level(float ms)
{
	int sgn=turn/abs(turn);
	turn-=0.1*(ms/1000)*sgn;
	if(turn/abs(turn)!=sgn)
		turn=0;

	sgn=climb/abs(climb);
	climb-=0.1*(ms/1000)*sgn;
	if(climb/abs(climb)!=sgn)
		climb=0;
}
void nPlane::die()
{
	//if(!dead)	explode=new explosion(position);
	if(!dead)
	{
		//particleManager.addEmitter(new particle::blackSmoke(id));
	}
	dead =true;
	if(controled)
	{
		exitAutoPilot();
	//	controled=true;
	}

	if(!respawning)
		respawnTime=world.time()+5000;
	respawning=true;
}
void nPlane::findTargetVector()
{
	//Vec3f enemy;
	//for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	//{
	//	enemy=(*i).second->pos;
	//	if(acos( velocity.dot( (enemy-pos).normalize() )) < 0.1 && (*i).first!=id && !(*i).second->dead && pos.distance(enemy)<4000)
	//	{
	//		targeter=(enemy-pos).normalize();
	//		return;
	//	}
	//}
	//targeter=velocity;
	targeter = rotation * Vec3f(0,0,1);
}

void nPlane::ShootMissile()
{
	if(rockets.coolDownLeft>0 || rockets.left<=0 || controled)
		return;

	Vec3f fwd	= (rotation * Vec3f(0,0,1)).normalize(),
		  up	= (rotation * Vec3f(0,1,0)).normalize(),
		  right	= (rotation * Vec3f(1,0,0)).normalize();

	int pId=0;
	Angle minAng=0.0;
	const map<objId,nPlane*>& planes = world.planes();
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		Angle ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
		if(!i->second->dead && team != i->second->team && (ang < minAng || pId == 0 ))
		{
			minAng = ang;
			pId = i->second->id;
		}
	}
	if(pId == 0) 
		return;

	int d=settings.missileStats[settings.planeStats[defaultPlane].hardpoints[rockets.max-rockets.left].missileNum].dispList;
	Vec3f o=settings.planeStats[defaultPlane].hardpoints[rockets.max-rockets.left].offset;
	world.objectList.newMissile(MISSILE,team,position+right*o.x+up*o.y+fwd*o.z,fwd*speed,d,id,pId);
	rockets.coolDownLeft=rockets.coolDown;
	rockets.left--;
}
void nPlane::loseHealth(float healthLoss)
{
	health-=healthLoss;
	if(health<0)
	{
		health=0;
		die();
	}
}
void nPlane::drawExplosion(bool flash)
{
	if(explode!=NULL)
	{
		explode->pos=position;
		explode->render(flash);
	}
}
void nPlane::initArmaments()
{
	rockets.max				= rockets.left				= settings.planeStats[defaultPlane].hardpoints.size();
	rockets.roundsMax		= rockets.roundsLeft		= 1;
	rockets.coolDown		= rockets.coolDownLeft		= 700.0f;
	rockets.rechargeTime	= rockets.rechargeLeft		= 10000.0f;
	//rockets.firing										= false;	 not used

	machineGun.max			= machineGun.left			= 1000; 
	machineGun.roundsMax	= machineGun.roundsLeft		= 200;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 450.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 6.0;
	machineGun.firing									= false;
}

void nPlane::spawn()
{
	initArmaments();
	if(explode!=NULL) 
	{
		delete explode;
		explode=NULL;
	}
	position = startPos;
	rotation = startRot;

	Vec3f f = (rotation * Vec3f(0,0,1)).normalize();
	Vec3f u = (rotation * Vec3f(0,1,0)).normalize();
	if(f.magnitudeSquared() < 0.001)
	{
		rotation = Quat4f();
		climb = 0;
		direction = 0;
	}
	else if(abs(f.y) > 0.999)
	{
		climb = PI/2 * abs(f.y)/f.y;
		direction = PI+atan2(u.x,u.z);
	}
	else//only condition that *should* happen
	{
		direction = atan2(f.x,f.z);
		climb = atan2(f.y,sqrt(f.x*f.x+f.z*f.z));
	}
	turn = 0;
	speed=400.0;

	dead = false;
	controled=false;
	maneuver=0;
	hitGround=false;
	health=maxHealth;
	//updateAll(controlState());

	planePath.currentPoint(position,rotation);

	respawning=false;
	shotsFired = 0;
}