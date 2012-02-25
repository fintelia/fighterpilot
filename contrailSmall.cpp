
#include "game.h"

namespace particle
{
	contrailSmall::contrailSmall(): emitter("particle", 48, 800.0, true)
	{

	}
	void contrailSmall::init()
	{
		//velocity =	fuzzyAttribute(0.2, 0.1);
		spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(50.0);
	}
	bool contrailSmall::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();

		p.vel = random3<float>() * velocity();
		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 0.0;

		//p.r = 0.8;
		//p.g = 0.8;
		//p.b = 0.8;
		p.r = 1.0;
		p.g = 0.48;
		p.b = 0.17;
		p.a = 0.0;
		return true;
	}
	void contrailSmall::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		//if(t<0.1)
		//{
		//	p.a = t * 10.0;
		//	p.size = 1.0 * (t * 10.0);
		//}
		//else
		//{
		//	t = (t-0.1)/0.9;
			p.a = 1.0 - t;
			p.size = 1.0 - 0.25*t;
		//}
	}
}

