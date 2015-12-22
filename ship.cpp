
#include "engine.h"
#include "game.h"

ship::ship(Vec3f sPos, Quat4f sRot, objectType Type, int Team):object(Type, Team), health(100.0)
{
	lastPosition = position = sPos;
	lastRotation = rotation = sRot;
	meshInstance = objectInfo[type]->newMeshInstance(Mat4f(rotation, position));
	collisionInstance = objectInfo[type]->newCollisionInstance(Mat4f(rotation, position));

	dead = false;
	health = 100.0;

	position = lastPosition = Vec3f(position.x,10,position.z);
	rotation = Quat4f(Vec3f(0,0,-1));
}
void ship::updateFrame(float interpolation) const
{
	if(meshInstance)
		meshInstance->update(Mat4f(slerp(lastRotation,rotation, interpolation), lerp(lastPosition,position,interpolation)));
}
void ship::updateSimulation(double time, double ms)
{
	lastPosition = position;
	position += rotation * Vec3f(0,0,100) * ms/1000.0;

	if(collisionInstance)
	{
		collisionInstance->update(Mat4f(rotation,position), Mat4f(lastRotation,lastPosition));
	}
}
void ship::die()
{
	dead = true;
	particleManager.addEmitter(new particle::explosion(),id);
	particleManager.addEmitter(new particle::explosionFlash(),id);
	particleManager.addEmitter(new particle::blackSmoke(),id);
}
void ship::loseHealth(float healthLoss)
{
	health-=healthLoss;
	if(health<0)
	{
		health=0;
		die();
	}
}
