#include <cstdio>
#include <iostream>
//#include <stdlib.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
using namespace std;
const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;
const float size = 64;
#include "vec3f.h"
#include "Terain.h"
#include "imageloader.h"
#include "partical.h"
#include "entity.h"
#include "bullet.h" 
#include "missile.h"
#include "plane.h"



void plane::Accelerate()
{
	if(controled) return;
	acceleration += 0.07f;
	if(acceleration > 7.0) 
		acceleration = 7.0f;
}
void plane::Brake()
{
	if(controled) return;
	acceleration -= 0.07f;
	if(acceleration < 2.0) 
		acceleration = 2.0f;
}

void plane::Turn_Right()
{
	if(controled) return;
	turn -= 0.5f;
	if(turn < -40)
		turn=-40.0f;
}
void plane::Turn_Left()
{
	if(controled) return;
	turn += 0.5f;
	if(turn > 40)
		turn=40.0f;
}

void plane::Climb()
{
	if(controled) return;
	climb += 0.02f;
	if(climb > 1)
		climb = 1.0f;
}
void plane::Dive()
{
	if(controled) return;
	climb -= 0.02f;
	if(climb < -1)
		climb = -1.0f;
}
void plane::Level()
{
	if(climb < 0)
	{
		climb += 0.01f * fabs(climb);
	}
	else if(climb > 0)
	{
		climb -= 0.01f * climb;
	}

	if(turn < 0.1)
	{
		turn += 0.01f * fabs(turn);
	}
	if(turn > 0.1)
	{
		turn -= 0.01f * turn;
	}

}

void plane::Shoot()
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
	for(int i=0;i<(signed int)planes->size();i++)
	{
		enemy[0]=planes->at(i)->x;
		enemy[1]=planes->at(i)->y;
		enemy[2]=planes->at(i)->z;
		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.1 && d(self[0],enemy) && i!=player)
		{
			//enemies[i]-=self[0];
			bullet tmp(x,y,z,(enemy-self[0]).normalize()[0],(enemy-self[0]).normalize()[1],(enemy-self[0]).normalize()[2],player);
			bullets->push_back(tmp);
			cooldown=3;
			return;
		}
	}
	bullet tmp(x,y,z,(float)sin(angle/180*PI)*5,((float)climb/1.0f)*5,(float)cos(angle/180*PI)*5,player);
	bullets->push_back(tmp);
	cooldown=3;
}
void plane::ShootMissile()
{
	if(mCooldown>0)
		return;
	//	Vec3f self[2];//current,future
	//self[0][0]=x;self[0][1]=y;self[0][2]=z;
	//self[1][0]=(float)sin(angle * PI / 180);
	//self[1][1]=climb;
	//self[1][2]=(float)cos(angle * PI / 180);
	//self[1]=self[1].normalize();

	//Vec3f enemy;
	//for(int i=0;i<(signed int)planes->size();i++)
	//{
	//	enemy[0]=planes->at(i)->x;
	//	enemy[1]=planes->at(i)->y;
	//	enemy[2]=planes->at(i)->z;
	//	if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.3 && d(self[0],enemy) && i!=player)
	//	{
	//		//enemies[i]-=self[0];
	//		missile tmp1(x+sin(float((angle+90) * PI / 180))*25,y+cos((turn+270)/180*3.1415926535)*25-10,z+cos(float((angle+90) * PI / 180))*25,(float)sin(angle/180*PI)*3,((float)climb/1.0f)*3,(float)cos(angle/180*PI)*3,player,angle,climb,particles,(vector<entity*>)planes);
	//		missiles->push_back(tmp1);
	//		missile tmp2(x-sin(float((angle+90) * PI / 180))*25,y-cos((turn+270)/180*3.1415926535)*25-10,z-cos(float((angle+90) * PI / 180))*25,(float)sin(angle/180*PI)*3,((float)climb/1.0f)*3,(float)cos(angle/180*PI)*3,player,angle,climb,particles,(vector<entity*>)planes);
	//		missiles->push_back(tmp2);
	//		mCooldown=25;
	//		return;
	//	}
	//}
	missile tmp1(x+sin(float((angle+90) * PI / 180))*25,y+cos((turn+270)/180*3.1415926535)*25-10,z+cos(float((angle+90) * PI / 180))*25,player,angle,sin(climb/acceleration),particles,(vector<entity*>*)planes);
	missiles->push_back(tmp1);
	missile tmp2(x-sin(float((angle+90) * PI / 180))*25,y-cos((turn+270)/180*3.1415926535)*25-10,z-cos(float((angle+90) * PI / 180))*25,player,angle,sin(climb/acceleration),particles,(vector<entity*>*)planes);
	missiles->push_back(tmp2);
	mCooldown=25;
}

