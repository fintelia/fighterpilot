

#include "game.h"

namespace particle
{
	smokeTrail::smokeTrail(): emitter("smoke", 30, 120.0)
	{

	}
	void smokeTrail::init()
	{
		velocity =	fuzzyAttribute(0.0);
		spread =	fuzzyAttribute(1.0, 1.0);
		life =		fuzzyAttribute(3200.0);
	}
	bool smokeTrail::createParticle(particle& p, Vec3f currentPosition)
	{
		if(!world[parentObject] || !(world[parentObject]->type & PLANE) /*|| ((nPlane*)world[parentObject].get())->death != nPlane::DEATH_TRAILING_SMOKE*/)
		{
			particlesPerSecond = 0;
			return false;
		}

		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();

		p.vel = random3<float>() * velocity();
		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 1.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		p.ang = random<float>(2.0*PI);

		float g = random<float>(-0.05,0.05);

		p.r = color.r + g;
		p.g = color.b + g;
		p.b = color.g + g;
		p.a = 0.5;
		return true;
	}
	void smokeTrail::updateParticle(particle& p)
	{
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.2)
		{
			p.a = 0.5;//(t * 5.0)*(t * 5.0);
			p.size = 1.0 + 4.0*(t*5.0);
		}
		else
		{
			t = (t-0.01)/0.99;
			p.a = 0.5 * (1.0 - t);
			p.size = lerp(5.0, 10.0, t);
			//p.pos.y += world.time.length()/100;
		}
	}
	void smokeTrail::setColor(Color c)
	{
		color = c;
	}
}
