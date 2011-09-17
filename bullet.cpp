
#include "main.h"

bullet::bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime): life(1000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*BULLET_SPEED), owner(Owner)
{

}
bullet::bullet(Vec3f pos,Vec3f vel,int Owner): life(1000), startTime(world.time()), startPos(pos), velocity(vel.normalize()*BULLET_SPEED), owner(Owner)
{

}


void bulletCloud::init()
{
	if(vertices)
		delete vertices;

	totalVertices = 1024;
	vertices = new texturedVertex3D[totalVertices];

	glGenBuffers(1, &VBO);
}

void bulletCloud::draw()
{
	double time = world.time();
	double lTime = time - 20.0;//world.time.getLastTime();
	Vec3f start, end, end2;

	auto v = graphics->getView().camera;
	Vec3f up = v.up.normalize();
	Vec3f right = ( v.up.cross(v.center - v.eye) ).normalize();

	if(totalVertices < bullets.size())
	{
		if(vertices)
			delete vertices;

		totalVertices = totalVertices !=0 ? totalVertices*2 : 1024;
		vertices = new texturedVertex3D[totalVertices];
	}

	for(int n=0;n < bullets.size(); n++)
	{
		auto i = bullets[n];

		start=i.startPos+i.velocity*(time-i.startTime)/1000;
		end=i.startPos+i.velocity*(time-i.startTime)/1000-i.velocity.normalize()*2;
		end2=i.startPos+i.velocity*max(lTime-i.startTime,0.0)/1000;

		Vec3f dir = i.velocity.normalize();
		float a1 = dir.dot(up);
		float a2 = dir.dot(right);
		float len = 0.3/sqrt(a1*a1+a2*a2);

		vertices[n*4 + 0].position = start + (right*a1 - up*a2)*len;
		vertices[n*4 + 1].position = start - (right*a1 - up*a2)*len;
		vertices[n*4 + 2].position = end2 - (right*a1 - up*a2)*len;
		vertices[n*4 + 3].position = end2 + (right*a1 - up*a2)*len;

		vertices[n*4 + 0].UV = Vec2f(1,1);
		vertices[n*4 + 1].UV = Vec2f(1,0);
		vertices[n*4 + 2].UV = Vec2f(0,0);
		vertices[n*4 + 3].UV = Vec2f(0,1);
	}

	

	dataManager.bind("bullet");

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedVertex3D)*bullets.size()*4, vertices, GL_STREAM_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,	GL_FLOAT,	sizeof(texturedVertex3D), (void*)0);
	glTexCoordPointer(2,GL_FLOAT,	sizeof(texturedVertex3D), (void*)(3*sizeof(float)));

	glDrawArrays(GL_QUADS,0, bullets.size()*4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	dataManager.unbind("bullet");
}

void bulletCloud::addBullet(Vec3f pos,Vec3f vel,int Owner, double StartTime)
{
	bullets.push_back(bullet(pos, vel, Owner, StartTime));
}
void bulletCloud::addBullet(Vec3f pos,Vec3f vel,int Owner)
{
	bullets.push_back(bullet(pos, vel, Owner));
}
void bulletCloud::update(double time, double ms)
{
	double t = world.time();
	bullets.erase(remove_if(bullets.begin(), bullets.end(), [&t] (bullet& b) -> bool {return b.startTime + b.life < t;}), bullets.end());
}