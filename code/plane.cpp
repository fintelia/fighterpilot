#include <cstdio>
#include <iostream>
//#include <stdlib.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <fstream>
#include <cassert>
using namespace std;
const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;
const long double DegToRad=PI/180.0;
extern const float size;

#ifdef WIN32
#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4018)
#endif 

#include "vec3f.h"
#include "Terain.h"
#include "imageloader.h"
#include "settings.h"
#include "partical.h"
#include "entity.h"
#include "bullet.h" 
#include "missile.h"
#include "plane.h"

extern Settings settings;
extern Terrain *terrain;
extern vector<bullet> bullets;
extern vector<missile> missiles;
extern vector<spark> fire;
extern vector<spark> smoke;
extern map<int,planeBase*> planes;

void planeBase::updatePos(int value)
{
	if(respawn>=0)
	{
		respawn--;

		acceleration -= 0.025f;
		if(acceleration<2)
			acceleration=2;

		float mX=x/size;
		float mZ=z/size;
		//altitude=terrain->distanceToGround(x/size,y,z/size);
		altitude=y-terrain->getHeight(mX, mZ);
		if(altitude>y-terrain->getHeight(mX+1, mZ))		altitude=y-terrain->getHeight(mX+1, mZ);
		if(altitude>y-terrain->getHeight(mX, mZ+1))		altitude=y-terrain->getHeight(mX, mZ+1);
		if(altitude>y-terrain->getHeight(mX+1, mZ+1))		altitude=y-terrain->getHeight(mX+1, mZ+1);
		if(altitude<3.0||hitGround)
		{
			y-=altitude-3;
			hitGround=true;
		}
		else
		{
			x -= (-sin(float(angle * PI  / 180)) * acceleration);
			z -= (-cos(float(angle * PI / 180)) * acceleration);
			angle -= turn/110;
			climb-=0.1;
			y += climb;
		}

		if(x > terrain->length() * size)	{x = terrain->length()*size;angle+=180;}
		if(x < 0)							{x = 0;angle+=180;}
		if(z > terrain->width() * size)		{z = terrain->width()*size;angle+=180;}
		if(z < 0)							{z = 0;angle+=180;}
		if(angle > 360)						angle -= 360;
		if(angle < 0)						angle += 360;
		if(y>250)							y=250;

		for(int i=0;i<4;i++)
		{
			spark tmp(x,y,z,45,0,0,0,5);
			fire.push_back(tmp);
		}
		for(int i=0;i<3;i++)
		{
			spark tmp(x,y,z,45,0.7,0,0,5);
			fire.push_back(tmp);
			tmp.change(x,y,z,45,0.8,0,0,5);
			fire.push_back(tmp);
			tmp.change(x,y,z,45,0.9,0,0,5);
			fire.push_back(tmp);

			tmp.change(x,y,z,45,1,0.5,0,5);
			fire.push_back(tmp);
			tmp.change(x,y,z,45,1,0.6,0,5);
			fire.push_back(tmp);
			tmp.change(x,y,z,45,1,0.7,0,5);
			fire.push_back(tmp);
		}
	}
	else
	{
		float t=value/100.0;//acounts for time
		int mX=x/size;
		int mZ=z/size;
		//altitude=terrain->distanceToGround(x/size,y,z/size);
		altitude=y-terrain->getHeight(mX, mZ);
		if(altitude>y-terrain->getHeight(mX+1, mZ))		altitude=y-terrain->getHeight(mX+1, mZ);
		if(altitude>y-terrain->getHeight(mX, mZ+1))		altitude=y-terrain->getHeight(mX, mZ+1);
		if(altitude>y-terrain->getHeight(mX+1, mZ+1))		altitude=y-terrain->getHeight(mX+1, mZ+1);
		if(altitude<3.0){die();y-=altitude-3;hitGround=true;}

		if(--cooldown<0)
			cooldown=0;
		if(--mCooldown<0)
			mCooldown=0;

		angle -= turn/90*t+sin(turn*DegToRad)*climb*t;//second term acounts for climb
		x += sin(float(angle *DegToRad))*acceleration*4.5*t;
		z += cos(float(angle *DegToRad))*acceleration*4.5*t;
		y += cos(turn*DegToRad)*climb*t;

		if(x > terrain->length() * size)	{x = terrain->length()*size;angle+=180;}
		if(x < 0)							{x = 0;angle+=180;}
		if(z > terrain->width() * size)		{z = terrain->width()*size;angle+=180;}
		if(z < 0)							{z = 0;angle+=180;}
		if(angle > 360)						angle -= 360;
		if(angle < 0)						angle += 360;
		if(y>250)							y=250;

		Vec3f v(sin(float(angle * PI / 180)),climb,cos(float(angle * PI / 180)));
		Vec3f o(sin(float((angle+90) * PI / 180)),cos((turn+270)/180*3.1415926535),cos(float((angle+90) * PI / 180)));
		v=v.normalize();
		o=o.normalize();
		normal = v.cross(o);
		Level((float)value/33);
	}
}

