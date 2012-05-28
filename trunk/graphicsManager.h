
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
struct litVertex3D
{
	Vec3f position;
	Vec3f normal;
	float padding[2];
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
	enum RenderTarget{RT_FBO_0,RT_FBO_1,RT_MULTISAMPLE_FBO,RT_SCREEN};
	enum Primitive{POINTS, LINES, LINE_STRIP, LINE_LOOP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, QUADS, QUAD_STRIP, POLYGON};
	enum BlendMode{ALPHA_ONLY, TRANSPARENCY, ADDITIVE};

	class View
	{
	private:
		friend class GraphicsManager;

		struct Viewport{
			float x;
			float y;
			float width;
			float height;
		}mViewport;

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
		}mProjection;

		struct Camera{
			Vec3f eye;

			Vec3f up;		//normalized
			Vec3f fwd;		//normalized
			Vec3f right;	//normalized
		}mCamera;

		Mat4f mProjectionMat;
		Mat4f mModelViewMat;

		Plane<float> mClipPlanes[6];

		bool mRenderParticles;

		int mRenderFuncParam;
		function<void(int)> mRenderFunc;

		Vec3f cameraShift;

	public:
		View();
		Vec2f project(Vec3f p);
		Vec3f project3(Vec3f p);
		Vec3f unProject(Vec3f p);
		void viewport(float x,float y,float width,float height);
		void perspective(float fovy, float aspect, float near, float far);
		void ortho(float left, float right, float bottom, float top, float near, float far);
		void ortho(float left, float right, float bottom, float top){ortho(left, right, bottom, top, 0.0, 1.0);}
		void lookAt(Vec3f eye, Vec3f center, Vec3f up);

		const Viewport& viewport(){return mViewport;}
		const Projection& projection(){return mProjection;}
		const Camera& camera(){return mCamera;}

		const Mat4f& projectionMatrix(){return mProjectionMat;}
		const Mat4f& modelViewMatrix(){return mModelViewMat;}

		void setRenderFunc(function<void(int)> f, int param=0){mRenderFunc = f; mRenderFuncParam = param;}

		bool sphereInFrustum(Sphere<float> s);
		bool boundingBoxInFrustum(BoundingBox<float> b);

		void render(){if(mRenderFunc)mRenderFunc(mRenderFuncParam);}

		bool renderParticles(){return mRenderParticles;}
		void renderParticles(bool b){mRenderParticles=b;}

