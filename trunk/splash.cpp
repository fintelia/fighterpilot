

#include "game.h"

namespace particle
{
	//splash::splash(): emitter(SMOKE, "splash", 1.0, 100.0, 50), totalCreated(0)
	//{
	//	position.y = 0;

	//	velocity =	fuzzyAttribute(10.0, 10.0);
	//	spread =	fuzzyAttribute(1.0, 0.5);
	//	life =		fuzzyAttribute(6000.0, 100.0);

	//	//particle p;
	//	//for(int i=0; i < 50;i++)
	//	//{
	//	//	createParticle(p,position);
	//	//	addParticle(p);
	//	//}
	//}
	splash::splash(): emitter("splash", 50, 100.0), totalCreated(0)
	{

	}
	void splash::init()
	{
		position.y = 0;

		//velocity =	fuzzyAttribute(3.0, 0.0);
		spread =	fuzzyAttribute(4.0, 2.0);
		life =		fuzzyAttribute(6000.0, 100.0);

		//particle p;
		//for(int i=0; i < 50;i++)
		//{
		//	createParticle(p,position);
		//	addParticle(p);
		//}
	}
	bool splash::createParticle(particle& p, Vec3f currentPosition)
	{
		totalCreated++;
		if(totalCreated == 50)
			particlesPerSecond = 0;

		p.startTime = world.time() - extraTime;
		p.endTime = world.time() - extraTime + life();
		
		float r = random<float>(0, 5.0);
		Vec2f v = random2<float>();
		p.vel.x = v.x * r;
		p.vel.z = v.y * r;
		p.vel.y = random<float>(8.0,28.0);

		p.pos = currentPosition + random3<float>()*spread() + p.vel * extraTime/1000.0;

		p.size = 5.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

		p.r = 0.7;
		p.g = 0.7;
		p.b = 1;
		p.a = 0;

		return true;
	}
	void splash::updateParticle(particle& p)
	{
		float elapsedTime = (world.time() - p.startTime)/1000.0;

		p.pos.x += p.vel.x * world.time.length()/1000.0;
		p.pos.z += p.vel.z * world.time.length()/1000.0;
		p.pos.y = p.vel.y * elapsedTime - 9.8 * elapsedTime * elapsedTime;
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		if(t<0.05)
		{
			p.a = t * 20.0 * 0.15;
			p.size = 5.0;
		}
		else
		{
			t = (t-0.05)/0.99;
			p.a = (1.0 - t) * 0.15;
			p.size = (1.0-t) * 5.0 + t * 30.0;
			//p.pos.y += world.time.length()/100;
		}
	//	if(p.size/2 > p.pos.y)
	//		p.size = p.pos.y * 2;
	}
}