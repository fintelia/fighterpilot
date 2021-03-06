
#include "game.h"

const float bullet::bulletSpeed = 1000.0;

bullet::bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime): life(1000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*bulletSpeed), owner(Owner)
{

}
bullet::bullet(Vec3f pos,Vec3f vel,int Owner): life(1000), startTime(world.time()), startPos(pos), velocity(vel.normalize()*bulletSpeed), owner(Owner)
{

}

bulletCloud::bulletCloud(): object(BULLET_CLOUD)
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
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&t] (bullet& b) -> bool {return b.startTime + b.life <= t;}), bullets.end());
	// ---
}
void bulletCloud::updateFrame(float interpolation) const
{
	// ---
}