		void shiftCamera(Vec3f shift);
	};
	class indexBuffer;
	class vertexBuffer
	{
	public:
		enum UsageFrequency{STATIC,DYNAMIC,STREAM};
	protected:
		unsigned int positionDataSize;
		unsigned int positionDataOffset;

		unsigned int texCoordDataSize;
		unsigned int texCoordDataOffset;

		unsigned int normalDataSize;		
		unsigned int normalDataOffset;

		unsigned int colorDataSize;
		unsigned int colorDataOffset;


		unsigned int totalVertexSize;

		UsageFrequency usageFrequency;

		vertexBuffer(UsageFrequency u): usageFrequency(u), positionDataSize(0), positionDataOffset(0), texCoordDataSize(0), texCoordDataOffset(0), normalDataSize(0), normalDataOffset(0), colorDataSize(0), colorDataOffset(0), totalVertexSize(0){}

		virtual void bindBuffer(unsigned int offset){}
		virtual void bindBuffer(){bindBuffer(0);}

	public:
		friend class GraphicsManager::indexBuffer;
		virtual ~vertexBuffer(){}
		virtual void addPositionData(unsigned int size, unsigned int offset)	{positionDataSize = size; positionDataOffset = offset;}
		virtual void addTexCoordData(unsigned int size, unsigned int offset)	{texCoordDataSize = size; texCoordDataOffset = offset;}
		virtual void addNormalData(unsigned int size, unsigned int offset)		{normalDataSize = size;	normalDataOffset = offset;}
		virtual void addColorData(unsigned int size, unsigned int offset)		{colorDataSize = size;	colorDataOffset = offset;}

		virtual void setTotalVertexSize(unsigned int totalSize){totalVertexSize = totalSize;}

		virtual void setVertexData(unsigned int size, void* data)=0;

		virtual void drawBuffer(Primitive primitive, unsigned int bufferOffset, unsigned int count)=0;
	};
	class indexBuffer
	{
	public:
		enum UsageFrequency{STATIC,DYNAMIC,STREAM};
	protected:
		UsageFrequency usageFrequency;
		indexBuffer(UsageFrequency u): usageFrequency(u){}
		void bindVertexBuffer(shared_ptr<vertexBuffer> b,unsigned int offset=0){b->bindBuffer(offset);}
	public:
		virtual ~indexBuffer(){}
		virtual void setData(unsigned char* data, unsigned int count)=0;
		virtual void setData(unsigned short* data, unsigned int count)=0;
		virtual void setData(unsigned int* data, unsigned int count)=0;
		virtual void drawBuffer(Primitive primitive, shared_ptr<vertexBuffer> buffer, unsigned int offset)=0;
		void drawBuffer(Primitive primitive, shared_ptr<vertexBuffer> buffer){drawBuffer(primitive, buffer, 0);}
	};
	class texture
	{
	public:
		enum Format{NONE=0, LUMINANCE=1, LUMINANCE_ALPHA=2, RGB=3, RGBA=4};
	protected:
		Format format;
	public:
		texture():format(NONE){}
		virtual void bind(unsigned int textureUnit=0)=0;
	};
	class texture2D: public texture
	{
	protected:
		unsigned int width;
		unsigned int height;
		texture2D():width(0),height(0){}
	public:
		virtual void setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data, bool tileable)=0;
		void setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data){setData(Width, Height, f, data, false);}
	};
	class texture3D: public texture
	{
	protected:
		unsigned int width;
		unsigned int height;
		unsigned int depth;
		texture3D():width(0),height(0){}
	public:
		virtual void setData(unsigned int Width, unsigned int Height, unsigned int Depth, Format f, unsigned char* data, bool tileable)=0;
		void setData(unsigned int Width, unsigned int Height, unsigned int Depth, Format f, unsigned char* data){setData(Width, Height, Depth, f,  data, false);}
	};
	class textureCube: public texture
	{
	protected:
		unsigned int width;
		unsigned int height;
		textureCube():width(0),height(0){}
	public:
		virtual void setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data)=0;
	};
	class shader
	{
	public:
		virtual bool init(const char* vert, const char* frag, const char* geometry)=0;
		bool init(const char* vert, const char* frag){return init(vert, frag, nullptr);}

		virtual void bind()=0;
		virtual void unbind()=0;

		virtual void setUniform1f(string name, float v0)=0;
		virtual void setUniform2f(string name, float v0, float v1)=0;
		virtual void setUniform3f(string name, float v0, float v1, float v2)=0;
		virtual void setUniform4f(string name, float v0, float v1, float v2, float v3)=0;
		void setUniform2f(string name, Vec2f v){setUniform2f(name,v.x,v.y);}
		void setUniform3f(string name, Vec3f v){setUniform3f(name,v.x,v.y,v.z);}
		void setUniform3f(string name, Color c){setUniform3f(name,c.r,c.r,c.b);}
		void setUniform4f(string name, Color c, float a = 1.0){setUniform4f(name,c.r,c.r,c.b,a);}
		virtual void setUniform1i(string name, int v0)=0;
		virtual void setUniform2i(string name, int v0, int v1)=0;
		virtual void setUniform3i(string name, int v0, int v1, int v2)=0;
		virtual void setUniform4i(string name, int v0, int v1, int v2, int v3)=0;

		virtual void setUniformMatrix(string name, const Mat3f& m)=0;
		virtual void setUniformMatrix(string name, const Mat4f& m)=0;

		virtual string getErrorStrings(){return "";}
	};
private:
	gID currentId;

protected:
	gID idGen(){return ++currentId;}

	vector<weak_ptr<View>> views_new;
	shared_ptr<View> currentView;

	//vector<View> views;
	//unsigned int currentView;

	float fps;

	float currentGamma;

	bool stereo;
	bool leftEye;
	float interOcularDistance;

	Vec3f lightPosition;

	GraphicsManager();
	virtual ~GraphicsManager(){}