bool plane::Update()
{
	if(respawn>=0)
	{
		respawn--;
		if(respawn<=0)
			spawn();
	}
	else
	{
		//int lx = int (x/size);// if(lx > 62){lx=62;}if(lx < 1){lx=1;} //prevent index out of bounds
		//int lz = int (z/size);// if(lz > 62){lz=62;}if(lz < 1){lz=1;} //prevent index out of bounds
		//if     (terrain->getHeight(lx  ,lz  )*10 >= y-5)
		//{ die();} 
		//else if(terrain->getHeight(lx+1,lz+1)*10 >= y-5)
		//{ die();} 
		//else if(terrain->getHeight(lx  ,lz+1)*10 >= y-5)
		//{ die();} 
		//else if(terrain->getHeight(lx+1,lz  )*10 >= y-5)
		//{ die();}
		float mX=(float)(int)x-int(x)%(int)size;
		float mZ=(float)(int)z-int(z)%(int)size;
		//altitude=terrain->distanceToGround(x/size,y,z/size);
		altitude=y-terrain->getHeight(mX, mZ);
		if(altitude<0.0){die();}
		
		if(--cooldown<0)
			cooldown=0;
		if(--mCooldown<0)
			mCooldown=0;
		acceleration -= 0.025f;
		if(acceleration<2)
			acceleration=2;
		x -= (-sin(float(angle * PI / 180)) * acceleration);
		z -= (-cos(float(angle * PI / 180)) * acceleration);

		if(x > 64 * size){x = 64*size;}
		if(x < 0){x = 0;}
		if(z > 64 * size){z = 64*size;}
		if(z < 0){z = 0;}
		y += climb;
		if(y>135)
			y=135;

		angle -= turn/110;
		if(angle > 360)
			angle -= 360;
		if(angle < 0)
			angle += 360;

		Vec3f v(sin(float(angle * PI / 180)),climb,cos(float(angle * PI / 180)));
		Vec3f o(sin(float((angle+90) * PI / 180)),cos((turn+270)/180*3.1415926535),cos(float((angle+90) * PI / 180)));
		v=v.normalize();
		o=o.normalize();
		normal = v.cross(o);
		Level();
	}
	return true;
}

void plane::spawn()
{
	//srand ( ((time(NULL))*5 + rand() + 347));
	x = rand() % int(size*64);
	z = rand() % int(size*64);
	float mX=(float)(int)x-int(x)%(int)size;
	float mZ=(float)(int)z-int(z)%(int)size;
	float h=terrain->getHeight(mX, mZ) * 10;
	//float h=terrain->distanceToGround(x/(float)size,0,z/(float)size);
	y = 35-h;
	angle = atan2((x-size*32),(z-size*32))*180/3.141592653589+180;
	acceleration=2;
	climb = 0;
	turn = 0;

	//cout << "x    : " << x << endl;
	//cout << "y    : " << y << endl;
	//cout << "z    : " << z << endl;
	//cout << "angle: " << angle << endl;
	//cout << "accel: " << acceleration << endl;
	//cout << "climb: " << climb << endl;
	//cout << "turn : " << turn << endl << endl;
	hit = false;
	respawn=-1;
	controled=false;
	maneuver=0;
	cooldown=0;
	mCooldown;
	//Update();
}
plane::plane(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<planeBase*> *p)
{
	terrain=T;
	player =_player;
	spawn();
	bullets = b;
	missiles = m;
	particles = s;
	planes=p;
	id=player;
}
plane::plane(){}
void plane::die()
{
	hit =true;
	if(respawn==-1)
		respawn=50;
}


