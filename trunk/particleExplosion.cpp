
#include "main.h"

namespace particle
{
	explosion::explosion(Vec3f pos): emitter(EXPLOSION, pos, "smoke", 1.0, 3.0, 64)
	{
		velocity =	fuzzyAttribute(3.0, 1.0);
		spread =	fuzzyAttribute(1.0, 0.5);
		life =		fuzzyAttribute(6000.0, 100.0);
	}
	explosion::explosion(int parent, Vec3f offset): emitter(EXPLOSION, parent, offset, "smoke", 1.0, 3.0, 64)
	{
		velocity =	fuzzyAttribute(3.0, 1.0);
		spread =	fuzzyAttribute(1.0, 0.5);
		life =		fuzzyAttribute(6000.0, 100.0);
	}

	bool explosion::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() + life();
		
		p.vel = random<Vec3f>() * velocity();
		p.pos = currentPosition + random<Vec3f>()*spread() + p.vel * extraTime/1000.0;

		p.size = 0.0;

		p.r = 0;
		p.g = 0;
		p.b = 0;
		p.a = 0;
		return true;
	}
	void explosion::updateParticle(particle& p)
	{
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.05)
		{
			p.a = t * 20.0;
			p.size = 10;
		}
		else
		{
			t = (t-0.05)/0.99;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 10.0 + t * 30.0;
			p.pos.y += world.time.length()/100;
		}
	}
}