public:
	virtual bool drawOverlay(Rect4f r, string tex="")=0;
	virtual bool drawRotatedOverlay(Rect4f r, Angle rotation, string tex="")=0;
	virtual bool drawPartialOverlay(Rect4f r, Rect4f t, string tex="")=0;

	virtual void drawText(string text, Vec2f pos, string font)=0;
	virtual void drawText(string text, Rect rect, string font)=0;
	virtual Vec2f getTextSize(string text, string font)=0;

	virtual void drawText(string text, Vec2f pos){drawText(text, pos, "default font");}
	virtual void drawText(string text, Rect rect){drawText(text, rect, "default font");}
	virtual Vec2f getTextSize(string text){return getTextSize(text, "default font");}

	virtual void resize(int w, int h)=0;//not really used that much but...
	virtual void render()=0;
	virtual void destroyWindow()=0;
	virtual void setGamma(float gamma)=0;
	virtual bool createWindow(string title, Vec2i screenResolution, unsigned int maxSamples)=0;
	virtual bool changeResolution(Vec2i resolution, unsigned int maxSamples)=0;
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

	//virtual void drawVertexArray(vertex3D* vertices, unsigned int numVertices, const Mat4f& transform)=0;
	//virtual void drawVertexArray(texturedVertex3D* vertices, unsigned int numVertices, const Mat4f& transform)=0;
	//virtual void drawVertexArray(texturedLitVertex3D* vertices, unsigned int numVertices, const Mat4f& transform)=0;

	virtual shared_ptr<vertexBuffer> genVertexBuffer(vertexBuffer::UsageFrequency usage)=0;
	virtual shared_ptr<indexBuffer> genIndexBuffer(indexBuffer::UsageFrequency usage)=0;

	virtual shared_ptr<texture2D> genTexture2D()=0;
	virtual shared_ptr<texture3D> genTexture3D()=0;
	virtual shared_ptr<textureCube> genTextureCube()=0;
	virtual shared_ptr<shader> genShader()=0;

	virtual shared_ptr<View> genView();

	virtual void setColor(float r, float g, float b, float a)=0;
	virtual void setColorMask(bool mask)=0;
	virtual void setDepthMask(bool mask)=0;
	virtual void setDepthTest(bool enabled)=0;
	virtual void setBlendMode(BlendMode blend)=0;
	void setColor(float r, float g, float b){setColor(r,g,b,1.0);}
	void setLightPosition(Vec3f position){lightPosition = position;}
	
	virtual void setVSync(bool enabled)=0;

	Vec3f getLightPosition(){return lightPosition;}

	float getGamma(){return currentGamma;}

	void useAnagricStereo(bool b){stereo = b;}
	void setInterOcularDistance(float d){interOcularDistance = d;}

	//virtual Vec2f project(Vec3f p, unsigned int view=0);
	//virtual Vec3f unProject(Vec3f p, unsigned int view=0);

	//const View& getView(unsigned int view){return views[view < views.size() ? view : 0];}
	//const View& getView(){return views[currentView];}

	//void resetViews(unsigned int numViews=1);
	//void viewport(float x,float y,float width,float height, unsigned int view=0);
	//void perspective(float fovy, float aspect, float near, float far, unsigned int view=0);
	//void ortho(float left, float right, float bottom, float top, float near, float far, unsigned int view=0);
	//void ortho(float left, float right, float bottom, float top, unsigned int view=0){ortho(left, right, bottom, top, 0.0, 1.0, view);}
	//void lookAt(Vec3f eye, Vec3f center, Vec3f up, unsigned int view=0);

	//bool sphereInFrustum(Sphere<float> s);

	virtual set<Vec2u> getSupportedResolutions()=0;

	virtual void flashTaskBar(int times, int length=0)=0;
	virtual void minimizeWindow()=0;

	virtual void checkErrors()=0;
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
	struct Context;

	texturedVertex2D overlay[4];
	vertex3D shapes3D[4];
	shared_ptr<vertexBuffer> overlayVBO;
	shared_ptr<vertexBuffer> shapesVBO;


	struct FBO{
		unsigned int color;
		//unsigned int normals;
		unsigned int depth;
		unsigned int fboID;
		bool colorBound;
		bool depthBound;

		FBO():color(0), depth(0), fboID(0),colorBound(true),depthBound(true){}
	}FBOs[2], multisampleFBO;	//first FBO is a multisample FBO if multisampling == true


	bool multisampling;

	int samples;
	RenderTarget renderTarget;
	Context* context;
	bool colorMask;
	bool depthMask;

	bool texCoord_clientState;
	bool normal_clientState;
	bool color_clientState;

#ifdef _DEBUG
	double errorGlowEndTime;
#endif

	OpenGLgraphics();
	~OpenGLgraphics();

	bool initFBOs(unsigned int maxSamples);
	void destroyFBOs();

	void bindRenderTarget(RenderTarget t);
	void renderFBO(RenderTarget src);