//<>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <><
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><>
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//<>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <>< <><
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><> ><>


void AIplane::Accelerate()
{
	if(controled) return;
	acceleration += 0.07f;
	if(acceleration > 7.0) 
		acceleration = 7.0f;
}
void AIplane::Brake()
{
	if(controled) return;
	acceleration -= 0.07f;
	if(acceleration < 2.0) 
		acceleration = 2.0f;
}

void AIplane::Turn_Right()
{
	if(controled) return;
	turn -= 0.5f;
	if(turn < -40)
		turn=-40.0f;
}
void AIplane::Turn_Left()
{
	if(controled) return;
	turn += 0.5f;
	if(turn > 40)
		turn=40.0f;
}

void AIplane::Climb()
{
	if(controled) return;
	climb += 0.02f;
	if(climb > 1)
		climb = 1.0f;
}
void AIplane::Dive()
{
	if(controled) return;
	climb -= 0.02f;
	if(climb < -1)
		climb = -1.0f;
}
void AIplane::Level()
{
	if(climb < 0)
	{
		climb += 0.01f * fabs(climb);
	}
	else if(climb > 0)
	{
		climb -= 0.01f * climb;
	}

	if(turn < 0.1)
	{
		turn += 0.01f * fabs(turn);
	}
	if(turn > 0.1)
	{
		turn -= 0.01f * turn;
	}

}

void AIplane::Shoot()
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
	for(int i=0;i<(signed int)planes->size();i++)
	{
		enemy[0]=planes->at(i)->x;
		enemy[1]=planes->at(i)->y;
		enemy[2]=planes->at(i)->z;
		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.1 && d(self[0],enemy) && i!=player)
		{
			//enemies[i]-=self[0];
			bullet tmp(x,y,z,(enemy-self[0]).normalize()[0],(enemy-self[0]).normalize()[1],(enemy-self[0]).normalize()[2],player);
			bullets->push_back(tmp);
			return;
		}
	}
	bullet tmp(x,y,z,(float)sin(angle/180*PI)*5,((float)climb/1.0f)*5,(float)cos(angle/180*PI)*5,player);
	bullets->push_back(tmp);
	cooldown=3;
}
void AIplane::ShootMissile()
{
	if(mCooldown>0)
		return;
	missile tmp1(x+sin(float((angle+90) * PI / 180))*25,y+cos((turn+270)/180*3.1415926535)*25-10,z+cos(float((angle+90) * PI / 180))*25,player,angle,tan(climb),particles,(vector<entity*>*)planes);
	missiles->push_back(tmp1);
	missile tmp2(x-sin(float((angle+90) * PI / 180))*25,y-cos((turn+270)/180*3.1415926535)*25-10,z-cos(float((angle+90) * PI / 180))*25,player,angle,tan(climb),particles,(vector<entity*>*)planes);
	missiles->push_back(tmp2);
	mCooldown=25;
}

AIplane::AIplane(){cout << "error!!!!!!!!!";}
AIplane::AIplane(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<planeBase*> *p)
{
	terrain=T;
	player =_player;
	spawn();
	bullets = b;
	missiles = m;
	particles = s;
	planes=p;
	id=player;
}

