
#include "main.h"
void GraphicsManager::render3D()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		if(i->second->drawFlag && i->second->is_3D && !i->second->transparent)
			i->second->render();
	}
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		if(i->second->drawFlag && i->second->is_3D && i->second->transparent)
			i->second->render();
	}
}
void GraphicsManager::render2D()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		if(!i->second->is_3D && !i->second->transparent)
			i->second->render();
	}
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		if(!i->second->is_3D && i->second->transparent)
			i->second->render();
	}

}
GraphicsManager::gID OpenGLgraphics::newModel(string disp)
{
	gID nID = idGen();
	objects.insert(pair<gID,object*>(nID, new model(disp)));
	return nID;
}
GraphicsManager::gID OpenGLgraphics::newParticleEffect(string texture, int size)
{
	gID nID = idGen();
	objects.insert(pair<gID,object*>(nID, new particleEffect(texture,size)));
	return nID;
}
void GraphicsManager::reset()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		i->second->reset(); 
	}
}
bool OpenGLgraphics::drawModel(gID obj, Vec3f pos, Quat4f rot)
{
	if(objects.find(obj) !=objects.end() && objects[obj]->type==object::MODEL)
	{
		objects[obj]->drawFlag = true;
		((model*)objects[obj])->pos = pos;
		((model*)objects[obj])->rot = rot;
		return true;
	}
	return false;
}
bool OpenGLgraphics::drawModel(gID obj)
{
	if(objects.find(obj) !=objects.end() && objects[obj]->type==object::MODEL)
	{
		objects[obj]->drawFlag = true;
		return true;
	}
	return false;
}
OpenGLgraphics::particleEffect::particleEffect(string tex, int s): object(PARTICLE_EFFECT,true,true), texture(tex), size(s), num(0), compacity(32), particles(NULL)
{
	particles = new (nothrow) particle[compacity];
	if(particles == NULL)
		compacity = 0;
	glGenBuffers(1,&VBO);

}
void OpenGLgraphics::particleEffect::reset()
{
	drawFlag=false;
	num = 0;
}
bool OpenGLgraphics::particleEffect::setParticle(Vec3f p, Color c)
{

	if(compacity <= num)
	{
		particle* tmpParticles = new (nothrow) particle[compacity*2];
		if(tmpParticles == NULL) 
			return false;

		memcmp(tmpParticles,particles,compacity*sizeof(particle));
		delete[] particles;
		particles = tmpParticles;

		compacity *= 2;
	}
	particles[num].vert = p;
	particles[num].r = c.r;
	particles[num].g = c.g;
	particles[num].b = c.b;
	particles[num].a = c.a;

	drawFlag=true;
	num++;
	return true;
}
void OpenGLgraphics::particleEffect::render()
{
	if(num==0)return;
	glColor4f(1,1,1,1);
	dataManager.bind(texture);

	glPointSize(   size   );

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle)*num, particles, GL_DYNAMIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(particle), 0);
	glColorPointer(4, GL_FLOAT, sizeof(particle), (void*)sizeof(Vec3f));

	glDrawArrays(GL_POINTS, 0, num);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	dataManager.bindTex(0);
}
OpenGLgraphics::particleEffect::~particleEffect()
{
	glDeleteBuffers(1,&VBO);
	delete[] particles;
}
bool OpenGLgraphics::drawParticle(gID id, Vec3f pos, Color c)
{
	if(objects.find(id) !=objects.end() && objects[id]->type==object::PARTICLE_EFFECT)
	{
		return ((particleEffect*)objects[id])->setParticle(pos, c);
	}
	return false;
}