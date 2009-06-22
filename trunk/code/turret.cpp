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
const float size = 64;

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
#include "turret.h"

extern Settings settings;
extern Terrain *terrain;
extern vector<bullet> bullets;
extern vector<missile> missiles;
extern vector<spark> smoke;
extern vector<spark> fire;
extern map<int,planeBase*> planes;


	turret::turret(int _player)
	{
		angle=0;
		Vangle=0;
		player=_player;
		respawn=0;
		cooldown=0;
		spawn();
	}
	void turret::spawn()
	{
		dead=false;
		respawn=0;
		x = rand() % int(size*64);
		z = rand() % int(size*64);
		angle=double(rand()%36000)/100;
		y=terrain->getHeight(x/size,y/size)+1;
	}
	bool turret::Update()
	{
		if(dead)
		{
			if(--respawn<=0)
			{
				spawn();
			}
		}
		return true;
	}

	void turret::die()
	{
		dead=true;
		respawn=50;
	}
//
//class AIplane:public planeBase
//{
//public:
//	int nextShot;//determines how long until the plane can fire again
//	int rBullets;//remaining bullits
//	
//	AIturret(){}
//	AIturret(int _player,vector<bullet> *b,vector<planeBase*> *p)
//	{
//		angle=0;
//		Vangle=0;
//		player=_player;
//		respawn0;
//		cooldown0;
//		bullets=b;
//		planes=p;
//	}
//	void calcMove(){}
//	bool Update()
//	{
//		if(!dead)
//		{
//			calcMove();
//		}
//	}
//	void die()
//	{
//		dead=true;
//	}
//};