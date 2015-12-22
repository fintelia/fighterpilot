
#include "engine.h"
#include "game.h"

namespace particle
{
	groundExplosionFlash::groundExplosionFlash(): sparkEmitter("spark", 128, 0.0, true)
	{

	}
	void groundExplosionFlash::init()
	{
		speed =		fuzzyAttribute(20.0, 7.0);
		spread =	fuzzyAttribute(3.0, 3.0);
		life =		fuzzyAttribute(500,25);
		//color =	fuzzyColor(1.0,0.5,0.4);

		particle p;
		for(int i = 0; i < 128; i++)
		{
			p.startTime = world->time();
			p.invLife = 1.0 / life();
		
			Vec3f dir = (random3<float>() + Vec3f(0,1.5,0.0)).normalize();
			p.velocity = dir * speed();
			p.pos = position + dir * spread() + p.velocity * extraTime/1000.0;
			p.size = 0.0;

			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

			p.r = 1.0;
			p.g = 1.0;
			p.b = 0.5;
			p.a = 1.0;

			addParticle(p);
		}
	}
	void groundExplosionFlash::updateParticle(particle& p)
	{
		p.pos += p.velocity * world->time.length()/1000.0;

		float t = (world->time() - p.startTime) * p.invLife;

		if(t<0.05)
		{
			p.a = (t/0.05) * 0.75;
			p.size = (t / 0.05 + 1.0) * 0.5 * 0.3 * radius;
		}
		else if(t<0.50)
		{
			t = (t-0.03)/0.47;
			p.a = 0.75;
			p.size = (1.0-t) * 0.3 * radius + t * radius*0.7;		
		}
		else
		{
			t = (t-0.5)/0.5;
			p.a = (1.0 - t) * 0.75;

			p.size = radius*0.7 * (1.0-t*0.5);
		}
	}
}
