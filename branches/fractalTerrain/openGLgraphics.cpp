

#include "engine.h"

#ifdef OPENGL

#define GLEW_STATIC
#if defined(WINDOWS)
	#include <Windows.h>
	#include "GL/glew.h"
	#include "GL/wglew.h"
#elif defined(LINUX)
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/extensions/xf86vmode.h>
	#include <GL/glew.h>
	#include <GL/glxew.h>
	#include <GL/glx.h>
#else
	#error OS not supported by openGLgraphics
#endif

#include "png/png.h"

// NOTE: Versions of OpenGL prior to 3.3 are no longer supported. However not
// all the code has been updated to reflect this.

#if defined WINDOWS
struct OpenGLgraphics::Context
{
	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance; // not initialized?
	Context(): hDC(NULL),hRC(NULL),hWnd(NULL){}
};
#elif defined(LINUX)
struct OpenGLgraphics::Context
{
//	Display*				display;
//	int						screen;
//	/* our window instance */
//	Window					window;
	GLXContext				context;
	XSetWindowAttributes	winAttr;
	/*original desktop mode which we save so we can restore it later*/
	XF86VidModeModeInfo		desktopMode;
	bool					fullscreen;
};
#endif
class OpenGLgraphics::vertexBufferGL: public GraphicsManager::vertexBuffer
{
private:
    unsigned int vBufferID;
    void bindBuffer(unsigned int offset);

public:
    vertexBufferGL(UsageFrequency u);
    ~vertexBufferGL();
    void setVertexData(unsigned int size, void* data);
    void setVertexDataRange(unsigned int offset, unsigned int size, void* data);
    void bindTransformFeedback(GraphicsManager::Primitive primitive);
    void bindTransformFeedbackRange(GraphicsManager::Primitive primitive, unsigned int offset, unsigned int size);
    unsigned int unbindTransformFeedback();

    void drawBuffer(Primitive primitive, unsigned int bufferOffset, unsigned int count);
    void drawBufferInstanced(Primitive primitive, unsigned int bufferOffset, unsigned int count, unsigned int instances);
};
class OpenGLgraphics::indexBufferGL: public GraphicsManager::indexBuffer
{
private:
    unsigned int bufferID;
    unsigned int dataCount;
    unsigned int dataType;
    unsigned int primitiveType;
#ifdef _DEBUG
    unsigned int maxIndex;
    bool vboToSmallForMaxIndex;
#endif
    friend class multiDrawGL;
public:
    indexBufferGL(UsageFrequency u);
    ~indexBufferGL();
    void setData(unsigned char* data, Primitive primitive, unsigned int count);
    void setData(unsigned short* data, Primitive primitive, unsigned int count);
    void setData(unsigned int* data, Primitive primitive, unsigned int count);

    void bindBuffer();
    void bindBuffer(shared_ptr<vertexBuffer> buffer, unsigned int vertexBufferOffset);

    void drawBuffer();
    void drawBuffer(unsigned int numIndicies);
};
class OpenGLgraphics::multiDrawGL: public GraphicsManager::multiDraw
{
private:
    vector<GLsizei> count;
    vector<GLvoid*> indices;
    vector<GLint> baseVertex;

    shared_ptr<vertexBufferGL> VBO;
    shared_ptr<indexBufferGL> IBO;
    multiDrawGL(shared_ptr<vertexBufferGL> vBuffer, shared_ptr<indexBufferGL> iBuffer):VBO(vBuffer), IBO(iBuffer) {}
    friend class OpenGLgraphics;
public:
    void clearDraws();
    void addDraw(unsigned int indexOffset, unsigned int baseVertex, unsigned int count);
    void render();
};
class OpenGLgraphics::texture2DGL: public GraphicsManager::texture2D
{
private:
    unsigned int textureID;
public:
    friend class OpenGLgraphics;
    texture2DGL();
    ~texture2DGL();
    void bind(unsigned int textureUnit);
    void generateMipmaps();
    unsigned char* getData(unsigned int level);
    void setData(unsigned int Width, unsigned int Height, Format f, bool tileable, bool compress, unsigned char* data);
};
class OpenGLgraphics::texture3DGL: public GraphicsManager::texture3D
{
private:
    unsigned int textureID;
public:
    friend class OpenGLgraphics;
    texture3DGL();
    ~texture3DGL();
    void bind(unsigned int textureUnit);
    unsigned char* getData(unsigned int level);
    void setData(unsigned int Width, unsigned int Height, unsigned int Depth, Format f, unsigned char* data, bool tileable);
};
class OpenGLgraphics::textureCubeGL: public GraphicsManager::textureCube
{
private:
    unsigned int textureID;
public:
    friend class OpenGLgraphics;
    textureCubeGL();
    ~textureCubeGL();
    void bind(unsigned int textureUnit);
    unsigned char* getData(unsigned int level);
    void setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data);
};
class OpenGLgraphics::shaderGL: public GraphicsManager::shader
{
private:
    string fragErrorLog;
    string geomErrorLog;
    string vertErrorLog;
    string linkErrorLog;

    unsigned int shaderId;
    map<string, int> uniforms;
    map<string, int> attributes;
    int getUniformLocation(string uniform);
    bool compileShader(unsigned int sId, const char* src, string& errorLog);
public:
    shaderGL(): shaderId(0){}
    ~shaderGL();

    void bind();

    bool init5(const char* vert, const char* geometry, const char* tessellationControl, const char* tessellationEvaluation, const char* frag);
    bool init4(const char* vert, const char* geometry, const char* frag);
    bool init4(const char* vert, const char* geometry, const char* frag, vector<const char*> feedbackTransformVaryings);
    bool init(const char* vert, const char* frag);

    void setUniform1f(string name, float v0);
    void setUniform2f(string name, float v0, float v1);
    void setUniform3f(string name, float v0, float v1, float v2);
    void setUniform4f(string name, float v0, float v1, float v2, float v3);
    void setUniform1i(string name, int v0);
    void setUniform2i(string name, int v0, int v1);
    void setUniform3i(string name, int v0, int v1, int v2);
    void setUniform4i(string name, int v0, int v1, int v2, int v3);
    void setUniform1fv(string name, unsigned int n, float* v);
    void setUniform2fv(string name, unsigned int n, float* v);
    void setUniform3fv(string name, unsigned int n, float* v);
    void setUniform4fv(string name, unsigned int n, float* v);
    void setUniform1iv(string name, unsigned int n, int* v);
    void setUniform2iv(string name, unsigned int n, int* v);
    void setUniform3iv(string name, unsigned int n, int* v);
    void setUniform4iv(string name, unsigned int n, int* v);

    void setUniformMatrix(string name, const Mat3f& m);
    void setUniformMatrix(string name, const Mat4f& m);

    string getErrorStrings();
};

OpenGLgraphics::vertexBufferGL::vertexBufferGL(UsageFrequency u): vertexBuffer(u), vBufferID(0)
{
	glGenBuffers(1, &vBufferID);
}
OpenGLgraphics::vertexBufferGL::~vertexBufferGL()
{
	glBindBuffer(GL_ARRAY_BUFFER, vBufferID);

	glDeleteBuffers(1, &vBufferID);
}
void OpenGLgraphics::vertexBufferGL::setVertexData(unsigned int size, void* data)
{
	//note: size must be greater than 0
	vertexBuffer::bindBuffer();
	totalSize = size;
	if(usageFrequency == STATIC)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
}
    void OpenGLgraphics::vertexBufferGL::setVertexDataRange(unsigned int offset, unsigned int size, void* data)
{
    debugAssert(offset + size <= totalSize);

	//note: size must be greater than 0
	vertexBuffer::bindBuffer();
	if(usageFrequency == STATIC)
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	else if(usageFrequency == DYNAMIC)
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	else if(usageFrequency == STREAM)
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}
void OpenGLgraphics::vertexBufferGL::bindBuffer(unsigned int offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, vBufferID);

	for(unsigned int i=0; i<=7; i++)
	{
		auto v = (VertexAttribute)i;
		auto f = vertexAttributes.find(v);
		bool b = (f != vertexAttributes.end());
        dynamic_cast<OpenGLgraphics*>(graphics)->setClientState(i, b);
	}

	//if(dataManager.getBoundShader() == "model")
	//{
	//	dynamic_cast<OpenGLgraphics*>(graphics)->setClientStates(false, false, false);
	//}
	//else
	//{
	//	bool texCoord = false;
	//	bool normals = false;
	//	bool colors = false;

	//	for(auto i=vertexAttributes.begin(); i!=vertexAttributes.end(); i++)
	//	{
	//		if(i->first == TEXCOORD)
	//			texCoord = true;
	//		else if(i->first == NORMAL)
	//			normals = true;
	//		else if(i->first == COLOR3 || i->first == COLOR4)
	//			colors = true;
	//	}
	//	dynamic_cast<OpenGLgraphics*>(graphics)->setClientStates(texCoord, normals, colors);
	//}

	for(auto i=vertexAttributes.begin(); i!=vertexAttributes.end(); i++)
	{
		//if(dataManager.getBoundShader() != "model")
		//{
			//glDisableClientState(GL_VERTEX_ARRAY);
			if(i->first == POSITION2)
			{
                glVertexAttribPointer(i->first, 2, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			}
			else if(i->first == POSITION3)
			{
                glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			}
			else if(i->first == TEXCOORD)		glVertexAttribPointer(i->first, 2, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == NORMAL)			glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == COLOR3)			glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == COLOR4)			glVertexAttribPointer(i->first, 4, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == TANGENT)		glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == BITANGENT)		glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == GENERIC_FLOAT)	glVertexAttribPointer(i->first, 1, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
		//}
		//else
		//{
		//	if(i->first == POSITION2)
		//		glVertexPointer(2, GL_FLOAT, totalVertexSize, (void*)(i->second.offset+offset));
		//	else if(i->first == POSITION3)
		//		glVertexPointer(3, GL_FLOAT, totalVertexSize, (void*)(i->second.offset+offset));
		//	else if(i->first == TEXCOORD)
		//		glTexCoordPointer(2, GL_FLOAT, totalVertexSize, (void*)(i->second.offset+offset));
		//	else if(i->first == NORMAL)
		//		glNormalPointer(GL_FLOAT, totalVertexSize, (void*)(i->second.offset+offset));
		//	else if(i->first == COLOR3)
		//		glColorPointer(3, GL_FLOAT, totalVertexSize, (void*)(i->second.offset+offset));
		//	else if(i->first == COLOR4)
		//		glColorPointer(4, GL_FLOAT, totalVertexSize, (void*)(i->second.offset+offset));
		//}
	}
}
void OpenGLgraphics::vertexBufferGL::drawBuffer(Primitive primitive, unsigned int bufferOffset, unsigned int count)
{
	if(count != 0)
	{
		//vertexBuffer::bindBuffer();
		if(primitive == POINTS)					glDrawArrays(GL_POINTS,			bufferOffset, count);
		else if(primitive == LINES)				glDrawArrays(GL_LINES,			bufferOffset, count);
		else if(primitive == LINE_STRIP)		glDrawArrays(GL_LINE_STRIP,		bufferOffset, count);
		else if(primitive == LINE_LOOP)			glDrawArrays(GL_LINE_LOOP,		bufferOffset, count);
		else if(primitive == TRIANGLES)			glDrawArrays(GL_TRIANGLES,		bufferOffset, count);
		else if(primitive == TRIANGLE_STRIP)	glDrawArrays(GL_TRIANGLE_STRIP,	bufferOffset, count);
		else if(primitive == TRIANGLE_FAN)		glDrawArrays(GL_TRIANGLE_FAN,	bufferOffset, count);
		else if(primitive == QUADS)				glDrawArrays(GL_QUADS,			bufferOffset, count);
		else if(primitive == QUAD_STRIP)		glDrawArrays(GL_QUAD_STRIP,		bufferOffset, count);
		else if(primitive == POLYGON)			glDrawArrays(GL_POLYGON,		bufferOffset, count);
	}
}
void OpenGLgraphics::vertexBufferGL::drawBufferInstanced(Primitive primitive, unsigned int bufferOffset, unsigned int count, unsigned int instances)
{
	if(count != 0 && instances != 0)
	{
		if(graphics->hasShaderModel4())
		{
			if(primitive == POINTS)					glDrawArraysInstanced(GL_POINTS,		bufferOffset, count, instances);
			else if(primitive == LINES)				glDrawArraysInstanced(GL_LINES,			bufferOffset, count, instances);
			else if(primitive == LINE_STRIP)		glDrawArraysInstanced(GL_LINE_STRIP,	bufferOffset, count, instances);
			else if(primitive == LINE_LOOP)			glDrawArraysInstanced(GL_LINE_LOOP,		bufferOffset, count, instances);
			else if(primitive == TRIANGLES)			glDrawArraysInstanced(GL_TRIANGLES,		bufferOffset, count, instances);
			else if(primitive == TRIANGLE_STRIP)	glDrawArraysInstanced(GL_TRIANGLE_STRIP,bufferOffset, count, instances);
			else if(primitive == TRIANGLE_FAN)		glDrawArraysInstanced(GL_TRIANGLE_FAN,	bufferOffset, count, instances);
			else if(primitive == QUADS)				glDrawArraysInstanced(GL_QUADS,			bufferOffset, count, instances);
			else if(primitive == QUAD_STRIP)		glDrawArraysInstanced(GL_QUAD_STRIP,	bufferOffset, count, instances);
			else if(primitive == POLYGON)			glDrawArraysInstanced(GL_POLYGON,		bufferOffset, count, instances);
		}
		else if(GLEW_EXT_draw_instanced)
		{
			if(primitive == POINTS)					glDrawArraysInstancedEXT(GL_POINTS,			bufferOffset, count, instances);
			else if(primitive == LINES)				glDrawArraysInstancedEXT(GL_LINES,			bufferOffset, count, instances);
			else if(primitive == LINE_STRIP)		glDrawArraysInstancedEXT(GL_LINE_STRIP,		bufferOffset, count, instances);
			else if(primitive == LINE_LOOP)			glDrawArraysInstancedEXT(GL_LINE_LOOP,		bufferOffset, count, instances);
			else if(primitive == TRIANGLES)			glDrawArraysInstancedEXT(GL_TRIANGLES,		bufferOffset, count, instances);
			else if(primitive == TRIANGLE_STRIP)	glDrawArraysInstancedEXT(GL_TRIANGLE_STRIP,	bufferOffset, count, instances);
			else if(primitive == TRIANGLE_FAN)		glDrawArraysInstancedEXT(GL_TRIANGLE_FAN,	bufferOffset, count, instances);
			else if(primitive == QUADS)				glDrawArraysInstancedEXT(GL_QUADS,			bufferOffset, count, instances);
			else if(primitive == QUAD_STRIP)		glDrawArraysInstancedEXT(GL_QUAD_STRIP,		bufferOffset, count, instances);
			else if(primitive == POLYGON)			glDrawArraysInstancedEXT(GL_POLYGON,		bufferOffset, count, instances);
		}
		else
		{
			debugBreak();
		}
	}
}
void OpenGLgraphics::vertexBufferGL::bindTransformFeedback(GraphicsManager::Primitive primitive)
{
	if(graphics->hasShaderModel4())
	{
		glEnable(GL_RASTERIZER_DISCARD);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, dynamic_cast<OpenGLgraphics*>(graphics)->transformFeedbackQueryID);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vBufferID);
		if(primitive == POINTS)			glBeginTransformFeedback(GL_POINTS);
		else if(primitive == LINES)		glBeginTransformFeedback(GL_LINES);
		else if(primitive == TRIANGLES)	glBeginTransformFeedback(GL_TRIANGLES);
		else debugBreak();
	}
	else
	{
		debugBreak();
	}
}
void OpenGLgraphics::vertexBufferGL::bindTransformFeedbackRange(GraphicsManager::Primitive primitive, unsigned int offset, unsigned int size)
{
	if(graphics->hasShaderModel4())
	{
		glEnable(GL_RASTERIZER_DISCARD);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, dynamic_cast<OpenGLgraphics*>(graphics)->transformFeedbackQueryID);
		glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vBufferID, offset, size);
		if(primitive == POINTS)			glBeginTransformFeedback(GL_POINTS);
		else if(primitive == LINES)		glBeginTransformFeedback(GL_LINES);
		else if(primitive == TRIANGLES)	glBeginTransformFeedback(GL_TRIANGLES);
		else debugBreak();
	}
	else
	{
		debugBreak();
	}
}
unsigned int OpenGLgraphics::vertexBufferGL::unbindTransformFeedback()
{
	if(graphics->hasShaderModel4())
	{
		glEndTransformFeedback();
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		glDisable(GL_RASTERIZER_DISCARD);

		GLint64 primitiveCount = -1;
		glGetQueryObjecti64v(dynamic_cast<OpenGLgraphics*>(graphics)->transformFeedbackQueryID, GL_QUERY_RESULT, &primitiveCount);

		debugAssert(primitiveCount <=  INT_MAX);

		return primitiveCount;
	}
	else
	{
		debugBreak();
		return 0;
	}
}
OpenGLgraphics::indexBufferGL::indexBufferGL(UsageFrequency u): indexBuffer(u), bufferID(0), dataCount(0), dataType(0)
#ifdef _DEBUG
	, maxIndex(0), vboToSmallForMaxIndex(false)
