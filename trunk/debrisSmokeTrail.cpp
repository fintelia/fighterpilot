
#include "game.h"

namespace particle
{
	debrisSmokeTrail::debrisSmokeTrail(): emitter("smoke", 30, 120.0), color(0.25,0.25,0.25)
	{

	}
	void debrisSmokeTrail::init()
	{
		velocity =	fuzzyAttribute(0.0);
		spread =	fuzzyAttribute(1.0, 1.0);
		life =		fuzzyAttribute(2000.0, 20.0);

		emitterVelocity = (random3<float>() + Vec3f(0,1,0)) * random<float>(10.0, 25.0);
	}
	void debrisSmokeTrail::update()
	{
		double ms = world.time.length();
		double time = world.time();

		lastPosition = position;

		position += emitterVelocity * world.time.length()/1000;
		emitterVelocity.y -= 9.8 * world.time.length()/1000;
	
		for(int i=0; i < total; i++)
		{
			if(particles[i].endTime > time)
				particles[i].fadeIn = false;
		}

		if(active)
		{
			particle p;
			extraTime += ms;
			while(particlesPerSecond > 0.0 && extraTime > (1000.0 / particlesPerSecond))
			{
				extraTime -= 1000.0 / particlesPerSecond;

				particle p;
				float t = (ms-extraTime)/ms;
				if(createParticle(p,position*t + lastPosition*(1.0-t)))
				{
					p.fadeIn = true;
					addParticle(p);
				}
			}
		}

		liveParticles = 0;
		for(int i=0; i < total; i++)
		{
			if(particles[i].endTime > time)
			{
				liveParticles++;
			}
		}

		for(int i=0; i < total; i++)
		{
			if(particles[i].endTime > time)
			{
				particles[i].lastPos = particles[i].pos;
				updateParticle(particles[i]);
			}
		}
	}
	bool debrisSmokeTrail::createParticle(particle& p, Vec3f currentPosition)
	{
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
	void debrisSmokeTrail::updateParticle(particle& p)
	{
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.2)
		{
			p.a = 0.5;//(t * 5.0)*(t * 5.0);
			p.size = lerp(1.0, 2.0, t);
		}
		else
		{
			t = (t-0.01)/0.99;
			p.a = 0.5* (1.0 - t);
			p.size = (1.0-t) * 2.0 + t * 3.0;
			//p.pos.y += world.time.length()/100;
		}
	}
	void debrisSmokeTrail::setColor(Color c)
	{
		color = c;
	}
}
