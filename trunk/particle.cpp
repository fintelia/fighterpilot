
#include "main.h"

void Smoke::resize(int newItems)
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

Smoke::Smoke()
{
	size=0;
	compacity = 3000;
	positions = (Vec3f*)malloc(compacity*sizeof(Vec3f));
	velocity =	(Vec3f*)malloc(compacity*sizeof(Vec3f));
	startTime =	(float*)malloc(compacity*sizeof(float));
	endTime =	(float*)malloc(compacity*sizeof(float));
	mem =		(SVertex*)malloc(compacity*sizeof(SVertex)*4);
}
Smoke::~Smoke()
{
	free(positions);
	free(velocity);
	free(startTime);
	free(endTime);
	free(mem);
}
void Smoke::clean()
{
	int i;
	vector<int> clear;
	for(i=0;i<size;i++)
	{
		if(world.time() > endTime[i])	clear.push_back(i);
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
void Smoke::update(float ms)
{
	static float cleanCount=3.0;
	cleanCount-=ms/1000;
	if(cleanCount<=0.0)
	{
		cleanCount=3.0;
		clean();
	}
}
void Smoke::insert(Vec3f pos,float random,float spread,float startT, float life)
{
	if(size>=compacity) resize(size);

	positions[size] = pos + Vec3f(rand()%200/100,rand()%200/100,rand()%200/100).normalize()*random;
	velocity[size] = Vec3f(rand()%200/100,rand()%200/100+0.8,rand()%200/100).normalize()*spread;

	startTime[size] = startT;
	endTime[size] = startT + life;
	size++;
}
void Smoke::clear()
{
	size = 0;
}
int Smoke::getSize()
{
	return size;
}
SVertex* const Smoke::getMem()
{
	return mem;
}