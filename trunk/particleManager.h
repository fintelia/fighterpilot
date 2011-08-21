
namespace particle
{

struct particle
{
	Vec3f pos;
	Vec3f vel;
	float friction;
	float size;
	float r, g, b, a;
	float startTime;
	double endTime;
};

struct vertex
{
	Vec3f position;
	float energy;
	float r, g, b, a;
	float s, t;
};

class emitter
{
public:
	friend class ParticleManager;
	struct fuzzyAttribute
	{
		float value;
		float tolerance;
		fuzzyAttribute():value(0.0),tolerance(0.0){}
		fuzzyAttribute(float v):value(v),tolerance(0.0){}
		fuzzyAttribute(float v, float t):value(v),tolerance(t){}
		float operator() () {return value + tolerance * random<float>(-1.0,1.0);}
	};
	struct fuzzyColor
	{
		float r,g,b,a;
		float tr,tg,tb,ta;
		fuzzyColor():r(0.0),g(0.0),b(0.0),a(1.0),tr(0.0),tg(0.0),tb(0.0),ta(0.0){}
		fuzzyColor(float R, float G, float B):r(R),g(G),b(B),a(1.0),tr(0.0),tg(0.0),tb(0.0),ta(0.0){}
		fuzzyColor(float R, float G, float B, float A):r(R),g(G),b(B),a(A),tr(0.0),tg(0.0),tb(0.0),ta(0.0){}
		fuzzyColor(float R, float G, float B, float TR, float TG, float TB):r(R),g(G),b(B),a(1.0),tr(TR),tg(TG),tb(TB),ta(0.0){}
		fuzzyColor(float R, float G, float B, float A, float TR, float TG, float TB):r(R),g(G),b(B),a(A),tr(TR),tg(TG),tb(TB),ta(0.0){}
		fuzzyColor(float R, float G, float B, float A, float TR, float TG, float TB, float TA):r(R),g(G),b(B),a(A),tr(TR),tg(TG),tb(TB),ta(TA){}
		float red() {return r + tr * random<float>(-1.0,1.0);}
		float green() {return g + tg * random<float>(-1.0,1.0);}
		float blue() {return b + tb * random<float>(-1.0,1.0);}
		float alpha() {return a + ta * random<float>(-1.0,1.0);}
	};

	const enum Type{NONE=0,EXPLOSION,EXPLOSION_FLASH,SMOKE,SPLASH,CONTRAIL_SMALL,CONTRAIL_LARGE}type;



protected:
	int parentObject;
	Vec3f parentOffset;

	Vec3f position;
	Vec3f lastPosition;

	string texture;

	fuzzyAttribute velocity;
	fuzzyAttribute spread;
	fuzzyAttribute life;
	fuzzyColor color;

	float radius;
	//int sfactor;
    //int dfactor;
	//shape shapes;

	float friction;

	particle* particles;
	vertex* vertices;
	unsigned int compacity;
	unsigned int liveParticles;
	unsigned int total;
	
	unsigned int vNum;

	double startTime;
	double extraTime;
	double particlesPerSecond;

	Vec3f minXYZ;
	Vec3f maxXYZ;

	GLuint VBO;
	bool additiveBlending;

public:
	friend class manager;

	emitter(Type t, Vec3f pos, string tex, float Friction, float ParticlesPerSecond, unsigned int initalCompacity,bool AdditiveBlending=false);
	emitter(Type t, int parent, Vec3f offset, string tex, float Friction, float ParticlesPerSecond, unsigned int initalCompacity,bool AdditiveBlending=false);
	virtual ~emitter();
	void addParticle(particle& p);
	//virtual void setDefaults(...)=0;



	virtual bool createParticle(particle& p, Vec3f currentPosition){return false;}
	virtual void updateParticle(particle& p){}

	void update();
	void prepareRender(Vec3f up, Vec3f right);
	void render();
};

class explosion: public emitter
{
private:
	CollisionChecker::triangleList* trl;
public:
	explosion(Vec3f pos, float radius);
	explosion(int parent, Vec3f offset = Vec3f(0,0,0));

	void updateParticle(particle& p);
};
class explosionFlash: public emitter
{
private:
	CollisionChecker::triangleList* trl;
public:
	explosionFlash(Vec3f pos);
	explosionFlash(int parent, Vec3f offset = Vec3f(0,0,0));

	void updateParticle(particle& p);
};
class explosionSmoke: public emitter
{
private:
	CollisionChecker::triangleList* trl;
public:
	explosionSmoke(Vec3f pos);
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
class manager
{
private:
	vector<emitter*> emitters;
	
	manager(){}

public:
	static manager& getInstance()
	{
		static manager* instance = new manager();
		return *instance;
	}
	void init();
	void addEmitter(emitter* e);
	void update();
	void render();
	void shutdown();

};

}
extern particle::manager& particleManager;