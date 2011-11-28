

#include "engine.h"
#include "GL/glee.h"
#include <GL/glu.h>

particle::manager& particleManager = particle::manager::getInstance();

namespace particle{
//
//void explosion::init()
//{
//	velocity = fuzzyAttribute(50.0, 10.0);
//	spread = fuzzyAttribute(2.0, 0.8);
//	life = fuzzyAttribute(6000.0,100.0);
//	//color = fuzzyColor(0.8,0.2,0.1,0.4);//fuzzyColor(0.8,0.2,0.0,0.4, 0.1,0.1,0.0,0.2);
//	color = fuzzyColor(0.0,0.0,0.0,0.7);
//
//	friction = 1.0;
//	particlesPerSecond = 10.0;
//
//	glGenBuffers(1,&VBO);
//
//	double time = world.time();
//	particle p;
//	for(int i=0; i<40;i++)
//	{
//		p.startTime = time;
//		p.endTime = time + life();
//
//		p.vel = random3<float>() * random<float>() * velocity();
//		p.pos = Vec3f();
//
//		p.friction = 0.07;
//		p.size = 0.0;
//
//		p.r = color.red();
//		p.g = color.green();
//		p.b = color.blue();
//		p.a = color.alpha();
//
//		addParticle(p);
//	}
//}
//explosion::explosion(Vec3f pos): emitter(EXPLOSION,"particle",64)
//{
//	position = pos;
//	init();
//}
//explosion::explosion(int parent, Vec3f offset): emitter(EXPLOSION,"particle",64)
//{
//	parentObject = parent;
//	parentOffset = offset;
//	position = world.objectList[parentObject]->position + world.objectList[parentObject]->rotation * parentOffset;
//	init();
//}
//void explosion::update()
//{
//	static Vec3f lastPos = position;
//	double ms = world.time.getLength();
//	double time = world.time();
//
//	if(parentObject != 0)
//	{
//		position = world.objectList[parentObject]->position + world.objectList[parentObject]->rotation * parentOffset;
//	}
//
//	for(int i=0; i < total; i++)
//	{
//		particles[i].vel *= pow(particles[i].friction,(float)ms/1000);
//		particles[i].pos += particles[i].vel * ms/1000.0;
//		float t = (time - particles[i].startTime) / (particles[i].endTime - particles[i].startTime);
//		if(t<0.05)
//		{
//			particles[i].a = t * 10.0;
//			particles[i].size = 6;
//		}
//		else
//		{
//			t = (t-0.05)/0.99;
//			particles[i].a = 0.5 - 0.5*t;
//			particles[i].size = (1.0-t) * 6.0 + t * 20.0;
//			particles[i].pos.y += ms/100;
//		}
//		//const float T = 0.055;
//		//if(t < T)
//		//	particles[i].size = t * 6.0/T;
//		//else
//		//	particles[i].size = 6.0;
//
//		//particles[i].a = 1.0 - t;
//	}
//
//	particle p;
//	extraTime += ms;
//	while(extraTime > particlesPerSecond)
//	{
//		extraTime -= 1000.0 / particlesPerSecond;
//
//		p.startTime = time - extraTime;
//		p.endTime = time + life();
//
//		p.vel = random3<float>();
//		p.pos = /*position*(1.0-(ms-extraTime)/ms) + lastPos*(ms-extraTime)/ms +*/ random3<float>()*spread() + p.vel * extraTime/1000.0;
//
//		p.friction = 1.0;
//		p.size = 0.0;
//
//		p.r = color.red();
//		p.g = color.green();
//		p.b = color.blue();
//		p.a = color.alpha();
//
//		addParticle(p);
//	}
//
//	lastPos = position;
//}
//void explosion::render(Vec3f up, Vec3f right)
//{
//	//float mat[16];
//	//float mat2[16];
//	//glGetFloatv(GL_PROJECTION_MATRIX,mat);
//	//glGetFloatv(GL_MODELVIEW_MATRIX,mat2);
//	//graphics->bindRenderTarget(GraphicsManager::FBO_1);
//	//glViewport(0,0,sw,sh);
//	//glClearColor(0,0,0,0);
//	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//	//glMatrixMode(GL_PROJECTION);
//	//glLoadIdentity();
//	//glMultMatrixf((const GLfloat*)&mat);
//	//glMatrixMode(GL_MODELVIEW);
//	//glLoadIdentity();
//	//glMultMatrixf((const GLfloat*)&mat2);
//
//
//	glDisable(GL_DEPTH_TEST);
//	glEnable(GL_TEXTURE_2D);
//
//	double time = world.time();
//
//	int pNum,vNum,n;
//	for(pNum = 0, vNum=0; pNum < total; pNum++)
//	{
//		if(particles[pNum].endTime > time)
//		{
//			for(n = 0; n<4; n++)
//			{
//				vertices[vNum*4 + n].r = particles[pNum].r;
//				vertices[vNum*4 + n].g = particles[pNum].g;
//				vertices[vNum*4 + n].b = particles[pNum].b;
//				vertices[vNum*4 + n].a = particles[pNum].a;
//
//				vertices[vNum*4 + n].energy = (time - particles[pNum].startTime) / (particles[pNum].endTime - particles[pNum].startTime);
//			}
//			vertices[vNum*4 + 0].position = position + particles[pNum].pos + up * particles[pNum].size + right * particles[pNum].size;
//			vertices[vNum*4 + 1].position = position + particles[pNum].pos + up * particles[pNum].size - right * particles[pNum].size;
//			vertices[vNum*4 + 2].position = position + particles[pNum].pos - up * particles[pNum].size - right * particles[pNum].size;
//			vertices[vNum*4 + 3].position = position + particles[pNum].pos - up * particles[pNum].size + right * particles[pNum].size;
//
//			vertices[vNum*4 + 0].s = 0.0;	vertices[vNum*4 + 0].t = 0.0;
//			vertices[vNum*4 + 1].s = 0.0;	vertices[vNum*4 + 1].t = 1.0;
//			vertices[vNum*4 + 2].s = 1.0;	vertices[vNum*4 + 2].t = 1.0;
//			vertices[vNum*4 + 3].s = 1.0;	vertices[vNum*4 + 3].t = 0.0;
//
//			vNum++;
//		}
//	}
//
//	dataManager.bind("smoke");
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vNum*4, vertices, GL_DYNAMIC_DRAW);
//
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	//GLuint aLife = glGetAttribLocation(dataManager.getId("partical shader"),"life");
//	//glEnableVertexAttribArray(aLife);
//
//	glVertexPointer(3, GL_FLOAT, sizeof(vertex), 0);
//	//glVertexAttribPointer(aLife,1,GL_FLOAT,false, sizeof(particle), (void*)(sizeof(float)*3));
//	glColorPointer(4, GL_FLOAT, sizeof(vertex), (void*)(sizeof(float)*4));
//	glTexCoordPointer(2,GL_FLOAT,sizeof(vertex), (void*)(sizeof(float)*8));
//
//	glDrawArrays(GL_QUADS, 0, vNum*4);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	//glDisableVertexAttribArray(aLife);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	dataManager.bindTex(0);
//
//	//graphics->bindRenderTarget(GraphicsManager::FBO_0);
//	//graphics->renderFBO(GraphicsManager::FBO_1);
//
//}

