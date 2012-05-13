

#include "engine.h"

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

emitter::emitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond, bool AdditiveBlending):parentObject(0),parentOffset(0,0,0),texture(tex),particles(NULL), vertices(NULL), compacity(initalCompacity), liveParticles(0), total(0), startTime(world.time()),extraTime(0.0),particlesPerSecond(ParticlesPerSecond), minXYZ(position),maxXYZ(position),additiveBlending(AdditiveBlending), active(true), visible(true)
{
	if(compacity != 0)
	{
		particles = new particle[compacity];
		vertices = new vertex[compacity*4];

		memset(particles,0,compacity*sizeof(particle));
		memset(vertices,0,compacity*sizeof(vertex)*4);
	}
	VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM);

	VBO->addPositionData(	3,	0*sizeof(float));
	VBO->addTexCoordData(	2,	3*sizeof(float));
	VBO->addColorData(		4,	5*sizeof(float));

	VBO->setTotalVertexSize(sizeof(vertex));
}
//emitter::emitter(Type t, int parent, Vec3f offset, string tex, float Friction, float ParticlesPerSecond, unsigned int initalCompacity,bool AdditiveBlending):type(t),parentObject(parent),parentOffset(offset),position(world[parent]->position + world[parent]->rotation * offset),lastPosition(position),texture(tex),friction(Friction),particles(NULL), vertices(NULL), compacity(initalCompacity), total(0), startTime(world.time()),extraTime(0.0),particlesPerSecond(ParticlesPerSecond), minXYZ(position),maxXYZ(position),additiveBlending(AdditiveBlending)
//{
//	if(compacity != 0)
//	{
//		particles = new particle[compacity];
//		vertices = new vertex[compacity*4];
//
//		memset(particles,0,compacity*sizeof(particle));
//		memset(vertices,0,compacity*sizeof(vertex)*4);
//	}
//	VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM, false);
//
//	VBO->addPositionData(	3,	0*sizeof(float));
//	VBO->addTexCoordData(	2,	3*sizeof(float));
//	VBO->addColorData(		4,	5*sizeof(float));
//
//	VBO->setTotalVertexSize(sizeof(vertex));
//}
emitter::~emitter()
{
	delete[] particles;
	delete[] vertices;
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
	for(int i=0; i < total; i++)
	{
		if(particles[i].endTime > time)
			particles[i].fadeIn = false;
	}

	
	if(active)
	{
		particle p;
		extraTime += ms;
		while(particlesPerSecond > 0.0 && extraTime > (1000.0 / particlesPerSecond))
		{
			extraTime -= 1000.0 / particlesPerSecond;

			particle p;
			float t = (ms-extraTime)/ms;
			if(createParticle(p,position*t + lastPosition*(1.0-t)))
			{
				p.fadeIn = true;
				addParticle(p);
			}
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

	float n=0;
	double t = GetTime();
	for(int i=0; i < total; i++)
	{
		if(particles[i].endTime > time)
		{
			particles[i].lastPos = particles[i].pos;
			updateParticle(particles[i]);

			n += 1.0;
		}
	}
	t = GetTime() - t;
	if(n > 0)
	{
		//Profiler.setOutput("update time x 1000", (1000.0 * t) / n);
	}
}
void emitter::prepareRender(Vec3f up, Vec3f right)
{
	int pNum,n;
	float sAng, cAng;
	Vec3f r, u, pos;

	float interpolate = world.time.interpolate();

	for(pNum = 0, vNum=0; pNum < total; pNum++)
	{
		if(particles[pNum].endTime > world.time())
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = particles[pNum].r;
				vertices[vNum*4 + n].g = particles[pNum].g;
				vertices[vNum*4 + n].b = particles[pNum].b;
				vertices[vNum*4 + n].a = (particles[pNum].fadeIn && !additiveBlending) ? particles[pNum].a*interpolate : particles[pNum].a;

				vertices[vNum*4 + n].energy = (world.time() - particles[pNum].startTime) / (particles[pNum].endTime - particles[pNum].startTime);
			}
			sAng = sin(particles[pNum].ang);
			cAng = cos(particles[pNum].ang);
			u = -right * sAng + up * cAng;
			r = right * cAng + up * sAng;

			pos = particles[pNum].pos * interpolate + particles[pNum].lastPos * (1.0 - interpolate);

			vertices[vNum*4 + 0].position = pos + u * particles[pNum].size + r * particles[pNum].size;
			vertices[vNum*4 + 1].position = pos + u * particles[pNum].size - r * particles[pNum].size;
			vertices[vNum*4 + 2].position = pos - u * particles[pNum].size - r * particles[pNum].size;
			vertices[vNum*4 + 3].position = pos - u * particles[pNum].size + r * particles[pNum].size;

			vertices[vNum*4 + 0].s = 0.0;	vertices[vNum*4 + 0].t = 0.0;
			vertices[vNum*4 + 1].s = 0.0;	vertices[vNum*4 + 1].t = 1.0;
			vertices[vNum*4 + 2].s = 1.0;	vertices[vNum*4 + 2].t = 1.0;
			vertices[vNum*4 + 3].s = 1.0;	vertices[vNum*4 + 3].t = 0.0;

			vNum++;
		}
	}

	

	VBO->setVertexData(sizeof(vertex)*vNum*4, vertices);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vNum*4, vertices, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void emitter::render()
{

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glVertexPointer(3, GL_FLOAT, sizeof(vertex), 0);
	//glColorPointer(4, GL_FLOAT, sizeof(vertex), (void*)(sizeof(float)*4));
	//glTexCoordPointer(2,GL_FLOAT,sizeof(vertex), (void*)(sizeof(float)*8));

	//glDrawArrays(GL_QUADS, 0, vNum*4);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(vNum != 0)
	{
		dataManager.bind(texture);

		VBO->drawBuffer(GraphicsManager::QUADS, 0, vNum*4);
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}
void relativeEmitter::prepareRender(Vec3f up, Vec3f right)
{
	int pNum,n;
	float sAng, cAng;
	Vec3f r, u, pos;

	float interpolate = world.time.interpolate();
	Vec3f interpolatedPos = lerp(lastPosition, position, interpolate);
	Quat4f rotation = slerp(world[parentObject]->lastRotation, world[parentObject]->rotation, interpolate);

	for(pNum = 0, vNum=0; pNum < total; pNum++)
	{
		if(particles[pNum].endTime > world.time())
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = particles[pNum].r;
				vertices[vNum*4 + n].g = particles[pNum].g;
				vertices[vNum*4 + n].b = particles[pNum].b;
				vertices[vNum*4 + n].a = (particles[pNum].fadeIn && !additiveBlending) ? particles[pNum].a*interpolate : particles[pNum].a;

				vertices[vNum*4 + n].energy = (world.time() - particles[pNum].startTime) / (particles[pNum].endTime - particles[pNum].startTime);
			}
			sAng = sin(particles[pNum].ang);
			cAng = cos(particles[pNum].ang);
			u = -right * sAng + up * cAng;
			r = right * cAng + up * sAng;

			pos = interpolatedPos + rotation * (parentOffset + lerp(particles[pNum].lastPos, particles[pNum].pos, interpolate));

			vertices[vNum*4 + 0].position = pos + u * particles[pNum].size + r * particles[pNum].size;
			vertices[vNum*4 + 1].position = pos + u * particles[pNum].size - r * particles[pNum].size;
			vertices[vNum*4 + 2].position = pos - u * particles[pNum].size - r * particles[pNum].size;
			vertices[vNum*4 + 3].position = pos - u * particles[pNum].size + r * particles[pNum].size;

			vertices[vNum*4 + 0].s = 0.0;	vertices[vNum*4 + 0].t = 0.0;
			vertices[vNum*4 + 1].s = 0.0;	vertices[vNum*4 + 1].t = 1.0;
			vertices[vNum*4 + 2].s = 1.0;	vertices[vNum*4 + 2].t = 1.0;
			vertices[vNum*4 + 3].s = 1.0;	vertices[vNum*4 + 3].t = 0.0;

			vNum++;
		}
	}

	VBO->setVertexData(sizeof(vertex)*vNum*4, vertices);
};


