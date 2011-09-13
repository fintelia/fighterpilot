
#include "main.h"

namespace particle
{
	smokeTrail::smokeTrail(int parent, Vec3f offset): emitter(SMOKE, parent, offset, "smoke", 1.0, 20.0, 19)
	{
		velocity =	fuzzyAttribute(0.0);
		spread =	fuzzyAttribute(10.0, 3.0);
		life =		fuzzyAttribute(8000.0, 1500.0);
	}
	bool smokeTrail::createParticle(particle& p, Vec3f currentPosition)
	{
		if(!world[parentObject] || world[parentObject]->type & PLANE && ((nPlane*)world[parentObject].get())->death != nPlane::DEATH_TRAILING_SMOKE)
		{
			particlesPerSecond = 0;
			return false;
		}

		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();
		
		p.vel = random<Vec3f>() * velocity();
		p.pos = currentPosition + random<Vec3f>()*spread() + p.vel * extraTime/1000.0;

		p.size = 30.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		p.r = 0;
		p.g = 0;
		p.b = 0;
		p.a = 1;
		return true;
	}
	void smokeTrail::updateParticle(particle& p)
	{
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.01)
		{
			p.a = t * 100.0;
			p.size = 30.0;
		}
		else
		{
			t = (t-0.01)/0.99;
			p.a = 1.0 - t*t;
			p.size = (1.0-t) * 30.0 + t * 100.0;
			//p.pos.y += world.time.length()/100;
		}
	}
}