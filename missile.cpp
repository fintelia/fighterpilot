#include "main.h"

void missile::findTarget()
{
	Vec3f enemy;
	Angle ang=0.5;

	const map<objId,planeBase*>& planes = world.planes();

	for(map<objId,planeBase*>::const_iterator i = planes.begin(); i != planes.end();i++)
	{
		enemy=(*i).second->pos;
		if(acosA( velocity.normalize().dot( (enemy-pos).normalize() )) < ang && /*dist(pos,enemy)<life*speed*0.4 &&*/ (*i).second->team!=team && !(*i).second->dead)
		{
			ang=acos( velocity.normalize().dot( (enemy-pos).normalize() ));
			target=(*i).first;
		}
	}
}
bool missile::Update(float ms)
{
	/////////////////follow target////////////////////
	planeBase* enemy = (planeBase*)world.objectList[target];
	if(enemy != NULL)
	{
		Vec3f e=enemy->pos;

		difAng=acosA((e-pos).normalize().dot(velocity.normalize()));
		if(difAng.inRange(5.24,1.05))
		{
			accel=(accel.normalize()*0.0+(e-pos).normalize()*1.0)*accel.magnitude();
			//velocity=(pos-e).normalize()*velocity.magnitude();
		}
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
		else
		{
			findTarget();
		}
	}
	else
	{
		findTarget();
	}
	//////////////////Accelerate//////////////
	accel=accel.normalize()*(accel.magnitude()+ms*70);
	
	velocity+=accel*(ms/1000);
	if(velocity.magnitude()>MISSILE_SPEED) velocity=velocity.normalize()*MISSILE_SPEED;
	/////////////////////move///////////////////////////
	pos+=velocity*(ms/1000);
	////////////////////sparks//////////////////////////
	static float distLeft=0.0;
	distLeft += ms/1000;
	while(distLeft > 0.006)
	{
		world.exaust.insert(pos-velocity*distLeft,0,0,world.time()-distLeft*1000,3000);
		distLeft-=0.006;
	}
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
	return life > 0.0;
}