void sparkEmitter::prepareRender(Vec3f up, Vec3f right)
{
	int pNum,n;
	Vec3f start, end, dir;
	float a1, a2, len;

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

			start = particles[pNum].pos;
			end = particles[pNum].pos - particles[pNum].dir*particles[pNum].size;

			dir = end - start;
			a1 = dir.dot(up);
			a2 = dir.dot(right);
			len = 0.2/sqrt(a1*a1+a2*a2);

			vertices[vNum*4 + 0].position = start + (right*a1 - up*a2)*len;
			vertices[vNum*4 + 1].position = start - (right*a1 - up*a2)*len;
			vertices[vNum*4 + 2].position = end - (right*a1 - up*a2)*len;
			vertices[vNum*4 + 3].position = end + (right*a1 - up*a2)*len;


			vertices[vNum*4 + 0].s = 0.0;	vertices[vNum*4 + 0].t = 0.0;
			vertices[vNum*4 + 1].s = 0.0;	vertices[vNum*4 + 1].t = 1.0;
			vertices[vNum*4 + 2].s = 1.0;	vertices[vNum*4 + 2].t = 1.0;
			vertices[vNum*4 + 3].s = 1.0;	vertices[vNum*4 + 3].t = 0.0;

			vNum++;
		}
	}

	VBO->setVertexData(sizeof(vertex)*vNum*4, vertices);

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
//particleType::particleType(unsigned int initialCompacity): particles(nullptr), vertices(nullptr), compacity(initialCompacity), total(0), vNum(0), VBO(nullptr), additiveBlending(false)
//{
//	particles = new particle[initialCompacity];
//	vertices = new vertex[initialCompacity*4];
//
//	memset(particles,0,initialCompacity*sizeof(particle));
//	memset(vertices,0,initialCompacity*sizeof(vertex)*4);
//
//	VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM, false);
//
//	VBO->addPositionData(	3,	0*sizeof(float));
//	VBO->addTexCoordData(	2,	3*sizeof(float));
//	VBO->addColorData(		4,	5*sizeof(float));
//
//	VBO->setTotalVertexSize(sizeof(vertex));
//}
//particleType::~particleType()
//{
//	delete VBO;
//	delete[] particles;
//	delete[] vertices;
//}
//void particleType::update()
//{
//	double time = world.time();
//	for(int i=0; i < total; i++)
//	{
//		if(particles[i].endTime > time)
//		{
//			updateParticle(particles[i]);
//		}
//	}
//}
//particle& particleType::newParticle()
//{
//	for(int i = 0; i < total; i++)
//	{
//		if(particles[i].endTime <= world.time())
//		{
//			return particles[i];
//		}
//	}
//
//	if(total == compacity)
//	{
//		unsigned int tmpCompacity = (compacity != 0) ? compacity * 2 : 64;
//		particle* tmpParticles = new particle[tmpCompacity];
//		vertex *tmpVertices = new vertex[tmpCompacity*4];
//
//		memset(tmpParticles,0,tmpCompacity*sizeof(particle));
//		memset(tmpVertices,0,tmpCompacity*sizeof(vertex)*4);
//
//		memcpy(tmpParticles,particles,compacity*sizeof(particle));
//		memcpy(tmpVertices,vertices,compacity*sizeof(vertex)*4);
//		delete [] particles; particles = NULL;
//		delete[] vertices; vertices = NULL;
//		particles = tmpParticles;
//		vertices = tmpVertices;
//		compacity = tmpCompacity;
//	}
//	return particles[total++];
//}
//void pointSprite::prepareRender(Vec3f up, Vec3f right)
//{
//	int pNum,n;
//	float sAng, cAng;
//	Vec3f r, u;
//	for(pNum = 0, vNum=0; pNum < total; pNum++)
//	{
//		if(particles[pNum].endTime > world.time())
//		{
//			for(n = 0; n<4; n++)
//			{
//				vertices[vNum*4 + n].r = particles[pNum].r;
//				vertices[vNum*4 + n].g = particles[pNum].g;
//				vertices[vNum*4 + n].b = particles[pNum].b;
//				vertices[vNum*4 + n].a = particles[pNum].a;
//
//				vertices[vNum*4 + n].energy = (world.time() - particles[pNum].startTime) / (particles[pNum].endTime - particles[pNum].startTime);
//			}
//			sAng = sin(particles[pNum].ang);
//			cAng = cos(particles[pNum].ang);
//			u = -right * sAng + up * cAng;
//			r = right * cAng + up * sAng;
//
//
//			vertices[vNum*4 + 0].position = particles[pNum].pos + u * particles[pNum].size + r * particles[pNum].size;
//			vertices[vNum*4 + 1].position = particles[pNum].pos + u * particles[pNum].size - r * particles[pNum].size;
//			vertices[vNum*4 + 2].position = particles[pNum].pos - u * particles[pNum].size - r * particles[pNum].size;
//			vertices[vNum*4 + 3].position = particles[pNum].pos - u * particles[pNum].size + r * particles[pNum].size;
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
//	
//
//	VBO->bindBuffer();
//	VBO->setVertexData(sizeof(vertex)*vNum*4, vertices);
//}
//void pointSprite::render()
//{
//	if(vNum != 0)
//	{
//		VBO->bindBuffer();
//		VBO->drawBuffer(GraphicsManager::QUADS, 0, vNum*4);
//	}
//}
//particleEffect::particleEffect(): position(), lastPosition(), parentObject(0), parentOffset(), radius(0), startTime(world.time()), particlesPerSecond(0), extraTime(0)
//{
//
//}
//bool particleEffect::update()
//{
//	double ms = world.time.length();
//	double time = world.time();
//
//	lastPosition = position;
//
//	if(parentObject != 0)
//	{
//		if(world[parentObject] == nullptr || world[parentObject]->awaitingDelete)
//		{
//			parentObject = 0;
//			particlesPerSecond = 0;
//		}
//		else
//		{
//			position = world[parentObject]->position + world[parentObject]->rotation * parentOffset;
//		}
//	}
//
//
//	particle p;
//	extraTime += ms;
//	while(particlesPerSecond > 0.0 && extraTime > (1000.0 / particlesPerSecond))
//	{
//		extraTime -= 1000.0 / particlesPerSecond;
//
//		particle p;
//		float t = (ms-extraTime)/ms;
//		newParticle(position*(1.0-t) + lastPosition*t, time - extraTime);
//	}
//
//	return particlesPerSecond > 0.0;
//}

