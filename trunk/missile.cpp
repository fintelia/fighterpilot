#include "main.h"

missile::missile(Vec3f position,Vec3f vel,int Owner, int dispList)
{
	pos=position+Vec3f(0,5,0);
	velocity=vel;
	accel=vel.normalize()*0.01;

	life=15.0;
	owner=Owner;
	//angle=_angle*PI/180.0;
	//climbAngle=_cAngle*PI/180.0;
	target=-1;
	difAng=0;
	lastAng=0;
	//speed=_speed;
	//accel=0;
	displayList=dispList;
}
void missile::findTarget()
{

	Vec3f enemy;
	Angle ang=0.5;
	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	{
		enemy=(*i).second->pos;
		if(acosA( velocity.normalize().dot( (enemy-pos).normalize() )) < ang && /*dist(pos,enemy)<life*speed*0.4 &&*/ (*i).first!=owner && !(*i).second->dead)
		{
			ang=acos( velocity.normalize().dot( (enemy-pos).normalize() ));
			target=(*i).first;
		}
	}
}
bool missile::Update(float ms)
{
	/////////////////follow target////////////////////
	if(target!=-1 && planes.find(target)!=planes.end())
	{
		Vec3f e=planes[target]->pos;

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
	if(velocity.magnitude()>2500) velocity=velocity.normalize()*7500;
	/////////////////////move///////////////////////////
	pos+=velocity*(ms/1000);
	////////////////////sparks//////////////////////////
	static float distLeft=0.0;
	distLeft += ms/1000;
	while(distLeft > 0.009)
	{
		newExaust.insert(pos-velocity*distLeft,0,0,gameTime()-distLeft*1000,3000);
		distLeft-=0.009;
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