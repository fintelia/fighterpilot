

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
		if(!world->getObjectById(parentObject) || !(world->getObjectById(parentObject)->type & PLANE) /*|| ((nPlane*)world->getObjectById(parentObject).get())->death != nPlane::DEATH_TRAILING_SMOKE*/)
		{
			particlesPerSecond = 0;
			return false;
		}

		p.startTime = world->time() - extraTime;
		p.invLife = 1.0 / life();

		p.velocity = random3<float>() * speed();
		p.pos = currentPosition + random3<float>()*spread() + p.velocity * extraTime/1000.0;

		p.size = 3.5;

		float e = world->elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		p.ang = random<float>(2.0*PI);

		float g = random<float>(-0.05,0.05);

		p.r = color.r + g;
		p.g = color.b + g;
		p.b = color.g + g;
		p.a = 0.0;
		return true;
	}
	void smokeTrail::updateParticle(particle& p)
	{
		p.pos += p.velocity * world->time.length() * bullet::bulletSpeed;
		float t = (world->time() - p.startTime) * p.invLife;
		if(t<0.2f)
		{
			p.a = t*5.0f;//0.5;//(t * 5.0)*(t * 5.0);
			p.size = 3.5f + 4.0f*(t*5.0f);
			float i = min(t*50.0f,1.0f);
			p.r = i*color.r + (1.0f-i)*1.0f;
			p.g = i*color.b + (1.0f-i)*0.48f;
			p.b = i*color.g + (1.0f-i)*0.17f;
		}
		else
		{
			t = (t-0.2f) * 1.25f;
			p.a = 0.5f * (1.0f - t);
			p.size = lerp(7.5f, 35.0f, t*t);
			//p.pos.y += world->time.length()/100;
		}
	}
	void smokeTrail::setColor(Color c)
	{
		color = c;
	}
}