#endif
{
	glGenBuffers(1, &bufferID);
}
OpenGLgraphics::indexBufferGL::~indexBufferGL()
{
	glDeleteBuffers(1, &bufferID);
}
void OpenGLgraphics::indexBufferGL::setData(unsigned char* data, Primitive primitive, unsigned int count)
{
	dataType = GL_UNSIGNED_BYTE;
	dataCount = count;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	if(usageFrequency == STATIC)		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STREAM_DRAW);
	else cout << "useageFrequency not set: ";
	
#ifdef _DEBUG
	maxIndex = 0;
	for(unsigned int i=0; i<count; i++)
	{
		if(data[i] > maxIndex)
			maxIndex = data[i];
	}
#endif
}
void OpenGLgraphics::indexBufferGL::setData(unsigned short* data, Primitive primitive, unsigned int count)
{
	static_assert(sizeof(unsigned short) == 2, "unsigned short assumed to have size of 2 bytes");
	
	dataType = GL_UNSIGNED_SHORT;
	dataCount = count;

	if(primitive == POINTS)					primitiveType = GL_POINTS;
	else if(primitive == LINES)				primitiveType = GL_LINES;
	else if(primitive == LINE_STRIP)		primitiveType = GL_LINE_STRIP;
	else if(primitive == LINE_LOOP)			primitiveType = GL_LINE_LOOP;
	else if(primitive == TRIANGLES)			primitiveType = GL_TRIANGLES;
	else if(primitive == TRIANGLE_STRIP)	primitiveType = GL_TRIANGLE_STRIP;
	else if(primitive == TRIANGLE_FAN)		primitiveType = GL_TRIANGLE_FAN;
	else if(primitive == QUADS)				primitiveType = GL_QUADS;
	else if(primitive == QUAD_STRIP)		primitiveType = GL_QUAD_STRIP;
	else if(primitive == POLYGON)			primitiveType = GL_POLYGON;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	if(usageFrequency == STATIC)		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*2, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*2, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*2, data, GL_STREAM_DRAW);
	else cout << "useageFrequency not set: ";
	
#ifdef _DEBUG
	maxIndex = 0;
	for(unsigned int i=0; i<count; i++)
	{
		if(data[i] > maxIndex)
			maxIndex = data[i];
	}
#endif
}
void OpenGLgraphics::indexBufferGL::setData(unsigned int* data, Primitive primitive, unsigned int count)
{

	static_assert(sizeof(unsigned int) == 4, "unsigned int assumed to have size of 4 bytes");

	dataCount = count;
	dataType = GL_UNSIGNED_INT;

	if(primitive == POINTS)					primitiveType = GL_POINTS;
	else if(primitive == LINES)				primitiveType = GL_LINES;
	else if(primitive == LINE_STRIP)		primitiveType = GL_LINE_STRIP;
	else if(primitive == LINE_LOOP)			primitiveType = GL_LINE_LOOP;
	else if(primitive == TRIANGLES)			primitiveType = GL_TRIANGLES;
	else if(primitive == TRIANGLE_STRIP)	primitiveType = GL_TRIANGLE_STRIP;
	else if(primitive == TRIANGLE_FAN)		primitiveType = GL_TRIANGLE_FAN;
	else if(primitive == QUADS)				primitiveType = GL_QUADS;
	else if(primitive == QUAD_STRIP)		primitiveType = GL_QUAD_STRIP;
	else if(primitive == POLYGON)			primitiveType = GL_POLYGON;
	else if(primitive == PATCHES && graphics->hasShaderModel5()) primitiveType = GL_PATCHES;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	if(usageFrequency == STATIC)		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_STREAM_DRAW);
	else debugBreak()//useageFrequency not set
	
#ifdef _DEBUG
	maxIndex = 0;
	for(unsigned int i=0; i<count; i++)
	{
		if(data[i] > maxIndex)
			maxIndex = data[i];
	}
#endif
}
void OpenGLgraphics::indexBufferGL::bindBuffer() //vertex buffer MUST be bound seperately!!!
{
#ifdef _DEBUG
	vboToSmallForMaxIndex = false;
#endif

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}
void OpenGLgraphics::indexBufferGL::bindBuffer(shared_ptr<vertexBuffer> buffer, unsigned int vertexBufferOffset)
{
#ifdef _DEBUG
	vboToSmallForMaxIndex = buffer->getNumVertices() <= maxIndex;
#endif

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	buffer->bindBuffer(vertexBufferOffset);
}
void OpenGLgraphics::indexBufferGL::drawBuffer()
{
	debugAssert(!vboToSmallForMaxIndex);
	debugAssert(dataType == GL_UNSIGNED_BYTE || dataType == GL_UNSIGNED_SHORT || dataType == GL_UNSIGNED_INT);
	debugAssert(primitiveType == GL_POINTS		 || primitiveType == GL_LINES		|| primitiveType == GL_LINE_STRIP		||
				primitiveType == GL_LINE_LOOP	 || primitiveType == GL_TRIANGLES	|| primitiveType == GL_TRIANGLE_STRIP	||
				primitiveType == GL_TRIANGLE_FAN || primitiveType == GL_QUADS		|| primitiveType == GL_QUAD_STRIP ||
				(graphics->hasShaderModel5() && primitiveType == GL_PATCHES));

	if(dataCount != 0)
	{
//		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(primitiveType, dataCount, dataType, 0);
	}
}
void OpenGLgraphics::indexBufferGL::drawBuffer(unsigned int numIndicies)
{
	if(dataCount == 0 || numIndicies == 0)
		return;

	debugAssert(dataType == GL_UNSIGNED_BYTE || dataType == GL_UNSIGNED_SHORT || dataType == GL_UNSIGNED_INT);
	debugAssert(primitiveType == GL_POINTS		 || primitiveType == GL_LINES		|| primitiveType == GL_LINE_STRIP		||
				primitiveType == GL_LINE_LOOP	 || primitiveType == GL_TRIANGLES	|| primitiveType == GL_TRIANGLE_STRIP	||
				primitiveType == GL_TRIANGLE_FAN || primitiveType == GL_QUADS		|| primitiveType == GL_QUAD_STRIP);
	debugAssert(numIndicies != 0);

	if(dataCount != 0)
	{
		glDrawElements(primitiveType, min(dataCount,numIndicies), dataType, 0);
	}
}
void OpenGLgraphics::multiDrawGL::clearDraws()
{
    count.clear();
    indices.clear();
    baseVertex.clear();
}
void OpenGLgraphics::multiDrawGL::addDraw(unsigned int indexOffset, unsigned int bVertex, unsigned int c)
{
    indices.push_back((GLvoid*)(intptr_t)indexOffset);
    baseVertex.push_back((GLint)bVertex);
    count.push_back((GLsizei)c);
}
void OpenGLgraphics::multiDrawGL::render()
{
    IBO->bindBuffer(VBO, 0);

	debugAssert(IBO->dataType == GL_UNSIGNED_BYTE ||
                IBO->dataType == GL_UNSIGNED_SHORT ||
                IBO->dataType == GL_UNSIGNED_INT);

	debugAssert(IBO->primitiveType == GL_POINTS ||
                IBO->primitiveType == GL_LINES ||
                IBO->primitiveType == GL_LINE_STRIP ||
				IBO->primitiveType == GL_LINE_LOOP ||
                IBO->primitiveType == GL_TRIANGLES ||
                IBO->primitiveType == GL_TRIANGLE_STRIP ||
				IBO->primitiveType == GL_TRIANGLE_FAN ||
                IBO->primitiveType == GL_QUADS ||
                IBO->primitiveType == GL_QUAD_STRIP);

    /*for(int i = 0; i < count.size(); i++)
        glDrawElementsBaseVertex(IBO->primitiveType,
                                 count[i],
                                 IBO->dataType,
                                 indices[i],
                                 baseVertex[i]);*/

    glMultiDrawElementsBaseVertex(IBO->primitiveType,
                                  count.data(),
                                  IBO->dataType,
                                  indices.data(),
                                  count.size(),
                                  baseVertex.data());
}
OpenGLgraphics::texture2DGL::texture2DGL()
{
	glGenTextures(1, &textureID);
}
OpenGLgraphics::texture2DGL::~texture2DGL()
{
	glDeleteTextures(1, &textureID);
}
void OpenGLgraphics::texture2DGL::bind(unsigned int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}
void OpenGLgraphics::texture2DGL::generateMipmaps()
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	if(graphics->hasShaderModel4()) //means we are running OpenGL 3+
		glGenerateMipmap(GL_TEXTURE_2D);
	else
		glGenerateMipmapEXT(GL_TEXTURE_2D);
}
void OpenGLgraphics::texture2DGL::setData(unsigned int Width, unsigned int Height, Format f, bool tileable, bool compress, unsigned char* data)
{
	if(f != INTENSITY && f != LUMINANCE_ALPHA && f != BGR && f != BGRA && f != RGB && f != RGBA && f != RGB16 && f != RGBA16 && f != RGB16F && f != RGBA16F && f != DEPTH)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if(tileable)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	
//	if(!gl2Hacks || GLEW_ARB_texture_non_power_of_two || (!(width & (width-1)) && !(height & (height-1)))) //if we have support for NPOT textures, or texture is power of 2
//	{
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		if(GLEW_EXT_texture_compression_s3tc && compress && (format == BGR || format == BGRA || format == RGB || format == RGBA))
		{
			if(format == BGR)					glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
			else if(format == BGRA)				glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
			else if(format == RGB)				glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if(format == RGBA)				glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			if(format == INTENSITY)				glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
			else if(format == LUMINANCE_ALPHA)	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
			else if(format == BGR)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
			else if(format == BGRA)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
			else if(format == RGB)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if(format == RGBA)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else if(format == RGB16)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data);
			else if(format == RGBA16)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
			else if(format == RGB16F)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
			else if(format == RGBA16F)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
			else if(format == DEPTH)			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, data);
		}
//		if(gl2Hacks)
//		{
//			glEnable(GL_TEXTURE_2D); //required for some ATI drivers?
//		}

		if(graphics->hasShaderModel4()) //means we are running OpenGL 3+
			glGenerateMipmap(GL_TEXTURE_2D);
		else
			glGenerateMipmapEXT(GL_TEXTURE_2D);
