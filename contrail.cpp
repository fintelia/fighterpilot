
#include "game.h"

namespace particle
{
	contrail::contrail(): emitter("contrail", 48, 300.0)
	{

	}
	void contrail::init()
	{
		life =		fuzzyAttribute(8000.0, 1500.0);
	}
	bool contrail::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();

		p.vel = random3<float>() * velocity();
		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 3.0;
		p.r = 0.6;
		p.g = 0.6;
		p.b = 0.6;
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
			p.a = min(t*20.0,1.0) * 0.6;
			p.size = 2.0;
		}
		else if(t < 0.75)
		{
			t = (t-0.10)/0.65;
			p.a = lerp(0.6, 0.15, t);
			p.size = lerp(2.0, 3.5, t);
		}
		else
		{
			t = (t-0.75)/0.25;
			p.a = lerp(0.15, 0.0, t);
			p.size = lerp(3.5, 5.0, t);
		}

	}
}
