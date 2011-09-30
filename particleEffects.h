
namespace particle
{
class explosion: public emitter
{
public:
	explosion(Vec3f pos, float radius);
	explosion(int parent, Vec3f offset = Vec3f(0,0,0));

	void updateParticle(particle& p);
};
class explosionFlash: public emitter
{
public:
	explosionFlash(Vec3f pos);
	explosionFlash(int parent, Vec3f offset = Vec3f(0,0,0));

	void updateParticle(particle& p);
};
class explosionSmoke: public emitter
{
public:
	explosionSmoke(Vec3f pos, float radius);
	explosionSmoke(int parent, Vec3f offset = Vec3f(0,0,0));

	void updateParticle(particle& p);
};
class blackSmoke: public emitter
{
public:
	blackSmoke(Vec3f pos);
	blackSmoke(int parent, Vec3f offset = Vec3f(0,0,0));

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class splash: public emitter
{
private:
	unsigned int totalCreated;
public:
	splash(Vec3f pos);
	splash(int parent, Vec3f offset = Vec3f(0,0,0));

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class contrail: public emitter
{
public:
	contrail(int parent, Vec3f offset = Vec3f(0,0,0));

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class contrailSmall: public emitter
{
public:
	contrailSmall(int parent, Vec3f offset = Vec3f(0,0,0));

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class planeContrail: public emitter
{
public:
	planeContrail(int parent, Vec3f offset = Vec3f(0,0,0));

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
class smokeTrail: public emitter
{
public:
	smokeTrail(int parent, Vec3f offset = Vec3f(0,0,0));

	bool createParticle(particle& p, Vec3f currentPosition);
	void updateParticle(particle& p);
};
}