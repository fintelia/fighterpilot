
namespace particle
{

struct particle
{
	Vec3f pos;
	Vec3f velocity;

	float ang;
	float angularSpeed;

	float size;
	float r, g, b, a;
	float startTime;
	float invLife;	
};
const unsigned char FADE_IN = 0x01;

struct vertex
{
	Vec3f position;
	float s, t;       //stores apothem, angle for point sprites
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

private:
	static shared_ptr<GraphicsManager::indexBuffer> quadIBO;
	static unsigned int IBOnumQuads;

protected:
	int parentObject;
	Vec3f parentOffset;

	Vec3f position;
	Vec3f lastPosition;

	string texture;

	fuzzyAttribute speed;
	fuzzyAttribute spread;
	fuzzyAttribute life;
	fuzzyColor color;

	float radius;

	particle* particles;
	Vec3f* oldParticlePositions;
	unsigned char* particleFlags;
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

	shared_ptr<GraphicsManager::vertexBuffer> VBO;
	bool additiveBlending;
	bool sparkParticles;
	const int vertsPerParticle; //4 if using quads, 1 for point sprites


	bool active;
	bool visible;

public:
	friend class manager;
	emitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond=0.0, bool AdditiveBlending=false, bool SparkParticles=false);
	virtual void init(){}
	void setPositionAndRadius(Vec3f Position, float Radius){position=Position;lastPosition=Position;radius=Radius;}
	void setParent(int Parent, Vec3f ParentOffset){parentObject=Parent;parentOffset=ParentOffset;}

	virtual ~emitter();
	unsigned int addParticle(particle& p);
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
	sparkEmitter(string tex, unsigned int initalCompacity, float ParticlesPerSecond=0.0, bool AdditiveBlending=false): emitter(tex, initalCompacity, ParticlesPerSecond, AdditiveBlending, true){}
	virtual void prepareRender(Vec3f up, Vec3f right);
};
class manager
{
private:
	vector<shared_ptr<emitter>> emitters;
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
};

}
extern particle::manager& particleManager;