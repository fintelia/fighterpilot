
struct texturedVertex2D
{
	Vec2f position;
	Vec2f texCoord;
};
struct vertex3D
{
	Vec3f position;
	float padding;
};
struct texturedVertex3D
{
	Vec3f position;
	Vec2f UV;
	float padding[3];
};
struct texturedLitVertex3D
{
	Vec3f position;
	Vec3f normal;
	Vec2f UV;
};
class GraphicsManager
{
public:
	typedef unsigned long gID;
	enum RenderTarget{FBO_0=0,FBO_1=1,SCREEN};

private:
	gID currentId;

protected:
	gID idGen(){return ++currentId;}

	struct View
	{
		struct Viewport{
			float x;
			float y;
			float width;
			float height;
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

			Vec3f up;		//normalized
			Vec3f fwd;		//normalized
			Vec3f right;	//normalized
		}camera;

		matrix4x4<float> projectionMat;
		matrix4x4<float> modelViewMat;

		Plane<float> clipPlanes[6];
	};
	vector<View> views;
	unsigned int currentView;

	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance;
	float		currentGamma;

	bool stereo;
	bool leftEye;
	float interOcularDistance;

	GraphicsManager();
	virtual ~GraphicsManager(){}
public:
	virtual bool drawOverlay(Rect4f r, string tex="")=0;
	virtual bool drawRotatedOverlay(Rect4f r, Angle rotation, string tex="")=0;
	virtual bool drawPartialOverlay(Rect4f r, Rect4f t, string tex="")=0;

	virtual void drawText(string text, Vec2f pos, string font)=0;
	virtual void drawText(string text, Rect rect, string font)=0;
	virtual Vec2f textSize(string text, string font)=0;

	virtual void drawText(string text, Vec2f pos){drawText(text, pos, "default font");}
	virtual void drawText(string text, Rect rect){drawText(text, rect, "default font");}
	virtual Vec2f textSize(string text){return textSize(text, "default font");}



	virtual bool init()=0;
	virtual void resize(int w, int h)=0;//not really used that much but...
	virtual void render()=0;
	virtual void destroyWindow()=0;
	virtual void setGamma(float gamma)=0;
	virtual bool createWindow(const char* title, RECT WindowRect, bool checkMultisample)=0;
	virtual bool recreateWindow(Vec2i resolution, int multisample)=0;
	virtual bool changeResolution(Vec2f resolution)=0;
	virtual void swapBuffers()=0;
	virtual void takeScreenshot()=0;
	virtual void bindRenderTarget(RenderTarget t)=0;
	virtual void renderFBO(RenderTarget src)=0;


	virtual void drawLine(Vec3f start, Vec3f end)=0;
	virtual void drawSphere(Vec3f position, float radius)=0;
	virtual void drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3)=0;
	virtual void drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4){drawTriangle(p1,p2,p3);drawTriangle(p1,p3,p4);}

	virtual void drawModel(string model, Vec3f position, Quat4f rotation, Vec3f scale)=0;
	virtual void drawModelCustomShader(string model, Vec3f position, Quat4f rotation, Vec3f scale)=0;

	virtual void drawModel(string model, Vec3f position, Quat4f rotation, float scale=1.0)	{drawModel(model,position,rotation,Vec3f(scale,scale,scale));}
	virtual void drawModel(objectType t, Vec3f position, Quat4f rotation, float scale=1.0)	{drawModel(objectTypeString(t),position,rotation,Vec3f(scale,scale,scale));}
	virtual void drawModel(objectType t, Vec3f position, Quat4f rotation, Vec3f scale)		{drawModel(objectTypeString(t),position,rotation,scale);}
	virtual void drawModelCustomShader(string model, Vec3f position, Quat4f rotation, float scale=1.0)		{drawModelCustomShader(model, position, rotation, Vec3f(1.0,1.0,1.0));}

	virtual void lookAt(Vec3f eye, Vec3f center, Vec3f up);

	void useAnagricStereo(bool b){stereo = b;}
	void setInterOcularDistance(float d){interOcularDistance = d;}

	virtual Vec2f project(Vec3f p, unsigned int view=0);
	virtual Vec3f unProject(Vec3f p, unsigned int view=0);

	const View& getView(unsigned int view){return views[view < views.size() ? view : 0];}
	const View& getView(){return views[currentView];}

	void resetViews(unsigned int numViews=1);
	void viewport(float x,float y,float width,float height, unsigned int view=0);
	void perspective(float fovy, float aspect, float near, float far, unsigned int view=0);
	void ortho(float left, float right, float bottom, float top, float near, float far, unsigned int view=0);
	void ortho(float left, float right, float bottom, float top, unsigned int view=0){ortho(left, right, bottom, top, 0.0, 1.0, view);}

	bool sphereInFrustum(Sphere<float> s);

	void flashTaskBar(int times, int length=0);
	void minimizeWindow();
};

#ifdef OPENGL2
class OpenGLgraphics: public GraphicsManager
{
public:
	static OpenGLgraphics* getInstance()
	{
		static OpenGLgraphics* pInstance = new OpenGLgraphics();
		return pInstance;
	}
protected:
	texturedVertex2D overlay[4];
	vertex3D shapes3D[4];

	GLuint renderTextures[2];//only second is used with multisampling
	GLuint depthTextures[2];//only second is used with multisampling
	GLuint colorRenderBuffers;//only used with multisampling
	GLuint depthRenderBuffers;//only used with multisampling
	GLuint FBOs[2];


	RenderTarget renderTarget;

	OpenGLgraphics();
public:


	bool init();
	void resize(int w, int h);
	void render();
	void destroyWindow();
	void setGamma(float gamma);
	bool createWindow(const char* title, RECT WindowRect, bool checkMultisample);
	bool recreateWindow(Vec2i resolution, int multisample);
	bool changeResolution(Vec2f resolution);
	void swapBuffers();
	void takeScreenshot();
	void bindRenderTarget(RenderTarget t);
	void renderFBO(RenderTarget src);

	void drawLine(Vec3f start, Vec3f end);
	void drawSphere(Vec3f position, float radius);
	void drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3);
	void drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4);

	void drawModel(string model, Vec3f position, Quat4f rotation, Vec3f scale);
	void drawModelCustomShader(string model, Vec3f position, Quat4f rotation, Vec3f scale);

	bool drawOverlay(Rect4f r, string tex="");
	bool drawRotatedOverlay(Rect4f r, Angle rotation, string tex="");
	bool drawPartialOverlay(Rect4f r, Rect4f t, string tex="");

	void drawText(string text, Vec2f pos, string font);
	void drawText(string text, Rect rect, string font);
	Vec2f textSize(string text, string font);


	//void viewport(int x,int y,int width,int height);
	//void perspective(float fovy, float aspect, float near, float far);
	//void ortho(float left, float right, float bottom, float top, float near, float far);
	void lookAt(Vec3f eye, Vec3f center, Vec3f up);
};
#endif

extern GraphicsManager* graphics;
