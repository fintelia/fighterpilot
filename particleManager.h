
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
	friend class ParticleManager;
public:
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

	const enum Type{NONE=0,EXPLOSION,SMOKE}type;
	Vec3f position;
	int parentObject;
	Vec3f parentOffset;
	string texture;

	fuzzyAttribute velocity;
	fuzzyAttribute spread;
	fuzzyAttribute life;
	fuzzyColor color;

	float friction;
	//int sfactor;
    //int dfactor;
	//shape shapes;
protected:
	particle* particles;
	vertex* vertices;
	unsigned int compacity;
	unsigned int total;

	double startTime;
	double extraTime;
	double particlesPerSecond;

	Vec3f minXYZ;
	Vec3f maxXYZ;

	GLuint VBO;

public:
	emitter(Type t, string tex, unsigned int initalCompacity=0);
	~emitter();
	void addParticle(particle& p);
	//virtual void setDefaults(...)=0;
	virtual void update()=0;
	virtual void render(Vec3f up, Vec3f right)=0;
};

class explosion: public emitter
{
public:
	void init();
	explosion(Vec3f pos);
	explosion(int parent, Vec3f offset = Vec3f(0,0,0));
	void update();
	void render(Vec3f up, Vec3f right);
};
class blackSmoke: public emitter
{
public:
	void init();
	blackSmoke(Vec3f pos);
	blackSmoke(int parent, Vec3f offset = Vec3f(0,0,0));
	void update();
	void render(Vec3f up, Vec3f right);
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