
#include "game.h"

bullet::bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime): life(1000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*1000), owner(Owner)
{

}
bullet::bullet(Vec3f pos,Vec3f vel,int Owner): life(1000), startTime(world.time()), startPos(pos), velocity(vel.normalize()*1000), owner(Owner)
{

}

bulletCloud::bulletCloud(): object(Vec3f(), Quat4f(), BULLET_CLOUD)
{
	
}
void bulletCloud::init()
{
	//if(vertices)
	//	delete vertices;

	//totalVertices = 1024;
	//vertices = new texturedVertex3D[totalVertices];
	
	//VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM, false);
	//VBO->addPositionData(3, 0);
	//VBO->addTexCoordData(2, 3*sizeof(float));
	//VBO->setTotalVertexSize(sizeof(texturedVertex3D));
}

void bulletCloud::draw()
{
	//double time = world.time();
	//double lTime = time - 20.0;//world.time.getLastTime();
	//Vec3f start, end, end2;

	//auto v = graphics->getView().camera;

	//if(totalVertices < bullets.size())
	//{
	//	if(vertices)
	//		delete vertices;

	//	totalVertices = totalVertices !=0 ? totalVertices*2 : 1024;
	//	vertices = new texturedVertex3D[totalVertices];
	//}

	//for(int n=0;n < bullets.size(); n++)
	//{
	//	auto i = bullets[n];

	//	start=i.startPos+i.velocity*(time-i.startTime)/1000;
	//	end=i.startPos+i.velocity*(time-i.startTime)/1000-i.velocity.normalize()*2;
	//	end2=i.startPos+i.velocity*max(lTime-i.startTime,0.0)/1000;

	//	Vec3f dir = i.velocity.normalize();
	//	float a1 = dir.dot(v.up);
	//	float a2 = dir.dot(v.right);
	//	float len = 0.3/sqrt(a1*a1+a2*a2);

	//	vertices[n*4 + 0].position = start + (v.right*a1 - v.up*a2)*len;
	//	vertices[n*4 + 1].position = start - (v.right*a1 - v.up*a2)*len;
	//	vertices[n*4 + 2].position = end2 - (v.right*a1 - v.up*a2)*len;
	//	vertices[n*4 + 3].position = end2 + (v.right*a1 - v.up*a2)*len;

	//	vertices[n*4 + 0].UV = Vec2f(1,1);
	//	vertices[n*4 + 1].UV = Vec2f(1,0);
	//	vertices[n*4 + 2].UV = Vec2f(0,0);
	//	vertices[n*4 + 3].UV = Vec2f(0,1);
	//}

	//

	//VBO->bindBuffer();
	//VBO->setVertexData(sizeof(texturedVertex3D)*bullets.size()*4, vertices);

	////glEnableClientState(GL_VERTEX_ARRAY);

	//dataManager.bind("bullet");

	//VBO->drawBuffer(GraphicsManager::QUADS, 0, bullets.size()*4); // will not work if other texturing modes are enabled (GL_TEXTURE_CUBE_MAP)

	//dataManager.unbind("bullet");
}

void bulletCloud::addBullet(Vec3f pos,Vec3f vel,int Owner, double StartTime)
{
	bullets.push_back(bullet(pos, vel, Owner, StartTime));
}
void bulletCloud::addBullet(Vec3f pos,Vec3f vel,int Owner)
{
	bullets.push_back(bullet(pos, vel, Owner));
}
void bulletCloud::updateSimulation(double time, double ms)
{
	double t = world.time();
	bullets.erase(remove_if(bullets.begin(), bullets.end(), [&t] (bullet& b) -> bool {return b.startTime + b.life <= t;}), bullets.end());
	// ---
}
void bulletCloud::updateFrame(float interpolation) const
{
	// ---
}