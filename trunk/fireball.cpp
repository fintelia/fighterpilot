

#include "game.h"

namespace particle
{
	fireball::fireball(): emitter("smoke2", 32, 300.0, true)
	{

	}
	void fireball::init()
	{
		speed =		fuzzyAttribute(0.0);
		spread =	fuzzyAttribute(0.2, 0.2);
		life =		fuzzyAttribute(60.0,15.0);
	}
	bool fireball::createParticle(particle& p, Vec3f currentPosition)
	{
		if(!world[parentObject] || !(world[parentObject]->type & PLANE) /*|| ((nPlane*)world[parentObject].get())->death != nPlane::DEATH_TRAILING_SMOKE*/)
		{
			particlesPerSecond = 0;
			return false;
		}

		p.startTime = world.time() - extraTime;
		p.invLife = 1.0 / life();

		p.velocity = random3<float>(); //note: velocity only stores direction
		p.pos = position + p.velocity * spread() /*+ p.velocity * radius * 0.3f*(extraTime-1.0)*(extraTime-1.0)*extraTime*p.invLife*/;
		p.size = 0.0;

		p.ang = random<float>(2.0*PI);
		p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

		p.r = 1.0;
		p.g = 0.48;
		p.b = 0.17;
		p.a = 0;

		return true;
	}
	void fireball::update()
	{
		emitter::update();
		if(world[parentObject] && world[parentObject]->type & PLANE && (dynamic_pointer_cast<nPlane>(world[parentObject])->death == nPlane::DEATH_HIT_GROUND ||dynamic_pointer_cast<nPlane>(world[parentObject])->death == nPlane::DEATH_HIT_WATER))
		{
			particlesPerSecond = 0;
		}
	}
	void fireball::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) * p.invLife;
		//p.pos += p.velocity * radius * 0.3f*(t-1.0)*(t-1.0)*world.time.length()*p.invLife; //NOTE: velocity is actually just a direction

		p.ang += p.angularSpeed * world.time.length() / 1000.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		if(t<0.05)
		{
			p.a = t * 20.0 * 0.75 * 3.0;
			p.size = (t / 0.05 + 1.0) * 0.5 * 0.3 * radius;
		}
		else if(t<0.75)
		{
			t = (t-0.05)/0.7;
			p.a = 0.75* 3.0;
			p.size = (1.0-t) * 0.3 * radius + t * radius*0.5;		
		}
		else
		{
			t = (t-0.75)/0.25;
			p.a = (1.0 - sqrt(t)) * 0.75* 3.0;

			p.r = 0.6*t + (1.0-t)*1.0;
			p.g = 0.6*t + (1.0-t)*0.48;
			p.b = 0.6*t + (1.0-t)*0.17;

			p.size = radius*0.5;
		//	p.pos.y += world.time.length()/100;
		}
	}
}
