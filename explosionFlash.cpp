
#include "engine.h"
#include "game.h"

namespace particle
{
	explosionFlash::explosionFlash(): emitter("flash", 1, 0.0, true)
	{

	}
	void explosionFlash::init()
	{
		particle p;
		p.startTime = world->time();
		p.invLife = 1.0 / 400.0;

		p.pos = position + random3<float>() * spread();

		p.ang = random<float>(2.0*PI);

		p.size = 0.0;

		p.r = 1.0;
		p.g = 1.0;
		p.b = 0.4;
		p.a = 0;

		
		addParticle(p);
	}

	void explosionFlash::updateParticle(particle& p)
	{
		//p.vel *= pow(friction, (float)world->time.length()/1000.0f);
		//p.pos += p.vel * world->time.length()/1000.0;
		float t = (world->time() - p.startTime) * p.invLife;

		float e = world->elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		if(t<0.5)
		{
			p.a = t * 2.0;
			p.size = t * 2.0 * 0.75 * radius;

		}
		else
		{
			t = (t-0.5)/0.5;
			//p.r = 1.0-0.6*t;
			//p.g = 0.3+0.1*t;
			//p.b = 0.1+0.3*t;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 0.75  * radius + t * 1.5 * radius;
		//	p.pos.y += world->time.length()/100;
		}
	}
}
