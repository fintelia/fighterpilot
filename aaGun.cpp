
#include "game.h"
void antiAircraftArtilleryBase::updateFrame(float interpolation) const
{
	Mat4f objectTransform = Mat4f(slerp(lastRotation,rotation, interpolation), lerp(lastPosition,position,interpolation));

	if(meshInstance)
	{
		meshInstance->update(objectTransform);
	}
	

	if(turretMesh && !objectInfo.aaaData(type)->turrets.empty())
	{
		Vec3f turretRotCenter = objectInfo.aaaData(type)->turrets.front().rotationCenter;
		Mat4f turretTransform = objectTransform;
		if(targeter.magnitudeSquared() >= 0.001)
			turretTransform = turretTransform * Mat4f(turretRotCenter) * Mat4f(Quat4f(Vec3f(0,0,-1), Vec3f(targeter.x,0,targeter.z).normalize())) * Mat4f(-turretRotCenter);
		turretMesh->update(turretTransform);

		if(cannonMesh && !objectInfo.aaaData(type)->turrets.empty())
		{
			Vec3f cannonRotCenter = objectInfo.aaaData(type)->cannons.front().rotationCenter;
			Mat4f cannonTransform = turretTransform * Mat4f(cannonRotCenter) * Mat4f(Quat4f(Vec3f(1,0,0), asinA(targeter.normalize().y))) * Mat4f(-cannonRotCenter);
			cannonMesh->update(cannonTransform);
		}
	}
}
void antiAircraftArtilleryBase::die()
{
	dead = true;
	particleManager.addEmitter(new particle::explosion(),id);
	particleManager.addEmitter(new particle::groundExplosionFlash(),id);
	particleManager.addEmitter(new particle::blackSmoke(),id);
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
antiAircraftArtilleryBase::antiAircraftArtilleryBase(Vec3f sPos, Quat4f sRot, objectType Type, int Team):object(Type, Team), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0)
{
	debugAssert(objectInfo.aaaData(type));

	lastPosition = position = Vec3f(sPos.x,world.elevation(sPos.x,sPos.z),sPos.z);
	lastRotation = rotation = Quat4f(Vec3f(0,1,0),world.terrainNormal(position.x,position.z));
	meshInstance = objectInfo[type]->newMeshInstance(position, rotation);

	auto& turrets = objectInfo.aaaData(Type)->turrets;
	if(!turrets.empty())
	{
		turretMesh = turrets.front().newMeshInstance(meshInstance);
	}

	auto& cannons = objectInfo.aaaData(Type)->cannons;
	if(!cannons.empty())
	{
		cannonMesh = cannons.front().newMeshInstance(meshInstance);
	}

	target.reset();
	health = 100.0;
	shotsFired = 0;
	extraShootTime=0.0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																		AA gun																				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AAgun::AAgun(Vec3f sPos, Quat4f sRot, objectType Type, int Team):antiAircraftArtilleryBase(sPos, sRot, Type, Team)
{


	initArmaments();
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

	if(dead)
		return;
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

	target.reset();
	float lDistSquared = 0.0;
	float nDistSquared;
	auto planes = world(PLANE);
	for(auto n = planes.begin(); n!= planes.end(); n++)
	{
		nDistSquared = n->second->position.distanceSquared(position);
		if(!n->second->dead && n->second->team != team && n->second->position.y - position.y > 100.0 && nDistSquared < 20000 * 20000 && (target.expired() || nDistSquared < lDistSquared))
		{
			target = static_pointer_cast<plane>(n->second);
			lDistSquared = nDistSquared;
		}
	}

	if(!target.expired())
	{
		auto targetPtr = target.lock();
		targeter = (targetPtr->position - position).normalize();

		/////////////find position and time that plane and bullet would be at the same location (makes AA guns too good!!!)
		//float s = 1000; //speed of bullets
		//Vec3f r = targetPtr->position - position;
		//Vec3f v = targetPtr->rotation * Vec3f(0,0,targetPtr->speed);
		//
		//float a = v.dot(v) - s*s;
		//float b = 2.0 * v.dot(r);
		//float c = r.dot(r);
		//float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);
		//if(b*b - 4.0*a*c >= 0.0 && t >= 0.0)
		//{
		//	targeter = (targetPtr->position + v * t - position).normalize();
		//}
		////////////////////////////////////////////////////////////////////////////

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
			dynamic_pointer_cast<bulletCloud>(world[bullets])->addBullet(position,targeter+random3<float>()*0.01,id,time-extraShootTime-machineGun.coolDown);

			extraShootTime-=machineGun.coolDown;
			machineGun.roundsLeft--;
			shotsFired++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																		SAM battery																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SAMbattery::updateFrame(float interpolation) const
{
	if(meshInstance)
		meshInstance->update(Mat4f(slerp(lastRotation,rotation, interpolation),lerp(lastPosition,position,interpolation)));
}
void SAMbattery::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;

	if(dead)
		return;

	/////////////////////CONTROL//////////////////////
	target.reset();
	float lDistSquared = 0.0;
	float nDistSquared;
	auto planes = world(PLANE);
	for(auto n = planes.begin(); n!= planes.end(); n++)
	{
		nDistSquared = n->second->position.distanceSquared(position);
		if(!n->second->dead && n->second->team != team && n->second->position.y - position.y > 100.0 && nDistSquared < 3000 * 3000 && (target.expired() || nDistSquared < lDistSquared))
		{
			target = static_pointer_cast<plane>(n->second);
			lDistSquared = nDistSquared;
		}
	}

	if(!target.expired())
	{
		targeter = (target.lock()->position - position).normalize();
//		rotation = Quat4f(targeter) * Quat4f(Vec3f(1,0,0), PI/2);
	}
	else
	{
		targeter = Vec3f();
//		rotation = Quat4f(targeter);
	}
	///////////////////END CONTROL////////////////////

	missileCoolDown -= ms;
	if(missileCoolDown <= 0.0 && !target.expired())
	{
		Vec3f p(position.x,world.elevation(position.x,position.z)+5.0,position.z);
		missileCoolDown = 18000;
		world.newObject(new SAMmissile(objectInfo.typeFromString("SAM_missile1"), team, position, rotation*Quat4f(Vec3f(-1,0,0),PI/2),1000, id, target.lock()->id));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																		flak cannon																			//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void flakCannon::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;

	if(dead)
		return;
	/////////////////////CONTROL//////////////////////
	target.reset();
	float lDistSquared = 0.0;
	float nDistSquared;
	auto planes = world(PLANE);
	for(auto n = planes.begin(); n!= planes.end(); n++)
	{
		nDistSquared = n->second->position.distanceSquared(position);
		if(!n->second->dead && n->second->team != team && n->second->position.y - position.y > 100.0 && nDistSquared < 2200 * 2200 && (target.expired() || nDistSquared < lDistSquared))
		{
			target = static_pointer_cast<plane>(n->second);
			lDistSquared = nDistSquared;
		}
	}

	if(!target.expired())
	{
		targeter = (target.lock()->position - position).normalize();
//		rotation = Quat4f(targeter) * Quat4f(Vec3f(1,0,0), PI/2);
	}
	else
	{
		targeter = Vec3f();
//		rotation = Quat4f(targeter);
	}
	///////////////////END CONTROL////////////////////

	missileCoolDown -= ms;
	if(missileCoolDown <= 0.0 && !target.expired())
	{
		shared_ptr<plane> planePtr = target.lock();

		missileCoolDown = random<float>(1900, 2200);
		Vec3f t = random3<float>();
		t.x = t.x * 200.0;
		t.y = t.y * 200.0;
		t.z = t.z * 200.0 + 185;

		t = planePtr->rotation * t;

		if(world.altitude(planePtr->position + t) >= 200.0)
		{
			float strength = clamp( (40.0*40.0 - t.magnitudeSquared()) / (40.0*40.0), 0.0, 1.0);
			planePtr->cameraShake = 0.3 * strength;
			planePtr->loseHealth(10.0 * strength);
			particleManager.addEmitter(new particle::explosion(),planePtr->position + t, 10.0);
			particleManager.addEmitter(new particle::explosionSmoke(),planePtr->position + t, 10.0);
		//	particleManager.addEmitter(new particle::explosionFlash(),planePtr->position + t, 10.0);
			particleManager.addEmitter(new particle::explosionFlash2(),planePtr->position + t, 15.0);
			particleManager.addEmitter(new particle::explosionFlash2(),planePtr->position + t, 15.0);
		}
	}
}
