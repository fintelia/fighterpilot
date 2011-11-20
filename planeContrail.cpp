

#include "game.h"

namespace particle
{
	planeContrail::planeContrail(int parent, Vec3f offset): emitter(SMOKE, parent, offset, "smoke", 1.0, 1600.0, 401)
	{
		//velocity =	fuzzyAttribute(0.2, 0.1);
		//spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(125.0);
	}
	bool planeContrail::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();
		
		p.vel = random3<float>() * velocity();
		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 0.25;

		p.r = 0.5;
		p.g = 0.5;
		p.b = 0.5;
		p.a = 0.0;
		return true;
	}
	void planeContrail::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.01)
		{
			p.a = t * 100.0;
			p.size = 0.25;
		}
		else
		{
			t = (t-0.01)/0.99;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 0.25 + t * 0.5;
		}
	}
}