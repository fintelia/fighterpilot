#include "main.h"

//const int PLANE		=0x01;
const int PLAYER_PLANE	=0x02;
const int AI_PLANE		=0x04;

void planeBase::updateAll()
{
	double time=gameTime();
	double ms=time-lastUpdateTime;
	lastUpdateTime=time;

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
		altitude=pos.y-terrain->getInterpolatedHeight(pos.x/size,pos.z/size);
	 	if(altitude<3.0||hitGround)
		{
			pos.y=pos.y-altitude+3;
			hitGround=true;
		}
		else
		{
			//rotation = rotation * (1.0-ms/10000.0) + Quat4f(1,0,0,PI/2.0) * ms/10000.0;
			//velocity.y-=ms/10;
			pos+=rotation * Vec3f(0,0,1) * (ms/1000);
			pos.y-=ms/10;
		}
		//for(int i=0;i<5;i++)
		//{
		//	newSmoke.insert(pos,15,10.0+5.0*(rand()%1000-500)/500,10.0-float(rand()%100)/20.0);
		//}
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
			Vec3f lastPos=pos;
			Quat4f lastRotation=rotation;
			Vec3f lastFwd = rotation * Vec3f(0,0,1), fwd;

			altitude=pos.y-terrain->getInterpolatedHeight(pos.x/size,pos.z/size);
			if(altitude<3.0){die();pos.y-=altitude-3;hitGround=true;}
			
			speed = clamp(speed + 10.0f*controller.accelerate*ms - 10.0f*controller.brake*ms,300.0,700.0);
			climb = clamp(climb + 0.9*controller.climb*(ms/1000) - 0.9*controller.dive*(ms/1000),-PI/3,PI/4);
			turn  = clamp(turn  + 0.9*controller.right*(ms/1000) - 0.9*controller.left*(ms/1000),-1.0,1.0);
			direction -= turn/120;

			rotation = Quat4f(0,0,0,1);
			rotation = Quat4f(Vec3f(0,0,1),turn) * rotation;
			rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;


			fwd = rotation * Vec3f(0,0,1);
			//////////////////////move//////////////////////////////////////
			if(ms>0)
			{
				//Vec3f up,right,move(0,0,0);					//define our vectors		
				//upAndRight(velocity,roll,up,right);			//set up and right
				//move+=Vec3f(0,-50*(ms/1000),0);				//gravity
				//move+=velocity*(ms/1000);					//thrust
				//move+=up*velocity.magnitude()*(ms/1000)*0.2;//lift
				//move*=0.95;									//
				//pos+=move;									//increase position

				Vec3f move = rotation * Vec3f(0,0,1) * speed * (ms/1000);
				pos+=move;
			}
			////////////////////end move////////////////////////////////////
			bool setAutoPilot=false;
			if(pos.x > settings.MAX_X * size && !controled){pos.x = settings.MAX_X * size-6;setAutoPilot=true;}
			if(pos.x < settings.MIN_X * size && !controled){pos.x = settings.MIN_X * size+6;setAutoPilot=true;}
			if(pos.z > settings.MAX_Y * size && !controled){pos.z = settings.MAX_Y * size-6;setAutoPilot=true;}
			if(pos.z < settings.MIN_Y * size && !controled){pos.z = settings.MIN_Y * size+6;setAutoPilot=true;}
			if(setAutoPilot)
			{
				returnToBattle();
			}

			//if(pos.y>300 && velocity.y>0)
			//	velocity.y=clamp(velocity.y-ms/2500,0,velocity.y);

			//if(pos.y>250)	pos.y=250;
			//roll=turn*PI/180;
			Level(ms);

			if(lastController.shoot1<=0.75)
				extraShootTime=0.0;
			if(controller.shoot1>0.75  )
			{
				extraShootTime+=ms;				
				while(extraShootTime > machineGun.coolDown)
				{
					extraShootTime-=machineGun.coolDown;
					machineGun.roundsLeft--;
					Vec3f l=pos*(1.0-extraShootTime/ms) + lastPos*extraShootTime/ms;
					Vec3f t=fwd.normalize()*(1.0-extraShootTime/ms) + lastFwd.normalize()*extraShootTime/ms;//+Vec3f(float(rand()%1000)/50000,float(rand()%1000-500)/50000,float(rand()%1000)/50000);
					bullets.push_back(bullet(l,t,id,time-extraShootTime-machineGun.coolDown));
				}
			}
			if(controller.shoot2>0.75)	ShootMissile();

			//static double lastWaypoint = 9999.9;
			//lastWaypoint += ms;
			//if(lastWaypoint >= 100.0)
			//{
			//	objectPath::point p;
			//	p.position = pos;
			//	p.rotation = rotation;
			//	p.ms = time;
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
	lastController = controller;
}
void planeBase::autoPilotUpdate(float value)
{
	wayPoint w1;
	wayPoint w2;
	double time=gameTime();
	float t;
	if(time>(wayPoints.back()).time)
	{
		controled=false;
		Vec3f fwd = wayPoints.back().rotation * Vec3f(0,0,1);

		rotation=		wayPoints.back().rotation;
		pos =			wayPoints.back().position;
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

	pos =		w1.position*(1.0-t)+w2.position*t;
	rotation =	slerp(w1.rotation,w2.rotation,t);
	center.x = pos.x;
	center.z = pos.z;
	//Vec3f fwd = rotation * Vec3f(0,0,1);
	//direction =	atan2A(fwd.x,fwd.z);
	//climb =		asin(fwd.y/fwd.magnitude());
	//turn =		0;

}
void planeBase::exitAutoPilot()
{
	wayPoint w1;
	wayPoint w2;
	double time=gameTime();
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
		pos =			wayPoints.back().position;
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

	pos =		w1.position*t+w2.position*(1.0-t);
	rotation =	slerp(w1.rotation,w2.rotation,t);
	Vec3f fwd = rotation * Vec3f(0,0,1);
	direction =	atan2A(fwd.x,fwd.z);
	climb =		asin(fwd.y/fwd.magnitude());
	turn =		0;

	controled=false;
	wayPoints.clear();
}
void planeBase::returnToBattle()
{

	wayPoints.clear();
	controled =true;
	double time=gameTime();

	Vec3f fwd	= rotation * Vec3f(0,0,1);
	Vec3f up	= rotation * Vec3f(0,1,0);
	Vec3f right	= rotation * Vec3f(1,0,0);
	fwd.y=0; fwd=fwd.normalize();

	Quat4f newRot(Vec3f(0,1,0),atan2A(pos.x-size*64,pos.z-size*64));
	Vec3f newFwd = newRot * Vec3f(0,0,1);

	camera=Vec3f(pos.x - fwd.x*175, pos.y + sin(45.0)*175,	 pos.z - fwd.z*175);
	center=Vec3f(pos.x + fwd.x*175, pos.y, pos.z + fwd.z*175);
	

	wayPoints.push_back(wayPoint(time,				pos,								rotation								));
	wayPoints.push_back(wayPoint(time+3000.0,		pos+newFwd*2500,					newRot									));
	wayPoints.push_back(wayPoint(time+3610.0,		pos+newFwd*2875+Vec3f(0,375,0),		newRot * Quat4f(Vec3f(-1,0,0),PI*0.25)	));
	wayPoints.push_back(wayPoint(time+4220.0,		pos+newFwd*2500+Vec3f(0,750,0),		newRot * Quat4f(Vec3f(-1,0,0),PI*0.5)	));
	wayPoints.push_back(wayPoint(time+4460.0,		pos+newFwd*2309+Vec3f(0,588,0),		newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5300.0,		pos+newFwd*1743+Vec3f(0,176,0),		newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5660.0,		pos+newFwd*1500,					newRot * Quat4f(Vec3f(-1,0,0),PI)		));
	wayPoints.push_back(wayPoint(time+7000.0,		pos-newFwd*5,						newRot * Quat4f(Vec3f(0,0,1),PI) * Quat4f(Vec3f(-1,0,0),PI) 	));

	////wayPoints.push_back(wayPoint(time+7000.0,	pos,
	////							0,				acceleration,
	////							angle+180,		0));


}

void planeBase::Level(float ms)
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
void planeBase::die()
{
	if(!dead)	explode=new explosion(pos);
	dead =true;
	if(controled)
	{
		exitAutoPilot();
	//	controled=true;
	}

	if(!respawning)
		respawnTime=gameTime()+5000;
	respawning=true;
}
void planeBase::findTargetVector()
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

void planeBase::ShootMissile()
{
	if(rockets.coolDownLeft>0 || rockets.left<=0 || controled)
		return;

	Vec3f fwd	= (rotation * Vec3f(0,0,1)).normalize(),
		  up	= (rotation * Vec3f(0,1,0)).normalize(),
		  right	= (rotation * Vec3f(1,0,0)).normalize();


	int d=settings.missileStats[settings.planeStats[defaultPlane].hardpoints[rockets.max-rockets.left].missileNum].dispList;
	Vec3f o=settings.planeStats[defaultPlane].hardpoints[rockets.max-rockets.left].offset;
	missiles.push_back(missile(pos+right*o.x*5+up*o.y*5+fwd*o.z*5,fwd*speed,id,d));
	rockets.coolDownLeft=rockets.coolDown;
	rockets.left--;
}
void planeBase::loseHealth(float healthLoss)
{
	health-=healthLoss;
	if(health<0)
	{
		health=0;
		die();
	}
}
void planeBase::drawExplosion(bool flash)
{
	if(explode!=NULL)
	{
		explode->pos=pos;
		explode->render(flash);
	}
}
void planeBase::initArmaments()
{
	rockets.max				= rockets.left				= settings.planeStats[defaultPlane].hardpoints.size();
	rockets.roundsMax		= rockets.roundsLeft		= 1;
	rockets.coolDown		= rockets.coolDownLeft		= 700.0f;
	rockets.rechargeTime	= rockets.rechargeLeft		= 10000.0f;
	//rockets.firing										= false;	 not used

	machineGun.max			= machineGun.left			= 1000; 
	machineGun.roundsMax	= machineGun.roundsLeft		= 15;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 300.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 40.0;
	machineGun.firing									= false;
}

//<>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <><
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><>

void plane::die()
{
	if(!dead)	explode=new explosion(pos);
	dead =true;
	if(controled)
	{
		exitAutoPilot();
	}
	//controled=true;

	if(!respawning)
		respawnTime=gameTime()+5000;
	respawning=true;
}
bool plane::Update(float ms)
{
	//updatePos(ms);
	updateAll();
	if(respawning && respawnTime<gameTime())
	{
		if(settings.ON_HIT==RESPAWN)
		{
			spawn();
		}
		else if(settings.ON_HIT==RESTART)
		{
			//not yet handled
		}
		else if(settings.ON_HIT==DIE)
		{
			//return false;
		}
	}

	return true;
}

void plane::spawn()
{
	initArmaments();
	if(explode!=NULL) 
	{
		delete explode;
		explode=NULL;
	}
	pos.x = rand() % int((settings.MAX_X-settings.MIN_X)*size)+settings.MIN_X*size;
	pos.z = rand() % int((settings.MAX_Y-settings.MIN_Y)*size)+settings.MIN_Y*size;
	float h=terrain->getHeight(pos.x/size, pos.z/size);
	//float h=y-terrain->getHeight(pos.x/size, pos.z/size);
	if(h<terrain->getHeight(pos.x/size+1, pos.z/size))		h=terrain->getHeight(pos.x/size+1, pos.z/size);
	if(h<terrain->getHeight(pos.x/size, pos.z/size+1))		h=terrain->getHeight(pos.x/size, pos.z/size+1);
	if(h<terrain->getHeight(pos.x/size+1, pos.z/size+1))	h=terrain->getHeight(pos.x/size+1, pos.z/size+1);
	float height=terrain->getInterpolatedHeight(pos.x/size,pos.z/size);
	pos.y = height+65;
	if(pos.y<settings.SEA_LEVEL+35)
		pos.y=settings.SEA_LEVEL+35;

	rotation = Quat4f(Vec3f(0,1,0),atan2A(pos.x-size*32,pos.y-size*32));
	turn = 0;
	climb = 0;
	direction = 0;
	speed=300.0;

	dead = false;
	controled=false;
	maneuver=0;
	hitGround=false;
	health=maxHealth;
	//updateAll(controlState());

	respawning=false;
}
void plane::setControlState(controlState c)
{
	controller=c;
}
plane::plane(int Id,int Team)
{
	maxHealth=100;
	id = Id;
	team=Team;
	dead=false;
	type=defaultPlane;

	spawn();
	Update(0);
}
//<>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <><
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><>

AIplane::AIplane(int Id,int Team)
{
	maxHealth=50;
	id = Id;
	team=Team;
	dead=false;
	type=defaultPlane;

	spawn();
	Update(0);
}

void AIplane::spawn()
{
	initArmaments();
	pos.x = rand() % int((settings.MAX_X-settings.MIN_X)*size)+settings.MIN_X*size;
	pos.z = rand() % int((settings.MAX_Y-settings.MIN_Y)*size)+settings.MIN_Y*size;
	float h=terrain->getHeight(pos.x/size, pos.z/size);
	//float h=y-terrain->getHeight(pos.x/size, pos.z/size);
	if(h<terrain->getHeight(pos.x/size+1, pos.z/size))		h=terrain->getHeight(pos.x/size+1, pos.z/size);
	if(h<terrain->getHeight(pos.x/size, pos.z/size+1))		h=terrain->getHeight(pos.x/size, pos.z/size+1);
	if(h<terrain->getHeight(pos.x/size+1, pos.z/size+1))		h=terrain->getHeight(pos.x/size+1, pos.z/size+1);
	//float h=terrain->distanceToGround(pos.x/size,0,pos.z/size);
	pos.y = 35+h;
	if(pos.y<settings.SEA_LEVEL+35)
		pos.y=settings.SEA_LEVEL+35;


	rotation = Quat4f(Vec3f(0,1,0),atan2A(pos.x-size*32,pos.y-size*32));
	turn = 0;
	climb = 0;
	direction = 0;
	speed=300.0;

	dead = false;
	controled=false;
	maneuver=0;
	hitGround=false;
	health=maxHealth;

	respawning=false;
}
void AIplane::freeForAll_calcMove(int value)
{
	//controller=controlState();
	//Vec3f self[2]={pos,pos+velocity};//current,future
	//Vec3f enemy;
	//if(machineGun.coolDownLeft<=0.0)
	//{
	//	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	//	{
	//		enemy=(*i).second->pos;
	//		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.5 && self[0].distance(enemy)<2000 && (*i).first!=id)
	//		{
	//			controller.shoot1=1.0;
	//			break;
	//		}
	//	}
	//}
	//target=-1;
	//float ang=999;
	//for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	//{
	//	enemy=(*i).second->pos;
	//	if(acos( self[1].dot( (enemy-self[0]).normalize() )) < ang && self[0].distance(enemy)<3000 && (*i).second->team!=team)
	//	{
	//		ang=acos( self[1].dot( (enemy-self[0]).normalize() ));
	//		target=(*i).first;
	//	}
	//}
	//
	//if(target>=0)
	//{
	//	if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
	//		controller.left=1.0;
	//	else if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
	//		controller.right=1.0;

	//	if(altitude>=30 && planes[target]->pos.y<pos.y)
	//		controller.dive=1.0;
	//	else if(planes[target]->pos.y>pos.y || altitude<25)
	//		controller.climb=1.0;

	//	if(velocity.magnitude()>planes[target]->velocity.magnitude()+50)
	//		controller.accelerate=1.0;
	//	else if(velocity.magnitude()<planes[target]->velocity.magnitude()-50)
	//		controller.brake=1.0;
	//}
	//else
	//{

	//	if(altitude<30 || pos.y<settings.SEA_LEVEL+30)
	//		controller.climb=1.0;
	//	else if(altitude>=25 && rand()%2>1)
	//		controller.dive=1.0;

	//	if(velocity.magnitude()<500)
	//		controller.accelerate=1.0;
	//}

}
void AIplane::playerVs_calcMove(int value)
{
	////Vec3f self[2]={		Vec3f(x,y,z), 		Vec3f(sin(angle*PI/180),climb,cos(angle*PI/180)).normalize()};//current,future
	//controller=controlState();

	//Vec3f enemy;

	//target	=planes.begin()->first;
	//Vec3f loc(planes[target]->pos);

	//if(acos( velocity.dot( (loc-pos).normalize() )) < 0.5 && pos.distance(loc)<2000)
	//	controller.shoot1=1.0;

	//if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
	//	controller.left=1.0;
	//else if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
	//	controller.right=1.0;

	//if(altitude>=30 && planes[target]->pos.y<pos.y)
	//	controller.dive=1.0;
	//else if(planes[target]->pos.y>pos.y || altitude<25)
	//	controller.climb=1.0;

	//if(velocity.magnitude()<planes[target]->velocity.magnitude()+50)
	//	controller.accelerate=1.0;
	//else if(velocity.magnitude()>planes[target]->velocity.magnitude()-50)
	//	controller.brake=1.0;
}
void AIplane::teams_calcMove(int value)
{
	//todo: add code
}
void AIplane::calcMove(int value)
{
	if(settings.GAME_TYPE==PLAYER_VS) playerVs_calcMove(value);
	else if(settings.GAME_TYPE==FFA) freeForAll_calcMove(value);
	else if(settings.GAME_TYPE==TEAMS) teams_calcMove(value);
	else cout << "Error: new game type!" << endl; 
}
bool AIplane::Update(float value)
{
	if(respawning && respawnTime<gameTime())
	{
		if(settings.ON_AI_HIT==RESPAWN)
		{
			spawn();
		}
		else if(settings.ON_AI_HIT==RESTART)
		{
			//not yet (or possibly ever) handled
		}
		else if(settings.ON_AI_HIT==DIE)
		{
			return false;
		}
	}

	if(!dead)
	{
		calcMove(value);
	}

	//updatePos(value);
	updateAll();
	return true;
}