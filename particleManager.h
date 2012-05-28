
namespace particle
{

struct particle
{
	Vec3f pos;
	float ang;
	float size;
	float r, g, b, a;
	Vec3f lastPos;
	Vec3f vel;
	Vec3f dir;
	Vec3f startPos;
	float initialSpeed;
	float angularSpeed;
	float totalDistance;
	float friction;
	float startTime;
	float endTime;
	bool fadeIn;
};

struct vertex
{
	Vec3f position;
	float s, t;
	float r, g, b, a;
	float padding[7];
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

	//const enum Type{NONE=0,EXPLOSION,EXPLOSION_FLASH,SMOKE,SPLASH,CONTRAIL_SMALL,CONTRAIL_LARGE}type;



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

	//float friction;

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
	shared_ptr<GraphicsManager::vertexBuffer> VBO;
	bool additiveBlending;

	bool active;
	bool visible;

public:
	friend class manager;

	emitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond=0.0, bool AdditiveBlending=false);
	virtual void init(){}
	void setPositionAndRadius(Vec3f Position, float Radius){position=Position;lastPosition=Position;radius=Radius;}
	void setParent(int Parent, Vec3f ParentOffset){parentObject=Parent;parentOffset=ParentOffset;}

	virtual ~emitter();
	void addParticle(particle& p);
	//virtual void setDefaults(...)=0;



	virtual bool createParticle(particle& p, Vec3f currentPosition){return false;}
	virtual void updateParticle(particle& p){}

	virtual void update();
	virtual void prepareRender(Vec3f up, Vec3f right);
	virtual void render();

	void setActive(bool a){active = a;}
	void setVisible(bool v){visible = v;}

	virtual bool toDelete(){return particlesPerSecond==0 && liveParticles==0;}
};
class relativeEmitter: public emitter
{
public:
	relativeEmitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond=0.0, bool AdditiveBlending=false):emitter(tex, initalCompacity, ParticlesPerSecond, AdditiveBlending){}
	virtual void prepareRender(Vec3f up, Vec3f right);
};
class sparkEmitter: public emitter
{
public:
	sparkEmitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond=0.0, bool AdditiveBlending=false): emitter(tex, initalCompacity, ParticlesPerSecond, AdditiveBlending){}
	virtual void prepareRender(Vec3f up, Vec3f right);
};
//class particleType
//{
//protected:
//	particle* particles;
//	unsigned int compacity;
//	unsigned int total;
//
//	vertex* vertices;
//	unsigned int vNum;
//
//	GraphicsManager::vertexBuffer* VBO;
//	bool additiveBlending;
//	friend class manager;
//
//	virtual void updateParticle(particle& p){}
//public:
//	particleType(unsigned int initialCompacity = 64);
//	~particleType();
//
//	virtual void prepareRender(Vec3f up, Vec3f right)=0;
//	virtual void render()=0;
//
//	void reset(){total = 0;}
//
//	virtual void update();
//
//	virtual particle& newParticle();
//};
//
//class pointSprite: public particleType
//{
//protected:
//
//public:
//	virtual void prepareRender(Vec3f up, Vec3f right);
//	virtual void render();
//};
//
//class spark: public particleType
//{
//protected:
//
//public:
//};
//
//class particleEffect
//{
//protected:
//	Vec3f position;
//	Vec3f lastPosition;
//
//	int parentObject;
//	Vec3f parentOffset;
//
//	float radius;
//
//	double startTime;
//	
//	double particlesPerSecond;
//	double extraTime;
//
//	
//	virtual void newParticle(Vec3f p, double time){}
//public:
//	void setPositionAndRadius(Vec3f Position, float Radius){position=Position;lastPosition=Position;radius=Radius;}
//	void setParent(int Parent, Vec3f ParentOffset){parentObject=Parent;ParentOffset=ParentOffset;}
//	virtual void init(){}
//
//	particleEffect();
//	virtual bool update();
//};

class manager
{
private:
	vector<shared_ptr<emitter>> emitters;
	
	//map<string, shared_ptr<particleType>> particleTypes;
	//vector<shared_ptr<particleEffect>> particleEffects;
	manager(){}


public:
	static manager& getInstance()
	{
		static manager* instance = new manager();
		return *instance;
	}
	void init();
	void addEmitter(shared_ptr<emitter> e, Vec3f position, float radius=1.0);
	void addEmitter(shared_ptr<emitter> e, int parent, Vec3f offset=Vec3f());
	void addEmitter(emitter* e, Vec3f position, float radius=1.0){addEmitter(shared_ptr<emitter>(e), position, radius);}
	void addEmitter(emitter* e, int parent, Vec3f offset=Vec3f()){addEmitter(shared_ptr<emitter>(e), parent, offset);}

	void update();
	void render(shared_ptr<GraphicsManager::View> view);
	void shutdown();

	//void addParticleEffect(particleEffect* p, Vec3f positoin, float radius=1.0);
	//void addParticleEffect(particleEffect* p, int parent, Vec3f offset=Vec3f());

	//void addParticleType(string type, shared_ptr<particleType> particleTypePtr);
	//shared_ptr<particleType> getParticleType(string type);

};

}
extern particle::manager& particleManager;