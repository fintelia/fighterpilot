
class missile: public entity
{
public:
	float life;
	int owner;
	int target;
	float angle;
	float climbAngle;
	vector<spark> *sparks;
	map<int,entity*> *planes;
	double difAng;
	double lastAng;
	//queue<Vec3f> enemyLoc;

	missile()
	{
		x=0;y=0;z=0;
		life=100;
		cout << "error!!! wrong constructor called!" << endl;
	}
	missile(float _x,float _y,float _z,int _owner,float a,float c,vector<spark> *s,map<int,entity*>*p)
	{
		x=_x;
		y=_y;
		z=_z;
		life=198;
		owner=_owner;
		angle=a;
		climbAngle=c/2;
		sparks=s;
		planes=p;
		target=-1;
		difAng=0;
		lastAng=0;
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
		double ang=1;
		for(map<int,entity*>::iterator i = planes->begin(); i != planes->end();i++)
		{
			enemy[0]=(*i).second->x;
			enemy[1]=(*i).second->y;
			enemy[2]=(*i).second->z;
			if(acos( self[1].dot( (enemy-self[0]).normalize() )) < ang && dist(self[0],enemy)<life*speed*0.4 && (*i).first!=owner && !(*i).second->dead)
			{
				ang=acos( self[1].dot( (enemy-self[0]).normalize() ));
				target=(*i).first;
			}
		}
		//enemyLoc.empty();
		//if(target!=-1)
		//{
		//	Vec3f v((*planes)[target]->x,(*planes)[target]->y,(*planes)[target]->z);
		//	for(int i=0;i<0;i++)
		//		enemyLoc.push(v);
		//}
	}
	double correctAng(double ang)
	{
		while(ang>360)
			ang-=360;
		while(ang<0)
			ang+=360;
		return ang;
	}
	bool Update(float value)
	{
		/////////////////follow target////////////////////
		if(target!=-1)
		{
			Vec3f e((*planes)[target]->x,(*planes)[target]->y,(*planes)[target]->z);
			//enemyLoc.push(v);
			//Vec3f e=enemyLoc.front();
			//enemyLoc.pop();

			difAng=correctAng(atan2( x-e[0] , z-e[2] )*180/PI+180);
			if(abs(lastAng-(angle-difAng)/5)>7.5)
			{
				difAng=7.5*(lastAng-(angle-difAng)/5)/abs(lastAng-(angle-difAng)/5);//if turn ang > 7.5 it = 7.5
			}
			if(abs(angle-difAng)<60)
			{
				lastAng=(angle-difAng)/5;
				angle-=(angle-difAng)/2;
				climbAngle-=(climbAngle-asin( (e[1]-y)/sqrt(  (x-e[0])*(x-e[0])+(y-e[1])*(y-e[1])+(z-e[2])*(z-e[2]))))/5;			
			}
			else
			{
				findTarget();
			}
		}
		else
		{
			findTarget();
		}
		angle=correctAng(angle);
		/////////////////////move///////////////////////////
		y += sin(climbAngle)*speed*value*0.7;
		x += sin(float(angle*PI/180))*speed*value*0.7;
		z += cos(float(angle*PI/180))*speed*value*0.7;

		////////////////////sparks//////////////////////////
		for(int i=0;i<1;i++)
		{
			spark tmp(x,y,z,2,0.5,0.5,0.5,5);
			sparks->push_back(tmp);
		}
		for(int i=0;i<1;i++)
		{
			spark tmp(x-sin(float(angle*PI/180))*speed/2,
					y-sin(climbAngle)*speed/2,
					z-cos(float(angle*PI/180))*speed/2,
					2,0.5,0.5,0.5,5);
			sparks->push_back(tmp);
		}
		life-=value;
		return life >= 0 ? true : false;
	}
};