//	}
//	else // we need to resize to a power of 2 (should only occur on GeForce FX graphics cards)
//	{
//		if(format == INTENSITY)				gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_INTENSITY, GL_UNSIGNED_BYTE, data);
//		else if(format == LUMINANCE_ALPHA)	gluBuild2DMipmaps(GL_TEXTURE_2D, 2, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
//		else if(format == BGR)				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
//		else if(format == BGRA)				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data);
//		else if(format == RGB)				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
//		else if(format == RGBA)				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
//		else if(format == RGB16)			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB16, GL_UNSIGNED_SHORT, data);
//		else if(format == RGBA16)			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA16, GL_UNSIGNED_SHORT, data);
//		else if(format == RGB16F)			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB16F, GL_FLOAT, data);
//		else if(format == RGBA16F)			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA16F, GL_FLOAT, data);
//		else if(format == DEPTH)			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_DEPTH_COMPONENT24, GL_UNSIGNED_BYTE, data);
//	}
}
unsigned char* OpenGLgraphics::texture2DGL::getData(unsigned int level)
{
	GLenum fmt=0;
	unsigned int bytesPerPixel;
	if(format == INTENSITY)
	{
		fmt = GL_INTENSITY;
		bytesPerPixel=1;
	}
	else if(format == LUMINANCE_ALPHA)
	{
		fmt = GL_LUMINANCE_ALPHA;
		bytesPerPixel=2;
	}
	else if(format == BGR)
	{
		fmt = GL_BGR;
		bytesPerPixel=3;
	}
	else if(format == BGRA)
	{
		fmt = GL_BGRA;
		bytesPerPixel=4;
	}
	else if(format == RGB)
	{
		fmt = GL_RGB;
		bytesPerPixel=3;
	}
	else if(format == RGBA)
	{
		fmt = GL_RGBA;
		bytesPerPixel=4;
	}
	else if(format == RGB16)
	{
		fmt = GL_RGB16;
		bytesPerPixel=6;
	}
	else if(format == RGBA16)
	{
		fmt = GL_RGBA16;
		bytesPerPixel=8;
	}
	else if(format == RGB16F)
	{
		fmt = GL_RGB16F;
		bytesPerPixel=6;
	}
	else if(format == RGBA16F)
	{
		fmt = GL_RGBA16F;
		bytesPerPixel=8;
	}
	else if(format == DEPTH)
	{
		fmt = GL_DEPTH_COMPONENT;
		bytesPerPixel=1;
	}
	else /* unrecognized format!!! */
	{
		debugBreak();
		fmt = GL_INTENSITY;
		bytesPerPixel=1;
	}

	unsigned char* data = new unsigned char[max(width>>level,1)*max(height>>level,1)*bytesPerPixel];
	glBindTexture(GL_TEXTURE_2D, textureID);
	glGetTexImage(GL_TEXTURE_2D, level, fmt, GL_UNSIGNED_BYTE, data);
	return data;
}
OpenGLgraphics::texture3DGL::texture3DGL()
{
	glGenTextures(1, &textureID);
}
OpenGLgraphics::texture3DGL::~texture3DGL()
{
	glDeleteTextures(1, &textureID);
}
void OpenGLgraphics::texture3DGL::bind(unsigned int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_3D, textureID);
}
void OpenGLgraphics::texture3DGL::setData(unsigned int Width, unsigned int Height, unsigned int Depth, Format f, unsigned char* data, bool tileable)
{
	if(f != INTENSITY && f != LUMINANCE_ALPHA && f != BGR && f != BGRA && f != RGB && f != RGBA && f != RGB16 && f != RGBA16 && f != RGB16F && f != RGBA16F)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;
	depth = Depth;

	glBindTexture(GL_TEXTURE_3D, textureID);
	if(format == INTENSITY)				glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	else if(format == LUMINANCE_ALPHA)	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, width, height, depth, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
	else if(format == BGR)				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	else if(format == BGRA)				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	else if(format == RGB)				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if(format == RGBA)				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else if(format == RGB16)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, width, height, depth, 0, GL_RGB, GL_UNSIGNED_SHORT, data);
	else if(format == RGBA16)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
	else if(format == RGB16F)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, width, height, depth, 0, GL_RGB, GL_FLOAT, data);
	else if(format == RGBA16F)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, width, height, depth, 0, GL_RGBA, GL_FLOAT, data);

	if(graphics->hasShaderModel4()) //means we are running OpenGL 3+
		glGenerateMipmap(GL_TEXTURE_3D);
	else
		glGenerateMipmapEXT(GL_TEXTURE_3D);

	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if(tileable)
	{
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	//glBindTexture(GL_TEXTURE_3D, 0);
}
unsigned char* OpenGLgraphics::texture3DGL::getData(unsigned int level)
{
	GLenum fmt=0;
	unsigned int bytesPerPixel;
	if(format == INTENSITY)
	{
		fmt = GL_INTENSITY;
		bytesPerPixel=1;
	}
	else if(format == LUMINANCE_ALPHA)
	{
		fmt = GL_LUMINANCE_ALPHA;
		bytesPerPixel=2;
	}
	else if(format == BGR)
	{
		fmt = GL_BGR;
		bytesPerPixel=3;
	}
	else if(format == BGRA)
	{
		fmt = GL_BGRA;
		bytesPerPixel=4;
	}
	else if(format == RGB)
	{
		fmt = GL_RGB;
		bytesPerPixel=3;
	}
	else if(format == RGBA)
	{
		fmt = GL_RGBA;
		bytesPerPixel=4;
	}
	else if(format == RGB16)
	{
		fmt = GL_RGB16;
		bytesPerPixel=6;
	}
	else if(format == RGBA16)
	{
		fmt = GL_RGBA16;
		bytesPerPixel=8;
	}
	else if(format == RGB16F)
	{
		fmt = GL_RGB16F;
		bytesPerPixel=6;
	}
	else if(format == RGBA16F)
	{
		fmt = GL_RGBA16F;
		bytesPerPixel=8;
	}
	else if(format == DEPTH)
	{
		fmt = GL_DEPTH_COMPONENT;
		bytesPerPixel=1;
	}
	else /* unrecognized format!!! */
	{
		debugBreak();
		fmt = GL_INTENSITY;
		bytesPerPixel=1;
	}

	unsigned char* data = new unsigned char[max(width>>level,1)*max(height>>level,1)*max(depth>>level,1)*bytesPerPixel];
	glBindTexture(GL_TEXTURE_3D, textureID);
	glGetTexImage(GL_TEXTURE_3D, level, fmt, GL_UNSIGNED_BYTE, data);
	return data;
}
OpenGLgraphics::textureCubeGL::textureCubeGL()
{
	glGenTextures(1, &textureID);
}
OpenGLgraphics::textureCubeGL::~textureCubeGL()
{
	glDeleteTextures(1, &textureID);
}
void OpenGLgraphics::textureCubeGL::bind(unsigned int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}
void OpenGLgraphics::textureCubeGL::setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data)
{
	if(f != INTENSITY && f != LUMINANCE_ALPHA && f != BGR && f != BGRA && f != RGB && f != RGBA && f != RGB16 && f != RGBA16 && f != RGB16F && f != RGBA16F)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	if(format == INTENSITY)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data ? data + width*height * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data ? data + width*height * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data ? data + width*height * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data ? data + width*height * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data ? data + width*height * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_INTENSITY, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data ? data + width*height * 5 : nullptr);
	}
	else if(format == LUMINANCE_ALPHA)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data ? data + width*height*2 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data ? data + width*height*2 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data ? data + width*height*2 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data ? data + width*height*2 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data ? data + width*height*2 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data ? data + width*height*2 * 5 : nullptr);
	}
	else if(format == BGR)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 5 : nullptr);
	}
	else if(format == BGRA)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 5 : nullptr);
	}
	else if(format == RGB)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ? data + width*height*3 * 5 : nullptr);
	}
	else if(format == RGBA)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ? data + width*height*4 * 5 : nullptr);
	}
	else if(format == RGB16)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data ? data + width*height*2*3 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data ? data + width*height*2*3 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data ? data + width*height*2*3 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data ? data + width*height*2*3 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data ? data + width*height*2*3 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data ? data + width*height*2*3 * 5 : nullptr);
	}
	else if(format == RGBA16)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data ? data + width*height*2*4 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data ? data + width*height*2*4 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data ? data + width*height*2*4 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data ? data + width*height*2*4 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data ? data + width*height*2*4 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data ? data + width*height*2*4 * 5 : nullptr);
	}
	else if(format == RGB16F)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data ? data + width*height*4*3 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data ? data + width*height*4*3 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data ? data + width*height*4*3 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data ? data + width*height*4*3 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data ? data + width*height*4*3 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data ? data + width*height*4*3 * 5 : nullptr);
	}
	else if(format == RGBA16F)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data + width*height*4*4 * 0 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data + width*height*4*4 * 1 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data + width*height*4*4 * 2 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data + width*height*4*4 * 3 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data + width*height*4*4 * 4 : nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data + width*height*4*4 * 5 : nullptr);
	}

	if(graphics->hasShaderModel4()) //means we are running OpenGL 3+
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	else
		glGenerateMipmapEXT(GL_TEXTURE_CUBE_MAP);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
