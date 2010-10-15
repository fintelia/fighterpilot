#include "main.h"

//const int PLANE		=0x01;
const int PLAYER_PLANE	=0x02;
const int AI_PLANE		=0x04;


//void planeBase::updatePos(float ms)
//{
//	if(respawning)
//	{
//
//		//float mX=pos.x/size;
//		//float mZ=pos.z/size;
//		//altitude=pos.y-terrain->getHeight(mX, mZ);
//		//if(altitude>pos.y-terrain->getHeight(mX+1, mZ))		altitude=pos.y-terrain->getHeight(mX+1, mZ);
//		//if(altitude>pos.y-terrain->getHeight(mX, mZ+1))		altitude=pos.y-terrain->getHeight(mX, mZ+1);
//		//if(altitude>pos.y-terrain->getHeight(mX+1, mZ+1))	altitude=pos.y-terrain->getHeight(mX+1, mZ+1);
//	
//		altitude=pos.y-terrain->getInterpolatedHeight(pos.x/size,pos.z/size);
//		//altitude=altitude+pos.y;
//	 	if(altitude<3.0||hitGround)
//		{
//			pos.y=pos.y-altitude+3;
//			hitGround=true;
//		}
//		else
//		{
//			velocity.y-=ms/10;
//			pos+=velocity*(ms/1000);
//		}
//		for(int i=0;i<1;i++)
//		{
//			newSmoke.insert(pos,95,10.0+5.0*(rand()%1000-500)/500,10.0-float(rand()%100)/20.0);
//		}
//	}
//	else
//	{	
//		if(rockets.left<rockets.max)
//			rockets.rechargeLeft-=ms;
//		if(rockets.rechargeLeft<0)
//		{
//			rockets.left++;
//			if(rockets.left==rockets.max)
//				rockets.rechargeLeft=rockets.rechargeTime;
//			else
//				rockets.rechargeLeft+=rockets.rechargeTime;
//		}
//		rockets.coolDownLeft-=ms;
//
//		if(machineGun.roundsLeft==0)
//			machineGun.rechargeLeft-=ms;
//		if(machineGun.rechargeLeft<0)
//		{
//			machineGun.left+=machineGun.roundsMax;
//			machineGun.roundsLeft=machineGun.roundsMax;
//			machineGun.rechargeLeft=machineGun.rechargeTime;
//		}
//		machineGun.coolDownLeft-=ms;
//
//		if(controled)
//		{
//			if(wayPoints.size()>0)
//				autoPilotUpdate(ms);
//			else
//				controled=false;
//		}
//		else
//		{
//			//float t=value/100.0;//acounts for time
//			//int mX=pos.x/size;
//			//int mZ=pos.z/size;
//			altitude=pos.y-terrain->getInterpolatedHeight(pos.x/size,pos.z/size);
//			//altitude=pos.y-terrain->getHeight(mX, mZ);
//			//if(altitude>pos.y-terrain->getHeight(mX+1, mZ))			altitude=pos.y-terrain->getHeight(mX+1, mZ);
//			//if(altitude>pos.y-terrain->getHeight(mX, mZ+1))			altitude=pos.y-terrain->getHeight(mX, mZ+1);
//			//if(altitude>pos.y-terrain->getHeight(mX+1, mZ+1))		altitude=pos.y-terrain->getHeight(mX+1, mZ+1);
//
//			if(altitude<3.0){die();pos.y-=altitude-3;hitGround=true;}
//			
//			//angle -= turn/60*t+sin(turn*DegToRad)*climb*t;//second term acounts for climb
//			Angle a=atan2A(velocity.z,velocity.x)+(turn/120)*PI/180;
//			float r=sqrt(velocity.x*velocity.x+velocity.z*velocity.z);
//			velocity.z=r*sin(a);
//			velocity.x=r*cos(a);
//			//pos.x += sin(float(angle *DegToRad))*acceleration*7.5*t;
//			//pos.z += cos(float(angle *DegToRad))*acceleration*7.5*t;
//			//pos.y += cos(turn*DegToRad)*climb*4.5*t;
//			
//			//////////////////////move//////////////////////////////////////
//			if(ms>0)
//			{
//				Vec3f up,right,move(0,0,0);					//define our vectors		
//				upAndRight(velocity,roll,up,right);			//set up and right
//				move+=Vec3f(0,-50*(ms/1000),0);				//gravity
//				move+=velocity*(ms/1000);					//thrust
//				move+=up*velocity.magnitude()*(ms/1000)*0.2;//lift
//				move*=0.95;									//drag
//				pos+=move;									//increase position
//			}
//			////////////////////end move////////////////////////////////////
//			bool setAutoPilot=false;
//			if(pos.x > settings.MAX_X * size && !controled){pos.x = settings.MAX_X * size-6;setAutoPilot=true;}
//			if(pos.x < settings.MIN_X * size && !controled){pos.x = settings.MIN_X * size+6;setAutoPilot=true;}
//			if(pos.z > settings.MAX_Y * size && !controled){pos.z = settings.MAX_Y * size-6;setAutoPilot=true;}
//			if(pos.z < settings.MIN_Y * size && !controled){pos.z = settings.MIN_Y * size+6;setAutoPilot=true;}
//			if(setAutoPilot)
//			{
//				returnToBattle();
//			}
//
//			if(pos.y>300 && velocity.y>0)
//				velocity.y=clamp(velocity.y-ms/2500,0,velocity.y);
//
//			//if(pos.y>250)	pos.y=250;
//			roll=turn*PI/180;
//			Level(ms);
//		}
//		//if(angle > 360)						angle -= 360;
//		//if(angle < 0)						angle += 360;
//		//Vec3f v(sin(float(angle * PI / 180)),climb,cos(float(angle * PI / 180)));
//		//Vec3f o(sin(float((angle+90) * PI / 180)),cos((turn+270)/180*3.1415926535),cos(float((angle+90) * PI / 180)));
//		//v=v.normalize();
//		//o=o.normalize();                         MUST BE REPLACED!
//		//normal = v.cross(o);
//		//forward=Vec3f(sin(angle*DegToRad)*acceleration,cos(turn*DegToRad)*climb,cos(angle *DegToRad)*acceleration).normalize();
//		
//		normal=Vec3f(0,1,0);
//		findTargetVector();
//
//	}
//}
void planeBase::updateAll()
{
	double time=gameTime();
	double ms=time-lastUpdateTime;
	lastUpdateTime=time;

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
			velocity.y-=ms/10;
			pos+=velocity*(ms/1000);
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
			Vec3f lastVelocity=velocity;

			altitude=pos.y-terrain->getInterpolatedHeight(pos.x/size,pos.z/size);
			if(altitude<3.0){die();pos.y-=altitude-3;hitGround=true;}
			
			velocity=velocity.normalize()*clamp(velocity.magnitude() + 10.0f*controller.accelerate*ms - 10.0f*controller.brake*ms,300.0,700.0);
			float r=sqrt(velocity.x*velocity.x+velocity.z*velocity.z)*sin(PI/3);
			velocity.y=clamp(velocity.y + ms*controller.climb*0.2 - ms*controller.dive*0.2,-r,r);
			turn=clamp(turn + 0.05f*controller.right*ms - 0.05f*controller.left*ms,-60,60);

			//angle -= turn/60*t+sin(turn*DegToRad)*climb*t;//second term acounts for climb
			Angle a=atan2A(velocity.z,velocity.x)+(turn/120)*PI/180;
			r=sqrt(velocity.x*velocity.x+velocity.z*velocity.z);
			velocity.z=r*sin(a);
			velocity.x=r*cos(a);
			//////////////////////move//////////////////////////////////////
			if(ms>0)
			{
				Vec3f up,right,move(0,0,0);					//define our vectors		
				upAndRight(velocity,roll,up,right);			//set up and right
				move+=Vec3f(0,-50*(ms/1000),0);				//gravity
				move+=velocity*(ms/1000);					//thrust
				move+=up*velocity.magnitude()*(ms/1000)*0.2;//lift
				move*=0.95;									//drag
				pos+=move;									//increase position
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

			if(pos.y>300 && velocity.y>0)
				velocity.y=clamp(velocity.y-ms/2500,0,velocity.y);

			//if(pos.y>250)	pos.y=250;
			roll=turn*PI/180;
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
					Vec3f t=velocity.normalize()*(1.0-extraShootTime/ms) + lastVelocity.normalize()*extraShootTime/ms;//+Vec3f(float(rand()%1000)/50000,float(rand()%1000-500)/50000,float(rand()%1000)/50000);
					bullets.push_back(bullet(l,t,id,time-extraShootTime-machineGun.coolDown));
				}
			}
			if(controller.shoot2>0.75)	ShootMissile();
		}
		//if(angle > 360)						angle -= 360;
		//if(angle < 0)						angle += 360;
		//Vec3f v(sin(float(angle * PI / 180)),climb,cos(float(angle * PI / 180)));
		//Vec3f o(sin(float((angle+90) * PI / 180)),cos((turn+270)/180*3.1415926535),cos(float((angle+90) * PI / 180)));
		//v=v.normalize();
		//o=o.normalize();                         MUST BE REPLACED!
		//normal = v.cross(o);
		//forward=Vec3f(sin(angle*DegToRad)*acceleration,cos(turn*DegToRad)*climb,cos(angle *DegToRad)*acceleration).normalize();
		
		normal=Vec3f(0,1,0);
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

		velocity=		wayPoints.back().fwd;
		pos=			wayPoints.back().position;		
		roll=			wayPoints.back().roll;
		turn=0;

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

	t=(w2.time-time)/(w2.time-w1.time);

	//pos.x=w1.position[0]*t + w2.position[0]*(1.0-t);
	//pos.y=w1.position[1]*t + w2.position[1]*(1.0-t);
	//pos.z=w1.position[2]*t + w2.position[2]*(1.0-t);
	//turn=w1.turn*t + w2.turn*(1.0-t);
	//acceleration=w1.velocity*t + w2.velocity*(1.0-t);
	//angle=w1.angle*t + w2.angle*(1.0-t);
	//climbAng=w1.climbAng*t + w2.climbAng*(1.0-t);

	pos=		w1.position*t+w2.position*(1.0-t);
	velocity=	w1.fwd*t+w2.fwd*(1.0-t);
	roll=		lerp(w1.roll,w2.roll,t);//w1.roll*t+w2.roll*(1.0-t);
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

		velocity=		wayPoints.back().fwd;
		pos=			wayPoints.back().position;		
		roll=			wayPoints.back().roll;
		turn=0;

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

	pos=		w1.position*t+w2.position*(1.0-t);
	velocity=	(w1.fwd*t+w2.fwd*(1.0-t)).normalize() * w1.position.distance(w2.position)/(w2.time-w1.time);
	roll=		lerp(w1.roll,w2.roll,t);

	turn=0;
	controled=false;
	wayPoints.clear();
}
void planeBase::returnToBattle()
{
	wayPoints.clear();
	controled =true;
	double time=gameTime();
	Vec3f vel2D(velocity.x,0,velocity.z);
	camera=Vec3f(pos.x - vel2D.normalize().x*175, pos.y + sin(45.0)*175,	 pos.z - vel2D.normalize().z*175);
	center=Vec3f(pos.x + vel2D.normalize().x*175, pos.y, pos.z + vel2D.normalize().z*175);
	
	Vec3f fwd(velocity.x,0,velocity.z); fwd=fwd.normalize();
	Vec3f right=fwd.cross(normal);
	wayPoints.push_back(wayPoint(time,									pos,
								velocity,								roll));

	wayPoints.push_back(wayPoint(time+4000.0,							pos+fwd*1100,
								fwd,									0));

	wayPoints.push_back(wayPoint(time+5000.0,							pos+fwd*1150+Vec3f(0,150,0),
								Vec3f(0,1,0),							0));

	wayPoints.push_back(wayPoint(time+5000.0,							pos+fwd*1150+Vec3f(0,150,0),
								Vec3f(0,1,0),							PI));

	wayPoints.push_back(wayPoint(time+6000.0,							pos+fwd*1100+Vec3f(0,300,0),
								-fwd,									PI));


	wayPoints.push_back(wayPoint(time+9000.0,							pos+fwd*600,
								-(fwd*11+Vec3f(0,3,0)).normalize(),		0));

	wayPoints.push_back(wayPoint(time+10500.0,							pos,
								-fwd,									0));

	////wayPoints.push_back(wayPoint(time+7000.0,	pos,
	////							0,				acceleration,
	////							angle+180,		0));


}
void planeBase::returnToBattle2()
{
	wayPoints.clear();
	controled =true;
	double time=gameTime();

	//wayPoints.push_back(wayPoint(time,		pos,
	//							turn,		acceleration,
	//							angle,		atan(climb/acceleration)*50));

	//wayPoints.push_back(wayPoint(time+8000.0,pos,
	//							turn,		acceleration,
	//							angle+180,	atan(climb/acceleration)*50));

}
//void planeBase::Accelerate(float value)
//{
//	if(controled) return;
//	velocity=velocity.normalize()*clamp(velocity.magnitude() + 1.0f*value,300.0,700.0);
//}
//void planeBase::Brake(float value)
//{
//	if(controled) return;
//	velocity=velocity.normalize()*clamp(velocity.magnitude() - 1.0f*value,300.0,700.0);
//}
//
//void planeBase::Turn_Right(float value)
//{
//	if(controled) return;
//	turn += 0.05f*value;
//	if(turn > 60)
//		turn=60.0f;
//}
//void planeBase::Turn_Left(float value)
//{
//	if(controled) return;
//	turn -= 0.05f*value;
//	if(turn < -60)
//		turn=-60.0f;
//}
//
//void planeBase::Climb(float value)
//{
//	if(controled) return;
//	float r=sqrt(velocity.x*velocity.x+velocity.z*velocity.z)*sin(PI/3);//range  for y so angle of elivation is less than PI/3
//	velocity.y=clamp(velocity.y+value*0.2,-r,r);
//}
//void planeBase::Dive(float value)
//{
//	if(controled) return;
//	float r=sqrt(velocity.x*velocity.x+velocity.z*velocity.z)*sin(PI/3);//range  for y so angle of elivation is less than PI/3
//	velocity.y=clamp(velocity.y-value*0.2,-r,r);
//}
void planeBase::Level(float ms)
{
	//Angle ang=acosA(velocity.normalize().dot(Vec3f(velocity.x,0,velocity.x).normalize()));
	//velocity=(Vec3f(velocity.x,0,velocity.x).normalize()*10*value/ang.degrees() + velcity*(1.0-10*value/ang.degrees())
	//if(climb < 0)
	//{
	//	climb += 0.01f * fabs(climb)*value;
	//}
	//else if(climb > 0)
	//{
	//	climb -= 0.01f * climb*value;
	//}
	int sgn=turn/abs(turn);
	turn-=5.0*(ms/1000)*sgn;
	if(turn/abs(turn)!=sgn)
		turn=0;

	sgn=velocity.y/abs(velocity.y);
	velocity.y-=30.0*(ms/1000)*sgn;
	if(velocity.y/abs(velocity.y)!=sgn)
		velocity.y=0;
	//if(turn < 0.1)
	//{
	//	turn += 0.01f * fabs(turn)*value;
	//}
	//if(turn > 0.1)
	//{
	//	turn -= 0.01f * turn*value;
	//}
	//acceleration -= 0.025f;
	//if(acceleration<2)
	//	acceleration=2;
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
	//Vec3f self[2];//current,future
	//self[0][0]=x;self[0][1]=y;self[0][2]=z;
	//self[1]=forward;

	Vec3f enemy;
	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	{
		enemy=(*i).second->pos;
		if(acos( velocity.dot( (enemy-pos).normalize() )) < 0.1 && (*i).first!=id && !(*i).second->dead && pos.distance(enemy)<4000)
		{
			targeter=(enemy-pos).normalize();
			return;
		}
	}
	targeter=velocity;
}
//void planeBase::Shoot()
//{
//	if(machineGun.coolDownLeft>0.0 || machineGun.roundsLeft==0 || controled)
//		return;
//
//	bullets.push_back(bullet(pos.x,pos.y,pos.z,targeter.x+float(rand()%1000-500)/500,targeter.y+float(rand()%1000-500)/500,targeter.z+float(rand()%1000-500)/500,id));
//	
//	machineGun.coolDownLeft=machineGun.coolDown;
//	machineGun.roundsLeft--;
//}
void planeBase::ShootMissile()
{
	if(rockets.coolDownLeft>0 || rockets.left<=0 || controled)
		return;

	Vec3f up,right;
	upAndRight(velocity.normalize(),roll,up,right);
	int d=settings.missileStats[settings.planeStats[defaultPlane].hardpoints[rockets.max-rockets.left].missileNum].dispList;
	Vec3f o=settings.planeStats[defaultPlane].hardpoints[rockets.max-rockets.left].offset;
	//missiles.push_back(missile(pos+right*25-up*10,velocity,id));
	//missiles.push_back(missile(pos-right*25-up*10,velocity,id));
	missiles.push_back(missile(pos+right*o.x*5+up*o.y*5+velocity.normalize()*o.z*5,velocity,id,d));
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

	velocity = (pos-Vec3f(size*32,pos.y,size*32)).normalize()*300.0;
	turn = 0;
	roll=0;

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

	velocity = (pos-Vec3f(size*32,pos.y,size*32)).normalize()*300.0;

	turn = 0;
	roll=0;
	dead = false;
	controled=false;
	maneuver=0;
	hitGround=false;
	health=maxHealth;

	respawning=false;
}
void AIplane::freeForAll_calcMove(int value)
{
	controller=controlState();
	Vec3f self[2]={pos,pos+velocity};//current,future
	Vec3f enemy;
	if(machineGun.coolDownLeft<=0.0)
	{
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			enemy=(*i).second->pos;
			if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.5 && self[0].distance(enemy)<2000 && (*i).first!=id)
			{
				controller.shoot1=1.0;
				break;
			}
		}
	}
	target=-1;
	float ang=999;
	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	{
		enemy=(*i).second->pos;
		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < ang && self[0].distance(enemy)<3000 && (*i).second->team!=team)
		{
			ang=acos( self[1].dot( (enemy-self[0]).normalize() ));
			target=(*i).first;
		}
	}
	
	if(target>=0)
	{
		if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
			controller.left=1.0;
		else if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
			controller.right=1.0;

		if(altitude>=30 && planes[target]->pos.y<pos.y)
			controller.dive=1.0;
		else if(planes[target]->pos.y>pos.y || altitude<25)
			controller.climb=1.0;

		if(velocity.magnitude()>planes[target]->velocity.magnitude()+50)
			controller.accelerate=1.0;
		else if(velocity.magnitude()<planes[target]->velocity.magnitude()-50)
			controller.brake=1.0;
	}
	else
	{

		if(altitude<30 || pos.y<settings.SEA_LEVEL+30)
			controller.climb=1.0;
		else if(altitude>=25 && rand()%2>1)
			controller.dive=1.0;

		if(velocity.magnitude()<500)
			controller.accelerate=1.0;
	}

}
void AIplane::playerVs_calcMove(int value)
{
	//Vec3f self[2]={		Vec3f(x,y,z), 		Vec3f(sin(angle*PI/180),climb,cos(angle*PI/180)).normalize()};//current,future
	controller=controlState();

	Vec3f enemy;

	target	=planes.begin()->first;
	Vec3f loc(planes[target]->pos);

	if(acos( velocity.dot( (loc-pos).normalize() )) < 0.5 && pos.distance(loc)<2000)
		controller.shoot1=1.0;

	if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
		controller.left=1.0;
	else if(atan2(pos.z-planes[target]->pos.z,pos.x-planes[target]->pos.x)<0)
		controller.right=1.0;

	if(altitude>=30 && planes[target]->pos.y<pos.y)
		controller.dive=1.0;
	else if(planes[target]->pos.y>pos.y || altitude<25)
		controller.climb=1.0;

	if(velocity.magnitude()<planes[target]->velocity.magnitude()+50)
		controller.accelerate=1.0;
	else if(velocity.magnitude()>planes[target]->velocity.magnitude()-50)
		controller.brake=1.0;
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