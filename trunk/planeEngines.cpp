
#include "game.h"

namespace particle
{
	planeEngines::planeEngines(): relativeEmitter("contrail", 48, 15.0, true)
	{

	}
	void planeEngines::init()
	{
		spread =	fuzzyAttribute(0.0);
		life =		fuzzyAttribute(800.0);
	}
	bool planeEngines::createParticle(particle& p, Vec3f currentPosition)
	{
		if(world[parentObject]->dead)
		{
			particlesPerSecond = 0.0;
			return false;
		}

		p.startTime = world.time() - extraTime;
		p.endTime = p.startTime + life();

		p.pos = Vec3f(0,0,0);//currentPosition;
		p.vel = Vec3f(0,0,-2.0);

		p.ang = random<float>(0,PI*2.0);
		p.angularSpeed = random<float>(-2.0,2.0);


		p.r = 1.0;
		p.g = 0.48;
		p.b = 0.17;
		p.a = 0.0;

		p.size = 0.8;

		return true;
	}
	void planeEngines::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		p.pos += p.vel * world.time.length() / 1000;
		p.ang += p.angularSpeed * world.time.length() / 1000;


		p.a = t < 0.1 ? t*10.0 : 1.0 - (t-0.1)/0.9;
		p.size = (1.0 - 0.25*t) * 0.8;
	}
	void planeEngines::render()
	{
		if(world[parentObject] != nullptr && !world[parentObject]->dead)
		{
			emitter::render();
		}
	}
}
 