unsigned char* OpenGLgraphics::textureCubeGL::getData(unsigned int level)
{
	GLenum fmt=0;
	unsigned int bytesPerPixel;
	if(format == INTENSITY)
	{
		fmt = GL_INTENSITY;
		bytesPerPixel=1;
	}
	else if(format == LUMINANCE_ALPHA)
	{
		fmt = GL_LUMINANCE_ALPHA;
		bytesPerPixel=2;
	}
	else if(format == BGR)
	{
		fmt = GL_BGR;
		bytesPerPixel=3;
	}
	else if(format == BGRA)
	{
		fmt = GL_BGRA;
		bytesPerPixel=4;
	}
	else if(format == RGB)
	{
		fmt = GL_RGB;
		bytesPerPixel=3;
	}
	else if(format == RGBA)
	{
		fmt = GL_RGBA;
		bytesPerPixel=4;
	}
	else if(format == RGB16)
	{
		fmt = GL_RGB16;
		bytesPerPixel=6;
	}
	else if(format == RGBA16)
	{
		fmt = GL_RGBA16;
		bytesPerPixel=8;
	}
	else if(format == RGB16F)
	{
		fmt = GL_RGB16F;
		bytesPerPixel=6;
	}
	else if(format == RGBA16F)
	{
		fmt = GL_RGBA16F;
		bytesPerPixel=8;
	}
	else if(format == DEPTH)
	{
		fmt = GL_DEPTH_COMPONENT;
		bytesPerPixel=1;
	}
	else /* unrecognized format!!! */
	{
		debugBreak();
		fmt = GL_INTENSITY;
		bytesPerPixel=1;
	}

	unsigned int faceSize = max(width>>level,1)*max(height>>level,1)*bytesPerPixel;
	unsigned char* data = new unsigned char[faceSize*6];
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, fmt, GL_UNSIGNED_BYTE, data);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, fmt, GL_UNSIGNED_BYTE, data+faceSize);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, fmt, GL_UNSIGNED_BYTE, data+faceSize*2);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, fmt, GL_UNSIGNED_BYTE, data+faceSize*3);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, fmt, GL_UNSIGNED_BYTE, data+faceSize*4);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, fmt, GL_UNSIGNED_BYTE, data+faceSize*5);
	return data;
}
int OpenGLgraphics::shaderGL::getUniformLocation(string uniform)
{
	auto it = uniforms.find(uniform);
	if(it == uniforms.end())
	{
		it = uniforms.insert(std::pair<string, int>(uniform, glGetUniformLocation(shaderId, uniform.c_str()))).first;
	}
	return it->second;
}
OpenGLgraphics::shaderGL::~shaderGL()
{
	if(shaderId != 0)
		glDeleteProgram(shaderId);
}
void OpenGLgraphics::shaderGL::bind()
{
	boundShader = this;
	glUseProgram(shaderId);
}
bool OpenGLgraphics::shaderGL::compileShader(GLuint sId, const char* src, string& errorLog)
{
	glShaderSource(sId, 1, &src, NULL);
	glCompileShader(sId);

	int length, success, i;
	glGetShaderiv(sId, GL_INFO_LOG_LENGTH, &length);

	char* str=new char[length];
	glGetShaderInfoLog(sId,length,&i,str);
	errorLog = string(str);
	delete[] str;
	//check for successful load
	glGetShaderiv(sId,GL_COMPILE_STATUS,&success);
	return success != 0;
}
bool OpenGLgraphics::shaderGL::init(const char* vert, const char* frag)
{
	if(shaderId != 0)
		glDeleteProgram(shaderId);

	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	string vertString;
	string fragString;

	if(graphics->hasShaderModel4()) //means we are running OpenGL 3+
	{
		vertString = string("#version 330\n")+vert;
		fragString = string("#version 330\n\nout vec4 _FragColor;\n")+frag;

		boost::replace_all(vertString, "\t", " ");
		boost::replace_all(vertString, "attribute ", "in ");
		boost::replace_all(vertString, "varying ", "out ");

		boost::replace_all(fragString, "\t", " ");
		boost::replace_all(fragString, "varying ", "in ");
		boost::replace_all(fragString, "gl_FragColor", "_FragColor");
		boost::replace_all(fragString, "texture1D", "texture");
		boost::replace_all(fragString, "texture2D", "texture");
		boost::replace_all(fragString, "texture3D", "texture");
		boost::replace_all(fragString, "textureCube", "texture");
	}
	else
	{
		vertString = string("#version 120\n")+vert;
		fragString = string("#version 120\n")+frag;
	}

	int i, length, success;//used whenever a pointer to int is required

	if(!compileShader(v, vertString.c_str(), vertErrorLog) || !compileShader(f, fragString.c_str(), fragErrorLog))
	{
		return false;
	}

	shaderId = glCreateProgram();
	glAttachShader(shaderId,f);
	glAttachShader(shaderId,v);

	glBindAttribLocation(shaderId, 0, "Position2");
	glBindAttribLocation(shaderId, 1, "Position");
	glBindAttribLocation(shaderId, 2, "TexCoord");
	glBindAttribLocation(shaderId, 3, "Normal");
	glBindAttribLocation(shaderId, 4, "Color3");
	glBindAttribLocation(shaderId, 5, "Color4");
	glBindAttribLocation(shaderId, 6, "Tangent");
	glBindAttribLocation(shaderId, 7, "Bitangent");
	glBindAttribLocation(shaderId, 8, "GenericFloat");
	glLinkProgram(shaderId);


	//get shader link errors
	glGetProgramiv(shaderId,GL_INFO_LOG_LENGTH,&length);
	char* cl=new char[length];
	glGetProgramInfoLog(shaderId,length,&i,cl);
	linkErrorLog = string(cl);
	delete[] cl;

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if(!success)
	{
		return false;
	}

	glUseProgram(0);
	glDeleteShader(f); // we no longer need these shaders individually, although they
	glDeleteShader(v); // will not actually be deleted until the shader program is deleted

	return true;
}
bool OpenGLgraphics::shaderGL::init4(const char* vertex, const char* geometry, const char* fragment)
{
	if(!graphics->hasShaderModel4()) //means we are running OpenGL 3+
	{
		vertErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		fragErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		geomErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		linkErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		return false;
	}

	if(shaderId != 0)
	{
		glDeleteProgram(shaderId);
		shaderId = 0;
	}

	GLuint v = vertex ? glCreateShader(GL_VERTEX_SHADER) : 0;
	GLuint f = fragment ? glCreateShader(GL_FRAGMENT_SHADER) : 0;
	GLuint g = geometry ? glCreateShader(GL_GEOMETRY_SHADER) : 0;
	int i, length, success;//used whenever a pointer to int is required

	if((vertex && !compileShader(v, vertex, vertErrorLog)) || (fragment && !compileShader(f, fragment, fragErrorLog)) || (geometry && !compileShader(g, geometry, geomErrorLog)))
	{
		return false;
	}

	shaderId = glCreateProgram();
	if(vertex)
	{
		glAttachShader(shaderId,v);
	}
	if(fragment)
	{
		glAttachShader(shaderId,f);
	}
	if(geometry)
	{
		glAttachShader(shaderId,g);
	}

	glBindAttribLocation(shaderId, 0, "Position2");
	glBindAttribLocation(shaderId, 1, "Position");
	glBindAttribLocation(shaderId, 2, "TexCoord");
	glBindAttribLocation(shaderId, 3, "Normal");
	glBindAttribLocation(shaderId, 4, "Color3");
	glBindAttribLocation(shaderId, 5, "Color4");
	glBindAttribLocation(shaderId, 6, "Tangent");
	glBindAttribLocation(shaderId, 7, "Bitangent");
	glBindAttribLocation(shaderId, 8, "GenericFloat");

	glLinkProgram(shaderId);


	//get shader link errors
	glGetProgramiv(shaderId,GL_INFO_LOG_LENGTH,&length);
	char* str=new char[length];
	glGetProgramInfoLog(shaderId,length,&i,str);
	linkErrorLog = string(str);
	delete[] str;

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if(!success)
	{
		return false;
	}

	glUseProgram(0);

	// we no longer need these shaders individually, although they
	// will not actually be deleted until the shader program is deleted
	if(vertex)
	{
		glDeleteShader(v);
	}
	if(fragment)
	{
		glDeleteShader(f);
	}
	if(geometry)
	{
		glDeleteShader(g);
	}

	return true;
}
bool OpenGLgraphics::shaderGL::init4(const char* vertex, const char* geometry, const char* fragment, vector<const char*> feedbackTransformVaryings)
{
	if(!graphics->hasShaderModel4()) //means we are running OpenGL 3+
	{
		vertErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		fragErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		geomErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		linkErrorLog = "Error: Must be running in OpenGL 3 mode to use OpenGL 3 shaders.\n";
		return false;
	}

	if(shaderId != 0)
	{
		glDeleteProgram(shaderId);
		shaderId = 0;
	}

	GLuint v = vertex ? glCreateShader(GL_VERTEX_SHADER) : 0;
	GLuint f = fragment ? glCreateShader(GL_FRAGMENT_SHADER) : 0;
	GLuint g = geometry ? glCreateShader(GL_GEOMETRY_SHADER) : 0;
	int i, length, success;//used whenever a pointer to int is required

	if((vertex && !compileShader(v, vertex, vertErrorLog)) || (fragment && !compileShader(f, fragment, fragErrorLog)) || (geometry && !compileShader(g, geometry, geomErrorLog)))
	{
		return false;
	}

	shaderId = glCreateProgram();
	if(vertex)
	{
		glAttachShader(shaderId,v);
	}
	if(fragment)
	{
		glAttachShader(shaderId,f);
	}
	if(geometry)
	{
		glAttachShader(shaderId,g);
	}

	glBindAttribLocation(shaderId, 0, "Position2");
	glBindAttribLocation(shaderId, 1, "Position");
	glBindAttribLocation(shaderId, 2, "TexCoord");
	glBindAttribLocation(shaderId, 3, "Normal");
	glBindAttribLocation(shaderId, 4, "Color3");
	glBindAttribLocation(shaderId, 5, "Color4");
	glBindAttribLocation(shaderId, 6, "Tangent");
	glBindAttribLocation(shaderId, 7, "Bitangent");
	glBindAttribLocation(shaderId, 8, "GenericFloat");

	if(!feedbackTransformVaryings.empty())
	{
		glTransformFeedbackVaryings(shaderId, feedbackTransformVaryings.size(), &feedbackTransformVaryings[0], GL_INTERLEAVED_ATTRIBS);
	}

	glLinkProgram(shaderId);


	//get shader link errors
	glGetProgramiv(shaderId,GL_INFO_LOG_LENGTH,&length);
	char* str=new char[length];
	glGetProgramInfoLog(shaderId,length,&i,str);
	linkErrorLog = string(str);
	delete[] str;

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if(!success)
	{
		return false;
	}

	glUseProgram(0);

	// we no longer need these shaders individually, although they
	// will not actually be deleted until the shader program is deleted
	if(vertex)
	{
		glDeleteShader(v);
	}
	if(fragment)
	{
		glDeleteShader(f);
	}
	if(geometry)
	{
		glDeleteShader(g);
	}

	return true;
}
bool OpenGLgraphics::shaderGL::init5(const char* vertex, const char* geometry, const char* tessellationControl, const char* tessellationEvaluation, const char* fragment)
{
	if(!graphics->hasShaderModel5()) //means we are running OpenGL 3+
	{
		vertErrorLog = "Error: Must be running in OpenGL 4 mode to use OpenGL 4 shaders.\n";
		fragErrorLog = "Error: Must be running in OpenGL 4 mode to use OpenGL 4 shaders.\n";
		geomErrorLog = "Error: Must be running in OpenGL 4 mode to use OpenGL 4 shaders.\n";
		linkErrorLog = "Error: Must be running in OpenGL 4 mode to use OpenGL 4 shaders.\n";
		return false;
	}

	if(shaderId != 0)
	{
		glDeleteProgram(shaderId);
		shaderId = 0;
	}

	GLuint v = vertex ? glCreateShader(GL_VERTEX_SHADER) : 0;
	GLuint f = fragment ? glCreateShader(GL_FRAGMENT_SHADER) : 0;
	GLuint tc = tessellationControl ? glCreateShader(GL_TESS_CONTROL_SHADER) : 0;
	GLuint te = tessellationEvaluation ? glCreateShader(GL_TESS_EVALUATION_SHADER) : 0;
	GLuint g = geometry ? glCreateShader(GL_GEOMETRY_SHADER) : 0;

	int i, length, success;//used whenever a pointer to int is required

	string tess_control_errors, tess_eval_errors; // we currently don't have error logs for these shaders, so they are added to the link error log

	if(	(vertex && !compileShader(v, vertex, vertErrorLog)) ||
		(fragment && !compileShader(f, fragment, fragErrorLog)) || 
		(geometry && !compileShader(g, geometry, geomErrorLog)) || 
		(tessellationControl && !compileShader(tc, tessellationControl, tess_control_errors)) || 
		(tessellationEvaluation && !compileShader(te, tessellationEvaluation, tess_eval_errors)))
	{
		return false;
	}

	shaderId = glCreateProgram();
	if(vertex)
	{
		glAttachShader(shaderId,v);
	}
	if(fragment)
	{
		glAttachShader(shaderId,f);
	}
	if(geometry)
	{
		glAttachShader(shaderId,g);
	}
	if(tessellationControl)
	{
		glAttachShader(shaderId,tc);
	}
	if(tessellationEvaluation)
	{
		glAttachShader(shaderId,te);
	}

	glBindAttribLocation(shaderId, 0, "Position2");
	glBindAttribLocation(shaderId, 1, "Position");
	glBindAttribLocation(shaderId, 2, "TexCoord");
	glBindAttribLocation(shaderId, 3, "Normal");
	glBindAttribLocation(shaderId, 4, "Color3");
	glBindAttribLocation(shaderId, 5, "Color4");
	glBindAttribLocation(shaderId, 6, "Tangent");
	glBindAttribLocation(shaderId, 7, "Bitangent");
	glBindAttribLocation(shaderId, 8, "GenericFloat");
//	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glLinkProgram(shaderId);


	//get shader link errors
	glGetProgramiv(shaderId,GL_INFO_LOG_LENGTH,&length);
	char* str=new char[length];
	glGetProgramInfoLog(shaderId,length,&i,str);
	linkErrorLog = tess_control_errors + tess_eval_errors + string(str);
	delete[] str;

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if(!success)
	{
		return false;
	}

	glUseProgram(0);



	// we no longer need these shaders individually, although they
	// will not actually be deleted until the shader program is deleted
	if(vertex)
	{
		glDeleteShader(v);
	}
	if(fragment)
	{
		glDeleteShader(f);
	}
	if(geometry)
	{
		glDeleteShader(g);
	}
	if(tessellationControl)
	{
		glDeleteShader(tc);
	}
	if(tessellationEvaluation)
	{
		glDeleteShader(te);
	}
	return true;
}
string OpenGLgraphics::shaderGL::getErrorStrings()
{
	//string str;
	//
	//int numAttributes=0;
	//int maxLength=0;
	//
	//glGetProgramiv(shaderId, GL_ACTIVE_ATTRIBUTES, &numAttributes);
	//glGetProgramiv(shaderId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	//
	//char* attribName = new char[maxLength];
	//
	//for(int i=0; i<numAttributes; i++)
	//{
	//	int size;
	//	GLenum type;
	//	glGetActiveAttrib(shaderId, i, maxLength, nullptr, &size, &type, attribName);
	//
	//	str += attribName;
	//	if(type==GL_FLOAT)		str += " (float): ";
	//	else if(type==GL_FLOAT_VEC2)	str += " (float2): ";
	//	else if(type==GL_FLOAT_VEC3)	str += " (float3): ";
	//	else if(type==GL_FLOAT_VEC4)	str += " (float4): ";
	//	else					str += " (???): ";
	//
	//	str += lexical_cast<string>(glGetAttribLocation(shaderId, attribName));
	//	str += "\n";
	//}
	//
	//
	//delete[] attribName;
	//return str;

	if(vertErrorLog != "") vertErrorLog = string("vert:") + vertErrorLog;
	if(geomErrorLog != "") geomErrorLog = string("geom:") + geomErrorLog;
	if(fragErrorLog != "") fragErrorLog = string("frag:") + fragErrorLog;
	if(linkErrorLog != "") linkErrorLog = string("link:") + linkErrorLog;
	return vertErrorLog + geomErrorLog + fragErrorLog + linkErrorLog;
}
void OpenGLgraphics::shaderGL::setUniform1f(string name, float v0)
{
	if(boundShader != this) bind();
	glUniform1f(getUniformLocation(name), v0);
}
void OpenGLgraphics::shaderGL::setUniform2f(string name, float v0, float v1)
{
	if(boundShader != this) bind();
	glUniform2f(getUniformLocation(name), v0, v1);
}
void OpenGLgraphics::shaderGL::setUniform3f(string name, float v0, float v1, float v2)
{
	if(boundShader != this) bind();
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}
void OpenGLgraphics::shaderGL::setUniform4f(string name, float v0, float v1, float v2, float v3)
{
	if(boundShader != this) bind();
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}
void OpenGLgraphics::shaderGL::setUniform1i(string name, int v0)
{
	if(boundShader != this) bind();
	glUniform1i(getUniformLocation(name), v0);
}
void OpenGLgraphics::shaderGL::setUniform2i(string name, int v0, int v1)
{
	if(boundShader != this) bind();
	glUniform2i(getUniformLocation(name), v0, v1);
}
void OpenGLgraphics::shaderGL::setUniform3i(string name, int v0, int v1, int v2)
{
	if(boundShader != this) bind();
	glUniform3i(getUniformLocation(name), v0, v1, v2);
}
void OpenGLgraphics::shaderGL::setUniform4i(string name, int v0, int v1, int v2, int v3)
{
	if(boundShader != this) bind();
	glUniform4i(getUniformLocation(name), v0, v1, v2, v3);
}
void OpenGLgraphics::shaderGL::setUniformMatrix(string name, const Mat3f& m)
{
	if(boundShader != this) bind();
	glUniformMatrix3fv(getUniformLocation(name),1,false,m.ptr());
}
void OpenGLgraphics::shaderGL::setUniformMatrix(string name, const Mat4f& m)
{
	if(boundShader != this) bind();
	glUniformMatrix4fv(getUniformLocation(name),1,false,m.ptr());
}
void OpenGLgraphics::shaderGL::setUniform1fv(string name, unsigned int n, float* v)
{
	if(boundShader != this) bind();
	glUniform1fv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform2fv(string name, unsigned int n, float* v)
{
	if(boundShader != this) bind();
	glUniform2fv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform3fv(string name, unsigned int n, float* v)
{
	if(boundShader != this) bind();
	glUniform3fv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform4fv(string name, unsigned int n, float* v)
{
	if(boundShader != this) bind();
	glUniform4fv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform1iv(string name, unsigned int n, int* v)
{
	if(boundShader != this) bind();
	glUniform1iv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform2iv(string name, unsigned int n, int* v)
{
	if(boundShader != this) bind();
	glUniform2iv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform3iv(string name, unsigned int n, int* v)
{
	if(boundShader != this) bind();
	glUniform3iv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::shaderGL::setUniform4iv(string name, unsigned int n, int* v)
{
	if(boundShader != this) bind();
	glUniform4iv(getUniformLocation(name), n, v);
}
void OpenGLgraphics::setEnabled(unsigned int glEnum, bool enabled)
{
	if(enabled)
		glEnable(glEnum);
	else
		glDisable(glEnum);
}
void OpenGLgraphics::flashTaskBar(int times, int length)
{
#ifdef VISUAL_STUDIO
	FLASHWINFO f;
	f.cbSize = sizeof(FLASHWINFO);
	f.hwnd = context->hWnd;
	f.dwFlags = FLASHW_TRAY;
	f.dwTimeout = length;
	f.uCount = times;
	FlashWindowEx(&f);
#endif
}
void OpenGLgraphics::minimizeWindow()
{
#ifdef WINDOWS
	ShowWindow(context->hWnd, SW_MINIMIZE);
#endif
}
OpenGLgraphics::OpenGLgraphics():renderTarget(RT_SCREEN),renderingToTexture(false),currentViewport(0,0,sw,sh),blurTexture(0),blurTexture2(0),multisampling(false),samples(0),colorMask(true), depthMask(true), redChannelMask(true), greenChannelMask(true), blueChannelMask(true), texCoord_clientState(false), normal_clientState(false), color_clientState(false), openGL3(false), openGL4(false)
{
	vSync = false;
#ifdef _DEBUG
	errorGlowEndTime = 0;
#endif
	context = new Context;
}
OpenGLgraphics::~OpenGLgraphics()
{
	delete context;
}
bool OpenGLgraphics::drawOverlay(Rect4f r, shared_ptr<texture> tex)
{
	if(tex) tex->bind();

	overlay[0].position = Vec2f(r.x,		r.y+r.h);
	overlay[1].position = Vec2f(r.x+r.w,	r.y+r.h);
	overlay[2].position = Vec2f(r.x+r.w,	r.y);
	overlay[3].position = Vec2f(r.x,		r.y);

	overlay[0].texCoord = Vec2f(0.0,1.0);
	overlay[1].texCoord = Vec2f(1.0,1.0);
	overlay[2].texCoord = Vec2f(1.0,0.0);
	overlay[3].texCoord = Vec2f(0.0,0.0);

	overlayVBO->setVertexData(4*sizeof(texturedVertex2D), overlay);
	overlayVBO->bindBuffer();
	overlayVBO->drawBuffer(TRIANGLE_FAN,0,4);

	return true;
}
bool OpenGLgraphics::drawRotatedOverlay(Rect4f r, Angle rotation, shared_ptr<texture> tex)
{
	if(tex) tex->bind();

	float w2 = r.w/2;
	float h2 = r.h/2;
	overlay[0].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*0.25) , r.y+h2 + h2*sin(-rotation+PI*0.25));
	overlay[1].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*0.75) , r.y+h2 + h2*sin(-rotation+PI*0.75));
	overlay[2].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*1.25) , r.y+h2 + h2*sin(-rotation+PI*1.25));
	overlay[3].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*1.75) , r.y+h2 + h2*sin(-rotation+PI*1.75));

	overlay[0].texCoord = Vec2f(0.0,0.0);
	overlay[1].texCoord = Vec2f(1.0,0.0);
	overlay[2].texCoord = Vec2f(1.0,1.0);
	overlay[3].texCoord = Vec2f(0.0,1.0);

	overlayVBO->setVertexData(4*sizeof(texturedVertex2D), overlay);
	overlayVBO->bindBuffer();
	overlayVBO->drawBuffer(TRIANGLE_FAN,0,4);

	return true;
}
bool OpenGLgraphics::drawPartialOverlay(Rect4f r, Rect4f t, shared_ptr<texture> tex)
{
	if(tex) tex->bind();

	overlay[0].position = Vec2f(r.x,		r.y);
	overlay[1].position = Vec2f(r.x+r.w,	r.y);
	overlay[2].position = Vec2f(r.x+r.w,	r.y+r.h);
	overlay[3].position = Vec2f(r.x,		r.y+r.h);

	overlay[0].texCoord = Vec2f(t.x,		t.y);
	overlay[1].texCoord = Vec2f(t.x+t.w,	t.y);
	overlay[2].texCoord = Vec2f(t.x+t.w,	t.y+t.h);
	overlay[3].texCoord = Vec2f(t.x,		t.y+t.h);

	overlayVBO->setVertexData(4*sizeof(texturedVertex2D), overlay);
	overlayVBO->bindBuffer();
	overlayVBO->drawBuffer(TRIANGLE_FAN,0,4);

	return true;
}
shared_ptr<GraphicsManager::vertexBuffer> OpenGLgraphics::genVertexBuffer(GraphicsManager::vertexBuffer::UsageFrequency usage)
{
	return shared_ptr<vertexBuffer>((new vertexBufferGL(usage)));
}
shared_ptr<GraphicsManager::indexBuffer> OpenGLgraphics::genIndexBuffer(indexBuffer::UsageFrequency usage)
{
	return shared_ptr<indexBuffer>((new indexBufferGL(usage)));
}
shared_ptr<GraphicsManager::multiDraw> OpenGLgraphics::genMultiDraw(shared_ptr<vertexBuffer> vBuffer, shared_ptr<indexBuffer> iBuffer)
{
    return shared_ptr<multiDraw>(new multiDrawGL(dynamic_pointer_cast<vertexBufferGL>(vBuffer), dynamic_pointer_cast<indexBufferGL>(iBuffer)));
}
shared_ptr<GraphicsManager::texture2D> OpenGLgraphics::genTexture2D()
{
	return shared_ptr<texture2D>((new texture2DGL()));
}
shared_ptr<GraphicsManager::texture3D> OpenGLgraphics::genTexture3D()
{
	return shared_ptr<texture3D>((new texture3DGL()));
}
shared_ptr<GraphicsManager::textureCube> OpenGLgraphics::genTextureCube()
{
	return shared_ptr<textureCube>((new textureCubeGL()));
}
shared_ptr<GraphicsManager::shader> OpenGLgraphics::genShader()
{
	return shared_ptr<shader>(new shaderGL());
}
void OpenGLgraphics::setGamma(float gamma)
{
	currentGamma = gamma;
}
void OpenGLgraphics::setColor(Color4 color)
{
	getBoundShader()->setUniform4f("color", color);
}
void OpenGLgraphics::setColorMask(bool mask)
{
		colorMask = mask;
		glColorMask(mask && redChannelMask, mask && greenChannelMask, mask && blueChannelMask, mask);
}
void OpenGLgraphics::setDepthMask(bool mask)
{
	if(mask != depthMask)
	{
		depthMask = mask;
		glDepthMask(depthMask);
	}
}
void OpenGLgraphics::setDepthTest(bool enabled)
{
	setEnabled(GL_DEPTH_TEST, enabled);
}
void OpenGLgraphics::setBlendMode(BlendMode blend)
{
	if(blend == TRANSPARENCY)				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	else if(blend == ADDITIVE)				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
	else if(blend == REPLACE)				glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
	else if(blend == PREMULTIPLIED_ALPHA)	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	else if(blend == ALPHA_ONLY)			glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
}
void OpenGLgraphics::setClientState(unsigned int index, bool state)
{
	if(clientStates[index] != state)
	{
		if(state)	glEnableVertexAttribArray(index);
		else		glDisableVertexAttribArray(index);
		clientStates[index] = state;
	}
}
void OpenGLgraphics::setVSync(bool enabled)
{
	vSync = enabled;
#ifdef WINDOWS
	if(WGLEW_EXT_swap_control)// wglSwapIntervalEXT)
		wglSwapIntervalEXT(enabled ? 1 : 0);//turn on/off vsync (0 = off and 1 = on)
#endif
}
void OpenGLgraphics::setAlphaToCoverage(bool enabled)
{
	if(multisampling)
	{
		setEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE, enabled);
	}
}
void OpenGLgraphics::setWireFrame(bool enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}
void OpenGLgraphics::setClearColor(Color4 c)
{
	clearColor = c;
}
void OpenGLgraphics::setSampleShadingFraction(float f)
{
	if(multisampling && openGL4)
	{
		glMinSampleShading(clamp(f, 0.0f, 1.0f));			
	}
	else if(multisampling && GLEW_ARB_sample_shading)
	{
		glMinSampleShadingARB(clamp(f, 0.0f, 1.0f));
	}
}
void OpenGLgraphics::setSampleShading(bool enabled)
{
	if(multisampling)
	{
		if(openGL4)
			setEnabled(GL_SAMPLE_SHADING, enabled);
		else if(GLEW_ARB_sample_shading)
			setEnabled(GL_SAMPLE_SHADING_ARB, enabled);
	}
}
void OpenGLgraphics::drawText(string text, Vec2f pos, string font)
{
	auto f = dataManager.getFont(font);

	if(f != NULL)
	{
		f->texture->bind();

		Vec2f charPos = pos;
		Rect charRect;
		for(auto i = text.begin(); i != text.end(); i++)
		{
			if(*i == '\n')
			{
				charPos.x = pos.x;
				charPos.y += f->height / 1024.0;
			}
			else if(*i == '\t')
			{
				charPos.x = pos.x + std::ceil((charPos.x-pos.x) * 32.0 + 0.001) / 32.0;
			}
			else if(f->characters.count(*i) != 0)
			{
				auto& c = f->characters.find(*i)->second;

				charRect.x = charPos.x + c.xOffset / 1024.0;
				charRect.y = charPos.y + c.yOffset / 1024.0;
				charRect.w = c.width / 1024.0;
				charRect.h = c.height / 1024.0;

				GraphicsManager::drawPartialOverlay(charRect, c.UV);
				charPos.x += c.xAdvance  / 1024.0;
			}
		}
	}
}
void OpenGLgraphics::drawText(string text, Rect rect, string font)
{
	auto f = dataManager.getFont(font);

	if(f != NULL)
	{
		f->texture->bind();

		Vec2f charPos = rect.origin();
		Rect charRect;
		for(auto i = text.begin(); i != text.end(); i++)
		{
			if(*i == '\n')
			{
				charPos.x = rect.x;
				charPos.y += f->height / 1024.0;
			}
			else if(*i == '\t')
			{
				charPos.x = rect.x + std::ceil((charPos.x-rect.x) * 32.0 + 0.001) / 32.0;
			}
			else if(f->characters.count(*i) != 0 && rect.inRect(charPos))
			{
				auto& c = f->characters.find(*i)->second;

				charRect.x = charPos.x + c.xOffset / 1024.0;
				charRect.y = charPos.y + c.yOffset / 1024.0;
				charRect.w = c.width / 1024.0;
				charRect.h = c.height / 1024.0;

				Rect UV = c.UV;
				if(charRect.x < rect.x + rect.w && charRect.y < rect.y + rect.h)
				{
					if(charRect.x + charRect.w > rect.x + rect.w)
					{
						UV.w *= ((rect.x + rect.w) - charRect.x) / charRect.w;
						charRect.w -= (charRect.x + charRect.w) - (rect.x + rect.w);
					}
					if(charRect.y + charRect.h > rect.y + rect.h)
					{
						UV.h *= ((rect.y + rect.h) - charRect.y) / charRect.h;
						charRect.h -= (charRect.y + charRect.h) - (rect.y + rect.h);
					}

					GraphicsManager::drawPartialOverlay(charRect, UV);
				}
				charPos.x += c.xAdvance / 1024.0;
			}
		}
	}
}
Vec2f OpenGLgraphics::getTextSize(string text, string font)
{
	auto f = dataManager.getFont(font);
	if(f == nullptr)
		return Vec2f(0,0);

	Vec2f textSize(0,f->height);
	for(auto i = text.begin(); i != text.end(); i++)
	{
		if(*i == '\n')
		{
			textSize.y += f->height;
		}
		else if(f->characters.count(*i) != 0)
		{
			textSize.x += f->characters.find(*i)->second.xAdvance;
		}
	}

	//if(dataManager.getBoundShader() == "ortho")
		return textSize / 1024.0;
	//else
	//	return textSize;
}
void OpenGLgraphics::setFrameBufferTextures(shared_ptr<texture2D> color, unsigned int color_level, shared_ptr<texture2D> depth, unsigned int depth_level)
{
	if(openGL3)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color ? dynamic_pointer_cast<texture2DGL>(color)->textureID : 0, color_level);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth ? dynamic_pointer_cast<texture2DGL>(depth)->textureID : 0, depth_level);
	}
	else
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color ? dynamic_pointer_cast<texture2DGL>(color)->textureID : 0, color ? color_level : 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth ? dynamic_pointer_cast<texture2DGL>(depth)->textureID : 0, depth ? depth_level : 0);

	}
	if(!color && depth)
	{
		glViewport(0,0,dynamic_pointer_cast<texture2DGL>(depth)->width >> depth_level, dynamic_pointer_cast<texture2DGL>(depth)->height >> depth_level);
	}
	else if(color && !depth)
	{
		glViewport(0,0,dynamic_pointer_cast<texture2DGL>(color)->width >> color_level, dynamic_pointer_cast<texture2DGL>(color)->height >> color_level);
	}
	else if(color && depth)
	{
		auto colorTex = dynamic_pointer_cast<texture2DGL>(color);
		auto depthTex = dynamic_pointer_cast<texture2DGL>(depth);
		debugAssert(colorTex->width>>color_level == depthTex->width>>depth_level && colorTex->height>>color_level == depthTex->height>>depth_level);
		glViewport(0,0,colorTex->width >> color_level, colorTex->height >> color_level);
	}
	cachedRenderTarget = renderTarget;
	renderTarget = RT_TEXTURE;

	GLenum status = openGL3 ? glCheckFramebufferStatus(GL_FRAMEBUFFER) : glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status == GL_FRAMEBUFFER_COMPLETE)							{} //frame buffer valid