void planeBase::Accelerate(float value)
{
	if(controled) return;
	acceleration += 0.05f*value;
	if(acceleration > 7.0) 
		acceleration = 7.0f;
}
void planeBase::Brake(float value)
{
	if(controled) return;
	acceleration -= 0.05f*value;
	if(acceleration < 2.0) 
		acceleration = 2.0f;
}

void planeBase::Turn_Right(float value)
{
	if(controled) return;
	turn += 0.6f*value;
	if(turn > 60)
		turn=60.0f;
}
void planeBase::Turn_Left(float value)
{
	if(controled) return;
	turn -= 0.6f*value;
	if(turn < -60)
		turn=-60.0f;
}

void planeBase::Climb(float value)
{
	if(controled) return;
	climb += 0.015f*value;
	if(climb > 1)
		climb = 1.0f;
}
void planeBase::Dive(float value)
{
	if(controled) return;
	climb -= 0.015f*value;
	if(climb < -1.3)
		climb = -1.3f;
}
void planeBase::Level(float value)
{
	if(climb < 0)
	{
		climb += 0.01f * fabs(climb)*value;
	}
	else if(climb > 0)
	{
		climb -= 0.01f * climb*value;
	}

	if(turn < 0.1)
	{
		turn += 0.01f * fabs(turn)*value;
	}
	if(turn > 0.1)
	{
		turn -= 0.01f * turn*value;
	}
	acceleration -= 0.025f;
	if(acceleration<2)
		acceleration=2;
}
void planeBase::die()
{
	dead =true;
	if(respawn==-1)
		respawn=200;
}
void planeBase::Shoot()
{
	if(cooldown>0)
		return;
	Vec3f self[2];//current,future
	self[0][0]=x;self[0][1]=y;self[0][2]=z;
	self[1][0]=(float)sin(angle * PI / 180);
	self[1][1]=climb;
	self[1][2]=(float)cos(angle * PI / 180);
	self[1]=self[1].normalize();

	Vec3f enemy;
	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	{
		enemy[0]=(*i).second->x;
		enemy[1]=(*i).second->y;
		enemy[2]=(*i).second->z;
		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.1 && dist(self[0],enemy) && (*i).first!=player)
		{
			//enemies[i]-=self[0];
			bullet tmp(x,y,z,(enemy-self[0]).normalize()[0],(enemy-self[0]).normalize()[1],(enemy-self[0]).normalize()[2],player);
			bullets.push_back(tmp);
			cooldown=2;
			return;
		}
	}
	bullet tmp(x,y,z,(float)sin(angle/180*PI)*5,((float)climb/1.0f)*5,(float)cos(angle/180*PI)*5,player);
	bullets.push_back(tmp);
	cooldown=2;
}
void planeBase::ShootMissile()
{
	if(mCooldown>0)
		return;
	missile tmp1(x+sin(float((angle+90) * PI / 180))*25,y+cos((turn+270)/180*3.1415926535)*25-10,z+cos(float((angle+90) * PI / 180))*25,player,angle,sin(climb/acceleration),&smoke,(map<int,entity*>*)(&planes));
	missiles.push_back(tmp1);
	missile tmp2(x-sin(float((angle+90) * PI / 180))*25,y-cos((turn+270)/180*3.1415926535)*25-10,z-cos(float((angle+90) * PI / 180))*25,player,angle,sin(climb/acceleration),&smoke,(map<int,entity*>*)(&planes));
	missiles.push_back(tmp2);
	mCooldown=25;
}



//<>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <><
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><>

void plane::die()
{
	dead =true;
	if(respawn==-1)
		respawn=50;
}
bool plane::Update(float value)
{
	updatePos(value);
	if(dead && respawn<=0)
	{
		if(settings.ON_HIT==RESPAWN)
		{
			spawn();
		}
		else if(settings.ON_HIT==RESTART)
		{
			//not yet handled
		}
		else if(settings.ON_HIT==DIE)
		{
			//return false;
		}
	}

	return true;
}

