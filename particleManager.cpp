

#include "engine.h"

particle::manager& particleManager = particle::manager::getInstance();

namespace particle{
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
}
void emitter::render()
{
	if(vNum != 0)
	{
		dataManager.bind(texture);
		VBO->drawBuffer(GraphicsManager::QUADS, 0, vNum*4);
	}
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
void manager::update()
{
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
}
void manager::shutdown()
{
	emitters.clear();
}
}