#if defined(WINDOWS) && defined(_DEBUG)
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)			MessageBoxA(NULL, "Framebuffer incomplete: Attachment is NOT complete.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)	MessageBoxA(NULL, "Framebuffer incomplete: No image is attached to FBO.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Attached images have different dimensions.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Color attached images have different internal formats.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)		MessageBoxA(NULL, "Framebuffer incomplete: Draw buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)		MessageBoxA(NULL, "Framebuffer incomplete: Read buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)		MessageBoxA(NULL, "Framebuffer incomplete: Multisample buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_UNSUPPORTED)					MessageBoxA(NULL, "Unsupported by FBO implementation.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_UNDEFINED)						MessageBoxA(NULL, "Framebuffer undefined.","ERROR",MB_OK);
	else 															MessageBoxA(NULL, "Unknow frame buffer error.","ERROR",MB_OK);
#else
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)			std::cerr << "Framebuffer incomplete: Attachment is NOT complete." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)	std::cerr << "Framebuffer incomplete: No image is attached to FBO." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)		std::cerr << "Framebuffer incomplete: Attached images have different dimensions." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)		std::cerr << "Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)		std::cerr << "Framebuffer incomplete: Draw buffer." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)		std::cerr << "Framebuffer incomplete: Read buffer." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)		std::cerr << "Framebuffer incomplete: Multisample buffer." << std::endl;
	else if(status == GL_FRAMEBUFFER_UNSUPPORTED)					std::cerr << "Unsupported by FBO implementation." << std::endl;
	else if(status == GL_FRAMEBUFFER_UNDEFINED)						std::cerr << "Framebuffer undefined." << std::endl;
	else 															std::cerr << "Unknow frame buffer error." << std::endl;

#endif
}
void OpenGLgraphics::setFrameBufferTextures(shared_ptr<textureCube> color, textureCube::Face face, unsigned int color_level)
{
	if(openGL3)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);

		if(face == textureCube::POSITIVE_X)				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::POSITIVE_Y)		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::POSITIVE_Z)		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::NEGATIVE_X)		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::NEGATIVE_Y)		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::NEGATIVE_Z)		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else
		{
			debugBreak();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}
	else
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);

		if(face == textureCube::POSITIVE_X)				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::POSITIVE_Y)		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::POSITIVE_Z)		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::NEGATIVE_X)		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::NEGATIVE_Y)		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else if(face == textureCube::NEGATIVE_Z)		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, color ? dynamic_pointer_cast<textureCubeGL>(color)->textureID : 0, color_level);
		else
		{
			debugBreak();
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
		}

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	}
	if(color)
	{
		glViewport(0,0,dynamic_pointer_cast<textureCubeGL>(color)->width >> color_level, dynamic_pointer_cast<textureCubeGL>(color)->height >> color_level);
	}
	cachedRenderTarget = renderTarget;
	renderTarget = RT_TEXTURE;

	GLenum status = openGL3 ? glCheckFramebufferStatus(GL_FRAMEBUFFER) : glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status == GL_FRAMEBUFFER_COMPLETE)							{} //frame buffer valid
#if defined(WINDOWS) && defined(_DEBUG)
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)			MessageBoxA(NULL, "Framebuffer incomplete: Attachment is NOT complete.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)	MessageBoxA(NULL, "Framebuffer incomplete: No image is attached to FBO.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Attached images have different dimensions.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Color attached images have different internal formats.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)		MessageBoxA(NULL, "Framebuffer incomplete: Draw buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)		MessageBoxA(NULL, "Framebuffer incomplete: Read buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)		MessageBoxA(NULL, "Framebuffer incomplete: Multisample buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_UNSUPPORTED)					MessageBoxA(NULL, "Unsupported by FBO implementation.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_UNDEFINED)						MessageBoxA(NULL, "Framebuffer undefined.","ERROR",MB_OK);
	else 															MessageBoxA(NULL, "Unknow frame buffer error.","ERROR",MB_OK);
#else
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)			std::cerr << "Framebuffer incomplete: Attachment is NOT complete." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)	std::cerr << "Framebuffer incomplete: No image is attached to FBO." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)		std::cerr << "Framebuffer incomplete: Attached images have different dimensions." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)		std::cerr << "Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)		std::cerr << "Framebuffer incomplete: Draw buffer." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)		std::cerr << "Framebuffer incomplete: Read buffer." << std::endl;
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)		std::cerr << "Framebuffer incomplete: Multisample buffer." << std::endl;
	else if(status == GL_FRAMEBUFFER_UNSUPPORTED)					std::cerr << "Unsupported by FBO implementation." << std::endl;
	else if(status == GL_FRAMEBUFFER_UNDEFINED)						std::cerr << "Framebuffer undefined." << std::endl;
	else 															std::cerr << "Unknow frame buffer error." << std::endl;
