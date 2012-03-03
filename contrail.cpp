
#include "game.h"

namespace particle
{
	contrail::contrail(): emitter("contrail", 48, 250.0)
	{

	}
	void contrail::init()
	{
		//velocity =	fuzzyAttribute(0.2, 0.1);
		//spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(8000.0, 1500.0);
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
	void contrail::update()
	{
		emitter::update();
		position += random3<float>() * 0.5;
	}
	void contrail::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.10)
		{
			p.a = min(t*20.0,1.0) * 0.3;
			p.size = 3.0;
		}
		else if(t < 0.75)
		{
			t = (t-0.10)/0.65;
			p.a = (1.0 - 0.75*t) * 0.3;
			p.size = (1.0-t) * 3.0 + t * 9.0;
		}
		else
		{
			t = (t-0.75)/0.25;
			p.a = (0.25 - 0.25*t) * 0.3;
			p.size = (1.0-t) * 9.0 + t * 15.0;
		}
		p.r = p.g = p.b = 0.6;
	}
}
