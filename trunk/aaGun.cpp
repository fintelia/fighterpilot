#include "main.h"

void aaGun::update(double time, double ms)
{
	control->update();
	controlState controller=control->getControlState();

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

	Quat4f lastRotation=rotation;

	elevation	+= 1.0*controller.up*(ms/1000) - 1.0*controller.down*(ms/1000);
	angle		+= 1.5*controller.left*(ms/1000) - 1.5*controller.right*(ms/1000);
		
	//speed = max(speed,clamp(speed + 10.0f*controller.accelerate*ms - 10.0f*controller.brake*ms,250.0,669.0));
	//climb = clamp(climb + 1.0*controller.climb*(ms/1000) - 1.0*controller.dive*(ms/1000),-PI/3,PI/4);
	//turn  = clamp(turn  + 1.5*controller.right*(ms/1000) - 1.5*controller.left*(ms/1000),-1.0,1.0);
	//direction -= turn*ms/3000.0;//needs to be adjusted to be continious

	//rotation = Quat4f(0,0,0,1);
	//rotation = Quat4f(Vec3f(0,0,1),turn) * rotation;
	//rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
	//rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;

//	findTargetVector();
	if(controller.shoot1 <= 0.75)
		extraShootTime=0.0;
	else if(controller.shoot1 > 0.75)
	{
		extraShootTime+=ms;
		while(extraShootTime > machineGun.coolDown && machineGun.roundsLeft > 0)
		{
			world.bullets.push_back(bullet(position,Vec3f(cos(angle),sin(elevation),sin(angle)),id,time-extraShootTime-machineGun.coolDown));

			extraShootTime-=machineGun.coolDown;
			machineGun.roundsLeft--;
			shotsFired++;
		}
	}
}
void aaGun::findTargetVector()
{
	//nPlane* closestPlane = NULL;
	//double cPlaneDist = 1000.0 * 1000.0;
	//for(auto i = world.planes().begin(); i != world.planes().end();i++)
	//{
	//	double iDist = position.distanceSquared(i->second->position);
	//	if(iDist < cPlaneDist)
	//	{
	//		closestPlane = i->second;
	//		cPlaneDist = iDist;
	//		break;
	//	}
	//}
	//if(closestPlane != NULL)
	//{
	//	cPlaneDist = sqrt(cPlaneDist);
	//	targeter = (closestPlane->position + closestPlane->rotation * Vec3f(0,0,1) * cPlaneDist / BULLET_SPEED * closestPlane->speed *0.5 - position).normalize();
	//}
	//else
	//{
	//	targeter = rotation * Vec3f(0,1,0);
	//}
}
void aaGun::die()
{
	if(!dead)	explode=new explosion(position);
	dead = true;
}
void aaGun::loseHealth(float healthLoss)
{
	health-=healthLoss;
	if(health<0)
	{
		health=0;
		die();
	}
}
void aaGun::spawn()
{
	initArmaments();
	if(explode!=NULL)
	{
		delete explode;
		explode=NULL;
	}
	position = startPos;
	rotation = startRot;

	dead = false;
	health=maxHealth;

	
	shotsFired = 0;

	angle = 0.0;
	elevation = PI/4;
}
void aaGun::drawExplosion(bool flash)
{
	if(explode!=NULL)
	{
		explode->pos=position;
		explode->render(flash);
	}
}
void aaGun::initArmaments()
{
	machineGun.max			= machineGun.left			= 1000; 
	machineGun.roundsMax	= machineGun.roundsLeft		= 200;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 15.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 15.0;
	machineGun.firing									= false;
}
aaGun::aaGun(Vec3f sPos, Quat4f sRot, objectType Type, objectController* c):controlledObject(Vec3f(sPos.x,world.elevation(position.x,position.z),sPos.z), sRot, Type, c), maxHealth(100), explode(NULL), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0)
{
	spawn();
}
aaGun::aaGun(Vec3f sPos, Quat4f sRot, objectType Type):controlledObject(Vec3f(sPos.x,world.elevation(position.x,position.z),sPos.z), sRot, Type, CONTROL_COMPUTER), maxHealth(100), explode(NULL), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0)
{
	spawn();
}