#endif
}
void OpenGLgraphics::bindRenderTarget(RenderTarget rTarget)
{
	if(openGL3)
	{
		if(rTarget == RT_FBO)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fboID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
			renderTarget = RT_FBO;
		}
		else if(rTarget == RT_MULTISAMPLE_FBO)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, multisampleFboID);
			renderTarget = RT_MULTISAMPLE_FBO;
		}
		else if(rTarget == RT_SCREEN)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			renderTarget = RT_SCREEN;
		}
		else debugBreak();
	}
	else
	{
		if(rTarget == RT_FBO)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderTexture, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);
			renderTarget = RT_FBO;
		}
		else if(rTarget == RT_MULTISAMPLE_FBO)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFboID);
			renderTarget = RT_MULTISAMPLE_FBO;
		}
		else if(rTarget == RT_SCREEN)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			renderTarget = RT_SCREEN;
		}
		else debugBreak();
	}
	
}
bool OpenGLgraphics::initFBOs(unsigned int maxSamples)
{
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
	samples = samples >= maxSamples ? maxSamples : samples;
	multisampling = samples > 1;
	//////////////////////////////////////////////////////////////////////////////////////
    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, sw, sh, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB10_A2, sw/2, sh/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(1, &renderTexture2);
    glBindTexture(GL_TEXTURE_2D, renderTexture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, sw, sh, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB10_A2, sw/2, sh/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT24, sw, sh, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	
    if(multisampling)
	{
		glGenFramebuffers(1, &multisampleFboID);
        glGenRenderbuffers(1, &multisampleRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, multisampleRenderBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGB10_A2, sw, sh);

        glGenRenderbuffers(1, &multisampleDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, multisampleDepthBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, sw, sh);

        glBindFramebuffer(GL_FRAMEBUFFER, multisampleFboID);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisampleRenderBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampleDepthBuffer);
	}
    else
    {
        multisampleRenderBuffer = 0;
        multisampleFboID = 0;
        multisampleDepthBuffer = 0;
    }
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status == GL_FRAMEBUFFER_COMPLETE)							{} //frame buffer valid
#ifdef WINDOWS
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)			MessageBoxA(NULL, "Framebuffer incomplete: Attachment is NOT complete.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)	MessageBoxA(NULL, "Framebuffer incomplete: No image is attached to FBO.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Attached images have different dimensions.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Color attached images have different internal formats.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)		MessageBoxA(NULL, "Framebuffer incomplete: Draw buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)		MessageBoxA(NULL, "Framebuffer incomplete: Read buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)		MessageBoxA(NULL, "Framebuffer incomplete: Multisample buffer.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_UNSUPPORTED)					MessageBoxA(NULL, "Unsupported by FBO implementation.","ERROR",MB_OK);
	else if(status == GL_FRAMEBUFFER_UNDEFINED)						MessageBoxA(NULL, "Framebuffer undefined.","ERROR",MB_OK);
	else 															MessageBoxA(NULL, "Unknow frame buffer error.","ERROR",MB_OK);
#endif

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenTextures(1, &blurTexture);
	glBindTexture(GL_TEXTURE_2D, blurTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sw/2, sh/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &blurTexture2);
	glBindTexture(GL_TEXTURE_2D, blurTexture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sw/2, sh/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return true;
}
void OpenGLgraphics::destroyFBOs()
{
	if(renderTexture != 0)		glDeleteTextures(1, &renderTexture);
	if(renderTexture2 != 0)		glDeleteTextures(1, &renderTexture2);
	if(depthTexture != 0)		glDeleteTextures(1, &depthTexture);
	if(blurTexture != 0)			glDeleteTextures(1, &blurTexture);
	if(blurTexture2 != 0)			glDeleteTextures(1, &blurTexture2);

	if(openGL3 || GLEW_ARB_framebuffer_object)
	{
		if(fboID != 0)						glDeleteFramebuffers(1, &fboID);
		if(multisampleFboID != 0)			glDeleteFramebuffers(1, &multisampleFboID);
		if(multisampleRenderBuffer != 0)	glDeleteRenderbuffers(1, &multisampleRenderBuffer);
		if(multisampleDepthBuffer != 0)		glDeleteRenderbuffers(1, &multisampleDepthBuffer);
	}
	else
	{
		if(fboID != 0)						glDeleteFramebuffersEXT(1, &fboID);
		if(multisampleFboID != 0)			glDeleteFramebuffersEXT(1, &multisampleFboID);
		if(multisampleRenderBuffer != 0)	glDeleteRenderbuffersEXT(1, &multisampleRenderBuffer);
		if(multisampleDepthBuffer != 0)		glDeleteRenderbuffersEXT(1, &multisampleDepthBuffer);
	}
}
void OpenGLgraphics::resize(int w, int h)
{
	if(sw > 0 && sh > 0)
	{
		sh = h;
		sw = w;
		sAspect = ((float)sw)/sh;
	}
}
void OpenGLgraphics::computeViewport(Rect& clipped_viewport, Rect& projectionConstraint)
{
	if(highResScreenshot)
	{
		Rect viewport = Rect::XYWH(currentView->viewport().x / sAspect, (1.0-currentView->viewport().y-currentView->viewport().height), currentView->viewport().width / sAspect, currentView->viewport().height);
		
		viewport.x = (viewport.x - viewConstraint.x) / viewConstraint.w;
		viewport.y = (viewport.y - viewConstraint.y) / viewConstraint.h;
		viewport.w = viewport.w / viewConstraint.w;
		viewport.h = viewport.h / viewConstraint.h;
		
		Rect unclipped_viewport;
		unclipped_viewport.x = floor(viewport.x*sw)/sw;
		unclipped_viewport.y = floor(viewport.y*sh)/sh;
		unclipped_viewport.w = ceil(viewport.w*sw)/sw;
		unclipped_viewport.h = ceil(viewport.h*sh)/sh;

		clipped_viewport.x = clamp(unclipped_viewport.x, 0.0, 1.0);
		clipped_viewport.y = clamp(unclipped_viewport.y, 0.0, 1.0);
		clipped_viewport.w = clamp(unclipped_viewport.w, 0.0, 1.0-clipped_viewport.x);
		clipped_viewport.h = clamp(unclipped_viewport.h, 0.0, 1.0-clipped_viewport.y);

		projectionConstraint.x = (clipped_viewport.x - unclipped_viewport.x) / unclipped_viewport.w;
		projectionConstraint.y = (clipped_viewport.y - unclipped_viewport.y) / unclipped_viewport.h;
		projectionConstraint.w = clipped_viewport.w / unclipped_viewport.w;
		projectionConstraint.h = clipped_viewport.h / unclipped_viewport.h;

		//Rect projConstraint;
		//projConstraint.x = (clamp(viewport.x, 0.0, 1.0) - viewport.x) / viewport.w;
		//projConstraint.y = (clamp(viewport.y, 0.0, 1.0) - viewport.y) / viewport.h;
		//projConstraint.w = (clamp(viewport.x+viewport.w, 0.0, 1.0) - clamp(viewport.x, 0.0, 1.0)) / viewport.w;
		//projConstraint.h = (clamp(viewport.y+viewport.h, 0.0, 1.0) - clamp(viewport.y, 0.0, 1.0)) / viewport.h;
		//currentView->constrainView(projConstraint);
		//
		//auto ortho = shaders.bind("ortho");
		//ortho->setUniform4f("viewConstraint", projConstraint);
	}
	else
	{
		clipped_viewport = Rect::XYWH(currentView->viewport().x / sAspect, (1.0 - currentView->viewport().y-currentView->viewport().height), currentView->viewport().width / sAspect, currentView->viewport().height);
		projectionConstraint = Rect::XYWH(0,0,1,1);
	}
}
void OpenGLgraphics::render()
{
/////////////////////////////////////START TIMING/////////////////////////////////////
	static vector<double> frameTimes;//in milliseconds
	double time=GetTime();
	static double lastTime=time;

	frameTimes.push_back(time-lastTime);
	if(frameTimes.size() > 20)
		frameTimes.erase(frameTimes.begin());

	lastTime=time;

	double totalTime=0.0; //total recorded time for all the frame times (in seconds)
	for(auto i=frameTimes.begin(); i != frameTimes.end(); i++)
		totalTime+=((*i)*0.001);

    if(totalTime > 0)
        fps = ((double)frameTimes.size()) /  totalTime;
////////////////////////////////////CLEAR SCREEN//////////////////////////////////////////
	bindRenderTarget(RT_SCREEN);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/////////////////////////////////////BIND BUFFER//////////////////////////////////////////
	if(multisampling)
	{
		bindRenderTarget(RT_MULTISAMPLE_FBO);
	}
	else
	{
		bindRenderTarget(RT_FBO);
	}
	setDepthMask(true);
	setColorMask(true);
///////////////////////////////////CLEAR BUFFERS/////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//////////////////////////////////SET 2D VIEW CONSTRAINTS///////////////////////////////////
	if(!highResScreenshot)
	{
		auto ortho = shaders.bind("ortho");
		ortho->setUniform4f("viewConstraint", 0,0,1,1);
	}
////////////////////////////////////3D RENDERING/////////////////////////////////////
	Vec3f cameraOffset;

	for(int eye=0; eye<(stereoMode!=STEREO_NONE?2:1); eye++)
	{
		//set up stereo
		if(!highResScreenshot && stereoMode==STEREO_ANAGLYPH && eye==0)
		{
			leftEye = true;
			redChannelMask = true;
			greenChannelMask = false;
			blueChannelMask = false;
			glColorMask(true,false,false,true);
			cameraOffset = Vec3f(-interOcularDistance/2,0,0);
		}
		else if(!highResScreenshot && stereoMode==STEREO_ANAGLYPH && eye==1)
		{
			leftEye = false;
			redChannelMask = false;
			greenChannelMask = true;
			blueChannelMask = true;
			glColorMask(false,true,true,true);
			cameraOffset = Vec3f(interOcularDistance/2,0,0);

			glClear(GL_DEPTH_BUFFER_BIT);
		}

		//render scene
		for(auto i = views.begin(); i != views.end();)
		{
			if(i->expired()) //if the view no longer exists
			{
				i = views.erase(i);
			}
			else
			{
				currentView = shared_ptr<View>(*(i++));

				Rect clipped_viewport, projConstraint;
				computeViewport(clipped_viewport, projConstraint);
				if(clipped_viewport.w <= 0.0 || clipped_viewport.h <= 0.0)
					continue;

				currentViewport.x = clipped_viewport.x*sw;
				currentViewport.y = clipped_viewport.y*sh;
				currentViewport.w = clipped_viewport.w*sw;
				currentViewport.h = clipped_viewport.h*sh;
				glViewport(clipped_viewport.x*sw, clipped_viewport.y*sh, clipped_viewport.w*sw, clipped_viewport.h*sh);
				if(highResScreenshot)
				{
					//Rect fullViewport = Rect::XYWH(currentView->viewport().x / sAspect, (1.0-currentView->viewport().y-currentView->viewport().height), currentView->viewport().width / sAspect, currentView->viewport().height);

					currentView->constrainView(projConstraint);

					auto ortho = shaders.bind("ortho");
					ortho->setUniform4f("viewConstraint", projConstraint);
				}
				else
				{
					if((stereoMode!=STEREO_NONE))	
						currentView->shiftCamera(cameraOffset);
				}


				auto model = shaders.bind("model");
				model->setUniformMatrix("cameraProjection",currentView->projectionMatrix() * currentView->modelViewMatrix());
				model->setUniformMatrix("modelTransform", Mat4f());
				model->setUniform1i("numLights",1);

				currentView->render();

				if(highResScreenshot)				currentView->constrainView(Rect::XYXY(0,0,1,1));
				else if((stereoMode!=STEREO_NONE))	currentView->shiftCamera(Vec3f(0,0,0));
			}
		}

		//capture depth buffer for reading and bind it to texture unit 47
		if(multisampling && (openGL3 || GLEW_ARB_framebuffer_object))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFboID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
			glBlitFramebuffer(0, 0, sw, sh, 0, 0, sw, sh, GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, multisampleFboID);
		}
		else if(multisampling)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, multisampleFboID);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, fboID);
			glBlitFramebufferEXT(0, 0, sw, sh, 0, 0, sw, sh, GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFboID);
		}
		else
		{
			setDepthMask(false);
		}
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0 + 47);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glActiveTexture(GL_TEXTURE0 + 46);
		glBindTexture(GL_TEXTURE_2D, renderTexture);

		for(auto i = views.begin(); i != views.end();)
		{
			if(i->expired()) //check if the view no longer exists (just to be safe)
			{
				i = views.erase(i);
			}
			else
			{
				currentView = shared_ptr<View>(*(i++));
				if(currentView->transparentRenderFunc())
				{
					//glViewport(currentView->viewport().x * sh, (1.0 - currentView->viewport().y-currentView->viewport().height) * sh, currentView->viewport().width * sh, currentView->viewport().height * sh);
		
					Rect clipped_viewport, projConstraint;
					computeViewport(clipped_viewport, projConstraint);
					if(clipped_viewport.w <= 0.0 || clipped_viewport.h <= 0.0)
						continue;

					currentViewport.x = clipped_viewport.x*sw;
					currentViewport.y = clipped_viewport.y*sh;
					currentViewport.w = clipped_viewport.w*sw;
					currentViewport.h = clipped_viewport.h*sh;
					glViewport(clipped_viewport.x*sw, clipped_viewport.y*sh, clipped_viewport.w*sw, clipped_viewport.h*sh);
					if(highResScreenshot)
					{
						//Rect fullViewport = Rect::XYWH(currentView->viewport().x / sAspect, (1.0-currentView->viewport().y-currentView->viewport().height), currentView->viewport().width / sAspect, currentView->viewport().height);

						currentView->constrainView(projConstraint);

						auto ortho = shaders.bind("ortho");
						ortho->setUniform4f("viewConstraint", projConstraint);
					}
					else
					{
						if((stereoMode!=STEREO_NONE))	
							currentView->shiftCamera(cameraOffset);
					}
		
					currentView->renderTransparent();
		
					if(highResScreenshot)				currentView->constrainView(Rect::XYXY(0,0,1,1));
					else if((stereoMode!=STEREO_NONE))	currentView->shiftCamera(Vec3f(0,0,0));
				}
			}
		}
	}

	glDisable(GL_DEPTH_TEST);
	currentView.reset(); //set the current view to null
	if(stereoMode==STEREO_ANAGLYPH)
	{
		redChannelMask = true;
		greenChannelMask = true;
		blueChannelMask = true;
		glColorMask(true,true,true,true);
	}
	sceneManager.endRender(); //do some post render cleanup

