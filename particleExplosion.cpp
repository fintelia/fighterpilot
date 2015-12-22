
#include "engine.h"
#include "game.h"

namespace particle
{
	explosion::explosion(): emitter("smoke2", 64, 0.0, true)
	{

	}
	void explosion::init()
	{
		flash = sceneManager.genPointLight();
		flash->position = position;
		flash->strength = 0.001;
		startTime = world->time();

		float alt = world->altitude(position);
		if(alt < 3.0*radius)
		{
			world->addDecal(Vec2f(position.x,position.z), 5.0*radius, 5.0*radius, "scorch", world->time());
		}

		speed =		fuzzyAttribute(1.0, 1.0);
		spread =	fuzzyAttribute(radius/8, radius/8);
		life =		fuzzyAttribute(1000.0,100.0);

		particle p;
		for(int i = 0; i < 64; i++)
		{
			p.startTime = world->time();
			p.invLife = 1.0 / life();
		
			p.velocity = random3<float>(); //note: velocity only stores direction
			p.pos = position + p.velocity * spread() + p.velocity * sqrt(speed() * speed()) * extraTime/1000.0;
			p.size = 0.0;

			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

			p.r = 1.0;
			p.g = 0.48;
			p.b = 0.17;
			p.a = 0;

			addParticle(p);
		}
	}
	void explosion::update()
	{
		double t = (world->time() - startTime) / 1100.0;

		float s;
		if(t<0.05)
		{
			s = (1.0 * t * 20);
		}
		else if(t<0.75)
		{
			s = 1.0 - 0.5 * (t-0.05)/0.7;
		}
		else if(t<1.0)
		{
			s = 0.5 - 0.5 * (t-0.75)/0.25;
		}
		else
		{
			s = 0.0;
		}
		flash->color.r = 1.0 * s;
		flash->color.g = 0.6 * s;
		flash->color.b = 0.3 * s;
		flash->strength = 45.0 * s;
		emitter::update();
	}
	void explosion::updateParticle(particle& p)
	{
		float t = (world->time() - p.startTime) * p.invLife;
		p.pos += p.velocity * radius * 3.0f*(t-1.0)*(t-1.0)*world->time.length()*p.invLife; //NOTE: velocity is actually just a direction

		p.ang += p.angularSpeed * world->time.length() / 1000.0;

		float e = world->elevation(p.pos.x,p.pos.z);
		if(p.pos.y /*- p.size*/ < e)
			p.pos.y = e /*+ p.size*/;

		if(t<0.05)
		{
			p.a = t * 20.0 * 0.75;
			p.size = (t / 0.05 + 1.0) * 0.5 * 0.3 * radius;
		}
		else if(t<0.75)
		{
			t = (t-0.05)/0.7;
			p.a = 0.75;
			p.size = (1.0-t) * 0.3 * radius + t * radius*0.7;		
		}
		else
		{
			t = (t-0.75)/0.25;
			p.a = (1.0 - sqrt(t)) * 0.75;

			p.r = 0.6*t + (1.0-t)*1.0;
			p.g = 0.6*t + (1.0-t)*0.48;
			p.b = 0.6*t + (1.0-t)*0.17;

			p.size = radius*0.7;
		//	p.pos.y += world->time.length()/100;
		}
	}
}
