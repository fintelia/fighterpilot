
#include "main.h"
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
missile::missile(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float Speed, int dispList, int Owner, int Target):selfControlledObject(sPos, sRot, Type), life(15.0), target(Target), difAng(0), lastAng(0), speed(Speed), acceleration(MISSILE_SPEED/3.0), displayList(dispList), owner(Owner)
{
	
}
void missile::init()
{
	particleManager.addEmitter(new particle::contrail(id));
}
void missile::update(double time, double ms)
{
	/////////////////follow target////////////////////
	nPlane* enemy = (nPlane*)world.objectList[target];
	if(enemy != NULL)
	{
		Vec3f destVec = (enemy->position - position).normalize();
		Vec3f fwd = rotation * Vec3f(0,0,1);
		Angle ang = acosA(destVec.dot(fwd));

		//if( ang > PI/4)
		//{
		//	target = 0;
		//}
		//else 
		if(ang < PI * 0.5 * ms/1000)
		{
			rotation = Quat4f(fwd.cross(destVec), ang);
		}
		else
		{
			Quat4f destRot(fwd.cross(destVec), ang);
			rotation = slerp(rotation,destRot,(float)(PI * 0.5 * ms/1000/ang));
		}
		//difAng=acosA((e-position).normalize().dot(velocity.normalize()));
		//if(difAng.inRange(5.24,1.05))
		//{
			//accel=(accel.normalize()*0.0+(e-position).normalize()*1.0)*accel.magnitude();
			//velocity=(velocity.normalize()*0.96+(e-position).normalize()*0.04)*velocity.magnitude();
			//velocity=(pos-e).normalize()*velocity.magnitude();
		//}
		//if(abs(lastAng-(angle-difAng)/5)>7.5)
		//{
		//	difAng=7.5*(lastAng-(angle-difAng)/5)/abs(lastAng-(angle-difAng)/5);//if turn ang > 7.5 set it to 7.5
		//}
		//if(abs(angle-difAng)<60)
		//{ 
		//	lastAng=(angle-difAng)/5;
		//	angle-=(angle-difAng)/2;
		//	climbAngle-=(climbAngle-asin( (e[1]-y)/e.distance(Vec3f(x,y,z))))/5;	
		//}
	}
	//////////////////Movement//////////////
	speed+=acceleration*(ms/1000);
	if(speed > MISSILE_SPEED) speed = MISSILE_SPEED;
	position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);

	//path.currentPoint(position,rotation);
	////////////////////sparks//////////////////////////
	//static float distLeft=0.0;
	//distLeft += ms/1000;
	//while(distLeft > 0.006)
	//{
	//	world.exaust.insert(position-velocity*distLeft,0,0.75,world.time()-distLeft*1000,random(3000.0,4000.0));
	//	distLeft-=0.006;
	//}

	//for(int i=0;i<1;i++)
	//{
	//	spark tmp(pos.x,pos.y,pos.z,2,0.5,0.5,0.5,5);
	//	exhaust.push_back(tmp);
	//}
	//for(int i=0;i<1;i++)
	//{
	//	spark tmp(pos.x-velocity.x/2,
	//			pos.y-velocity.y/2,
	//			pos.z-velocity.z/2,
	//			2,0.5,0.5,0.5,5);
	//	exhaust.push_back(tmp);
	//}

	//newExaust.insert(pos-velocity*0.005,0,0,10.0);

	//newExaust.insert(pos-velocity*0.01,0,0,10.0);
	//newExaust.insert(pos-velocity*0.015,0,0,10.0);

	life-=ms/1000;
	if(life < 0.0 || world.altitude(position) < 0.0)
		awaitingDelete = true;
}