void AIplane::spawn()
{
	x = rand() % int(size*64);
	z = rand() % int(size*64);
	//float mX=(float)(int)x-int(x)%(int)size;
	//float mZ=(float)(int)z-int(z)%(int)size;
	//float h=terrain->getHeight(mX, mZ) * 10;
	//y = h +35;
	float h=terrain->distanceToGround(x/size,0,z/size)*10;
	y = 35-h;
	angle = atan2((x-size*32),(z-size*32))*180/3.141592653589+180;
	acceleration=2;
	climb = 0;
	turn = 0;
	hit = false;
	respawn=-1;
	controled=false;
	maneuver=0;
	cooldown=0;
	mCooldown;
}
void AIplane::calcMove()
{
	int i;
	Vec3f self[2];//current,future
	self[0][0]=x;self[0][1]=y;self[0][2]=z;
	self[1][0]=(float)sin(angle * PI / 180);
	self[1][1]=climb;
	self[1][2]=(float)cos(angle * PI / 180);
	self[1]=self[1].normalize();
	Vec3f enemy;
	if(cooldown==0)
	{
		for(i=0;i<(signed int)planes->size();i++)
		{
			enemy[0]=planes->at(i)->x;
			enemy[1]=planes->at(i)->y;
			enemy[2]=planes->at(i)->z;
			if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.5 && d(self[0],enemy)<2000 && i!=player)
			{
				Shoot();
				break;
			}
		}
	}
	int nearest=-1;
	float ang=999;
	for(i=0;i<(signed int)planes->size();i++)
	{
		enemy[0]=planes->at(i)->x;
		enemy[1]=planes->at(i)->y;
		enemy[2]=planes->at(i)->z;
		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < ang && d(self[0],enemy)<3000 && i!=player)
		{
			ang=acos( self[1].dot( (enemy-self[0]).normalize() ));
			nearest=i;
		}
	}
	
	if(nearest>=0)
	{
		if(atan2(y-planes->at(nearest)->y,x-planes->at(nearest)->x)<0)
		{
			Turn_Left();
		}
		else if(atan2(y-planes->at(nearest)->y,x-planes->at(nearest)->x)<0)
		{
			Turn_Left();
		}

		if(altitude>=5 && planes->at(nearest)->y<y)
		{
			Dive();
		}
		else if(planes->at(nearest)->y>y || altitude<5)
		{
			Climb();
		}
	}
	if(acceleration<7)
	{
		Accelerate();
	}
}
bool AIplane::Update()
{
	if(respawn>=0)
	{
		respawn--;
		if(respawn<=0)
			spawn();
		return true;
	}
	calcMove();
	float mX=(float)(int)x-int(x)%(int)size;
	float mZ=(float)(int)z-int(z)%(int)size;//terrain->distanceToGround(x/size,y/10,z/size)*10;
	altitude=y-terrain->getHeight(mX, mZ) * 10;
	if(altitude<0.0){die();}
	
	if(--cooldown<0)
		cooldown=0;
	if(--mCooldown<0)
		mCooldown=0;
	acceleration -= 0.025f;
	if(acceleration<2)
		acceleration=2;
	x -= (-sin(float(angle * PI / 180)) * acceleration);
	z -= (-cos(float(angle * PI / 180)) * acceleration);

	if(x > 64 * size){x = 64*size;}
	if(x < 0){x = 0;}
	if(z > 64 * size){z = 64*size;}
	if(z < 0){z = 0;}
	y += climb;
	if(y>135)
		y=135;

	angle -= turn/110;
	if(angle > 360)
		angle -= 360;
	if(angle < 0)
		angle += 360;

	Vec3f v(sin(float(angle * PI / 180)),climb,cos(float(angle * PI / 180)));
	Vec3f o(sin(float((angle+90) * PI / 180)),cos((turn+270)/180*3.1415926535),cos(float((angle+90) * PI / 180)));
	v=v.normalize();
	o=o.normalize();
	normal = v.cross(o);
	Level();
	return true;
}
void AIplane::die()
{
	hit =true;
	if(respawn==-1)
		respawn=50;
}