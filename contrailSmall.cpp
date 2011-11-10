
#include "game.h"

namespace particle
{
	contrailSmall::contrailSmall(int parent, Vec3f offset): emitter(CONTRAIL_SMALL, parent, offset, "particle", 1.0, 300.0, 48)
	{
		//velocity =	fuzzyAttribute(0.2, 0.1);
		//spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(200.0);
	}
	bool contrailSmall::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();
		
		p.vel = random<Vec3f>() * velocity();
		p.pos = currentPosition + random<Vec3f>()*spread() + p.vel * extraTime/1000.0;

		p.size = 0.0;

		p.r = 0.8;
		p.g = 0.8;
		p.b = 0.8;
		p.a = 0.0;
		return true;
	}
	void contrailSmall::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.1)
		{
			p.a = t * 10.0;
			p.size = 2.0 * (t * 10.0);
		}
		else
		{
			t = (t-0.1)/0.9;
			p.a = 1.0 - t;
			p.size = 2.0 - 0.5*t;
		}
	}
}

