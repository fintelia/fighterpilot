
#include "game.h"
void antiAircraftArtilleryBase::updateFrame(float interpolation) const
{
	if(meshInstance)
	meshInstance->update(lerp(lastPosition,position,interpolation), slerp(lastRotation,rotation, interpolation), !dead);
}

void antiAircraftArtilleryBase::die()
{
	dead = true;
}
void antiAircraftArtilleryBase::loseHealth(float healthLoss)
{
	health-=healthLoss;
	if(health<0)
	{
		health=0;
		die();
	}
}
void antiAircraftArtilleryBase::spawn()
{
	position = startPos;
	rotation = startRot;

	dead = false;
	health=maxHealth;


	position = Vec3f(position.x,world.elevation(position.x,position.z)+5.0,position.z);

	target = 0;
	shotsFired = 0;
	extraShootTime=0.0;
}
antiAircraftArtilleryBase::antiAircraftArtilleryBase(Vec3f sPos, Quat4f sRot, objectType Type, int Team):object(Vec3f(sPos.x,world.elevation(position.x,position.z),sPos.z), sRot, Type, Team), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0), maxHealth(100)
{
	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
	spawn();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																		AA gun																				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AAgun::spawn()
{
	initArmaments();
	antiAircraftArtilleryBase::spawn();
}
void AAgun::initArmaments()
{
	machineGun.max			= machineGun.left			= 1000;
	machineGun.roundsMax	= machineGun.roundsLeft		= 200;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 150.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 20.0;
	machineGun.firing									= false;
}
void AAgun::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;
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
		rotation = Quat4f(targeter) * Quat4f(Vec3f(1,0,0), PI/2);
		shoot = true;
	}
	else
	{
		targeter = Vec3f();
		rotation = Quat4f(targeter);
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
			dynamic_pointer_cast<bulletCloud>(world[bullets])->addBullet(position,targeter+random3<float>()*0.010,id,time-extraShootTime-machineGun.coolDown);

			extraShootTime-=machineGun.coolDown;
			machineGun.roundsLeft--;
			shotsFired++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																		SAM battery																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SAMbattery::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;
	//control->update();
	//controlState controller=control->getControlState();

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
		rotation = Quat4f(targeter) * Quat4f(Vec3f(1,0,0), PI/2);
		shoot = true;
	}
	else
	{
		targeter = Vec3f();
		rotation = Quat4f(targeter);
		shoot = false;
	}
	///////////////////END CONTROL////////////////////

	missileCoolDown -= ms;
	if(missileCoolDown <= 0.0 && target != 0)
	{
		Vec3f p(position.x,world.elevation(position.x,position.z)+5.0,position.z);
		missileCoolDown = 7000;
		world.newObject(new missile(SAM_MISSILE, team, position, Quat4f(Vec3f(0,1,0))/*rotation*Quat4f(Vec3f(-1,0,0),PI/2)*/,1000, id, target));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																		flak cannon																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void flakCannon::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;
	//control->update();
	//controlState controller=control->getControlState();

	/////////////////////CONTROL//////////////////////
	bool shoot;

	target=0;
	float lDistSquared = 0.0;
	float nDistSquared;
	auto planes = world(PLANE);
	for(auto n = planes.begin(); n!= planes.end(); n++)
	{
		nDistSquared = n->second->position.distanceSquared(position);
		if(!n->second->dead && n->second->team != team && nDistSquared < 5000 * 5000 && (target == 0 || nDistSquared < lDistSquared))
		{
			target = n->second->id;
			lDistSquared = nDistSquared;
		}
	}

	if(target != 0)
	{
		targeter = (world[target]->position - position).normalize();
		rotation = Quat4f(targeter) * Quat4f(Vec3f(1,0,0), PI/2);
		shoot = true;
	}
	else
	{
		targeter = Vec3f();
		rotation = Quat4f(targeter);
		shoot = false;
	}
	///////////////////END CONTROL////////////////////

	missileCoolDown -= ms;
	if(missileCoolDown <= 0.0 && target != 0)
	{
		Vec3f p(position.x,world.elevation(position.x,position.z)+5.0,position.z);
		missileCoolDown = random<float>(1900, 2200);
		Vec3f t = random3<float>();
		t.x *= 500.0;
		t.z *= 500.0;
		t.y *= 100.0;
		//world.newObject(new flakMissile(FLAK_MISSILE, team, position, rotation*Quat4f(Vec3f(-1,0,0),PI/2), 0, id, world[target]->position + t));
		particleManager.addEmitter(new particle::flakExplosionSmoke(),world[target]->position + t,20.0);
	}
}