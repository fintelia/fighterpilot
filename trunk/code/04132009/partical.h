class spark
{
public:	
	int life;
	float x,y,z;
	float vx, vy, vz;
	int owner;
	spark()
	{
		cout << "error" << endl;
	}
	spark(float _x,float _y,float _z,float _vx,float _vy,float _vz,int _life)
	{
		x=_x;
		y=_y;
		z=_z;
		vx=0;
		vy=0;
		vz=0;
		life=_life;
	}
	void random()
	{
		vx=((float)(rand()%100)/50-1)/15;
		vy=((float)(rand()%100)/50-1)/15;
		vz=((float)(rand()%100)/50-1)/15;
	}
	bool update(float length)
	{
		x+=(float)vx*length;
		y+=(float)vy*length-0.01f;
		z+=(float)vz*length;
		return --life >= 0 ? true : false;
	}
};
const int spark_life=5;
//class sparker
//{
//public:
//	vector<spark> sparks;
//	vector<plane> *r;
//	int t;
//	sparker(vector<plane> *_r,int _t)
//	{
//		r=_r;
//		t=_t;
//	}
//	void update()
//	{
//		for(int i=0;i<(signed int)sparks.size();i++)
//		{
//			if(!sparks[i].update(1))
//				sparks.erase(sparks.begin()+i);
//		}
//		spark tmp(0,0,0,0,0,0,spark_life);
//		for(int i=0;i<25;i++)
//		{
//			tmp.random();
//			sparks.push_back(tmp);
//		}
//	}
//	void draw()
//	{
//		glBegin(GL_POINTS);
//		for(int i=0;i<(signed int)sparks.size();i++)
//		{
//			glColor3f(1,0,0);
//			glVertex3f(sparks[i].x+r->at(t).x,sparks[i].y+r->at(t).y,sparks[i].z+r->at(t).z);
//		}
//	}
//};