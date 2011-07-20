
class GraphicsManager
{
public:
	typedef unsigned long gID;
	enum RenderTarget{FBO_0=0,FBO_1=1,SCREEN};

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

	struct View
	{
		struct Viewport{
			int x;
			int y;
			int width;
			int height;
		}viewport;

		struct Projection{
			enum Type{PERSPECTIVE,ORTHOGRAPHIC}type;
			//both
			float zNear;
			float zFar;

			//perspective
			float fovy;
			float aspect;

			//orthographic
			float left;
			float right;
			float top;
			float bottom;
		}projection;

		struct Camera{
			Vec3f eye;
			Vec3f center;
			Vec3f up;
		}camera;

		matrix4x4<float> projectionMat;
		matrix4x4<float> modelViewMat;
	}view;


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

	bool drawObject(gID obj);
	virtual bool drawModel(gID obj, Vec3f pos, Quat4f rot)=0;
	virtual bool drawModel(gID obj)=0;
	virtual bool drawOverlay(Vec2f Origin, Vec2f Size)=0;
	virtual bool drawOverlay(Vec2f Origin, Vec2f Size, string tex)=0;
	virtual bool drawOverlay(float x,float y,float width,float height)=0;
	virtual bool drawOverlay(float x,float y,float width,float height,string tex)=0;
	virtual bool drawRotatedOverlay(Vec2f Origin, Vec2f Size, Angle rotation, string tex)=0;
	virtual bool drawPartialOverlay(Vec2f Origin, Vec2f Size, Vec2f tOrigin, Vec2f tSize, string tex)=0;

	virtual bool init()=0;
	virtual void resize(int w, int h)=0;//not really used that much but...
	virtual void render()=0;
	virtual void destroyWindow()=0;
	virtual void setGamma(float gamma)=0;
	virtual bool createWindow(char* title, RECT WindowRect, bool checkMultisample)=0;
	virtual bool recreateWindow(Vec2i resolution, int multisample)=0;
	virtual bool changeResolution(Vec2f resolution)=0;
	virtual void swapBuffers()=0;
	virtual void takeScreenshot()=0;
	virtual void bindRenderTarget(RenderTarget t)=0;
	virtual void renderFBO(RenderTarget src)=0;
	
	virtual void drawSphere(Vec3f position, float radius)=0;

	virtual void viewport(int x,int y,int width,int height)=0;
	virtual void perspective(float fovy, float aspect, float near, float far)=0;
	virtual void ortho(float left, float right, float bottom, float top, float near, float far)=0;
	virtual void ortho(float left, float right, float bottom, float top){ortho(left, right, bottom, top, 0.0, 1.0);}
	virtual void lookAt(Vec3f eye, Vec3f center, Vec3f up)=0;

	virtual Vec2f project(Vec3f p);
	const View& getView(){return view;}

	void flashTaskBar(int times, int length=0);
	void minimizeWindow();
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
	struct texturedVertex2D
	{
		Vec2f position;
		Vec2f texCoord;
	};
	struct model: public object
	{
		string disp;
		Vec3f pos;
		Quat4f rot;
		model(string d): object(MODEL,false), disp(d){}
		void render();
	};

	texturedVertex2D overlay[4];

	GLuint renderTextures[2];
	GLuint depthRenderBuffers[2];
	GLuint FBOs[2];
	GLuint RBOs[2];
	


	RenderTarget renderTarget;

	OpenGLgraphics():renderTarget(SCREEN){}
public:

	gID newModel(string disp);

	bool drawModel(gID obj, Vec3f pos, Quat4f rot);
	bool drawModel(gID obj);
	bool drawOverlay(Vec2f Origin, Vec2f Size);
	bool drawOverlay(Vec2f Origin, Vec2f Size,string tex);
	bool drawOverlay(float x,float y,float width,float height);
	bool drawOverlay(float x,float y,float width,float height,string tex);
	bool drawRotatedOverlay(Vec2f Origin, Vec2f Size, Angle rotation, string tex);
	bool drawPartialOverlay(Vec2f Origin, Vec2f Size, Vec2f tOrigin, Vec2f tSize, string tex);

	bool init();
	void resize(int w, int h);
	void render();
	void destroyWindow();
	void setGamma(float gamma);
	bool createWindow(char* title, RECT WindowRect, bool checkMultisample);
	bool recreateWindow(Vec2i resolution, int multisample);
	bool changeResolution(Vec2f resolution);
	void swapBuffers();
	void takeScreenshot();
	void bindRenderTarget(RenderTarget t);
	void renderFBO(RenderTarget src);

	void drawSphere(Vec3f position, float radius);

	void viewport(int x,int y,int width,int height);
	void perspective(float fovy, float aspect, float near, float far);
	void ortho(float left, float right, float bottom, float top, float near, float far);
	void lookAt(Vec3f eye, Vec3f center, Vec3f up);
};
extern GraphicsManager* graphics;