
#include "main.h"

namespace particle
{

	void blackSmoke::init()
	{
		velocity = fuzzyAttribute(3.0, 1.0);
		spread = fuzzyAttribute(1.0, 0.5);
		life = fuzzyAttribute(6000.0,100.0);

		friction = 1.0;
		particlesPerSecond = 3.0;

		glGenBuffers(1,&VBO);
	}
	blackSmoke::blackSmoke(Vec3f pos): emitter(EXPLOSION,"particle",64)
	{
		position = pos;
		init();
	}
	blackSmoke::blackSmoke(int parent, Vec3f offset): emitter(EXPLOSION,"particle",64)
	{
		parentObject = parent;
		parentOffset = offset;
		position = world.objectList[parentObject]->position + world.objectList[parentObject]->rotation * parentOffset;
		init();
	}
	void blackSmoke::update()
	{
		static Vec3f lastPos = position;
		double ms = world.time.getLength();
		double time = world.time();
	
		if(parentObject != 0)
		{
			position = world.objectList[parentObject]->position + world.objectList[parentObject]->rotation * parentOffset;
		}

		particle p;
		extraTime += ms;
		while(extraTime > particlesPerSecond)
		{
			extraTime -= 1000.0 / particlesPerSecond;

			p.startTime = time - extraTime;
			p.endTime = time + life();
		
			p.vel = random<Vec3f>() * velocity();
			p.pos = position*(1.0-(ms-extraTime)/ms) + lastPos*(ms-extraTime)/ms + random<Vec3f>()*spread() + p.vel * extraTime/1000.0;

			p.size = 0.0;

			p.r = 0;
			p.g = 0;
			p.b = 0;
			p.a = 0;

			addParticle(p);
		}

		for(int i=0; i < total; i++)
		{
			particles[i].pos += particles[i].vel * ms/1000.0;
			float t = (time - particles[i].startTime) / (particles[i].endTime - particles[i].startTime);
			if(t<0.05)
			{
				particles[i].a = t * 20.0;
				particles[i].size = 10;
			}
			else
			{
				t = (t-0.05)/0.99;
				particles[i].a = 1.0 - t;
				particles[i].size = (1.0-t) * 10.0 + t * 30.0;
				particles[i].pos.y += ms/100;
			}
		}
		lastPos = position;
	}
	void blackSmoke::render(Vec3f up, Vec3f right)
	{
		double time = world.time();

		int pNum,vNum,n;
		for(pNum = 0, vNum=0; pNum < total; pNum++)
		{
			if(particles[pNum].endTime > time)
			{
				for(n = 0; n<4; n++)
				{
					vertices[vNum*4 + n].r = particles[pNum].r;
					vertices[vNum*4 + n].g = particles[pNum].g;
					vertices[vNum*4 + n].b = particles[pNum].b;
					vertices[vNum*4 + n].a = particles[pNum].a;

					vertices[vNum*4 + n].energy = (time - particles[pNum].startTime) / (particles[pNum].endTime - particles[pNum].startTime);
				}
				vertices[vNum*4 + 0].position = particles[pNum].pos + up * particles[pNum].size + right * particles[pNum].size;
				vertices[vNum*4 + 1].position = particles[pNum].pos + up * particles[pNum].size - right * particles[pNum].size;
				vertices[vNum*4 + 2].position = particles[pNum].pos - up * particles[pNum].size - right * particles[pNum].size;
				vertices[vNum*4 + 3].position = particles[pNum].pos - up * particles[pNum].size + right * particles[pNum].size;

				vertices[vNum*4 + 0].s = 0.0;	vertices[vNum*4 + 0].t = 0.0;
				vertices[vNum*4 + 1].s = 0.0;	vertices[vNum*4 + 1].t = 1.0;
				vertices[vNum*4 + 2].s = 1.0;	vertices[vNum*4 + 2].t = 1.0;
				vertices[vNum*4 + 3].s = 1.0;	vertices[vNum*4 + 3].t = 0.0;

				vNum++;
			}
		}

		dataManager.bind("smoke");

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vNum*4, vertices, GL_DYNAMIC_DRAW);

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
}