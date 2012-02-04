
#include "game.h"

namespace particle
{
	contrail::contrail(): emitter(CONTRAIL_LARGE, "contrail", 1.0, 160.0, 48)
	{

	}
	void contrail::init()
	{
		//velocity =	fuzzyAttribute(0.2, 0.1);
		//spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(8000.0, 500.0);
	}
	bool contrail::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();

		p.vel = random3<float>() * velocity();
		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 3.0;
		p.r = 0.0;
		p.g = 0.0;
		p.b = 0.0;
		p.a = 0.0;

		p.ang = random<double>(0.0,2.0*PI);

		return true;
	}
	void contrail::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.10)
		{
			p.a = t * 10.0;
			p.size = 3.0;
		}
		else if(t < 0.75)
		{
			t = (t-0.10)/0.65;
			p.a = 1.0 - 0.75*t;
			p.size = (1.0-t) * 3.0 + t * 9.0;
		}
		else
		{
			t = (t-0.75)/0.25;
			p.a = 0.25 - 0.25*t;
			p.size = (1.0-t) * 9.0 + t * 15.0;
		}
		p.r = p.g = p.b = 1.0;
	}
}