//////////////////////////////////Blit Framebuffer///////////////////////////////
	if(multisampling)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFboID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
		glBlitFramebuffer(0, 0, sw, sh, 0, 0, sw, sh, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
////////////////////////////////////Motion Blur//////////////////////////////////
	//glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, FBOs[0].color);
	//glActiveTexture(GL_TEXTURE1);	glBindTexture(GL_TEXTURE_2D, FBOs[0].depth);
	//
	//bindRenderTarget(RT_FBO_1);
	//
	//auto motionBlur = shaders.bind("motion blur shader");
	//if(motionBlur)
	//{
	//	motionBlur->setUniform1i("colorTexture", 0);
	//	motionBlur->setUniform1i("depthTexture", 1);
	//	for(auto i = views.begin(); i != views.end();)
	//	{
	//		if(i->expired()) //check if the view no longer exists (just to be safe)
	//		{
	//			i = views.erase(i);
	//		}
	//		else
	//		{
	//			currentView = shared_ptr<View>(*i);
	//			Mat4f viewProjectionInverseMatrix;
	//			(currentView->projectionMatrix() * currentView->modelViewMatrix()).inverse(viewProjectionInverseMatrix);
	//
	//			motionBlur->setUniformMatrix("viewProjectionInverseMatrix", viewProjectionInverseMatrix);
	//			motionBlur->setUniformMatrix("previousViewProjectionMatrix", currentView->projectionMatrix() * currentView->lastModelViewMatrix());
	//
	//			glViewport(currentView->viewport().x * sh, (1.0 - currentView->viewport().y-currentView->viewport().height) * sh, currentView->viewport().width * sh, currentView->viewport().height * sh);
	//			drawPartialOverlay(Rect::XYWH(currentView->viewport().x/sAspect*2.0-1.0,currentView->viewport().y*2.0-1.0,currentView->viewport().width/sAspect*2.0,currentView->viewport().height*2.0), 
	//								Rect::XYWH(currentView->viewport().x/sAspect,currentView->viewport().y,currentView->viewport().width/sAspect,currentView->viewport().height));
	//			i++;
	//		}
	//	}
	//	currentView.reset(); //set the current view to null
	//}

///////////////////////////////////////Post Processing//////////////////////////////////
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, FBOs[0].color);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glGenerateMipmapEXT(GL_TEXTURE_2D);


	currentViewport = Rect4i::XYWH(0,0,sw,sh);
	glViewport(0,0,sw,sh);
	//bindRenderTarget(RT_SCREEN);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(auto i = views.begin(); i != views.end();)
	{
		if(i->expired()) //if the view no longer exists
		{
			i = views.erase(i);
		}
		else
		{
			currentView = shared_ptr<View>(*(i++));

			Rect overlayRect, textureRect, projectionConstraint;
			computeViewport(textureRect, projectionConstraint);
			overlayRect.x = textureRect.x * 2.0 - 1.0;
			overlayRect.y = textureRect.y * 2.0 - 1.0;
			overlayRect.w = textureRect.w * 2.0;
			overlayRect.h = textureRect.h * 2.0;

			if(currentView->postProcessShader() && overlayRect.w*overlayRect.h > 0.0)
			{
				if(currentView->blurStage())
				{
					auto blurX = shaders("blurX");
					auto blurY = shaders("blurY");
					if(blurX && blurY)
					{
						glViewport(0,0,sw/2,sh/2);

						if(openGL3)	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
						else		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
			
				
						blurX->bind();
						blurX->setUniform1i("tex",0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, renderTexture);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glGenerateMipmapEXT(GL_TEXTURE_2D);
						if(openGL3)	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture2, 0);
						else		glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture2, 0);
						GraphicsManager::drawPartialOverlay(overlayRect, textureRect);
				
				
						blurY->bind();
						blurY->setUniform1i("tex",0);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, blurTexture2);
						if(openGL3)	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);
						else		glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);
						GraphicsManager::drawPartialOverlay(overlayRect, textureRect);
				
				
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, blurTexture);//bind the blur texture

						if(openGL3)	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
						else		glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

						glViewport(0,0,sw,sh);
					}
					else
					{
						debugBreak();
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}

				bindRenderTarget(RT_SCREEN);
				currentView->postProcessShader()->bind();
				currentView->postProcessShader()->setUniform1f("gamma",currentGamma);
				currentView->postProcessShader()->setUniform1i("tex",0);
				currentView->postProcessShader()->setUniform1i("noiseTex",2);
				dataManager.bind("LCnoise",2);
//				currentView->postProcessShader()->setUniform1f("time",world->time());
				if(currentView->blurStage())
				{
					currentView->postProcessShader()->setUniform1i("blurTex",1);
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, renderTexture);
				if(openGL3)	glGenerateMipmap(GL_TEXTURE_2D);
				else		glGenerateMipmapEXT(GL_TEXTURE_2D);
				GraphicsManager::drawPartialOverlay(overlayRect, textureRect);
			}
		}
	}
	currentView = nullptr;

/////////////////////////////////////START 2D////////////////////////////////////
	bindRenderTarget(RT_SCREEN);

	auto ortho = shaders.bind("ortho");
	ortho->setUniform4f("color",white);
	ortho->setUniform4f("viewConstraint", viewConstraint);
	menuManager.render();

	#ifdef _DEBUG
		if(!highResScreenshot)
		{
			//ortho->setUniform4f("color",white);
			//GraphicsManager::drawOverlay(Rect::XYXY(0,0,0.15,0.045), "white");
			ortho->setUniform4f("color",black);
			drawText(lexical_cast<string>(fps), Vec2f(0.005, 0.005), "default font");
			ortho->setUniform4f("color",white);

			Profiler.draw();
	
			if(errorGlowEndTime > GetTime() && dataManager.assetLoaded("errorGlow"))
			{
				setColor(Color(1,0,0,clamp((errorGlowEndTime-GetTime())/1000.0, 0.0, 1.0)));
				GraphicsManager::drawOverlay(Rect::XYXY(0,0,sAspect,1), "errorGlow");
				setColor(Color4(1,1,1,1));
			}
		}
	#endif
	ortho->setUniform4f("viewConstraint", 0,0,1,1);
///////////////////////////////////////////////////////////////////////////////////////
	checkErrors();
}
void OpenGLgraphics::destroyWindow()
{
	destroyFBOs();

	shapesVBO.reset();
	overlayVBO.reset();

	//glDeleteRenderbuffersEXT(2, depthRenderBuffers);

#if defined(WINDOWS)
	ChangeDisplaySettings(NULL,0);					// Switch Back To The Desktop
	ShowCursor(true);								// Show Mouse Pointer

	if (context->hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			//MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(context->hRC))						// Are We Able To Delete The RC?
		{
			//MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		context->hRC=NULL;										// Set RC To NULL
	}

	if (context->hDC && !ReleaseDC(context->hWnd,context->hDC))					// Are We Able To Release The DC
	{
		//MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		context->hDC=NULL;										// Set DC To NULL
	}

	if (context->hWnd && !DestroyWindow(context->hWnd))					// Are We Able To Destroy The Window?
	{
		//MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		context->hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(L"OpenGL",context->hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		context->hInstance=NULL;									// Set hInstance To NULL
	}
#elif defined(LINUX)
	if(context->context)
	{
		glXMakeCurrent(x11_display, None, nullptr);
		glXDestroyContext(x11_display, context->context);
		context->context = nullptr;
	}
	/* switch back to original desktop resolution if we were in fullscreen */
	if(context->fullscreen)
	{
		XF86VidModeSwitchToMode(x11_display, x11_screen, &context->desktopMode);
		XF86VidModeSetViewPort(x11_display, x11_screen, 0, 0);
	}
	//XDestroyWindow(x11_display, x11_window);
	//XFreeColormap(x11_display, context->winAttr.colormap);
	XCloseDisplay(x11_display);

#endif
	this->~OpenGLgraphics();
}
#ifdef WINDOWS
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
bool OpenGLgraphics::createWindow(string title, Vec2i screenResolution, unsigned int maxSamples, bool fullscreen, unsigned int targetRendererVersion)
{
	bool fullscreenflag=fullscreen;

#if defined(WINDOWS)
	static bool arbMultisampleSupported=false;
	static int arbMultisampleFormat=0;
	
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASSEX	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style

	context->hInstance	= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									//- No Extra Window Data
	wc.hInstance		= context->hInstance;					// Set The Instance
	wc.hIcon			= (HICON)LoadImage(NULL,L"media/icon.ico",IMAGE_ICON,256,256,LR_LOADFROMFILE);// Load The Icon
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= L"OpenGL";							// Set The Class Name

	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return false
	}

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = screenResolution.x;
	WindowRect.bottom = screenResolution.y;

	DEVMODE dmScreenSettings;								// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= WindowRect.right;		// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= WindowRect.bottom;	// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
		// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
		if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
		{
			fullscreenflag=false;		// Windowed Mode Selected.
		}
		else
		{
			// Pop Up A Message Box Letting User Know The Program Is Closing.
			return false;									// Return false
		}
	}

	if (fullscreenflag)											// Are We Still In Fullscreen Mode?
	{
		dwExStyle=0;//WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(false);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(context->hWnd=CreateWindowExA(	dwExStyle,				// Extended Style For The Window
								"OpenGL",							// Class Name
								title.c_str(),						// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								context->hInstance,					// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		destroyWindow();					// Reset The Display
		MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}


	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | 0x00008000/*PFD_SUPPORT_COMPOSITION*/ | PFD_STEREO;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (!(context->hDC=GetDC(context->hWnd)))			// Did We Get A Device Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	if(!arbMultisampleSupported)
	{
		if (!(PixelFormat=ChoosePixelFormat(context->hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			destroyWindow();								// Reset The Display
			MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
			return false;								// Return false
		}
	}
	else
	{
		PixelFormat = arbMultisampleFormat;
	}
	if(!SetPixelFormat(context->hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}
	DescribePixelFormat(context->hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if(pfd.dwFlags & PFD_STEREO)
	{
		stereoMode = STEREO_3D;
	}

	if(!(context->hRC=wglCreateContext(context->hDC)))				// Are We Able To Get A Rendering Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}


	if(!wglMakeCurrent(context->hDC,context->hRC))					// Try To Activate The Rendering Context
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

#elif defined(LINUX)
	glXCreateContextAttribsARB = (GLXContext(*)(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list))glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	glXChooseFBConfig = (GLXFBConfig*(*)(Display *dpy, int screen, const int *attrib_list, int *nelements))glXGetProcAddressARB((GLubyte*)"glXChooseFBConfig");
	glXGetVisualFromFBConfig = (XVisualInfo*(*)(Display *dpy, GLXFBConfig config))glXGetProcAddressARB((GLubyte*)"glXGetVisualFromFBConfig");

	fullscreenflag = false;
	//sw = screenResolution.x = 1024;
	//sh = screenResolution.y = 786;
	//sAspect = ((float)sw)/sh;
	
	
	context->fullscreen = fullscreenflag;
	//based on http://content.gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
	int fbAttribs[] =
	{
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 16,
	  // GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
	};
    int contextAttribs[] =
	{
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
	};
	
	//int glxMajor, glxMinor;//, vmMajor, vmMinor;
	//XF86VidModeModeInfo **modes;
	int fbModeCount, bestFbModeNum=0;
	XSetWindowAttributes winAttr;

	//glXQueryVersion(x11_display, &vmMajor, &vmMinor);
	GLXFBConfig* fbModes = glXChooseFBConfig(x11_display, x11_screen, fbAttribs, &fbModeCount);
	//XF86VidModeGetAllModeLines(x11_display, x11_screen, &modeNum, &modes);

	/* save desktop-resolution before switching modes */
	//context->desktopMode = nullptr;//*modes[0];
	/* look for mode with requested resolution */
	//for (int i = 0; i < modeNum; i++)
	//{
	//	if ((modes[i]->hdisplay == screenResolution.x) && (modes[i]->vdisplay == screenResolution.y))
	//		bestMode = i;
	//}
	GLXFBConfig bestFbMode = fbModes[bestFbModeNum];
	XFree(fbModes);
	/* get an appropriate visual */
	XVisualInfo* vi = glXGetVisualFromFBConfig(x11_display, bestFbMode);//glXChooseVisual(x11_display, x11_screen, attrListDbl);
	if (vi == nullptr)
	{
		//TODO: add cleanup code
		std::cout << "FATAL ERROR: could not find visual";
		return false;// could not find
	}



	//glXQueryVersion(x11_display, &glxMajor, &glxMinor);
	/* create a GLX context */
	//context->context = glXCreateContext(x11_display, vi, 0, true);
	/* create a color map */
	Colormap cmap = XCreateColormap(x11_display, RootWindow(x11_display, vi->screen), vi->visual, AllocNone);
	winAttr.colormap = cmap;
	winAttr.background_pixmap = None;
	winAttr.background_pixel = 0;
	winAttr.border_pixel = 0;
	//winAttr.cursor = 0;
	
	Pixmap bm_no;
	XColor black, dummy;
	Colormap colormap;
	static char blankData[] = {0,0,0,0,0,0,0,0};
	colormap = DefaultColormap(x11_display, DefaultScreen(x11_display));
	XAllocNamedColor(x11_display, colormap, "black", &black, &dummy);
	bm_no = XCreateBitmapFromData(x11_display, RootWindow(x11_display, vi->screen), blankData, 8, 8);
	winAttr.cursor = XCreatePixmapCursor(x11_display, bm_no, bm_no, &black, &black, 0, 0);
	
	/*if (fullscreenflag)
	{
		/-* switch to fullscreen *-/
		XF86VidModeSwitchToMode(x11_display, x11_screen, bestFbMode);
		XF86VidModeSetViewPort(x11_display, x11_screen, 0, 0);
		int dpyWidth = modes[bestMode]->hdisplay;
		int dpyHeight = modes[bestMode]->vdisplay;
		XFree(modes);

		/-* set window attributes *-/
		winAttr.override_redirect = True;
		winAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |  StructureNotifyMask;
		x11_window = XCreateWindow(x11_display, RootWindow(x11_display, vi->screen), 0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect | CWCursor, &winAttr);
		XWarpPointer(x11_display, None, x11_window, 0, 0, 0, 0, 0, 0);
		XMapRaised(x11_display, x11_window);
		XGrabKeyboard(x11_display, x11_window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
		XGrabPointer(x11_display, x11_window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, x11_window, None, CurrentTime);
	}
	else*/
	{
		/* create a window in window mode*/
		winAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
		x11_window = XCreateWindow(x11_display, RootWindow(x11_display, vi->screen), 0, 0, screenResolution.x, screenResolution.y, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask | CWCursor, &winAttr);
		/* only set window title and handle wm_delete_events if in windowed mode */
		Atom wmDelete = XInternAtom(x11_display, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(x11_display, x11_window, &wmDelete, 1);
		Atom wmFullScreen = XInternAtom(x11_display, "_NET_WM_STATE_FULLSCREEN", True);
		XChangeProperty(x11_display, x11_window, XInternAtom(x11_display, "_NET_WM_STATE", True), ((Atom) 4)/*XA_ATOM*/, 32, 
						PropModeReplace, (unsigned char*) &wmFullScreen,  1);
		XSetStandardProperties(x11_display, x11_window, "FighterPilot", "FighterPilot", None, NULL, 0, NULL);
		XMapRaised(x11_display, x11_window);
	}
	XFree(vi);

	XWindowAttributes wAttributes;
	XGetWindowAttributes(x11_display, x11_window, &wAttributes);

	XSizeHints sizeHints;
	sizeHints.flags = PAspect;
	sizeHints.min_aspect.x = wAttributes.width;
	sizeHints.min_aspect.y = wAttributes.height;
	sizeHints.max_aspect.x = wAttributes.width;
	sizeHints.max_aspect.y = wAttributes.height;
	XSetWMNormalHints(x11_display, x11_window, &sizeHints);

//	typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
//	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
//	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
//		glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	context->context = glXCreateContextAttribsARB(x11_display, bestFbMode, 0, true, contextAttribs);
	/* connect the glx-context to the window */
	glXMakeCurrent(x11_display, x11_window, context->context);
	if(!glXIsDirect(x11_display, context->context))
	{
		//we don't have hardware acceleration!
		cout << "No hardware acceleration!!!" << endl;
	}
	const char* cTitle = title.c_str();
	XTextProperty textProperty;
	XStringListToTextProperty( (char **) &cTitle, 1, &textProperty );
	XSetWMProperties(x11_display, x11_window, &textProperty, &textProperty, 0, 0, nullptr, nullptr, nullptr);
    XFree(textProperty.value);
#endif

	glewExperimental = true; //force glew to attempt to get all function pointers (even for "unsupported" extensions)
	GLenum err = glewInit();
	glGetError(); //ignore any errors caused by glewInit
	string errorString;
	if(GLEW_OK != err)						errorString = string("Glew initialization failed with error: \"") + string((const char*)glewGetErrorString(err)) + "\"";
	else if(!GLEW_VERSION_3_3)				errorString = "Your version of OpenGL must be at least 3.3: Please update your graphics drivers";
//	else if(!GLEW_EXT_framebuffer_object)	errorString = "Your graphics card must support GL_EXT_framebuffer_object: Please update your graphics drivers.";
//	else if(!GLEW_ARB_vertex_array_object)	errorString = "Your graphics card must support ARB_vertex_array_object: Please update your graphics drivers.";

	if(errorString != "")
	{
		destroyWindow();
		const char* version = (const char*)glGetString(GL_VERSION);
		const char* renderer = (const char*)glGetString(GL_RENDERER);
		const char* vender = (const char*)glGetString(GL_VENDOR);

		if(version) errorString += string("\n   OpenGL version: ") + version;
		if(renderer) errorString += string("\n   Renderer: ") + renderer;
		if(vender) errorString += string("\n   Vender: ") + vender;
#ifdef WINDOWS
		MessageBoxA(NULL, errorString.c_str(),"ERROR",MB_OK);
#else
		std::cout << "FATAL ERROR: " << errorString << endl;
#endif
		return false;
	}


#if defined(WINDOWS)
	if(wglCreateContextAttribsARB && targetRendererVersion >= 4) //attempt to create an openGL 4 context
	{
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};
	
		auto gl_hrc = wglCreateContextAttribsARB(context->hDC,0, attribs);
		if(gl_hrc)
		{
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(context->hRC);
			context->hRC = gl_hrc;
			wglMakeCurrent(context->hDC, context->hRC);
			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenQueries(1, &transformFeedbackQueryID);
			openGL3 = true;
			openGL4 = true;
		}
	}
	if(!openGL4)
		messageBox("failed to create OpenGL 4 context");

	if(wglCreateContextAttribsARB && !openGL4 /*&& targetRendererVersion >= 3*/) //attempt to create an openGL 3 context
	{
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};
	
		auto gl_hrc = wglCreateContextAttribsARB(context->hDC,0, attribs);
		if(gl_hrc)
		{
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(context->hRC);
			context->hRC = gl_hrc;
			wglMakeCurrent(context->hDC, context->hRC);
			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenQueries(1, &transformFeedbackQueryID);
			openGL3 = true;
		}
	}

	ShowWindow(context->hWnd,SW_SHOW);				// Show The Window
	SetForegroundWindow(context->hWnd);				// Slightly Higher Priority
	SetFocus(context->hWnd);						// Sets Keyboard Focus To The Window
	RegisterHotKey(context->hWnd,IDHOT_SNAPWINDOW,0,VK_SNAPSHOT);
	RegisterHotKey(context->hWnd,IDHOT_SNAPDESKTOP,0,VK_SNAPSHOT);
#elif defined(LINUX)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	openGL3 = true;	
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glDepthFunc(GL_LEQUAL);

	if (!initFBOs(maxSamples))
	{
		destroyWindow();
		return false;
	}

	if(game->hasCommandLineOption("--stereo") && stereoMode == STEREO_NONE)
	{
		stereoMode = STEREO_ANAGLYPH;
		setInterOcularDistance(0.25);
	}

	isFullscreen = fullscreenflag;

	overlayVBO = genVertexBuffer(vertexBuffer::STREAM);
	overlayVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION2,	0);
	overlayVBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		2*sizeof(float));
	overlayVBO->setTotalVertexSize(sizeof(texturedVertex2D));

	shapesVBO = genVertexBuffer(vertexBuffer::STREAM);
	shapesVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3, 0);
	shapesVBO->setTotalVertexSize(sizeof(vertex3D));

