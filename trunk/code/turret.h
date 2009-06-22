
class turretBase:public entity
{
private:
public:
	float angle;
	float Vangle;
	int player;
	int respawn;
	int cooldown;
	vector<bullet> *bullets;
	vector<planeBase*> *planes;

	virtual void Turn_Right()
	{
		angle-=0.2;
		if(angle<0)
			angle+=360;
	}
	virtual void Turn_Left()
	{
		angle+=0.2;
		if(angle>=360)
			angle-=360;
	}
	virtual void Up()
	{
		Vangle+=0.2;
		if(Vangle>=70)
			Vangle=70;
	}
	virtual void Down()
	{
		Vangle-=0.2;
		if(Vangle<0)
			Vangle=0;
	}
	//virtual void Shoot()
	//{
	//	if(cooldown>0)
	//		return;
	//	Vec3f self[2];//current,future
	//	self[0][0]=x;self[0][1]=y;self[0][2]=z;
	//	self[1][0]=(float)sin(angle * PI / 180);
	//	self[1][1]=0;
	//	self[1][2]=(float)cos(angle * PI / 180);
	//	self[1]=self[1].normalize();

	//	Vec3f enemy;
	//	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	//	{
	//		enemy[0]=(*i).second->x;
	//		enemy[1]=(*i).second->y;
	//		enemy[2]=(*i).second->z;
	//		if(acos( self[1].dot( (enemy-self[0]).normalize() )) < 0.1 && d(self[0],enemy) && i!=player)
	//		{
	//			//enemies[i]-=self[0];
	//			bullet tmp(x,y,z,(enemy-self[0]).normalize()[0],(enemy-self[0]).normalize()[1],(enemy-self[0]).normalize()[2],player);
	//			bullets->push_back(tmp);
	//			cooldown=2;
	//			return;
	//		}
	//	}
	//	bullet tmp(x,y,z,(float)sin(angle/180*PI)*5,((float)climb/1.0f)*5,(float)cos(angle/180*PI)*5,player);
	//	bullets->push_back(tmp);
	//	cooldown=2;
	//}
	virtual void die()=0;
	virtual bool Update()=0;
};

class turret: public turretBase
{
public:
	turret(){}
	turret(int _player);
	void spawn();
	bool Update();
	void die();
};
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