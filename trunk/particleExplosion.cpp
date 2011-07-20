
#include "main.h"

namespace particle
{
	explosion::explosion(Vec3f pos): emitter(EXPLOSION, pos, "explosion fireball", 1.0, 10.0, 64,true), trl(NULL)
	{
		velocity =	fuzzyAttribute(3.0, 1.0);
		spread =	fuzzyAttribute(8.0, 5.5);
		life =		fuzzyAttribute(3000.0, 100.0);
	}
	explosion::explosion(int parent, Vec3f offset): emitter(EXPLOSION, parent, offset, "fire", 0.1, 0.0, 32,true)
	{
		trl = dataManager.getModel(world.objectList[parent]->type);
		velocity =	fuzzyAttribute(30.0, 0.0);
		spread =	fuzzyAttribute(trl->getRadius()/2, trl->getRadius()/2);
		life =		fuzzyAttribute(600.0);
		color =		fuzzyColor(1.0,0.3,0.1);

		particle p;
		for(int i = 0; i < 32; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			Vec3f dir = random<Vec3f>();
			p.vel = dir * velocity();
			p.pos = world.objectList[parent]->position + dir * spread() + p.vel * extraTime/1000.0;

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
			p.size = t * 80.0;

		}
		else
		{
			t = (t-0.05)/0.95;
			//p.r = 1.0-0.6*t;
			//p.g = 0.3+0.1*t;
			//p.b = 0.1+0.3*t;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 4.0 + t * 20.0;
		//	p.pos.y += world.time.length()/100;
		}
	}
}