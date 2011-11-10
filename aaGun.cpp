
#include "game.h"
void aaGun::updateSimulation(double time, double ms)
{
	//control->update();
	//controlState controller=control->getControlState();

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

	/////////////////////CONTROL//////////////////////
	bool shoot;

	target=0;
	float lDistSquared = 0.0;
	float nDistSquared;
	auto planes = world(PLANE);
	for(auto n = planes.begin(); n!= planes.end(); n++)
	{
		nDistSquared = n->second->position.distanceSquared(position);
		if(!n->second->dead && n->second->team != team && nDistSquared < 20000 * 20000 && (target == 0 || nDistSquared < lDistSquared))
		{
			target = n->second->id;
			lDistSquared = nDistSquared;
		}
	}

	if(target != 0)
	{
		targeter = (world[target]->position - position).normalize();
		rotation = Quat4f(targeter);
		shoot = true;
	}
	else
	{
		targeter = Vec3f();
		shoot = false;
	}
	///////////////////END CONTROL////////////////////

	if(!shoot)
		extraShootTime=0.0;
	else
	{
		extraShootTime+=ms;
		while(extraShootTime > machineGun.coolDown && machineGun.roundsLeft > 0)
		{

	//		world.bullets.push_back(bullet(position,targeter+random<Vec3f>()*0.010,id,time-extraShootTime-machineGun.coolDown));

			extraShootTime-=machineGun.coolDown;
			machineGun.roundsLeft--;
			shotsFired++;
		}
	}
	//static float missileCoolDown = 7000;
	//missileCoolDown -= ms;
	//if(missileCoolDown <= 0.0 && target != 0)
	//{
	//	Vec3f p(position.x,world.elevation(position.x,position.z)+5.0,position.z);
	//	missileCoolDown = 7000;
	//	world.objectList.newMissile(MISSILE1,team,p,rotation*Quat4f(Vec3f(-1,0,0),PI/2),0,id,target);
	//}
}
void aaGun::updateFrame(float interpolation) const
{
	if(meshInstance)
	meshInstance->update(lerp(lastPosition,position,interpolation), slerp(lastRotation,rotation, interpolation), !dead);
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
	position = startPos;
	rotation = startRot;

	dead = false;
	health=maxHealth;


	position = Vec3f(position.x,world.elevation(position.x,position.z)+5.0,position.z);

	target = 0;
	shotsFired = 0;
}
void aaGun::initArmaments()
{
	machineGun.max			= machineGun.left			= 1000;
	machineGun.roundsMax	= machineGun.roundsLeft		= 200;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 150.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 20.0;
	machineGun.firing									= false;
}
aaGun::aaGun(Vec3f sPos, Quat4f sRot, objectType Type):selfControlledObject(Vec3f(sPos.x,world.elevation(position.x,position.z),sPos.z), sRot, Type), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0), maxHealth(100)
{
	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
	spawn();
}
