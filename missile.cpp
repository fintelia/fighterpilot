
#include "game.h"
//
//void missile::findTarget()
//{
//	Vec3f enemy;
//	Angle ang=0.5;
//
//	const map<objId,nPlane*>& planes = world.planes();
//
//	for(auto i = planes.begin(); i != planes.end();i++)
//	{
//		enemy=(*i).second->position;
//		if(acosA( velocity.normalize().dot( (enemy-position).normalize() )) < ang && /*dist(pos,enemy)<life*speed*0.4 &&*/ (*i).second->team!=team && !(*i).second->dead)
//		{
//			ang=acos( velocity.normalize().dot( (enemy-position).normalize() ));
//			target=(*i).first;
//		}
//	}
//}
missileBase::missileBase(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float Speed, int Owner):object(sPos, sRot, Type), life(15.0), difAng(0), lastAng(0), speed(Speed), acceleration(1180.0/3.0), owner(Owner)
{
	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
}

void missileBase::updateFrame(float interpolation) const
{
	if(meshInstance)
	{
		meshInstance->update(lerp(lastPosition,position,interpolation), slerp(lastRotation,rotation, interpolation), !awaitingDelete);
	}
}	bool engineStarted;
	double engineStartTime;
missile::missile(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target):missileBase(Type, Team, sPos, sRot, speed, Owner), target(Target), launchTime(world.time()), engineStarted(false), smallContrailStarted(false), contrailStarted(false)
{

}
void missile::updateSimulation(double time, double ms)
{
	if(!contrailStarted && time - launchTime >= 200.0)
	{
		contrailStarted = true;
		particleManager.addEmitter(new particle::contrail(),id);
	}
	if(!smallContrailStarted && time - launchTime >= 300.0)
	{
		smallContrailStarted = true;
		particleManager.addEmitter(new particle::contrailSmall(),id,Vec3f(0,0,-5.0));
	}
	lastPosition = position;
	lastRotation = rotation;
	/////////////////follow target////////////////////
	nPlane* enemy = (nPlane*)world[target].get();
	Vec3f destVec=rotation*Vec3f(0,0,1);
	if(enemy != NULL && !enemy->dead && engineStarted)
	{
		destVec = (enemy->position - position).normalize();
		Vec3f fwd = rotation * Vec3f(0,0,1);

		Angle angle = acosA(destVec.dot(fwd));

		Quat4f targetRot(destVec);

		if( angle > PI*3/4)
		{
			target = 0;
		}
		else if(angle > 0.01)// we don't want to (or need to) divide by zero
		{
			float turnRate = PI * 1.5 * ms/1000 * (speed/1180.0);
			rotation = slerp(rotation,targetRot,(float)turnRate/angle);
		}

	}
	else if(enemy != NULL && enemy->dead)
	{
		particleManager.addEmitter(new particle::explosionSmoke(),position,2.0);
		life = 0.0;
	}
	//////////////////Movement//////////////
	if(!engineStarted && time - launchTime >= 150.0)
		engineStarted = true;
	
	if(engineStarted)
	{
		speed = min(speed + acceleration*(ms/1000), 1180.0);
		position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);
	}
	else
	{
		double lTime = (time-launchTime - ms)/1000.0;
		double cTime = (time-launchTime)/1000.0;

		position.y += 84.9 * (lTime*lTime - cTime*cTime);
		position += (rotation*Vec3f(0,0,1)) * speed * (ms/1000);
	}

	life-=ms/1000;
	if(life < 0.0 || world.altitude(position) < 0.0)
	{
		awaitingDelete = true;
		meshInstance->setDeleteFlag(true);
		meshInstance = nullptr;
	}

}
void SAMmissile::init()
{
	particleManager.addEmitter(new particle::contrail(),id);
	particleManager.addEmitter(new particle::contrailSmall(),id,Vec3f(0,0,-5.0));
}
void SAMmissile::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;
	/////////////////follow target////////////////////
	nPlane* enemy = (nPlane*)world[target].get();
	Vec3f destVec=rotation*Vec3f(0,0,1);
	if(enemy != NULL && !enemy->dead)
	{
		destVec = (enemy->position - position).normalize();
		Vec3f fwd = rotation * Vec3f(0,0,1);

		Angle angle = acosA(destVec.dot(fwd));

		Quat4f targetRot(destVec);

		if( angle > PI*3/4)
		{
			target = 0;
		}
		else if(angle > 0.01)// we don't want to (or need to) divide by zero
		{
			rotation = slerp(rotation,targetRot,(float)((PI * 2.5 * ms/1000)/angle));
		}

	}
	else if(enemy != NULL && enemy->dead)
	{
		particleManager.addEmitter(new particle::explosionSmoke(),position,2.0);
		life = 0.0;
	}
	//////////////////Movement//////////////
	speed = min(speed + acceleration*(ms/1000), 700.0);
	position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);

	life-=ms/1000;
//	if(life < 0.0 || world.altitude(position) < 0.0)
//	{
//		awaitingDelete = true;
//		meshInstance->setDeleteFlag(true);
//		meshInstance = nullptr;
//	}
}
