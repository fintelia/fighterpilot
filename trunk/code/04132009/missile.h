
class missile: public entity
{
public:
	int life;
	int owner;
	int target;
	float angle;
	float climbAngle;
	vector<spark> *sparks;
	vector<entity*> *planes;
	double difAng;
	
	missile()
	{
		x=0;y=0;z=0;
		life=100;
		cout << "error!!! wrong constructor called!" << endl;
	}
	missile(float _x,float _y,float _z,int _owner,float a,float c,vector<spark> *s,vector<entity*>*p)
	{
		x=_x;
		y=_y;
		z=_z;
		life=100;
		owner=_owner;
		angle=a;
		climbAngle=c;
		sparks=s;
		planes=p;
		target=-1;
		difAng=0;
	}
	void findTarget()
	{
		Vec3f self[2];//current,future
		self[0][0]=x;self[0][1]=y;self[0][2]=z;
		self[1][0]=(float)sin(angle * PI / 180);
		self[1][1]=sin(climbAngle);
		self[1][2]=(float)cos(angle * PI / 180);
		self[1]=self[1].normalize();

		Vec3f enemy;
		double ang=2.5;
		for(int i=0;i<(signed int)planes->size();i++)
		{
			enemy[0]=planes->at(i)->x;
			enemy[1]=planes->at(i)->y;
			enemy[2]=planes->at(i)->z;
			if(acos( self[1].dot( (enemy-self[0]).normalize() )) < ang && d(self[0],enemy)<1000 && planes->at(i)->id!=owner)
			{
				ang=acos( self[1].dot( (enemy-self[0]).normalize() ));
				target=i;
			}
		}
	}
	double correctAng(double ang)
	{
		while(ang>360)
			ang-=360;
		while(ang<0)
			ang+=360;
		return ang;
	}
	bool Update()
	{

		/////////////////follow target////////////////////
		//if(target!=-1)
		//{
		//	difAng=correctAng(   atan2(planes->at(target)->x-x,planes->at(target)->z-z)/PI*180    );
		//	angle-=(angle-difAng)/8;
		//	//if(difAng>angle)
		//	//	angle+=0.2;
		//	//if(difAng>angle)
		//	//	angle-=0.2;
		//	//climbAngle=0;
		//	//y=planes->at(target)->y;
		//	angle=correctAng(angle);
		//}
		//else
		//{
		//	findTarget();
		//}
		/////////////////////move///////////////////////////
		y += sin(climbAngle)*speed*2;
		x += sin(float(angle*PI/180))*speed;
		z += cos(float(angle*PI/180))*speed;

		////////////////////sparks//////////////////////////
		for(int i=0;i<30;i++)
		{
			spark tmp(x+rand()%100/10-5.0f,y+rand()%100/10-5.0f,z+rand()%100/10-5.0f,0,0,0,5);
			sparks->push_back(tmp);
		}
		for(int i=0;i<30;i++)
		{
			spark tmp(x+rand()%100/10-5.0f-sin(float(angle*PI/180))*speed/2,
					y+rand()%100/10-5.0f-sin(climbAngle)*speed/2,
					z+rand()%100/10-5.0f-cos(float(angle*PI/180))*speed/2,0,0,0,5);
			sparks->push_back(tmp);
		}

		return --life >= 0 ? true : false;
	}
};