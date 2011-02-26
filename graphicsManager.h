
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
		const bool transparent;
		bool drawFlag;

		object(objectType t, bool trans): type(t), transparent(trans){}
		virtual void reset(){drawFlag=false;}
		virtual void render()=0;
	};

	map<gID,object*> objects;


protected:
	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance;			
	float		currentGamma;
	GraphicsManager(): currentId(0),hDC(NULL),hRC(NULL),hWnd(NULL){}

public:
	virtual void render3D();
	virtual void reset();

	virtual gID newModel(string disp)=0;
	virtual gID newParticleEffect(string texture, int size)=0;

	bool drawObject(gID obj);
	virtual bool drawModel(gID obj, Vec3f pos, Quat4f rot)=0;
	virtual bool drawModel(gID obj)=0;
	//virtual bool drawParticle(gID id, Vec3f pos, Color c)=0;
	virtual	bool drawParticle(gID id, Vec3f pos, float life)=0;
	virtual bool drawOverlay(Vec2f Origin, Vec2f Size)=0;
	virtual bool drawOverlay(Vec2f Origin, Vec2f Size, string tex)=0;
	virtual bool drawRotatedOverlay(Vec2f Origin, Vec2f Size, Angle rotation, string tex)=0;
	virtual bool drawPartialOverlay(Vec2f Origin, Vec2f Size, Vec2f tOrigin, Vec2f tSize, string tex)=0;

	virtual bool init()=0;
	virtual void resize(int w, int h)=0;//not really used that much but...
	virtual void render()=0;
	virtual void destroyWindow()=0;
	virtual void setGamma(float gamma)=0;
	virtual bool createWindow(char* title, RECT WindowRect, bool checkMultisample)=0;
	virtual void swapBuffers()=0;
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
		model(string d): object(MODEL,false), disp(d){}
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
			float life;
		} *particles;

		particleEffect(string tex, int s);
		~particleEffect();
		void reset();
		bool setParticle(Vec3f p, float life);
		void render();
	};

public:

	gID newModel(string disp);
	gID newParticleEffect(string texture, int size);

	bool drawModel(gID obj, Vec3f pos, Quat4f rot);
	bool drawModel(gID obj);
	bool drawParticle(gID id, Vec3f pos, float life);
	//bool drawParticle(gID id, Vec3f pos, Color c)=0;
	bool drawOverlay(Vec2f Origin, Vec2f Size);
	bool drawOverlay(Vec2f Origin, Vec2f Size,string tex);
	bool drawRotatedOverlay(Vec2f Origin, Vec2f Size, Angle rotation, string tex);
	bool drawPartialOverlay(Vec2f Origin, Vec2f Size, Vec2f tOrigin, Vec2f tSize, string tex);

	bool init();
	void resize(int w, int h);
	void render();
	void destroyWindow();
	void setGamma(float gamma);
	bool createWindow(char* title, RECT WindowRect, bool checkMultisample);
	void swapBuffers();
};
extern GraphicsManager* graphics;