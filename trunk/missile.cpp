
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
missile::missile(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float Speed, int Owner, int Target):object(sPos, sRot, Type), life(15.0), target(Target), difAng(0), lastAng(0), speed(Speed), acceleration(MISSILE_SPEED/3.0), owner(Owner)
{
	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
}
void missile::init()
{
	particleManager.addEmitter(new particle::contrail(id));
	particleManager.addEmitter(new particle::contrailSmall(id));
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
			rotation = slerp(rotation,targetRot,(float)((PI * 2.5 * ms/1000)/angle));
		}

	}
	else if(enemy != NULL && enemy->dead)
	{
		particleManager.addEmitter(new particle::explosionSmoke(position,2.0));
		life = 0.0;
	}
	//////////////////Movement//////////////
	speed+=acceleration*(ms/1000);
	if(speed > MISSILE_SPEED) speed = MISSILE_SPEED;
		position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);

	life-=ms/1000;
	if(life < 0.0 || world.altitude(position) < 0.0)
	{
		awaitingDelete = true;
		meshInstance->setDeleteFlag(true);
		meshInstance = nullptr;
	}

}
void missile::updateFrame(float interpolation) const
{
	if(meshInstance)
	meshInstance->update(lerp(lastPosition,position,interpolation), slerp(lastRotation,rotation, interpolation), !awaitingDelete);
}