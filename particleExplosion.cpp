
#include "main.h"

namespace particle
{
	explosion::explosion(Vec3f pos, float Radius): emitter(EXPLOSION, pos, "fire", 0.1, 0.0, 32,true)
	{
		radius = Radius;
		velocity =	fuzzyAttribute(4.0*radius, 0.0);
		spread =	fuzzyAttribute(radius, radius);
		life =		fuzzyAttribute(600.0);
		color =		fuzzyColor(1.0,0.3,0.1);

		particle p;
		for(int i = 0; i < 32; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			Vec3f dir = random<Vec3f>();
			p.vel = dir * velocity();
			p.pos = pos + dir * spread() + p.vel * extraTime/1000.0;

			p.size = 0.0;

			p.r = 1.0;
			p.g = 0.5;
			p.b = 0.1;
			p.a = 0;

			addParticle(p);
		}
	}
	explosion::explosion(int parent, Vec3f offset): emitter(EXPLOSION, parent, offset, "fire", 0.1, 0.0, 32,true)
	{
		radius = dataManager.getModel(world[parent]->type)->boundingSphere.radius / 2;

		velocity =	fuzzyAttribute(30.0, 0.0);
		spread =	fuzzyAttribute(radius, radius);
		life =		fuzzyAttribute(600.0);
		color =		fuzzyColor(1.0,0.3,0.1);

		particle p;
		for(int i = 0; i < 32; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			Vec3f dir = random<Vec3f>();
			p.vel = dir * velocity();
			p.pos = world[parent]->position + dir * spread() + p.vel * extraTime/1000.0;

			p.size = 0.0;

			p.r = 1.0;
			p.g = 0.5;
			p.b = 0.1;
			p.a = 0;

			addParticle(p);
		}
	}
	void explosion::updateParticle(particle& p)
	{
		p.vel *= pow(friction, (float)world.time.length()/1000.0f);	
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		if(t<0.05)
		{
			p.a = t * 20.0;
			p.size = (t / 0.05) * radius;

		}
		else
		{
			t = (t-0.05)/0.95;
			//p.r = 1.0-0.6*t;
			//p.g = 0.3+0.1*t;
			//p.b = 0.1+0.3*t;
			p.a = 1.0 - t;
			p.size = (1.0-t) * radius + t * radius*4.0;
		//	p.pos.y += world.time.length()/100;
		}
	}
}