

#include "game.h"

namespace particle
{
	smokeTrail::smokeTrail(): emitter("smoke", 64, 160.0)
	{

	}
	void smokeTrail::init()
	{
		speed =		fuzzyAttribute(0.0);
		spread =	fuzzyAttribute(1.0, 1.0);
		life =		fuzzyAttribute(5000.0);
	}
	bool smokeTrail::createParticle(particle& p, Vec3f currentPosition)
	{
		if(!world[parentObject] || !(world[parentObject]->type & PLANE) /*|| ((nPlane*)world[parentObject].get())->death != nPlane::DEATH_TRAILING_SMOKE*/)
		{
			particlesPerSecond = 0;
			return false;
		}

		p.startTime = world.time() - extraTime;
		p.invLife = 1.0 / life();

		p.velocity = random3<float>() * speed();
		p.pos = currentPosition + random3<float>()*spread() + p.velocity * extraTime/1000.0;

		p.size = 3.5;

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
		p.pos += p.velocity * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) * p.invLife;
		if(t<0.2)
		{
			p.a = 0.5;//(t * 5.0)*(t * 5.0);
			p.size = 3.5 + 4.0*(t*5.0);
			float i = min(t*50.0,1.0);
			p.r = i*color.r + (1.0-i)*1.0;
			p.g = i*color.b + (1.0-i)*0.48;
			p.b = i*color.g + (1.0-i)*0.17;
		}
		else
		{
			t = (t-0.2)/0.8;
			p.a = 0.5 * (1.0 - t);
			p.size = lerp(7.5, 35.0, t*t);
			//p.pos.y += world.time.length()/100;
		}
	}
	void smokeTrail::setColor(Color c)
	{
		color = c;
	}
}
