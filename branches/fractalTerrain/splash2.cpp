

#include "game.h"

namespace particle
{
	splash2::splash2(): sparkEmitter("splash", 256, 0.0, false)
	{

	}
	void splash2::init()
	{
		speed =	fuzzyAttribute(19.0, 10.0);
		spread =	fuzzyAttribute(radius/2, radius/2);
		life =		fuzzyAttribute(2000,300);

		particle p;
		for(int i = 0; i < 256; i++)
		{
			p.startTime = world->time();
			p.invLife = 1.0 / life();
		
			Vec3f dir = (random3<float>() + Vec3f(0,3.5,0.0)).normalize();
			p.velocity = dir * speed() * dir.y * dir.y;
			p.pos = position + Vec3f(dir.x,0,dir.z) * spread() + p.velocity * extraTime/1000.0;
			p.size = 0.0;

			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

			p.r = 1.0;
			p.g = 1.0;
			p.b = 1.0;
			p.a = 0.1;

			addParticle(p);
		}
	}
	void splash2::updateParticle(particle& p)
	{
		float t = world->time() - p.startTime;
		Vec3f v(p.velocity.x, p.velocity.y - 4.9e-6*t*t, p.velocity.z);
	//	p.velocity.y = max(p.velocity.y - 9.81 * world->time.length()/1000.0, 0.0);
		p.pos += v * world->time.length()/1000.0;

		t = t * p.invLife;

		if(t<0.05)
		{
			p.a = (t/0.05) * 0.1;
			p.size = (t / 0.05 + 1.0) * 0.5 * 0.3 * radius;
		}
		else if(t<0.50)
		{
			t = (t-0.03)/0.47;
			p.a = 0.1;
			p.size = (1.0-t) * 0.3 * radius + t * radius*0.7;		
		}
		else
		{
			t = (t-0.5)/0.5;
			p.a = (1.0 - t) * 0.1;

			p.size = radius*0.7 * (1.0-t*0.5);
		}
	}
}
