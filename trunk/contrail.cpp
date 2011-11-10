
#include "game.h"

namespace particle
{
	contrail::contrail(int parent, Vec3f offset): emitter(CONTRAIL_LARGE, parent, offset, "smoke", 1.0, 75.0, 48)
	{
		//velocity =	fuzzyAttribute(0.2, 0.1);
		//spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(8000.0, 1000.0);
	}
	bool contrail::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();
		
		p.vel = random<Vec3f>() * velocity();
		p.pos = currentPosition + random<Vec3f>()*spread() + p.vel * extraTime/1000.0;

		p.size = 7.5;

		p.r = 0.6;
		p.g = 0.6;
		p.b = 0.6;
		p.a = 0.0;
		return true;
	}
	void contrail::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.10)
		{
			p.a = t * 10.0;
			p.size = 7.5;//7.0 * (t * 20.0);
		}
		else if(t < 0.75)
		{
			t = (t-0.10)/0.65;
			p.a = 1.0 - 0.75*t;
			p.size = (1.0-t) * 7.5 + t * 15.0;
		}
		else
		{
			t = (t-0.75)/0.25;
			p.a = 0.25 - 0.25*t;
			p.size = (1.0-t) * 15.0 + t * 25.0;
		}
	}
}