
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
}

void missile::init()
{
	particleManager.addEmitter(new particle::contrail(),id);
	particleManager.addEmitter(new particle::contrailSmall(),id);
}
void missile::updateSimulation(double time, double ms)
{
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
			rotation = slerp(rotation,targetRot,(float)((PI * 1.5 * ms/1000)/angle));
		}

	}
	else if(enemy != NULL && enemy->dead)
	{
		particleManager.addEmitter(new particle::explosionSmoke(),position,2.0);
		life = 0.0;
	}
	//////////////////Movement//////////////
	speed+=acceleration*(ms/1000);
	if(speed > 1180.0) speed = 1180.0;
		position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);

	life-=ms/1000;
	if(life < 0.0 || world.altitude(position) < 0.0)
	{
		awaitingDelete = true;
		meshInstance->setDeleteFlag(true);
		meshInstance = nullptr;
	}

}
void flakMissile::updateSimulation(double time, double ms)
{
	if(position.distanceSquared(target) < 50.0*50.0)
	{
		awaitingDelete = true;
		meshInstance->setDeleteFlag(true);
		meshInstance = nullptr;

	//	particleManager.addEmitter(new particle::explosion(), position, 20.0);
		particleManager.addEmitter(new particle::flakExplosionSmoke(),position,20.0);
	}
	/////////////////follow target////////////////////
	rotation = Quat4f(target - position);

	//////////////////Movement//////////////
	speed += 800.0*(ms/1000);
	if(speed > 3000.0) speed = 3000.0;
	position += (rotation*Vec3f(0,0,1)) * speed * (ms/1000);

	life-=ms/1000;
	if(life < 0.0 || world.altitude(position) < 0.0)
	{
		awaitingDelete = true;
		meshInstance->setDeleteFlag(true);
		meshInstance = nullptr;
	}

}
