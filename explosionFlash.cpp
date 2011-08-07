
#include "main.h"

namespace particle
{
	explosionFlash::explosionFlash(Vec3f pos): emitter(EXPLOSION_FLASH, pos, "fire", 0.1, 0.0, 64,true), trl(NULL)
	{
		velocity =	fuzzyAttribute(2.0, 0.5);
		spread =	fuzzyAttribute(4.0, 3.5);
		life =		fuzzyAttribute(1000.0, 100.0);
	}
	explosionFlash::explosionFlash(int parent, Vec3f offset): emitter(EXPLOSION, parent, offset, "smoke", 0.1, 0.0, 32,true)
	{
		trl = dataManager.getModel(world.objectList[parent]->type);
		velocity =	fuzzyAttribute(35.0, 0.0);
		spread =	fuzzyAttribute(trl->getRadius()/4, trl->getRadius()/4);
		life =		fuzzyAttribute(70.0,30.0);
		//color =		fuzzyColor(1.0,1.0,0.8);

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
			p.g = 1.0;
			p.b = 0.4;
			p.a = 0;

			addParticle(p);
		}
	}
	void explosionFlash::updateParticle(particle& p)
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
			p.size = t * 60.0;

		}
		else
		{
			t = (t-0.05)/0.95;
			//p.r = 1.0-0.6*t;
			//p.g = 0.3+0.1*t;
			//p.b = 0.1+0.3*t;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 3.0 + t * 6.0;
		//	p.pos.y += world.time.length()/100;
		}
	}
}