
#include "game.h"

bomb::bomb(bombType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner):object(sPos, sRot, Type), launchTime(world.time()), velocity(sRot * Vec3f(0,0,speed)-Vec3f(0,30,0)), owner(Owner)
{
	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
}
void bomb::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;

	position = startPos + velocity * (time-launchTime)/1000 + Vec3f(0,-9.81*20.0,0) * (time-launchTime) * (time-launchTime) / 1000000;

	float alt = world.altitude(position);
	if(alt < 0.0)
	{
		particleManager.addEmitter(new particle::explosion(position-Vec3f(0,alt,0),4.0));
		awaitingDelete = true;
		meshInstance->setDeleteFlag(true);
		meshInstance = nullptr;
	}
}
void bomb::updateFrame(float interpolation) const
{
	if(meshInstance)
	meshInstance->update(lerp(lastPosition,position,interpolation), slerp(lastRotation,rotation, interpolation), !awaitingDelete);
}