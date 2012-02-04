
namespace particle
{
class explosion: public emitter
{
public:
	explosion();
	void init();

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
class smokeTrail: public emitter
{
public:
	smokeTrail();
	void init();

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
}