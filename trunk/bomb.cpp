
#include "game.h"

bomb::bomb(bombType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner):object(Type), launchTime(world.time()), velocity(sRot * Vec3f(0,0,speed)-Vec3f(0,30,0)), owner(Owner)
{
	lastPosition = position = sPos;
	lastRotation = rotation = sRot;
	meshInstance = sceneManager.newMeshInstance(objectInfo[type]->mesh, position, rotation);
}
void bomb::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;

	velocity.y -= 9.81 * 20.0 * ms/1000.0;
	position += velocity * ms/1000.0;

	float alt = world.altitude(position);
	if(alt < 0.0)
	{
		position.y -= alt;
		particleManager.addEmitter(new particle::explosion,position,15.0);
		particleManager.addEmitter(new particle::groundExplosionFlash,position,15.0);
		awaitingDelete = true;
		meshInstance.reset();
	}
}
void bomb::updateFrame(float interpolation) const
{
	if(meshInstance)
	meshInstance->update(lerp(lastPosition,position,interpolation), slerp(lastRotation,rotation, interpolation), !awaitingDelete);
}