void plane::spawn()
{
	//srand ( ((time(NULL))*5 + rand() + 347));
	x = rand() % int(size*64);
	z = rand() % int(size*64);
	float h=terrain->getHeight(x/(float)size, z/(float)size);
	//float h=terrain->distanceToGround(x/(float)size,0,z/(float)size);
	y = 35-h;
	angle = atan2((x-size*32),(z-size*32))*180/3.141592653589+180;
	acceleration=2;
	climb = 0;
	turn = 0;

	dead = false;
	respawn=-1;
	controled=false;
	maneuver=0;
	cooldown=0;
	mCooldown;
	hitGround=false;
	//Update();
}
plane::plane(int _player,int _team)
{
	player =_player;
	dead=false;
	spawn();
	id=player;
	Update(0);
	team=_team;
}
plane::plane(){cout << "error!!!!!!!!!";}

//<>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <><
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><>

AIplane::AIplane(){cout << "error!!!!";}
AIplane::AIplane(int _player,int _team)
{
	player =_player;
	dead=false;
	spawn();
	id=player;
	Update(0);
	team=_team;
}

void AIplane::spawn()
{
	x = rand() % int(size*64);
	z = rand() % int(size*64);
	//float mX=(float)(int)x-int(x)%(int)size;
	//float mZ=(float)(int)z-int(z)%(int)size;
	//float h=terrain->getHeight(mX, mZ) * 10;
	//y = h +35;
	float h=terrain->getHeight(x/(float)size, z/(float)size);
	//float h=terrain->distanceToGround(x/(float)size,0,z/(float)size);
	y = 35-h;
	angle = atan2((x-size*32),(z-size*32))*180/3.141592653589+180;
	acceleration=2;
	climb = 0;
	turn = 0;
	dead = false;
	respawn=-1;
	controled=false;
	maneuver=0;
	cooldown=0;
	mCooldown;
	hitGround=false;
}
void AIplane::calcMove(int value)
{
	Vec3f self[2];//current,future
	self[0][0]=x;self[0][1]=y;self[0][2]=z;
	self[1][0]=(float)sin(angle * PI / 180);
	self[1][1]=climb;
	self[1][2]=(float)cos(angle * PI / 180);
	self[1]=self[1].normalize();
	Vec3f enemy;
	if(cooldown==0)
	{
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			enemy[0]=(*i).second->x;
			enemy[1]=(*i).second->y;
			enemy[2]=(*i).second->z;
			if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.5 && dist(self[0],enemy)<2000 && (*i).first!=player)
			{
				Shoot();
				break;
			}
		}
	}
	int nearest=-1;
	float ang=999;
	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	{
		enemy[0]=(*i).second->x;
		enemy[1]=(*i).second->y;
		enemy[2]=(*i).second->z;
		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < ang && dist(self[0],enemy)<3000 && (*i).second->team!=team)
		{
			ang=acos( self[1].dot( (enemy-self[0]).normalize() ));
			nearest=(*i).first;
		}
	}
	
	if(nearest>=0)
	{
		if(atan2(y-planes[nearest]->y,x-planes[nearest]->x)<0)
		{
			Turn_Left((float)value/33.0);
		}
		else if(atan2(y-planes[nearest]->y,x-planes[nearest]->x)<0)
		{
			Turn_Left((float)value/33.0);
		}
		if(altitude>=30 && planes[nearest]->y<y)
		{
			Dive((float)value/33.0);
		}
		else if(planes[nearest]->y>y || altitude<25)
		{
			Climb((float)value/33.0);
		}
		if(acceleration<planes[nearest]->acceleration+1)
		{
			Accelerate((float)value/33.0);
		}
		else if(acceleration>planes[nearest]->acceleration+2)
		{
			Accelerate((float)value/33.0);
		}
	}
	else
	{

		if(altitude<30)
		{
			Climb((float)value/33.0);
		}
		if(altitude>=25 && rand()%2>1)
		{
			Dive((float)value/33.0);
		}
		if(acceleration<4+rand()%10/5-1)
		{
			Accelerate((float)value/33.0);
		}
	}

}
bool AIplane::Update(float value)
{
	updatePos(value);
	if(dead && respawn<=0)
	{
		if(settings.ON_AI_HIT==RESPAWN)
		{
			spawn();
		}
		else if(settings.ON_AI_HIT==RESTART)
		{
			//not yet (or possibly ever) handled
		}
		else if(settings.ON_AI_HIT==DIE)
		{
			return false;
		}
	}

	if(!dead)
		calcMove(value);
    if(--nextShot<0)
        nextShot=0;
	return true;
}