public:
	class vertexBufferGL: public GraphicsManager::vertexBuffer
	{
	private:
		unsigned int vBufferID;
		unsigned int iBufferID;
		void bindBuffer(unsigned int offset);

	public:
		vertexBufferGL(UsageFrequency u);
		~vertexBufferGL();
		void setVertexData(unsigned int size, void* data);

		void drawBuffer(Primitive primitive, unsigned int bufferOffset, unsigned int count);
	};
	class indexBufferGL: public GraphicsManager::indexBuffer
	{
	private:
		unsigned int bufferID;
		unsigned int dataCount;
		enum DataType{NO_TYPE,UCHAR,USHORT,UINT}dataType;

	public:
		indexBufferGL(UsageFrequency u);
		~indexBufferGL();
		void setData(unsigned char* data, unsigned int count);
		void setData(unsigned short* data, unsigned int count);
		void setData(unsigned int* data, unsigned int count);

		void drawBuffer(Primitive primitive, shared_ptr<vertexBuffer> buffer, unsigned int offset);
	};
	class texture2DGL: public GraphicsManager::texture2D
	{
	private:
		unsigned int textureID;
	public:
		texture2DGL();
		~texture2DGL();
		void bind(unsigned int textureUnit);
		void setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data, bool tileable);
	};
	class texture3DGL: public GraphicsManager::texture3D
	{
	private:
		unsigned int textureID;
	public:
		texture3DGL();
		~texture3DGL();
		void bind(unsigned int textureUnit);
		void setData(unsigned int Width, unsigned int Height, unsigned int Depth, Format f, unsigned char* data, bool tileable);
	};
	class textureCubeGL: public GraphicsManager::textureCube
	{
	private:
		unsigned int textureID;
	public:
		textureCubeGL();
		~textureCubeGL();
		void bind(unsigned int textureUnit);
		void setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data);
	};
	class shaderGL: public GraphicsManager::shader
	{
	private:
		string fragErrorLog;
		string vertErrorLog;
		string linkErrorLog;

		unsigned int shaderId;
		map<string, int> uniforms;
		int getUniformLocation(string uniform);
	public:
		shaderGL(): shaderId(0){}
		~shaderGL();

		void bind();
		void unbind();

		bool init(const char* vert, const char* frag, const char* geometry);

		void setUniform1f(string name, float v0);
		void setUniform2f(string name, float v0, float v1);
		void setUniform3f(string name, float v0, float v1, float v2);
		void setUniform4f(string name, float v0, float v1, float v2, float v3);
		void setUniform1i(string name, int v0);
		void setUniform2i(string name, int v0, int v1);
		void setUniform3i(string name, int v0, int v1, int v2);
		void setUniform4i(string name, int v0, int v1, int v2, int v3);

		void setUniformMatrix(string name, const Mat3f& m);
		void setUniformMatrix(string name, const Mat4f& m);

		string getErrorStrings();
	};
	bool createWindow(string title, Vec2i screenResolution, unsigned int maxSamples);
	bool changeResolution(Vec2i resolution, unsigned int maxSamples);
	void destroyWindow();

	void resize(int w, int h);

	void render();

	void swapBuffers();
	void takeScreenshot();

	void drawLine(Vec3f start, Vec3f end);
	void drawSphere(Vec3f position, float radius);
	void drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3);
	void drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4);

	void drawModel(string model, Vec3f position, Quat4f rotation, Vec3f scale);
	void drawModelCustomShader(string model, Vec3f position, Quat4f rotation, Vec3f scale);

	bool drawOverlay(Rect4f r, string tex="");
	bool drawRotatedOverlay(Rect4f r, Angle rotation, string tex="");
	bool drawPartialOverlay(Rect4f r, Rect4f t, string tex="");

	shared_ptr<vertexBuffer> genVertexBuffer(vertexBuffer::UsageFrequency usage);
	shared_ptr<indexBuffer> genIndexBuffer(indexBuffer::UsageFrequency usage);
	shared_ptr<texture2D> genTexture2D();
	shared_ptr<texture3D> genTexture3D();
	shared_ptr<textureCube> genTextureCube();
	shared_ptr<shader> genShader();

	void setGamma(float gamma);
	void setColor(float r, float g, float b, float a);
	void setColorMask(bool mask);
	void setDepthMask(bool mask);
	void setDepthTest(bool enabled);
	void setBlendMode(BlendMode blend);
	void setClientStates(bool texCoord, bool normal, bool color);

	void setVSync(bool enabled);

	void drawText(string text, Vec2f pos, string font);
	void drawText(string text, Rect rect, string font);
	Vec2f getTextSize(string text, string font);

	set<Vec2u> getSupportedResolutions();

	void flashTaskBar(int times, int length=0);
	void minimizeWindow();

	void checkErrors();
};
#endif

extern GraphicsManager* graphics;
