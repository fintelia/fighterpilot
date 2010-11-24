
class GraphicsManager
{
public:
	typedef unsigned long gID;

private:
	gID currentId;

protected:
	gID idGen(){return ++currentId;}
	struct object
	{
		const enum objectType{MODEL, PARTICLE_EFFECT}type;
		const bool is_3D;
		const bool transparent;
		bool drawFlag;

		object(objectType t, bool Is_3D, bool trans): type(t), is_3D(Is_3D), transparent(trans){}
		virtual void reset(){drawFlag=false;}
		virtual void render()=0;
	};

	map<gID,object*> objects;



protected:
	GraphicsManager(): currentId(0){}

public:
	virtual void render3D();
	virtual void render2D();

	virtual void reset();

	virtual gID newModel(string disp)=0;
	virtual gID newParticleEffect(string texture, int size)=0;

	bool drawObject(gID obj)
	{
		if(objects.find(obj) !=objects.end())
		{
			objects[obj]->drawFlag = true;
			return true;
		}
		return false;
	}
	virtual bool drawModel(gID obj, Vec3f pos, Quat4f rot)=0;
	virtual bool drawModel(gID obj)=0;
	virtual bool drawParticle(gID id, Vec3f pos, Color c)=0;

};

class OpenGLgraphics: public GraphicsManager
{
public:
	static OpenGLgraphics* getInstance()
	{
		static OpenGLgraphics* pInstance = new OpenGLgraphics();
		return pInstance;
	}
protected:
	struct model: public object
	{
		string disp;
		Vec3f pos;
		Quat4f rot;
		model(string d): object(MODEL,true,false), disp(d){}
		void render(){}
	};
	struct particleEffect: public object
	{
		string			texture;
		int				size;
		unsigned int	num,
						compacity;
		GLuint			VBO;
		struct particle{
			Vec3f vert;
			float r,g,b,a;
			float padding;
		} *particles;

		particleEffect(string tex, int s);
		~particleEffect();
		void reset();
		bool setParticle(Vec3f point, Color c);
		void render();
	};
public:
	gID newModel(string disp);
	gID newParticleEffect(string texture, int size);

	bool drawModel(gID obj, Vec3f pos, Quat4f rot);
	bool drawModel(gID obj);
	bool drawParticle(gID id, Vec3f pos, Color c);
};
extern GraphicsManager* graphics;