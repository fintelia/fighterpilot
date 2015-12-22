
#include "engine.h"
#include "game.h"

namespace particle
{
	explosionFlash2::explosionFlash2(): sparkEmitter("spark", 128, 0.0, true)
	{

	}
	void explosionFlash2::init()
	{
		speed =	fuzzyAttribute(45.0, 10.0);
		spread =	fuzzyAttribute(radius/8, radius/8);
		life =		fuzzyAttribute(300,50);
		//color =		fuzzyColor(1.0,0.5,0.4);

		particle p;
		for(int i = 0; i < 128; i++)
		{
			p.startTime = world->time();
			p.invLife = 1.0 / life();
		
			Vec3f dir = random3<float>();
			p.velocity = dir * speed();
			p.pos = position + dir * spread() + p.velocity * extraTime/1000.0;
			p.size = 0.0;

			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

			p.r = 1.0;
			p.g = 1.0;
			p.b = 0.6;
			p.a = 1.0;

			addParticle(p);
		}
	}
	void explosionFlash2::updateParticle(particle& p)
	{
		p.pos += p.velocity * world->time.length()/1000.0;

		////float velMag = p.vel.magnitude();

		////p.vel = p.dir * p.initialSpeed * pow(2.718,-friction*(world->time() - p.startTime)/1000);
		////p.vel *= (velMag - pow(friction, (float)world->time.length()/1000.0f)) / velMag;	
		////p.pos += p.vel * world->time.length()/1000.0;

		////p.pos = p.startPos + p.dir * p.initialSpeed * (1.0-pow(2.718,-friction*(world->time() - p.startTime)/1000));
		float t = (world->time() - p.startTime) * p.invLife;
		//float a = 1.0;//(2.0 - p.initialSpeed);
		////Profiler.setOutput("x",   (a*t*t*t - (2.0*a+1.0)*t*t + (2.0+a)*t)  );
		//p.pos = p.startPos + p.dir * p.totalDistance * (a*t*t*t - (2.0*a+1.0)*t*t + (2.0+a)*t);

		//p.ang += p.angularSpeed * world->time.length() / 1000.0;

		//float e = world->elevation(p.pos.x,p.pos.z);
		//if(p.pos.y - p.size < e)
		//	p.pos.y = e + p.size;

		if(t<0.03)
		{
			p.a = (t/0.03) * 0.75;
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
		//	p.pos.y += world->time.length()/100;
		}
	}
}
