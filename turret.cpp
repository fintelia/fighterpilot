//#include "main.h"
//
//turret::turret(int Player)
//{
//	angle=0;
//	Vangle=0;
//	player=Player;
//	respawn=0;
//	cooldown=0;
//	spawn();
//}
//void turret::spawn()
//{
//	dead=false;
//	respawn=0;
//	//do
//	{
//		pos.x = rand() % int(size*64);
//		pos.z = rand() % int(size*64);
//		pos.y = settings.SEA_LEVEL+25;//terrain->getInterpolatedHeight(pos.x/size,pos.z);
//	}
//	//while(pos.y<settings.SEA_LEVEL);
//	angle=double(rand()%36000)/100;
//
//}
//bool turret::Update(float ms)
//{
//
//	//cooldown-=ms;
//	//if(cooldown<0)
//	//{
//	//	int t=0;
//	//	float d;
//	//	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
//	//	{
//	//		if(t==0 || i->second->pos.distanceSquared(pos)<d)
//	//		{
//	//			t=i->first;
//	//			d=i->second->pos.distanceSquared(pos);
//	//		}
//	//	}
//	//	if(t!=0)
//	//	{
//	//		cooldown+=5000;
//	//		missiles.push_back(missile(pos,(planes[t]->pos-pos).normalize(),-1));
//	//	}
//	//}
//	return true;
//}
//
//void turret::die()
//{
//	dead=true;
//	respawn=50;
//}
////
////class AIplane:public planeBase
////{
////public:
////	int nextShot;//determines how long until the plane can fire again
////	int rBullets;//remaining bullits
////	
////	AIturret(){}
////	AIturret(int _player,vector<bullet> *b,vector<planeBase*> *p)
////	{
////		angle=0;
////		Vangle=0;
////		player=_player;
////		respawn0;
////		cooldown0;
////		bullets=b;
////		planes=p;
////	}
////	void calcMove(){}
////	bool Update()
////	{
////		if(!dead)
////		{
////			calcMove();
////		}
////	}
////	void die()
////	{
////		dead=true;
////	}
////};