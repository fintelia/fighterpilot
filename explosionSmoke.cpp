
#include "main.h"

namespace particle
{
	explosionSmoke::explosionSmoke(Vec3f pos): emitter(EXPLOSION, pos, "explosion fireball", 1.0, 10.0, 64), trl(NULL)
	{
		velocity =	fuzzyAttribute(3.0, 1.0);
		spread =	fuzzyAttribute(8.0, 5.5);
		life =		fuzzyAttribute(3000.0, 100.0);
	}
	explosionSmoke::explosionSmoke(int parent, Vec3f offset): emitter(EXPLOSION, parent, offset, "smoke", 0.1, 0.0, 32)
	{
		trl = dataManager.getModel(world.objectList[parent]->type);
		velocity =	fuzzyAttribute(30.0, 0.0);
		spread =	fuzzyAttribute(trl->getRadius(), trl->getRadius()/2);
		life =		fuzzyAttribute(1500.0);

		particle p;
		for(int i = 0; i < 32; i++)
		{
			
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			Vec3f dir = random<Vec3f>();
			p.vel = dir * velocity();
			p.pos = world.objectList[parent]->position + dir * spread() + p.vel * extraTime/1000.0;

			p.size = 0.0;

			float g = random<float>(0.1)+0.3;

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

		if(t<0.15)
		{

		}
		else if(t<0.6)
		{
			t = (t- 0.15)/0.45;
			p.a = t;
			p.size = t * 10.0;
		}
		else
		{
			t = (t-0.6)/0.4;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 10.0 + t * 20.0;
		//	p.pos.y += world.time.length()/100;
		}
	}
}