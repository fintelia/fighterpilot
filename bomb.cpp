
#include "engine.h"
#include "game.h"

bomb::bomb(bombType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner):object(Type), launchTime(world->time()), velocity(sRot * Vec3f(0,0,speed)-Vec3f(0,30,0)), owner(Owner)
{
	lastPosition = position = sPos;
	lastRotation = rotation = sRot;
	meshInstance = objectInfo[type]->newMeshInstance(Mat4f(rotation, position));
	collisionInstance = objectInfo[type]->newCollisionInstance(Mat4f(rotation, position));
}
void bomb::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;

	velocity.y -= 9.81 * 20.0 * ms * 0.001;
	position += velocity * (float)ms * 0.001f;

	float alt = world->altitude(position);
	if(alt < 0.0)
	{
		position.y -= alt;
		particleManager.addEmitter(new particle::explosion,position,15.0);
		particleManager.addEmitter(new particle::groundExplosionFlash,position,15.0);
		awaitingDelete = true;
		meshInstance.reset();
	}

	if(collisionInstance)
	{
		collisionInstance->update(Mat4f(rotation,position), Mat4f(lastRotation,lastPosition));
	}
}
void bomb::updateFrame(float interpolation) const
{
	if(meshInstance)
		meshInstance->update(Mat4f(slerp(lastRotation,rotation, interpolation), lerp(lastPosition,position,interpolation)), !awaitingDelete);
}
