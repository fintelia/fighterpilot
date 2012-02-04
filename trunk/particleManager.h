
namespace particle
{

struct particle
{
	Vec3f pos;
	Vec3f vel;
	Vec3f dir;
	Vec3f startPos;
	Angle ang;
	float initialSpeed;
	float angularSpeed;
	float totalDistance;
	float friction;
	float size;
	float r, g, b, a;
	float startTime;
	double endTime;
};

struct vertex
{
	Vec3f position;
	float s, t;
	float r, g, b, a;
	float energy;
	float padding[6];
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

	//unsigned int VBO;
	GraphicsManager::vertexBuffer* VBO;
	bool additiveBlending;

public:
	friend class manager;

	emitter(Type t, string tex, float Friction, float ParticlesPerSecond, unsigned int initalCompacity,bool AdditiveBlending=false);
	virtual void init(){}
	void setPositionAndRadius(Vec3f Position, float Radius){position=Position;lastPosition=Position;radius=Radius;}
	void setParent(int Parent, Vec3f ParentOffset){parentObject=Parent;ParentOffset=ParentOffset;}

	virtual ~emitter();
	void addParticle(particle& p);
	//virtual void setDefaults(...)=0;



	virtual bool createParticle(particle& p, Vec3f currentPosition){return false;}
	virtual void updateParticle(particle& p){}

	void update();
	void prepareRender(Vec3f up, Vec3f right);
	void render();
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
	void addEmitter(emitter* e, Vec3f position, float radius=1.0);
	void addEmitter(emitter* e, int parent, Vec3f offset=Vec3f());
	void update();
	void render();
	void shutdown();
};

}
extern particle::manager& particleManager;