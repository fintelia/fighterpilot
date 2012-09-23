

#include "engine.h"

particle::manager& particleManager = particle::manager::getInstance();

namespace particle{

shared_ptr<GraphicsManager::indexBuffer> emitter::quadIBO;
unsigned int emitter::IBOnumQuads=0;

emitter::emitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond, bool AdditiveBlending):parentObject(0),parentOffset(0,0,0),texture(tex),particles(NULL), oldParticlePositions(NULL), particleFlags(NULL), vertices(NULL), compacity(initalCompacity), liveParticles(0), total(0), startTime(world.time()),extraTime(0.0),particlesPerSecond(ParticlesPerSecond), minXYZ(position),maxXYZ(position),additiveBlending(AdditiveBlending), active(true), visible(true)
{
	if(compacity != 0)
	{
		particles = new particle[compacity];
		oldParticlePositions = new Vec3f[compacity];
		particleFlags = new unsigned char[compacity];
		vertices = new vertex[compacity*4];
	}
	VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM);

	VBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0*sizeof(float));
	VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,	3*sizeof(float));
	VBO->addVertexAttribute(GraphicsManager::vertexBuffer::COLOR4,		5*sizeof(float));

	VBO->setTotalVertexSize(sizeof(vertex));
}
emitter::~emitter()
{
	delete[] particles;
	delete[] oldParticlePositions;
	delete[] particleFlags;
	delete[] vertices;
}
unsigned int emitter::addParticle(particle& p)
{
	for(int i = 0; i < total; i++)
	{
		if((world.time() - particles[i].startTime) * particles[i].invLife > 1.0)
		{
			particles[i] = p;
			return i;
		}
	}

	if(total == compacity)
	{
		unsigned int tmpCompacity = (compacity != 0) ? compacity * 2 : 64;

		particle* tmpParticles			= new particle[tmpCompacity];
		Vec3f* tmpOldParticlePositions	= new Vec3f[tmpCompacity];
		unsigned char* tmpParticleFlags	= new unsigned char[tmpCompacity];
		vertex *tmpVertices				= new vertex[tmpCompacity*4];

		memcpy(tmpParticles,particles,compacity*sizeof(particle));
		memcpy(tmpOldParticlePositions,oldParticlePositions,compacity*sizeof(Vec3f));
		memcpy(tmpParticleFlags,particleFlags,compacity*sizeof(unsigned char));
		memcpy(tmpVertices,vertices,compacity*sizeof(vertex)*4);

		delete[] particles;
		delete[] oldParticlePositions;
		delete[] particleFlags;
		delete[] vertices;

		particles = tmpParticles;
		oldParticlePositions = tmpOldParticlePositions;
		particleFlags = tmpParticleFlags;
		vertices = tmpVertices;

		compacity = tmpCompacity;
	}
	particles[total] = p;
	oldParticlePositions[total] = p.pos;
	particleFlags[total] = 0;
	return total++;
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
		if((world.time() - particles[i].startTime) * particles[i].invLife < 1.0)
			particleFlags[i] &= ~FADE_IN;
	}

	
	if(active)
	{
		unsigned int pNum;
		particle p;
		extraTime += ms;
		while(particlesPerSecond > 0.0 && extraTime > (1000.0 / particlesPerSecond))
		{
			extraTime -= 1000.0 / particlesPerSecond;

			particle p;
			float t = (ms-extraTime)/ms;
			if(createParticle(p,position*t + lastPosition*(1.0-t)))
			{
				pNum = addParticle(p);
				particleFlags[pNum] |= FADE_IN;
			}
		}
	}

	liveParticles = 0;
	for(int i=0; i < total; i++)
	{
		if((world.time() - particles[i].startTime) * particles[i].invLife < 1.0)
		{
			liveParticles++;
		}
	}

	for(int i=0; i < total; i++)
	{
		oldParticlePositions[i] = particles[i].pos;
	}
	for(int i=0; i < total; i++)
	{
		updateParticle(particles[i]);
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
		if((world.time() - particles[pNum].startTime) * particles[pNum].invLife < 1.0)
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = particles[pNum].r;
				vertices[vNum*4 + n].g = particles[pNum].g;
				vertices[vNum*4 + n].b = particles[pNum].b;
				vertices[vNum*4 + n].a = ((particleFlags[pNum]&FADE_IN) && !additiveBlending) ? particles[pNum].a*interpolate : particles[pNum].a;
			}
			sAng = sin(particles[pNum].ang);
			cAng = cos(particles[pNum].ang);
			u = -right * sAng + up * cAng;
			r = right * cAng + up * sAng;

			pos = lerp(oldParticlePositions[pNum], particles[pNum].pos, interpolate);

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
		if(IBOnumQuads > vNum)
		{	
			IBOnumQuads = vNum + 1024;
			unsigned short* iboContents = new unsigned short[IBOnumQuads*6];

			for(unsigned int i=0; i<IBOnumQuads; i++)
			{
				iboContents[i*6 + 0] = i*4 + 0;
				iboContents[i*6 + 1] = i*4 + 1;
				iboContents[i*6 + 2] = i*4 + 2; 

				iboContents[i*6 + 3] = i*4 + 0; 
				iboContents[i*6 + 4] = i*4 + 2;
				iboContents[i*6 + 5] = i*4 + 3;
			}

			quadIBO = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);// Changes should be so rare (never?) that static is still the best option 
			quadIBO->setData(iboContents, IBOnumQuads*6);

			delete[] iboContents;
		}

		dataManager.bind(texture);
		quadIBO->drawBufferSegment(GraphicsManager::TRIANGLES, VBO, min(vNum*6, IBOnumQuads*6));
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
		if((world.time() - particles[pNum].startTime) * particles[pNum].invLife < 1.0)
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = particles[pNum].r;
				vertices[vNum*4 + n].g = particles[pNum].g;
				vertices[vNum*4 + n].b = particles[pNum].b;
				vertices[vNum*4 + n].a = ((particleFlags[pNum]&FADE_IN) && !additiveBlending) ? particles[pNum].a*interpolate : particles[pNum].a;
			}
			sAng = sin(particles[pNum].ang);
			cAng = cos(particles[pNum].ang);
			u = -right * sAng + up * cAng;
			r = right * cAng + up * sAng;

			pos = interpolatedPos + rotation * (parentOffset + lerp(oldParticlePositions[pNum], particles[pNum].pos, interpolate));

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
		if((world.time() - particles[pNum].startTime) * particles[pNum].invLife < 1.0)
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = particles[pNum].r;
				vertices[vNum*4 + n].g = particles[pNum].g;
				vertices[vNum*4 + n].b = particles[pNum].b;
				vertices[vNum*4 + n].a = particles[pNum].a;
			}

			start = particles[pNum].pos;
			end = particles[pNum].pos - particles[pNum].velocity.normalize()*particles[pNum].size;

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
	const unsigned int numQuads = 4096;
	unsigned short* iboContents = new unsigned short[numQuads*6];

	for(unsigned int i=0; i<numQuads; i++)
	{
		iboContents[i*6 + 0] = i*4 + 0;
		iboContents[i*6 + 1] = i*4 + 1;
		iboContents[i*6 + 2] = i*4 + 2; 

		iboContents[i*6 + 3] = i*4 + 0; 
		iboContents[i*6 + 4] = i*4 + 2;
		iboContents[i*6 + 5] = i*4 + 3;
	}

	emitter::quadIBO = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
	emitter::quadIBO->setData(iboContents, numQuads*6);
	emitter::IBOnumQuads = numQuads;

	delete[] iboContents;
}
void manager::addEmitter(shared_ptr<emitter> e, Vec3f position, float radius)
{
	emitters.push_back(e);
	e->setPositionAndRadius(position, radius);
	e->init();
}
void manager::addEmitter(shared_ptr<emitter> e, int parent, Vec3f offset)
{
	auto parentPtr = world[parent];
	if(parentPtr)
	{
		emitters.push_back(e);
		auto mesh = world[parent]->meshInstance;
		e->setPositionAndRadius(world[parent]->position + world[parent]->rotation * offset, mesh ? mesh->getBoundingSphere().radius : 0.0);
		e->setParent(parent, offset);
		e->init();
	}
	else
	{
		debugBreak();
	}
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
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		if((*i)->visible)
		{
			(*i)->prepareRender(up,right);
		}
	}
	auto Pred = [view](shared_ptr<emitter> e1, shared_ptr<emitter> e2)
	{
		return view->project3(e1->position).z < view->project3(e2->position).z;
	};

	sort(emitters.begin(), emitters.end(), Pred);

	auto particleShader = shaders.bind("particle shader");
	particleShader->setUniform1i("tex",0);
	particleShader->setUniform1i("depth",1);
	particleShader->setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());
	particleShader->setUniform2f("invScreenDims",1.0/sw, 1.0/sh);


	
	for(auto i = emitters.begin(); i!=emitters.end(); i++)
	{
		if(!(*i)->additiveBlending && (*i)->visible)
		{
			graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
			(*i)->render();
		}
		else if((*i)->additiveBlending && (*i)->visible)
		{
			graphics->setBlendMode(GraphicsManager::ALPHA_ONLY);
			(*i)->render();

			graphics->setBlendMode(GraphicsManager::ADDITIVE);
			(*i)->render();
		}
	}


	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
}
void manager::shutdown()
{
	emitters.clear();
	emitter::quadIBO.reset();
}
}
