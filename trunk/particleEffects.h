
namespace particle
{
class explosion: public emitter
{
private:
	double startTime;
	shared_ptr<SceneManager::pointLight> flash;
public:
	explosion();
	void init();
	void update();

	void updateParticle(particle& p);
};
class explosionFlash: public emitter
{
public:
	explosionFlash();
	void init();

	void updateParticle(particle& p);
};
class explosionSmoke: public emitter
{
public:
	explosionSmoke();
	void init();

	void updateParticle(particle& p);
};
class flakExplosionSmoke: public emitter
{
public:
	flakExplosionSmoke();
	void init();

	void updateParticle(particle& p);
};
class blackSmoke: public emitter
{
public:
	blackSmoke();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class splash: public emitter
{
private:
	unsigned int totalCreated;
public:
	splash();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class contrail: public emitter
{
public:
	contrail();
	void init();
	void update();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class contrailSmall: public emitter
{
public:
	contrailSmall();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class planeContrail: public emitter
{
public:
	planeContrail();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class planeEngines:public relativeEmitter
{
public:
	planeEngines();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
	void render();
};
class smokeTrail: public emitter
{
private:
	Color color;

public:
	smokeTrail();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);

	void setColor(Color c);
};
class debrisSmokeTrail: public emitter
{
private:
	Color color;
	Vec3f emitterVelocity;

public:
	debrisSmokeTrail();
	void init();
	void update();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);

	void setColor(Color c);
};
class explosionFlash2: public sparkEmitter
{
public:
	explosionFlash2();
	void init();

	void updateParticle(particle& p);
};
class groundExplosionFlash: public sparkEmitter
{
public:
	groundExplosionFlash();
	void init();

	void updateParticle(particle& p);
};
class splash2: public sparkEmitter
{
public:
	splash2();
	void init();

	void updateParticle(particle& p);
};
class explosionSparks: public sparkEmitter
{
public:
	explosionSparks();
	void init();

	void updateParticle(particle& p);
};
class bulletEffect: public sparkEmitter
{
public:
	bulletEffect();

	void update(){}
	void prepareRender(Vec3f up, Vec3f right);
	bool toDelete(){return false;}
};
class fireball: public emitter
{
public:
	fireball();
	void init();
	void update();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
}