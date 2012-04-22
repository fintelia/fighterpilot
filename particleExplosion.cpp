

#include "game.h"

namespace particle
{
	//explosion::explosion(Vec3f pos, float Radius): emitter(EXPLOSION, pos, "fire", 0.1, 0.0, 32,true)
	//{
	//	radius = Radius;
	//	velocity =	fuzzyAttribute(4.0*radius, 0.0);
	//	spread =	fuzzyAttribute(radius, radius);
	//	life =		fuzzyAttribute(600.0);
	//	color =		fuzzyColor(1.0,0.5,0.4);

	//	particle p;
	//	for(int i = 0; i < 32; i++)
	//	{
	//		p.startTime = world.time();
	//		p.endTime = world.time() + life();
	//	
	//		Vec3f dir = random3<float>();
	//		p.vel = dir * velocity();
	//		p.pos = pos + dir * spread() + p.vel * extraTime/1000.0;

	//		p.size = 0.0;

	//		p.r = 1.0;
	//		p.g = 0.5;
	//		p.b = 0.1;
	//		p.a = 0;

	//		addParticle(p);
	//	}
	//}
	explosion::explosion(): emitter("smoke2", 64, 0.0, true)
	{

	}
	void explosion::init()
	{
		velocity =	fuzzyAttribute(1.0, 1.0);
		spread =	fuzzyAttribute(radius/8, radius/8);
		life =		fuzzyAttribute(1000.0,100.0);
		//color =		fuzzyColor(1.0,0.5,0.4);

		particle p;
		for(int i = 0; i < 64; i++)
		{
			p.startTime = world.time();
			p.endTime = world.time() + life();
		
			p.dir = random3<float>();
			p.initialSpeed = sqrt(velocity() * velocity());
			p.vel = p.dir * p.initialSpeed;
			p.startPos = position + p.dir * spread();
			p.pos = p.startPos + p.vel * extraTime/1000.0;
			p.size = 0.0;
			p.totalDistance = radius;

			p.ang = random<float>(2.0*PI);
			p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

			p.r = 1.0;
			p.g = 0.48;
			p.b = 0.17;
			p.a = 0;

			addParticle(p);
		}
	}
	void explosion::updateParticle(particle& p)
	{
		//float velMag = p.vel.magnitude();

		//p.vel = p.dir * p.initialSpeed * pow(2.718,-friction*(world.time() - p.startTime)/1000);
		//p.vel *= (velMag - pow(friction, (float)world.time.length()/1000.0f)) / velMag;	
		//p.pos += p.vel * world.time.length()/1000.0;

		//p.pos = p.startPos + p.dir * p.initialSpeed * (1.0-pow(2.718,-friction*(world.time() - p.startTime)/1000));
		float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
		float a = 1.0;//(2.0 - p.initialSpeed);
		//Profiler.setOutput("x",   (a*t*t*t - (2.0*a+1.0)*t*t + (2.0+a)*t)  );
		p.pos = p.startPos + p.dir * p.totalDistance * (a*t*t*t - (2.0*a+1.0)*t*t + (2.0+a)*t);

		p.ang += p.angularSpeed * world.time.length() / 1000.0;

		float e = world.elevation(p.pos.x,p.pos.z);
		if(p.pos.y - p.size < e)
			p.pos.y = e + p.size;

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
		//	p.pos.y += world.time.length()/100;
		}
	}

	//void explosionEffect::init()
	//{
	//	nExplosions++;
	//	static int nExplosions = 0;
	//	nExplosions++;
	//	Profiler.setOutput("n explosions:", nExplosions);

	//	particleTypePtr = particleManager.getParticleType("explosionFireParticle");
	//	if(!particleTypePtr)
	//	{
	//		particleTypePtr = shared_ptr<particleType>(new explosionFireParticle);
	//		particleManager.addParticleType("explosionFireParticle", particleTypePtr);
	//	}

	//	for(int i = 0; i < 64; i++)
	//	{
	//		newParticle(position, world.time());
	//	}
	//}
	//void explosionEffect::newParticle(Vec3f pos, double time)
	//{
	//	auto& p = particleTypePtr->newParticle();

	//	p.startTime = world.time();
	//	p.endTime = world.time() + random<float>(900,1100);
	//	
	//	p.dir = random3<float>();
	//	p.initialSpeed = random<float>(0,2);
	//	p.vel = p.dir * p.initialSpeed;
	//	p.startPos = position + p.dir * random<float>(0,radius/4);
	//	p.pos = p.startPos + p.vel * extraTime/1000.0;
	//	p.size = 0.0;
	//	p.totalDistance = radius;

	//	p.ang = random<float>(2.0*PI);
	//	p.angularSpeed = random<float>(-0.3*PI,0.3*PI);

	//	p.r = 1.0;
	//	p.g = 0.48;
	//	p.b = 0.17;
	//	p.a = 0;
	//}

	//void explosionFireParticle::updateParticle(particle& p)
	//{
	//	Profiler.setOutput("n explosions:", nExplosions);
	//	float radius = 10.0;
	//	//float velMag = p.vel.magnitude();

	//	//p.vel = p.dir * p.initialSpeed * pow(2.718,-friction*(world.time() - p.startTime)/1000);
	//	//p.vel *= (velMag - pow(friction, (float)world.time.length()/1000.0f)) / velMag;	
	//	//p.pos += p.vel * world.time.length()/1000.0;

	//	//p.pos = p.startPos + p.dir * p.initialSpeed * (1.0-pow(2.718,-friction*(world.time() - p.startTime)/1000));
	//	float t = (world.time() - p.startTime) / (p.endTime - p.startTime);
	//	float a = 1.0;//(2.0 - p.initialSpeed);
	//	//Profiler.setOutput("x",   (a*t*t*t - (2.0*a+1.0)*t*t + (2.0+a)*t)  );
	//	p.pos = p.startPos + p.dir * p.totalDistance * (a*t*t*t - (2.0*a+1.0)*t*t + (2.0+a)*t);

	//	p.ang += p.angularSpeed * world.time.length() / 1000.0;

	//	float e = world.elevation(p.pos.x,p.pos.z);
	//	if(p.pos.y - p.size < e)
	//		p.pos.y = e + p.size;

	//	if(t<0.05)
	//	{
	//		p.a = t * 20.0 * 0.75;
	//		p.size = (t / 0.05 + 1.0) * 0.5 * 0.3 * radius;
	//	}
	//	else if(t<0.75)
	//	{
	//		t = (t-0.05)/0.7;
	//		p.a = 0.75;
	//		p.size = (1.0-t) * 0.3 * radius + t * radius*0.7;		
	//	}
	//	else
	//	{
	//		t = (t-0.75)/0.25;
	//		p.a = (1.0 - sqrt(t)) * 0.75;

	//		p.r = 0.6*t + (1.0-t)*1.0;
	//		p.g = 0.6*t + (1.0-t)*0.48;
	//		p.b = 0.6*t + (1.0-t)*0.17;

	//		p.size = radius*0.7;
	//	//	p.pos.y += world.time.length()/100;
	//	}
	//}
	//void explosionFireParticle::render()
	//{
	//	dataManager.bind("fire");
	//	pointSprite::render();
	//	dataManager.bindTex(0);
	//}
}