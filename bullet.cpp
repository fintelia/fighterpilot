
#include "main.h"

bullet::bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime): life(1000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*BULLET_SPEED), owner(Owner)
{

}
bullet::bullet(Vec3f pos,Vec3f vel,int Owner): life(1000), startTime(world.time()), startPos(pos), velocity(vel.normalize()*BULLET_SPEED), owner(Owner)
{

}