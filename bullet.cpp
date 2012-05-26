
#include "game.h"

bullet::bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime): life(1000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*1000), owner(Owner)
{

}
bullet::bullet(Vec3f pos,Vec3f vel,int Owner): life(1000), startTime(world.time()), startPos(pos), velocity(vel.normalize()*1000), owner(Owner)
{

}

bulletCloud::bulletCloud(): object(Vec3f(), Quat4f(), BULLET_CLOUD)
{
	
}

void bulletCloud::addBullet(Vec3f pos,Vec3f vel,int Owner, double StartTime)
{
	bullets.push_back(bullet(pos, vel, Owner, StartTime));
}
void bulletCloud::addBullet(Vec3f pos,Vec3f vel,int Owner)
{
	bullets.push_back(bullet(pos, vel, Owner));
}
void bulletCloud::updateSimulation(double time, double ms)
{
	double t = world.time();
	bullets.erase(remove_if(bullets.begin(), bullets.end(), [&t] (bullet& b) -> bool {return b.startTime + b.life <= t;}), bullets.end());
	// ---
}
void bulletCloud::updateFrame(float interpolation) const
{
	// ---
}