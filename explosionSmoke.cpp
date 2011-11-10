
#include "game.h"

namespace particle
{
	explosionSmoke::explosionSmoke(Vec3f pos, float r): emitter(EXPLOSION, pos, "smoke", 0.3, 0.0, 16, true)
	{
		radius = r;
		velocity =	fuzzyAttribute(8.0*radius, 0.0);
		spread =	fuzzyAttribute(radius*2, radius);
		life =		fuzzyAttribute(600.0);

		particle p;
		for(int i = 0; i < 16; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			Vec3f dir = random<Vec3f>();
			p.vel = dir * velocity();
			p.pos = pos + dir * spread() + p.vel * extraTime/1000.0;

			p.size = 0.0;

			float g = random<float>(0.1)+0.15;

			p.r = g;
			p.g = g;
			p.b = g;
			p.a = 0;

			addParticle(p);
		}
	}
	explosionSmoke::explosionSmoke(int parent, Vec3f offset): emitter(EXPLOSION, parent, offset, "smoke", 0.3, 0.0, 16, true)
	{
		
		radius = dataManager.getModel(world[parent]->type)->boundingSphere.radius / 2;

		velocity =	fuzzyAttribute(8.0*radius, 0.0);
		spread =	fuzzyAttribute(radius*2, radius);
		life =		fuzzyAttribute(600.0);

		particle p;
		for(int i = 0; i < 16; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			Vec3f dir = random<Vec3f>();
			p.vel = dir * velocity();
			p.pos = world[parent]->position + dir * spread() + p.vel * extraTime/1000.0;

			p.size = 0.0;

			float g = random<float>(0.1)+0.15;

			p.r = g;
			p.g = g;
			p.b = g;
			p.a = 0;

			addParticle(p);
		}
	}
	void explosionSmoke::updateParticle(particle& p)
	{
		p.vel *= pow(friction, (float)world.time.length()/1000.0f);	
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);

		if(t<0.6)
		{
			t = (t)/0.6;
			p.a = 0.5 + 0.5*t;
			p.size = t * 4.0*radius;
		}
		else
		{
			t = (t-0.6)/0.4;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 4.0*radius + t * 8.0*radius;
		//	p.pos.y += world.time.length()/100;
		}
	}
}