
#include "game.h"

namespace particle
{
	blackSmoke::blackSmoke(Vec3f pos): emitter(SMOKE, pos, "smoke", 1.0, 3.0, 19)
	{
		velocity =	fuzzyAttribute(3.0, 1.0);
		spread =	fuzzyAttribute(1.0, 0.5);
		life =		fuzzyAttribute(6000.0, 100.0);
	}
	blackSmoke::blackSmoke(int parent, Vec3f offset): emitter(SMOKE, parent, offset, "smoke", 1.0, 3.0, 19)
	{
		velocity =	fuzzyAttribute(3.0, 1.0);
		spread =	fuzzyAttribute(1.0, 0.5);
		life =		fuzzyAttribute(6000.0, 100.0);
	}
	bool blackSmoke::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();

		p.vel = random3<float>() * velocity();
		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 5.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		p.r = 0;
		p.g = 0;
		p.b = 0;
		p.a = 0;
		return true;
	}
	void blackSmoke::updateParticle(particle& p)
	{
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.05)
		{
			p.a = t * 20.0;
			p.size = 5.0;
		}
		else
		{
			t = (t-0.05)/0.99;
			p.a = 1.0 - t;
			p.size = (1.0-t) * 5.0 + t * 30.0;
			p.pos.y += world.time.length()/100;
		}
	}
}
