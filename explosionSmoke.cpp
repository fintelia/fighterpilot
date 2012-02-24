
#include "game.h"

namespace particle
{
	//explosionSmoke::explosionSmoke(): emitter(EXPLOSION, pos, "smoke", 0.3, 0.0, 16, true){}
	//{
	//	radius = r;
	//	velocity =	fuzzyAttribute(8.0*radius, 0.0);
	//	spread =	fuzzyAttribute(radius*2, radius);
	//	life =		fuzzyAttribute(600.0);

	//	particle p;
	//	for(int i = 0; i < 16; i++)
	//	{
	//		p.startTime = world.time();
	//		p.endTime = world.time() + life();
	//	
	//		Vec3f dir = random3<float>();
	//		p.vel = dir * velocity();
	//		p.pos = pos + dir * spread() + p.vel * p.speed * extraTime/1000.0;

	//		p.size = 0.0;

	//		float g = random<float>(0.1)+0.35;

	//		p.r = g;
	//		p.g = g;
	//		p.b = g;
	//		p.a = 0;

	//		addParticle(p);
	//	}
	//}
	explosionSmoke::explosionSmoke(): emitter("smoke", 64, 0.0, false)
	{
	
	}
	void explosionSmoke::init()
	{
		particle p;
		for(int i = 0; i < 48; i++)
		{
			p.startTime = world.time();
			p.endTime = p.startTime + 5000;

			Vec3f dir = random3<float>();
			p.vel = dir * radius;
			p.pos = position + dir * random<float>(radius*0.5,radius*1.5) + p.vel * extraTime/1000.0;

			p.size = radius*1.5;

			
			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.1*PI,0.1*PI);

			float g = random<float>(0.1)+0.25;

			p.r = g;
			p.g = g;
			p.b = g;
			p.a = 0;

			addParticle(p);
		}
	}
	void explosionSmoke::updateParticle(particle& p)
	{
		p.vel *= pow(0.2, world.time.length()/1000.0f);	
		p.pos += p.vel * world.time.length()/1000.0;

		float t = world.time() - p.startTime;

		p.ang += p.angularSpeed * world.time.length() / 1000.0;

		if(t > 500)
		{
			if(t < 1000)
			{
				p.a = 0.3 * ((t-500)/500);
				p.size = (1.0 + t/1000) * 0.75 * radius;
			}
			else if(t < 2500)
			{
				p.a = 0.3;
				p.size = 1.5 * radius;
			}
			else
			{
				p.a = 0.3 - 0.33 * ((t-2500)/3000);
			}

			p.pos.y += world.time.length()/300;
		}
	}


	flakExplosionSmoke::flakExplosionSmoke(): emitter("smoke", 64, 0.0, false)
	{
	
	}
	void flakExplosionSmoke::init()
	{
		particle p;
		for(int i = 0; i < 64; i++)
		{
			p.startTime = world.time();
			p.endTime = p.startTime + 5000;

			Vec3f dir = random3<float>();
			float r = random<float>();
			p.vel = dir * r*r*radius*20.0;//dir * radius;
			p.pos = position + /*dir * r*r*radius*2.0 + */ p.vel * extraTime/1000.0;

			p.size = radius*1.5;

			
			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.1*PI,0.1*PI);

			float g = random<float>(0.05)+0.3;

			p.r = g;
			p.g = g;
			p.b = g;
			p.a = 0;

			addParticle(p);
		}
	}
	void flakExplosionSmoke::updateParticle(particle& p)
	{
		//p.vel *= pow(0.2, world.time.length()/1000.0f);	
		//p.pos += p.vel * world.time.length()/1000.0;
		float t = world.time() - p.startTime;

		if(t < 100)
		{
			p.pos += p.vel * world.time.length()/1000.0;
		}
		else
		{
			p.pos += p.vel * 0.01 * world.time.length()/1000.0;
		}
		p.ang += p.angularSpeed * world.time.length() / 1000.0;

		if(t < 150)
		{
			p.a = 0.8 * (t/150);
			p.size = (t/150) * 1.0 * radius;
		}
		else if(t < 2000)
		{
			p.a = 0.8;
			p.size = 1.0 * radius;
		}
		else
		{
			p.a = 0.8 - 1.0 * ((t-2000)/3000);
		}

		p.pos.y += world.time.length()/200;
	}
}