//	shader_color3D->init("uniform mat4 transformationMatrix;\n attribute vec3 Position;\nvoid main(){gl_Position = transformationMatrix * position;}", "uniform vec4 color;\nvoid main(){gl_FragColor = color;}");
//	shader_color3D->setUniform4f("color", 0,0,0,1);

	return true;
}
//bool OpenGLgraphics::changeResolution(Vec2i resolution, unsigned int maxSamples) // may invalidate OpenGL function pointers?
//{
//	if(isFullscreen)
//	{
//		#if defined(WINDOWS)
//			DEVMODE dmScreenSettings;										// Device Mode
//			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// Makes Sure Memory's Cleared
//			dmScreenSettings.dmSize			= sizeof(dmScreenSettings);		// Size Of The Devmode Structure
//			dmScreenSettings.dmPelsWidth	= resolution.x;					// Selected Screen Width
//			dmScreenSettings.dmPelsHeight	= resolution.y;					// Selected Screen Height
//			dmScreenSettings.dmBitsPerPel	= 32;							// Selected Bits Per Pixel
//			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
//
//			if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
//			{
//				SetWindowPos(context->hWnd,0,0,0,resolution.x,resolution.y,0);
//				resize(resolution.x,resolution.y);
//				destroyFBOs();
//				initFBOs(maxSamples);
//				resize(resolution.x, resolution.y);
//				return true;
//			}
//		#elif defined(LINUX)
//			//TODO: add window resize code
//		#endif
//	}
//	else
//	{
//		#if defined(WINDOWS)
//			SetWindowPos(context->hWnd,0,0,0,resolution.x,resolution.y,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
//		#elif defined(LINUX)
//			//TODO: add window resize code
//		#endif
//	}
//	return false;
//}
set<GraphicsManager::displayMode> OpenGLgraphics::getSupportedDisplayModes()const
{
	displayMode tmpDM;
	set<displayMode> s;
#if defined(WINDOWS)
	int i=0;
	DEVMODE d;

	d.dmSize = sizeof(DEVMODE);
	d.dmDriverExtra = 0;

	while(EnumDisplaySettings(nullptr, i++, &d))
	{
		tmpDM.resolution = Vec2u(d.dmPelsWidth, d.dmPelsHeight);
		tmpDM.refreshRate = d.dmDisplayFrequency;
		s.insert(tmpDM);
	}

#elif defined(LINUX)
	XF86VidModeModeInfo **modes;
	int numModes;
	XF86VidModeGetAllModeLines(x11_display, x11_screen, &numModes, &modes);

	for(int i = 0; i < numModes; i++)
	{
		tmpDM.resolution = Vec2u(modes[i]->hdisplay, modes[i]->vdisplay);
		tmpDM.refreshRate = (modes[i]->dotclock * 1000 ) / (modes[i]->htotal * modes[i]->vtotal);
		s.insert(tmpDM);
	}
	free(modes);
#endif
	return s;
}
void OpenGLgraphics::swapBuffers()
{
#if defined(WINDOWS)
	SwapBuffers(context->hDC);
#elif defined(LINUX)
	glXSwapBuffers(x11_display, x11_window);
#endif
}
bool OpenGLgraphics::hasShaderModel4()const
{
	return openGL3;
}
bool OpenGLgraphics::hasShaderModel5()const
{
	return openGL4;
}
GraphicsManager::displayMode OpenGLgraphics::getCurrentDisplayMode()const
{
#if defined(WINDOWS)
	DEVMODE d;
	if(EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&d))
	{
		displayMode dm;
		if(isFullscreen)
		{
			dm.resolution = Vec2u(d.dmPelsWidth, d.dmPelsHeight);
		}
		else
		{
			dm.resolution = Vec2u(sw,sh);
		}
		dm.refreshRate = d.dmDisplayFrequency;
		return dm;
	}
	else
#elif defined(LINUX)
	{
		displayMode dm;	
		int dotclock;
		XF86VidModeModeLine modeLine;
		XF86VidModeGetModeLine(x11_display, x11_screen, &dotclock, &modeLine);
		
		dm.resolution = Vec2u(modeLine.hdisplay, modeLine.vdisplay);
		dm.refreshRate = (dotclock * 1000 ) / (modeLine.htotal * modeLine.vtotal);
		return dm;
	}
#endif
	{
		displayMode dm;
		dm.resolution = Vec2u(0, 0);
		dm.refreshRate = 0;
		return dm;
	}
}
void OpenGLgraphics::setRefreshRate(unsigned int rate)
{
#ifdef WINDOWS
	if(isFullscreen)
	{
		DEVMODE d;
		if(EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&d))
		{
			d.dmDisplayFrequency = rate;
			ChangeDisplaySettingsEx(NULL, &d, NULL, CDS_FULLSCREEN, NULL);
		}
	}
#else
	//TODO: add support for changing refresh rate under linux
#endif
}
void OpenGLgraphics::takeScreenshot(unsigned int tiles)
{
    cout << "Warning: can't pause for screenshot" << endl;
//	world->time.pause();
#ifdef WINDOWS
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	 
	string filename = "screen shots/FighterPilot ";
	filename += lexical_cast<string>(sTime.wYear) + "-" + lexical_cast<string>(sTime.wMonth) + "-" +
				lexical_cast<string>(sTime.wDay) + " " + lexical_cast<string>(sTime.wHour+1) + "-" +
				lexical_cast<string>(sTime.wMinute) + "-" + lexical_cast<string>(sTime.wSecond) + "-" +
				lexical_cast<string>(sTime.wMilliseconds) + ".bmp";
#else
	string filename = string("screen shots/FighterPilot") + lexical_cast<string>((unsigned int)GetTime()) + ".bmp";
#endif

	fileManager.createDirectory("screen shots");
	highResScreenshot = true;

	shared_ptr<FileManager::textureFile> file(new FileManager::textureFile(filename,FileManager::BMP));
	file->channels = 3;
	file->width = sw*tiles;
	file->height = sh*tiles;
	file->contents = new unsigned char[3*sw*tiles*sh*tiles];

	memset(file->contents, 0, 3*sw*tiles*sh*tiles);

	unsigned char* tileContents = new unsigned char[3*sw*sh];

	for(int x=0; x<tiles; x++)
	{
		for(int y=0; y<tiles; y++)
		{
			viewConstraint = Rect::XYWH(1.0/tiles*x, 1.0/tiles*y, 1.0/tiles, 1.0/tiles);

			//world->frameUpdate();
			render();
			bindRenderTarget(RT_SCREEN);
			glReadPixels(0, 0, sw, sh, GL_BGR, GL_UNSIGNED_BYTE, tileContents);

			for(int row=0;row<sh;row++)
			{
				memcpy(file->contents + (y*sw*sh*tiles + x*sw + row*sw*tiles)*3, tileContents+row*sw*3, 3*sw);
			}
		}
	}

	delete[] tileContents;

	fileManager.writeFile(file,true);
	viewConstraint = Rect::XYXY(0,0,1,1);
	highResScreenshot = false;
//	world->time.unpause();
}
void OpenGLgraphics::drawSphere(Vec3f position, float radius, Color4 color)
{
	if(!currentView)
		return;

	auto shader = shaders.bind("color3D");
	if(!shader)
		return;
	shader->setUniform4f("color", color);
	shader->setUniformMatrix("cameraProjection", currentView->projectionMatrix() * currentView->modelViewMatrix());
	shader->setUniformMatrix("modelTransform", Mat4f(Quat4f(), position, radius));

	auto sphereModel = dataManager.getModel("sphere");
	if(!sphereModel || sphereModel->materials.empty())
		return;

	sphereModel->VBO->bindBuffer();
	sphereModel->materials.front().indexBuffer->bindBuffer();
	sphereModel->materials.front().indexBuffer->drawBuffer();
}
void OpenGLgraphics::drawLine(Vec3f start, Vec3f end)
{
	debugAssert(currentView);

	shapes3D[0].position = start;
	shapes3D[1].position = end;

	shapesVBO->setVertexData(2*sizeof(vertex3D), shapes3D);
	shapesVBO->bindBuffer();
	shapesVBO->drawBuffer(LINES,0,2);
	
}
void OpenGLgraphics::drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3)
{
	debugAssert(currentView);

	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;

	shapesVBO->setVertexData(3*sizeof(vertex3D), shapes3D);
	shapesVBO->bindBuffer();
	shapesVBO->drawBuffer(TRIANGLES,0,3);
	
}
void OpenGLgraphics::drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4)
{
	debugAssert(currentView);

	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;
	shapes3D[3].position = p4;

	shapesVBO->setVertexData(4*sizeof(vertex3D), shapes3D);
	shapesVBO->bindBuffer();
	shapesVBO->drawBuffer(TRIANGLE_FAN,0,4);
}
void OpenGLgraphics::checkErrors()
{
#ifdef _DEBUG
	GLenum error = glGetError();
	const char* errorString = (const char*)gluErrorString(error);
	if(errorString == nullptr || strcmp(errorString, "no error") != 0)
	{
		//debugBreak();
		errorGlowEndTime = GetTime() + 1000.0;
	}
#endif
}
void OpenGLgraphics::startRenderToTexture(shared_ptr<texture2D> texture, unsigned int texture_level, shared_ptr<texture2D> depthTexture, unsigned int depth_level, bool clearTextures)
{
	setFrameBufferTextures(texture, texture_level, depthTexture, depth_level);
	if(clearTextures && !texture && depthTexture)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else if(clearTextures && texture && !depthTexture)
	{
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else if(clearTextures && texture && depthTexture)
	{
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}
	renderingToTexture = true;
}
void OpenGLgraphics::startRenderToTexture(shared_ptr<textureCube> texture, textureCube::Face face, unsigned int texture_level, bool clearTextures)
{
	setFrameBufferTextures(texture, face, texture_level);
	if(clearTextures)
	{
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	renderingToTexture = true;
}
void OpenGLgraphics::endRenderToTexture()
{	
	if(renderingToTexture)
	{
		//setBlendMode(GraphicsManager::TRANSPARENCY);
		bindRenderTarget(RT_FBO); //restore render textures on RT_FBO
		bindRenderTarget(cachedRenderTarget); //bind old render target
		glViewport(currentViewport.x, currentViewport.y, currentViewport.w, currentViewport.h);
		renderingToTexture = false;
	}
	else
	{
		debugBreak();
	}
}
void OpenGLgraphics::generateCustomMipmaps(shared_ptr<texture2D> tex, shared_ptr<shader> s)
{
	if(tex->getWidth() == 0 || tex->getHeight() == 0 || (tex->getWidth() == 1 && tex->getHeight() == 1))
		return;

	s->bind();
	s->setUniform1i("tex", 0);



	unsigned int width = tex->getWidth();
	unsigned int height = tex->getHeight();
	unsigned int level = 0;

	setDepthTest(false);
	//shared_ptr<GraphicsManager::View> view(new GraphicsManager::View());
	//view->ortho(-2048,2048, -2048,2048, -1, 1);
	//view->lookAt(Vec3f(0.5, 2.0, 0.5), Vec3f(0.5, 0.0, 0.5), Vec3f(0, 0, 1));

	do{
		width = max(width >> 1, 1);
		height = max(height >> 1, 1);
		level++;

		s->setUniform2f("invTexSize", 1.0 / width, 1.0 / height);
		s->setUniform1i("readLod", level-1);

		tex->bind(0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		startRenderToTexture(tex,level,nullptr,0,false);
		graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
		GraphicsManager::drawOverlay(Rect::XYXY(-1,-1,1,1));
		endRenderToTexture();

	}while(width > 1 || height > 1);


	tex->bind(0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}
#ifdef WINDOWS
HWND OpenGLgraphics::getWindowHWND()
{
	return context->hWnd;
}
#endif /*WINDOWS*/
#endif /*OPENGL*/
