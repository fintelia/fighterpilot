//class spark
//{
//public:	
//	int life;
//	int maxLife;
//	float x,y,z;
//	float r, g, b;
//	Vec3f vel;
//	int owner;
//	spark()
//	{
//		cout << "error" << endl;
//	}
//	spark(float _x,float _y,float _z,float rad,float _r,float _g,float _b,int _life,float spread=0.0)
//	{
//		x=_x+rand()%int(200*rad)/100-rad;
//		y=_y+rand()%int(200*rad)/100-rad;
//		z=_z+rand()%int(200*rad)/100-rad;
//		r=_r;g=_g;b=_b;
//		life=_life;
//		maxLife=life;
//		vel=Vec3f(rand()%200/100,rand()%200/100,rand()%200/100).normalize()*spread;
//	}
//	void change(float _x,float _y,float _z,float rad,float _r,float _g,float _b,int _life,float spread=0.0)
//	{
//		x=_x+rand()%int(200*rad)/100-rad;
//		y=_y+rand()%int(200*rad)/100-rad;
//		z=_z+rand()%int(200*rad)/100-rad;
//		r=_r;g=_g;b=_b;
//		life=_life;
//		maxLife=life;
//		vel=Vec3f(rand()%200,rand()%200,rand()%200).normalize()*spread;
//	}
//	void random()
//	{
//		//vx=((float)(rand()%100)/50-1)/15;
//		//vy=((float)(rand()%100)/50-1)/15;
//		//vz=((float)(rand()%100)/50-1)/15;
//	}
//	bool update(float length)
//	{
//		x+=vel[0]*length;
//		y+=vel[1]*length;
//		z+=vel[2]*length;
//
//		return --life >= 0 ? true : false;
//	}
//};
//const int spark_life=5;
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

class Smoke
{
public:

	Vec3f *positions;
	Vec3f *velocity;
	float *startTime;
	float *endTime;
	SVertex *mem;

private:

	int size;
	int compacity;

	void resize(int newItems);
public:

	Smoke();
	~Smoke();
	void clean();
	void update(float ms);
	void insert(Vec3f pos,float random,float spread,float startT, float life);
	void clear();
	int getSize();
	SVertex* const getMem();
};

