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

	void resize(int newItems)
	{
		//Vec3f *Tpositions = positions;
		//Vec3f *Tvelocity = velocity;
		//float *TmaxLife = maxLife;
		//float *Tlife = life;

		//positions = (Vec3f*)malloc((compacity+newItems)*sizeof(Vec3f));
		//velocity =	(Vec3f*)malloc((compacity+newItems)*sizeof(Vec3f));
		//maxLife =	(float*)malloc((compacity+newItems)*sizeof(float));
		//life =		(float*)malloc((compacity+newItems)*sizeof(float));

		//memcpy(positions,Tpositions,compacity*sizeof(Vec3f));
		//memcpy(velocity,Tvelocity,compacity*sizeof(Vec3f));
		//memcpy(maxLife,TmaxLife,compacity*sizeof(float));
		//memcpy(life,Tlife,compacity*sizeof(float));
		//
		//free(Tpositions);
		//free(Tvelocity);
		//free(TmaxLife);
		//free(Tlife);
		mem =		(SVertex*)realloc(mem,(compacity+newItems)*sizeof(SVertex)*4);
		startTime =	(float*)realloc(startTime,(compacity+newItems)*sizeof(float));
		endTime =	(float*)realloc(endTime,(compacity+newItems)*sizeof(float));
		velocity =	(Vec3f*)realloc(velocity,(compacity+newItems)*sizeof(Vec3f));
		positions = (Vec3f*)realloc(positions,(compacity+newItems)*sizeof(Vec3f));

		if(positions==NULL || velocity==NULL || startTime==NULL || endTime==NULL || mem==NULL)
		{
			assert(0 || "could not allocate memory for smoke");
		}
		compacity+=newItems;
	}
public:

	Smoke()
	{
		size=0;
		compacity = 3000;
		positions = (Vec3f*)malloc(compacity*sizeof(Vec3f));
		velocity =	(Vec3f*)malloc(compacity*sizeof(Vec3f));
		startTime =	(float*)malloc(compacity*sizeof(float));
		endTime =	(float*)malloc(compacity*sizeof(float));
		mem =		(SVertex*)malloc(compacity*sizeof(SVertex)*4);
	}
	~Smoke()
	{
		free(positions);
		free(velocity);
		free(startTime);
		free(endTime);
		free(mem);
	}
	void clean()
	{
		int i;
		vector<int> clear;
		for(i=0;i<size;i++)
		{
			if(gameTime() > endTime[i])	clear.push_back(i);
		}
		for(i=0;i<clear.size() && clear[i]<size-1;i++)
		{
			memcpy(positions+clear[i],positions+clear[i]+1,(size-clear[i]-1)*sizeof(Vec3f));
			memcpy(velocity+clear[i],velocity+clear[i]+1  ,(size-clear[i]-1)*sizeof(Vec3f));
			memcpy(startTime+clear[i],startTime+clear[i]+1 ,(size-clear[i]-1)*sizeof(float));
			memcpy(endTime+clear[i],endTime+clear[i]+1  ,(size-clear[i]-1)*sizeof(float));
		}
		size-=clear.size();
	}
	void update(float ms)
	{
		static float cleanCount=3.0;
		cleanCount-=ms/1000;
		if(cleanCount<=0.0)
		{
			cleanCount=3.0;
			clean();
		}
	}
	void insert(Vec3f pos,float random,float spread,float startT, float life)
	{
		if(size>=compacity) resize(compacity);

		positions[size] = pos + Vec3f(rand()%200/100,rand()%200/100,rand()%200/100).normalize()*random;
		velocity[size] = Vec3f(rand()%200/100,rand()%200/100+0.8,rand()%200/100).normalize()*spread;

		startTime[size] = startT;
		endTime[size] = startT + life;
		size++;
	}
	void clear()
	{
		size = 0;
	}
	int getSize()
	{
		return size;
	}
	SVertex* const getMem()
	{
		return mem;
	}
};

