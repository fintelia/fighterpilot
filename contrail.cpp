
#include "game.h"

namespace particle
{
	contrail::contrail(): emitter("contrail", 48, 300.0)
	{

	}
	void contrail::init()
	{
		life = fuzzyAttribute(8000.0, 1500.0);
	}
	bool contrail::createParticle(particle& p, Vec3f currentPosition)
	{
		p.startTime = world.time() - extraTime;
		p.invLife = 1.0 / life();

		p.pos = currentPosition + random3<float>()*spread();

		p.size = 3.0;
		p.r = 0.55;
		p.g = 0.55;
		p.b = 0.55;
		p.a = 0.0;

		p.ang = random<double>(0.0,2.0*PI);
		return true;
	}
	void contrail::update()
	{	
		position += random3<float>() * 0.5;
		emitter::update();
	}
	
	void contrail::updateParticle(particle& p)
	{
		float t = (world.time() - p.startTime) * p.invLife;

		if(t<0.10f)
		{
			p.a = t*6.0;
			//p.a = min(t*20.0,1.0) * 0.6;
			p.size = 2.0;
		}
		else if(t < 0.75f)
		{
			p.a = 0.669f - 0.692f * t;
			p.size = 1.769f + 2.308f * t;

			//t = (t-0.10)/0.65;
			//p.a = lerp(0.6, 0.15, t);
			//p.size = lerp(2.0, 3.5, t);

		}
		else
		{
			p.a = 0.6f - 0.6f * t;
			p.size = -1.0f + 6.0f * t;

			//t = (t-0.75)/0.25;
			//p.a = lerp(0.15, 0.0, t);
			//p.size = lerp(3.5, 5.0, t);
		}

	}
}
