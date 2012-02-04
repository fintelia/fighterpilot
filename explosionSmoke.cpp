
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
	explosionSmoke::explosionSmoke(): emitter(EXPLOSION, "smoke", 0.3, 0.0, 16, false)
	{
	
	}
	void explosionSmoke::init()
	{
		velocity =	fuzzyAttribute(radius, 0.0);
		spread =	fuzzyAttribute(radius*1.5, radius);
		life =		fuzzyAttribute(5000.0);

		particle p;
		for(int i = 0; i < 16; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();

			Vec3f dir = random3<float>();
			p.vel = dir * velocity();
			p.pos = position + dir * spread() + p.vel * extraTime/1000.0;

			p.size = radius*1.5;

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
		p.vel *= pow(friction, (float)world.time.length()/1000.0f);	
		p.pos += p.vel * world.time.length()/1000.0;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		p.ang = 0.5*t;

		if(t<0.15)
		{
			//
		}
		else if(t<0.2)
		{
			p.a = 0.3 * ((t-0.15)/0.05);
			//p.size = (1.0 + t) * 0.5 * radius;
		}
		else if(t<0.5)
		{
			p.a = 0.3;
		}
		else
		{
			p.a = 0.3 - 0.33 * ((t-0.4)/0.6);
			//p.size = (1.0-t) * radius + (1.0 + t) * 0.5 * radius;
		//	p.pos.y += world.time.length()/100;
		}
	}
}