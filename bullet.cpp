
#include "main.h"

bullet::bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime): life(100000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*5000), owner(Owner)
{

}
bullet::bullet(Vec3f pos,Vec3f vel,int Owner): life(100000), startTime(world.time()), startPos(pos), velocity(vel.normalize()*5000), owner(Owner)
{

}
bool bullet::update(float length)
{
	return life+startTime-world.time() >= 0;
}