emitter::emitter(Type t, Vec3f pos, string tex, float Friction, float ParticlesPerSecond, unsigned int initalCompacity,bool AdditiveBlending):type(t),parentObject(0),parentOffset(0,0,0),position(pos),lastPosition(position),texture(tex),friction(Friction),particles(NULL), vertices(NULL), compacity(initalCompacity), total(0), startTime(world.time()),extraTime(0.0),particlesPerSecond(ParticlesPerSecond), minXYZ(position),maxXYZ(position),additiveBlending(AdditiveBlending)
{
	if(compacity != 0)
	{
		particles = new particle[compacity];
		vertices = new vertex[compacity*4];

		memset(particles,0,compacity*sizeof(particle));
		memset(vertices,0,compacity*sizeof(vertex)*4);
	}
	glGenBuffers(1,&VBO);
}
emitter::emitter(Type t, int parent, Vec3f offset, string tex, float Friction, float ParticlesPerSecond, unsigned int initalCompacity,bool AdditiveBlending):type(t),parentObject(parent),parentOffset(offset),position(world[parent]->position + world[parent]->rotation * offset),lastPosition(position),texture(tex),friction(Friction),particles(NULL), vertices(NULL), compacity(initalCompacity), total(0), startTime(world.time()),extraTime(0.0),particlesPerSecond(ParticlesPerSecond), minXYZ(position),maxXYZ(position),additiveBlending(AdditiveBlending)
{
	if(compacity != 0)
	{
		particles = new particle[compacity];
		vertices = new vertex[compacity*4];

		memset(particles,0,compacity*sizeof(particle));
		memset(vertices,0,compacity*sizeof(vertex)*4);
	}
	glGenBuffers(1,&VBO);
}
emitter::~emitter()
{
	if(particles != NULL)	delete[] particles;
	if(vertices != NULL)	delete[] vertices;
	particles = NULL;
	vertices = NULL;

	glDeleteBuffers(1,&VBO);
}
void emitter::addParticle(particle& p)
{
	for(int i = 0; i < total; i++)
	{
		if(particles[i].endTime <= world.time())
		{
			particles[i] = p;
			return;
		}
	}

	if(total == compacity)
	{
		unsigned int tmpCompacity = (compacity != 0) ? compacity * 2 : 64;
		particle* tmpParticles = new particle[tmpCompacity];
		vertex *tmpVertices = new vertex[tmpCompacity*4];

		memset(tmpParticles,0,tmpCompacity*sizeof(particle));
		memset(tmpVertices,0,tmpCompacity*sizeof(vertex)*4);

		memcpy(tmpParticles,particles,compacity*sizeof(particle));
		memcpy(tmpVertices,vertices,compacity*sizeof(vertex)*4);
		delete [] particles; particles = NULL;
		delete[] vertices; vertices = NULL;
		particles = tmpParticles;
		vertices = tmpVertices;
		compacity = tmpCompacity;
	}
	particles[total] = p;
	total++;
}
void emitter::update()
{
	double ms = world.time.length();
	double time = world.time();

	lastPosition = position;

	if(parentObject != 0)
	{
		if(world[parentObject] == nullptr || world[parentObject]->awaitingDelete)
		{
			parentObject = 0;
			particlesPerSecond = 0;
		}
		else
		{
			position = world[parentObject]->position + world[parentObject]->rotation * parentOffset;
		}
	}


	particle p;
	extraTime += ms;
	while(particlesPerSecond > 0.0 && extraTime > (1000.0 / particlesPerSecond))
	{
		extraTime -= 1000.0 / particlesPerSecond;

		particle p;
		float t = (ms-extraTime)/ms;
		if(createParticle(p,position*(1.0-t) + lastPosition*t))
		{
			addParticle(p);
		}
	}

	liveParticles = 0;
	for(int i=0; i < total; i++)
	{
		if(particles[i].endTime > time)
		{
			liveParticles++;
		}
	}

	for(int i=0; i < total; i++)
	{
		if(particles[i].endTime > time)
		{
			updateParticle(particles[i]);
		}
	}
}
void emitter::prepareRender(Vec3f up, Vec3f right)
{
	int pNum,n;
	float sAng, cAng;
	Vec3f r, u;
	for(pNum = 0, vNum=0; pNum < total; pNum++)
	{
		if(particles[pNum].endTime > world.time())
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = particles[pNum].r;
				vertices[vNum*4 + n].g = particles[pNum].g;
				vertices[vNum*4 + n].b = particles[pNum].b;
				vertices[vNum*4 + n].a = particles[pNum].a;

				vertices[vNum*4 + n].energy = (world.time() - particles[pNum].startTime) / (particles[pNum].endTime - particles[pNum].startTime);
			}
			sAng = sin(particles[pNum].ang);
			cAng = cos(particles[pNum].ang);
			u = -right * sAng + up * cAng;
			r = right * cAng + up * sAng;


			vertices[vNum*4 + 0].position = particles[pNum].pos + u * particles[pNum].size + r * particles[pNum].size;
			vertices[vNum*4 + 1].position = particles[pNum].pos + u * particles[pNum].size - r * particles[pNum].size;
			vertices[vNum*4 + 2].position = particles[pNum].pos - u * particles[pNum].size - r * particles[pNum].size;
			vertices[vNum*4 + 3].position = particles[pNum].pos - u * particles[pNum].size + r * particles[pNum].size;

			vertices[vNum*4 + 0].s = 0.0;	vertices[vNum*4 + 0].t = 0.0;
			vertices[vNum*4 + 1].s = 0.0;	vertices[vNum*4 + 1].t = 1.0;
			vertices[vNum*4 + 2].s = 1.0;	vertices[vNum*4 + 2].t = 1.0;
			vertices[vNum*4 + 3].s = 1.0;	vertices[vNum*4 + 3].t = 0.0;

			vNum++;
		}
	}



	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vNum*4, vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void emitter::render()
{
	dataManager.bind(texture);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(vertex), 0);
	glColorPointer(4, GL_FLOAT, sizeof(vertex), (void*)(sizeof(float)*4));
	glTexCoordPointer(2,GL_FLOAT,sizeof(vertex), (void*)(sizeof(float)*8));

	glDrawArrays(GL_QUADS, 0, vNum*4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	dataManager.bindTex(0);
}
//void emitter::render(Vec3f up, Vec3f right)
//{
//		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
//
//		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
//		glDrawArrays(GL_QUADS, 0, vNum*4);
//		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//	}
//	else
//	{
//		glDrawArrays(GL_QUADS, 0, vNum*4);
//	}
//}


void manager::init()
{

}
void manager::addEmitter(emitter* e)
{
	emitters.push_back(e);
}
void manager::update()
{
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		(*i)->update();
	}
}
void manager::render()
{
	Vec3f up, right;

	Mat4f modelview = graphics->getView().modelViewMat;
	right.x = modelview[0];
	right.y = modelview[4];
	right.z = modelview[8];
	up.x = modelview[1];
	up.y = modelview[5];
	up.z = modelview[9];



	dataManager.bind("particle shader");
	dataManager.setUniform1i("tex",0);
	dataManager.setUniform1i("depth",1);

	dataManager.setUniform2f("invScreenDims",1.0/sw, 1.0/sh);

	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		(*i)->prepareRender(up,right);
	}
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
		if((*i)->additiveBlending)	(*i)->render();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
		if((*i)->additiveBlending)	(*i)->render();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
		if(!(*i)->additiveBlending)	(*i)->render();

	dataManager.unbindShader();
}
void manager::shutdown()
{
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
		delete (*i);
	emitters.clear();
}
}