void manager::init()
{

}
void manager::addEmitter(shared_ptr<emitter> e, Vec3f position, float radius)
{
	emitters.push_back(e);
	e->setPositionAndRadius(position, radius);
	e->init();
}
void manager::addEmitter(shared_ptr<emitter> e, int parent, Vec3f offset)
{
	emitters.push_back(e);
	auto model = dataManager.getModel(world[parent]->type);
	e->setPositionAndRadius(world[parent]->position + world[parent]->rotation * offset, model!=nullptr ? model->boundingSphere.radius : 0.0);
	e->setParent(parent, offset);
	e->init();
}
//void manager::addParticleEffect(particleEffect* p, Vec3f position, float radius)
//{
//	particleEffects.push_back(shared_ptr<particleEffect>(p));
//	p->setPositionAndRadius(position, radius);
//	p->init();
//}
//void manager::addParticleEffect(particleEffect* p, int parent, Vec3f offset)
//{
//	particleEffects.push_back(shared_ptr<particleEffect>(p));
//	p->setPositionAndRadius(world[parent]->position + world[parent]->rotation * offset, dataManager.getModel(world[parent]->type)->boundingSphere.radius);
//	p->setParent(parent, offset);
//	p->init();
//}
//void manager::addParticleType(string type, shared_ptr<particleType> particleTypePtr)
//{
//	if(particleTypes.find(type)==particleTypes.end())
//		particleTypes[type] = particleTypePtr;
//}
//shared_ptr<particleType> manager::getParticleType(string type)
//{
//	auto i = particleTypes.find(type);
//	if(i==particleTypes.end())
//		return shared_ptr<particleType>();
//	return i->second;
//}
void manager::update()
{
	//Profiler.startElement("particle update");
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		(*i)->update();
	}

	unsigned int totalParticles=0;
	for(auto i = emitters.begin(); i!=emitters.end();)
	{
		if((*i)->toDelete())
		{
			i = emitters.erase(i);
		}
		else
		{
			totalParticles += (*i)->liveParticles;
			i++;
		}
	}

	//Profiler.setOutput("total particles:", (float)totalParticles);

	// Profiler.endElement("particle update");
	//for(auto i = particleEffects.begin(); i!=particleEffects.end(); i++)
	//{
	//	(*i)->update();
	//}

	//for(auto i = particleTypes.begin(); i!=particleTypes.end(); i++)
	//{
	//	(*i).second->update();
	//}
}
void manager::render(shared_ptr<GraphicsManager::View> view)
{
	Vec3f up, right;

	const Mat4f& modelview = view->modelViewMatrix();
	right.x = modelview[0];
	right.y = modelview[4];
	right.z = modelview[8];
	up.x = modelview[1];
	up.y = modelview[5];
	up.z = modelview[9];



	dataManager.bind("particle shader");
	dataManager.setUniform1i("tex",0);
	dataManager.setUniform1i("depth",1);
	dataManager.setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());

	dataManager.setUniform2f("invScreenDims",1.0/sw, 1.0/sh);

	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		if((*i)->visible)
		{
			(*i)->prepareRender(up,right);
		}
	}

	graphics->setBlendMode(GraphicsManager::ALPHA_ONLY);
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		if((*i)->additiveBlending && (*i)->visible)
		{
			(*i)->render();
		}
	}

	graphics->setBlendMode(GraphicsManager::ADDITIVE);
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		if((*i)->additiveBlending && (*i)->visible)
		{
			(*i)->render();
		}
	}

	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		if(!(*i)->additiveBlending && (*i)->visible)
		{
			(*i)->render();
		}
	}

	//dataManager.unbindShader();
}
void manager::shutdown()
{
	emitters.clear();

	//for(auto i = particleTypes.begin(); i!=particleTypes.end(); i++)
	//	i->second->reset();
}
}
