
#include "main.h"

bomb::bomb(bombType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner):selfControlledObject(sPos, sRot, Type), launchTime(world.time()), velocity(sRot * Vec3f(0,0,speed)-Vec3f(0,30,0)), owner(Owner)
{
	
}
void bomb::update(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;

	position = startPos + velocity * (time-launchTime)/1000 + Vec3f(0,-9.81*20.0,0) * (time-launchTime) * (time-launchTime) / 1000000;

	float alt = world.altitude(position);
	if(alt < 0.0)
	{
		particleManager.addEmitter(new particle::explosion(position-Vec3f(0,alt,0),4.0));
		awaitingDelete = true;
	}
}