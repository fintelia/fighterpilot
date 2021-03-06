
#include "game.h"

namespace particle
{
	blackSmoke::blackSmoke(): emitter("smoke", 36, 4.0)
	{

	}
	void blackSmoke::init()
	{
		speed =	fuzzyAttribute(0.0);
		spread =	fuzzyAttribute(1.0, 0.5);
		life =		fuzzyAttribute(7000);
	}
	bool blackSmoke::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.invLife = 1.0 / life();

		p.velocity = random3<float>() * speed();
		p.pos = currentPosition + random3<float>()*spread() + p.velocity * extraTime/1000.0;

		p.size = 5.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		p.ang = random<float>(2.0*PI);
		p.angularSpeed = random<float>(0.2);
		p.r = 0;
		p.g = 0;
		p.b = 0;
		p.a = 0;
		return true;
	}
	void blackSmoke::updateParticle(particle& p)
	{
		p.pos += p.velocity * world.time.length()/1000.0;
		p.ang += p.angularSpeed * world.time.length()/1000.0;
		p.pos.y += world.time.length()/100;
		float t = (world.time() - p.startTime);
		if(t < 300.0)
		{
			p.a = t / 300.0 * 0.75;
			p.size = 10.0;
		}
		else
		{
			p.a = (1.0 - (t-300.0) / 6700) * 0.75;
			p.size = 10.0 + 15.0 * (t-300.0) / 6700;
		}
	}
}
