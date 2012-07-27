
#include "engine.h"

#ifdef OPENGL

#define GLEW_STATIC

#if defined(WINDOWS)
	#include <Windows.h>
	#include "GL/glew.h"
	#include "GL/wglew.h"
#elif defined(LINUX)
	#include <X11/Xlib.h>
	#include <X11/extensions/xf86vmode.h>

	#include "GL/glew.h"
	#include <GL/glx.h>
#else
	#error OS not supported by openGLgraphics
#endif


#include <GL/glu.h>
#include "png/png.h"


//EXTENSIONS USED:
//GL_EXT_framebuffer_object (required)
//GL_EXT_framebuffer_multisample (optional)
//WGL_EXT_swap_control (optional)

bool gl2Hacks=true; //Some hacks are required to get code to run on old hardware (disabled if running in OpenGL 3 mode)
bool openGL3=false;

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
OpenGLgraphics::vertexBufferGL::vertexBufferGL(UsageFrequency u): vertexBuffer(u), vBufferID(0)
{
	glGenBuffers(1, &vBufferID);
}
OpenGLgraphics::vertexBufferGL::~vertexBufferGL()
{
	glBindBuffer(GL_ARRAY_BUFFER, vBufferID);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glDeleteBuffers(1, &vBufferID);
}
void OpenGLgraphics::vertexBufferGL::setVertexData(unsigned int size, void* data)
{
	vertexBuffer::bindBuffer();
	totalSize = size;
	if(usageFrequency == STATIC)		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
}
void OpenGLgraphics::vertexBufferGL::bindBuffer(unsigned int offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, vBufferID);


	for(unsigned int i=0; i<=7; i++)
	{
		auto v = (VertexAttribute)i;
		auto f = vertexAttributes.find(v);
		bool b = (f != vertexAttributes.end());
		if(gl2Hacks && i == 1)
		{
			dynamic_cast<OpenGLgraphics*>(graphics)->setClientState(0, b);
		}
		else
		{
			dynamic_cast<OpenGLgraphics*>(graphics)->setClientState(i, b);
		}
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
				if(gl2Hacks)
				{
					glVertexPointer(2, GL_FLOAT, totalVertexSize, (void*)((long)i->second.offset+offset));
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
				}
				else
				{
					glVertexAttribPointer(i->first, 2, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
				}
			}
			else if(i->first == POSITION3)
			{
				if(gl2Hacks)
				{
					glVertexPointer(3, GL_FLOAT, totalVertexSize, (void*)((long)i->second.offset+offset));
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
				}
				else
				{
					glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
				}
			}
			else if(i->first == TEXCOORD)		glVertexAttribPointer(i->first, 2, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == NORMAL)			glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == COLOR3)			glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == COLOR4)			glVertexAttribPointer(i->first, 4, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
			else if(i->first == TANGENT)		glVertexAttribPointer(i->first, 3, GL_FLOAT, GL_FALSE, totalVertexSize, (void*)((long)i->second.offset+offset));
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
	if(count == 0)
		return;

	vertexBuffer::bindBuffer();
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
OpenGLgraphics::indexBufferGL::indexBufferGL(UsageFrequency u): indexBuffer(u), bufferID(0), dataCount(0), dataType(NO_TYPE)
#ifdef _DEBUG
	, maxIndex(0)
#endif
{
	glGenBuffers(1, &bufferID);
}
OpenGLgraphics::indexBufferGL::~indexBufferGL()
{
	glDeleteBuffers(1, &bufferID);
}
void OpenGLgraphics::indexBufferGL::setData(unsigned char* data, unsigned int count)
{
	dataType = UCHAR;
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
void OpenGLgraphics::indexBufferGL::setData(unsigned short* data, unsigned int count)
{
	static_assert(sizeof(unsigned short) == 2, "unsigned short must have size of 2 bytes");
	
	dataType = USHORT;
	dataCount = count;
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
void OpenGLgraphics::indexBufferGL::setData(unsigned int* data, unsigned int count)
{
	static_assert(sizeof(unsigned int) == 4, "unsigned int must have size of 4 bytes");
	
	dataType = UINT;
	dataCount = count;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	if(usageFrequency == STATIC)		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_STREAM_DRAW);
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
void OpenGLgraphics::indexBufferGL::drawBuffer(Primitive primitive, shared_ptr<vertexBuffer> buffer, unsigned int offset)
{
	if(dataCount == 0 || dataType == NO_TYPE)
		return;

#ifdef _DEBUG
	if(buffer->getNumVertices() < maxIndex)
	{
		debugBreak();
		cout << "vertex index exceeds vertex count!!!" << endl;
	}
#endif

	GLenum type = UINT;//just to be safe
	if(dataType == UCHAR) type = GL_UNSIGNED_BYTE;
	else if(dataType == USHORT) type = GL_UNSIGNED_SHORT;
	else if(dataType == UINT) type = GL_UNSIGNED_INT;
	else cout << "indexBuffer datatype not set" << endl;
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	bindVertexBuffer(buffer, offset);

	if(primitive == POINTS)					glDrawElements(GL_POINTS,			dataCount, type, 0);
	else if(primitive == LINES)				glDrawElements(GL_LINES,			dataCount, type, 0);
	else if(primitive == LINE_STRIP)		glDrawElements(GL_LINE_STRIP,		dataCount, type, 0);
	else if(primitive == LINE_LOOP)			glDrawElements(GL_LINE_LOOP,		dataCount, type, 0);
	else if(primitive == TRIANGLES)			glDrawElements(GL_TRIANGLES,		dataCount, type, 0);
	else if(primitive == TRIANGLE_STRIP)	glDrawElements(GL_TRIANGLE_STRIP,	dataCount, type, 0);
	else if(primitive == TRIANGLE_FAN)		glDrawElements(GL_TRIANGLE_FAN,		dataCount, type, 0);
	else if(primitive == QUADS)				glDrawElements(GL_QUADS,			dataCount, type, 0);
	else if(primitive == QUAD_STRIP)		glDrawElements(GL_QUAD_STRIP,		dataCount, type, 0);
	else if(primitive == POLYGON)			glDrawElements(GL_POLYGON,			dataCount, type, 0);
}
void OpenGLgraphics::indexBufferGL::drawBufferSegment(Primitive primitive, shared_ptr<vertexBuffer> buffer, unsigned int numIndicies)
{
	if(dataCount == 0 || numIndicies == 0 || dataType == NO_TYPE)
		return;

	numIndicies = min(dataCount, numIndicies);

	GLenum type = UINT;//just to be safe
	if(dataType == UCHAR) type = GL_UNSIGNED_BYTE;
	else if(dataType == USHORT) type = GL_UNSIGNED_SHORT;
	else if(dataType == UINT) type = GL_UNSIGNED_INT;
	else cout << "indexBuffer datatype not set" << endl;
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	bindVertexBuffer(buffer, 0);

	if(primitive == POINTS)					glDrawElements(GL_POINTS,			numIndicies, type, 0);
	else if(primitive == LINES)				glDrawElements(GL_LINES,			numIndicies, type, 0);
	else if(primitive == LINE_STRIP)		glDrawElements(GL_LINE_STRIP,		numIndicies, type, 0);
	else if(primitive == LINE_LOOP)			glDrawElements(GL_LINE_LOOP,		numIndicies, type, 0);
	else if(primitive == TRIANGLES)			glDrawElements(GL_TRIANGLES,		numIndicies, type, 0);
	else if(primitive == TRIANGLE_STRIP)	glDrawElements(GL_TRIANGLE_STRIP,	numIndicies, type, 0);
	else if(primitive == TRIANGLE_FAN)		glDrawElements(GL_TRIANGLE_FAN,		numIndicies, type, 0);
	else if(primitive == QUADS)				glDrawElements(GL_QUADS,			numIndicies, type, 0);
	else if(primitive == QUAD_STRIP)		glDrawElements(GL_QUAD_STRIP,		numIndicies, type, 0);
	else if(primitive == POLYGON)			glDrawElements(GL_POLYGON,			numIndicies, type, 0);
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
	double t = GetTime();
	glBindTexture(GL_TEXTURE_2D, textureID);
	t = GetTime() - t;
	if(t > 5.0)
		cout << t << endl;
}
void OpenGLgraphics::texture2DGL::setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data, bool tileable)
{
	if(f != LUMINANCE && f != LUMINANCE_ALPHA && f != RGB && f != RGBA && f != RGB16 && f != RGBA16 && f != RGB16F && f != RGBA16F)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;

	//extention check needed since GeForce FX graphics cards do not support NPOT textures (even though they support OpenGL 2.0)
	bool NPOT = GLEW_ARB_texture_non_power_of_two && ((width & (width-1)) || (height & (height-1)));

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NPOT ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);

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

	if(!gl2Hacks || GLEW_ARB_texture_non_power_of_two || (!(width & (width-1)) && !(height & (height-1)))) //if we have support for NPOT textures, or texture is power of 2
	{
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		if(format == LUMINANCE)				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		else if(format == LUMINANCE_ALPHA)	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		else if(format == RGB)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(format == RGBA)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else if(format == RGB16)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data);
		else if(format == RGBA16)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
		else if(format == RGB16)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
		else if(format == RGBA16)			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);

		if(gl2Hacks)
		{
			glEnable(GL_TEXTURE_2D); //required for some ATI drivers?
		}

		if(openGL3)
			glGenerateMipmap(GL_TEXTURE_2D);
		else
			glGenerateMipmapEXT(GL_TEXTURE_2D);
	}
	else // we need to resize to a power of 2 (should only occur on GeForce FX graphics cards)
	{
		if(format == LUMINANCE)				gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		else if(format == LUMINANCE_ALPHA)	gluBuild2DMipmaps(GL_TEXTURE_2D, 2, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		else if(format == RGB)				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(format == RGBA)				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else if(format == RGB16)			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB16, GL_UNSIGNED_SHORT, data);
		else if(format == RGBA16)			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA16, GL_UNSIGNED_SHORT, data);
		else if(format == RGB16F)			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB16F, GL_FLOAT, data);
		else if(format == RGBA16F)			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA16F, GL_FLOAT, data);
	}
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
	if(f != LUMINANCE && f != LUMINANCE_ALPHA && f != RGB && f != RGBA && f != RGB16 && f != RGBA16 && f != RGB16F && f != RGBA16F)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;
	depth = Depth;

	glBindTexture(GL_TEXTURE_3D, textureID);
	if(format == LUMINANCE)				glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	else if(format == LUMINANCE_ALPHA)	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE_ALPHA, width, height, depth, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
	else if(format == RGB)				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if(format == RGBA)				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else if(format == RGB16)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, width, height, depth, 0, GL_RGB, GL_UNSIGNED_SHORT, data);
	else if(format == RGBA16)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
	else if(format == RGB16F)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, width, height, depth, 0, GL_RGB, GL_FLOAT, data);
	else if(format == RGBA16F)			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, width, height, depth, 0, GL_RGBA, GL_FLOAT, data);

	if(openGL3)
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
	if(f != LUMINANCE && f != LUMINANCE_ALPHA && f != RGB && f != RGBA && f != RGB16 && f != RGBA16 && f != RGB16F && f != RGBA16F)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	if(format == LUMINANCE)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width*height * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width*height * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width*height * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width*height * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width*height * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width*height * 5);
	}
	else if(format == LUMINANCE_ALPHA)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data + width*height*2 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data + width*height*2 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data + width*height*2 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data + width*height*2 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data + width*height*2 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data + width*height*2 * 5);
	}
	else if(format == RGB)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*3 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*3 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*3 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*3 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*3 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*3 * 5);
	}
	else if(format == RGBA)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + width*height*4 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + width*height*4 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + width*height*4 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + width*height*4 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + width*height*4 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data + width*height*4 * 5);
	}
	else if(format == RGB16)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data + width*height*2*3 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data + width*height*2*3 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data + width*height*2*3 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data + width*height*2*3 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data + width*height*2*3 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB16, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT, data + width*height*2*3 * 5);
	}
	else if(format == RGBA16)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data + width*height*2*4 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data + width*height*2*4 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data + width*height*2*4 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data + width*height*2*4 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data + width*height*2*4 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT, data + width*height*2*4 * 5);
	}
	else if(format == RGB16F)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data + width*height*4*3 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data + width*height*4*3 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data + width*height*4*3 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data + width*height*4*3 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data + width*height*4*3 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data + width*height*4*3 * 5);
	}
	else if(format == RGBA16F)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data + width*height*4*4 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data + width*height*4*4 * 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data + width*height*4*4 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data + width*height*4*4 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data + width*height*4*4 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data + width*height*4*4 * 5);
	}
	if(openGL3)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	else
		glGenerateMipmapEXT(GL_TEXTURE_CUBE_MAP);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

int OpenGLgraphics::shaderGL::getUniformLocation(string uniform)
{
	auto it = uniforms.find(uniform);
	if(it == uniforms.end())
	{
		it = uniforms.insert(pair<string, int>(uniform, glGetUniformLocation(shaderId, uniform.c_str()))).first;
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
	glUseProgram(shaderId);
}
void OpenGLgraphics::shaderGL::unbind()
{
	glUseProgram(0);
}
bool OpenGLgraphics::shaderGL::init(const char* vert, const char* frag, const char* geometry)
{
	if(shaderId != 0)
		glDeleteProgram(shaderId);

	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);


	if(openGL3)
	{
		string vertString = string("#version 130\n")+vert;
		string fragString = string("#version 130\nout vec4 _FragColor;\n")+frag;

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

		const char* vertexSource = vertString.c_str();
		const char* fragmentSource = fragString.c_str();

		glShaderSource(v, 1, &vertexSource, NULL);
		glShaderSource(f, 1, &fragmentSource, NULL);
	}
	else
	{
		static const char* versionMacro = "#version 120\n";
		const char* vertexSource[] = {versionMacro, vert};
		const char* fragmentSource[] = {versionMacro, frag};
		glShaderSource(v, 2, vertexSource, NULL);
		glShaderSource(f, 2, fragmentSource, NULL);
	}

	glCompileShader(v);
	glCompileShader(f);

	int i, length, success;//used whenever a pointer to int is required

	//get vertex shader errors
	glGetShaderiv(v, GL_INFO_LOG_LENGTH, &length);
	char* cv=new char[length];
	glGetShaderInfoLog(v,length,&i,cv);
	vertErrorLog = string(cv);
	delete[] cv;


	//get fragment shader errors
	glGetShaderiv(f,GL_INFO_LOG_LENGTH,&length);
	char* cf=new char[length];
	glGetShaderInfoLog(f,length,&i,cf);
	fragErrorLog = string(cf);
	delete[] cf;


	glGetShaderiv(v,GL_COMPILE_STATUS,&success);
	if(!success)
	{
		return false;
	}
	glGetShaderiv(f,GL_COMPILE_STATUS,&success);
	if(!success)
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
	glBindAttribLocation(shaderId, 7, "GenericFloat");

	if(gl2Hacks) //force both positions to occupy the location of gl_Vertex
	{
		//glBindAttribLocation(shaderId, 0, "Position2"); (already set)
		glBindAttribLocation(shaderId, 0, "Position");
	}

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

	if(vertErrorLog != "" && fragErrorLog != "" && linkErrorLog != "")
	{
		return string("vert:") + vertErrorLog + "frag:" + fragErrorLog + "link:" + linkErrorLog;
	}
	else if(vertErrorLog != "" && fragErrorLog != "")
	{
		return string("vert:") + vertErrorLog + "frag:" + fragErrorLog;
	}
	else if(vertErrorLog != "" && linkErrorLog != "")
	{
		return string("vert:") + vertErrorLog + string("link:") + linkErrorLog;
	}
	else if(vertErrorLog != "")
	{
		return string("vert:") + vertErrorLog;
	}
	else if(fragErrorLog != "" && linkErrorLog != "")
	{
		return string("frag:") + fragErrorLog + string("link:") + linkErrorLog;
	}
	else if(fragErrorLog != "")
	{
		return string("frag:") + fragErrorLog;
	}
	else if(linkErrorLog != "")
	{
		return string("link:") + linkErrorLog;
	}
	else
	{
		return "";
	}
}
void OpenGLgraphics::shaderGL::setUniform1f(string name, float v0)
{
	glUseProgram(shaderId);
	glUniform1f(getUniformLocation(name), v0);
}
void OpenGLgraphics::shaderGL::setUniform2f(string name, float v0, float v1)
{
	glUseProgram(shaderId);
	glUniform2f(getUniformLocation(name), v0, v1);
}
void OpenGLgraphics::shaderGL::setUniform3f(string name, float v0, float v1, float v2)
{
	glUseProgram(shaderId);
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}
void OpenGLgraphics::shaderGL::setUniform4f(string name, float v0, float v1, float v2, float v3)
{
	glUseProgram(shaderId);
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}
void OpenGLgraphics::shaderGL::setUniform1i(string name, int v0)
{
	glUseProgram(shaderId);
	glUniform1i(getUniformLocation(name), v0);
}
void OpenGLgraphics::shaderGL::setUniform2i(string name, int v0, int v1)
{
	glUseProgram(shaderId);
	glUniform2i(getUniformLocation(name), v0, v1);
}
void OpenGLgraphics::shaderGL::setUniform3i(string name, int v0, int v1, int v2)
{
	glUseProgram(shaderId);
	glUniform3i(getUniformLocation(name), v0, v1, v2);
}
void OpenGLgraphics::shaderGL::setUniform4i(string name, int v0, int v1, int v2, int v3)
{
	glUseProgram(shaderId);
	glUniform4i(getUniformLocation(name), v0, v1, v2, v3);
}
void OpenGLgraphics::shaderGL::setUniformMatrix(string name, const Mat3f& m)
{
	glUseProgram(shaderId);
	glUniformMatrix3fv(getUniformLocation(name),1,false,m.ptr());
}
void OpenGLgraphics::shaderGL::setUniformMatrix(string name, const Mat4f& m)
{
	glUseProgram(shaderId);
	glUniformMatrix4fv(getUniformLocation(name),1,false,m.ptr());
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
OpenGLgraphics::OpenGLgraphics():highResScreenshot(false),multisampling(false),samples(0),renderTarget(RT_SCREEN), colorMask(true), depthMask(true), texCoord_clientState(false), normal_clientState(false), color_clientState(false)
{
#ifdef _DEBUG
	errorGlowEndTime = 0;
#endif
	viewConstraint = Rect::XYXY(0,0,1,1);

	context = new Context;
	//useAnagricStereo(true);
	//setInterOcularDistance(0.25);
}
OpenGLgraphics::~OpenGLgraphics()
{
	delete context;
}
bool OpenGLgraphics::drawOverlay(Rect4f r, string tex)
{
	if(tex != "") dataManager.bind(tex);

	overlay[0].position = Vec2f(r.x,		r.y+r.h);
	overlay[1].position = Vec2f(r.x+r.w,	r.y+r.h);
	overlay[2].position = Vec2f(r.x+r.w,	r.y);
	overlay[3].position = Vec2f(r.x,		r.y);

	overlay[0].texCoord = Vec2f(0.0,1.0);
	overlay[1].texCoord = Vec2f(1.0,1.0);
	overlay[2].texCoord = Vec2f(1.0,0.0);
	overlay[3].texCoord = Vec2f(0.0,0.0);

	overlayVBO->setVertexData(4*sizeof(texturedVertex2D), overlay);
	overlayVBO->drawBuffer(TRIANGLE_FAN,0,4);

	return true;
}
bool OpenGLgraphics::drawRotatedOverlay(Rect4f r, Angle rotation, string tex)
{
	if(tex != "") dataManager.bind(tex);

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
	overlayVBO->drawBuffer(TRIANGLE_FAN,0,4);

	return true;
}
bool OpenGLgraphics::drawPartialOverlay(Rect4f r, Rect4f t, string tex)
{
	if(tex != "") dataManager.bind(tex);

	overlay[0].position = Vec2f(r.x,		r.y);
	overlay[1].position = Vec2f(r.x+r.w,	r.y);
	overlay[2].position = Vec2f(r.x+r.w,	r.y+r.h);
	overlay[3].position = Vec2f(r.x,		r.y+r.h);

	overlay[0].texCoord = Vec2f(t.x,		t.y);
	overlay[1].texCoord = Vec2f(t.x+t.w,	t.y);
	overlay[2].texCoord = Vec2f(t.x+t.w,	t.y+t.h);
	overlay[3].texCoord = Vec2f(t.x,		t.y+t.h);

	overlayVBO->setVertexData(4*sizeof(texturedVertex2D), overlay);
	overlayVBO->drawBuffer(TRIANGLE_FAN,0,4);

	return true;
}
shared_ptr<GraphicsManager::vertexBuffer> OpenGLgraphics::genVertexBuffer(GraphicsManager::vertexBuffer::UsageFrequency usage)
{
	return shared_ptr<vertexBuffer>(new vertexBufferGL(usage));
}
shared_ptr<GraphicsManager::indexBuffer> OpenGLgraphics::genIndexBuffer(indexBuffer::UsageFrequency usage)
{
	return shared_ptr<indexBuffer>(new indexBufferGL(usage));
}
shared_ptr<GraphicsManager::texture2D> OpenGLgraphics::genTexture2D()
{
	return shared_ptr<texture2D>(new texture2DGL());
}
shared_ptr<GraphicsManager::texture3D> OpenGLgraphics::genTexture3D()
{
	return shared_ptr<texture3D>(new texture3DGL());
}
shared_ptr<GraphicsManager::textureCube> OpenGLgraphics::genTextureCube()
{
	return shared_ptr<textureCube>(new textureCubeGL());
}
shared_ptr<GraphicsManager::shader> OpenGLgraphics::genShader()
{
	return shared_ptr<shader>(new shaderGL());
}
void OpenGLgraphics::setGamma(float gamma)
{
	currentGamma = gamma;
}
void OpenGLgraphics::setColor(float r, float g, float b, float a)
{
	dataManager.setUniform4f("color", r, g, b, a);
}
void OpenGLgraphics::setColorMask(bool mask)
{
	//if(renderTarget == RT_SCREEN && mask != colorMask)
	//{
		colorMask = mask;
		glColorMask(mask, mask, mask, mask);
	//}
	//else if(renderTarget == RT_FBO_0 && mask != FBOs[0].colorBound)
	//{
	//	FBOs[0].colorBound = mask;
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mask ? FBOs[0].color : 0, 0);
	//}
	//else if(renderTarget == RT_FBO_1 && mask != FBOs[1].colorBound)
	//{
	//	FBOs[1].colorBound = mask;
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mask ? FBOs[1].color : 0, 0);
	//}
	//else if(renderTarget == RT_MULTISAMPLE_FBO && mask != multisampleFBO.colorBound)
	//{
	//	multisampleFBO.colorBound = mask;
	//	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, mask ? multisampleFBO.color : 0);
	//}
}
void OpenGLgraphics::setDepthMask(bool mask)
{
	if(mask != depthMask)
	{
		depthMask = mask;
		glDepthMask(depthMask);
	}
	//if(renderTarget == RT_SCREEN && mask != depthMask)
	//{
	//	depthMask = mask;
	//	glDepthMask(depthMask);
	//}
	//else if(renderTarget == RT_FBO_0 && mask != FBOs[0].depthBound)
	//{
	//	FBOs[0].depthBound = mask;
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? FBOs[0].depth : 0, 0);
	//}
	//else if(renderTarget == RT_FBO_1 && mask != FBOs[1].depthBound)
	//{
	//	FBOs[1].depthBound = mask;
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? FBOs[1].depth : 0, 0);
	//}
	//else if(renderTarget == RT_MULTISAMPLE_FBO && mask != multisampleFBO.depthBound)
	//{
	//	multisampleFBO.depthBound = mask;
	//	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mask ? multisampleFBO.depth : 0);
	//}
}
void OpenGLgraphics::setDepthTest(bool enabled)
{
	if(enabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}
void OpenGLgraphics::setBlendMode(BlendMode blend)
{
	if(blend == TRANSPARENCY)				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	else if(blend == ADDITIVE)				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
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
#ifdef WINDOWS
	if(WGLEW_EXT_swap_control)// wglSwapIntervalEXT)
		wglSwapIntervalEXT(enabled ? 1 : 0);//turn on/off vsync (0 = off and 1 = on)
#endif
}
void OpenGLgraphics::setAlphaToCoverage(bool enabled)
{
	if(multisampling)
	{
		if(enabled)
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		else
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	}
}
void OpenGLgraphics::setWireFrame(bool enabled)
{
	glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}
void OpenGLgraphics::drawText(string text, Vec2f pos, string font)
{
	auto f = dataManager.getFont(font);

	if(f != NULL)
	{
		f->texture->bind();

		Vec2f charPos = pos;
		Rect charRect;
		//if(dataManager.getBoundShader() == "ortho")
		//{
			for(auto i = text.begin(); i != text.end(); i++)
			{
				if(*i == '\n')
				{
					charPos.x = pos.x;
					charPos.y += f->height / 1024.0;
				}
				else if(f->characters.count(*i) != 0)
				{
					auto& c = f->characters.find(*i)->second;

					charRect.x = charPos.x + c.xOffset / 1024.0;
					charRect.y = charPos.y + c.yOffset / 1024.0;
					charRect.w = c.width / 1024.0;
					charRect.h = c.height / 1024.0;

					drawPartialOverlay(charRect, c.UV);
					charPos.x += c.xAdvance  / 1024.0;
				}
			}
		//}
		//else
		//{
		//	for(auto i = text.begin(); i != text.end(); i++)
		//	{
		//		if(*i == '\n')
		//		{
		//			charPos.x = pos.x;
		//			charPos.y += f->height;
		//		}
		//		else if(f->characters.count(*i) != 0)
		//		{
		//			auto& c = f->characters.find(*i)->second;

		//			charRect.x = charPos.x + c.xOffset;
		//			charRect.y = charPos.y + c.yOffset;
		//			charRect.w = c.width;
		//			charRect.h = c.height;

		//			drawPartialOverlay(charRect, c.UV);
		//			charPos.x += c.xAdvance;
		//		}
		//	}
		//}
	}
}
void OpenGLgraphics::drawText(string text, Rect rect, string font)
{
	auto f = dataManager.getFont(font);

	if(f == NULL)
	{
		f->texture->bind();

		Vec2f charPos = rect.origin();
		Rect charRect;
		for(auto i = text.begin(); i != text.end(); i++)
		{
			if(*i == '\n')
			{
				charPos.x = rect.x;
				charPos.y += f->height;
			}
			else if(f->characters.count(*i) != 0 && rect.inRect(charPos))
			{
				auto& c = f->characters.find(*i)->second;

				charRect.x = charPos.x + c.xOffset;
				charRect.y = charPos.y + c.yOffset;
				charRect.w = c.width;
				charRect.h = c.height;

				Rect UV = c.UV;
				if(charRect.x < rect.x + rect.w && charRect.y < rect.y + rect.h)
				{
					if(charRect.x + charRect.w > rect.x + rect.w)	UV.w *= ((rect.x + rect.w) - charRect.x) / charRect.w;
					if(charRect.y + charRect.h > rect.y + rect.h)	UV.h *= ((rect.y + rect.h) - charRect.y) / charRect.h;

					drawPartialOverlay(charRect, UV);
				}
				charPos.x += c.xAdvance;
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
void OpenGLgraphics::bindRenderTarget(RenderTarget t)
{
	renderTarget = t;
	if(openGL3)
	{
		if(t == RT_FBO_0)					glBindFramebuffer(GL_FRAMEBUFFER, FBOs[0].fboID);
		else if(t == RT_FBO_1)				glBindFramebuffer(GL_FRAMEBUFFER, FBOs[1].fboID);
		else if(t == RT_MULTISAMPLE_FBO)	glBindFramebuffer(GL_FRAMEBUFFER, multisampleFBO.fboID);
		else if(t == RT_SCREEN)				glBindFramebuffer(GL_FRAMEBUFFER, 0);
		else debugBreak();
	}
	else
	{
		if(t == RT_FBO_0)					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[0].fboID);
		else if(t == RT_FBO_1)				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[1].fboID);
		else if(t == RT_MULTISAMPLE_FBO)	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFBO.fboID);
		else if(t == RT_SCREEN)				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		else debugBreak();
	}
}
void OpenGLgraphics::renderFBO(RenderTarget src) //right now can only be RT_FBO
{
	int bufferNum;
	if(src == RT_FBO_0)			bufferNum = 0;
	else if(src == RT_FBO_1)	bufferNum = 1;
	else return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOs[bufferNum].color);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, FBOs[bufferNum].normals);
	//glActiveTexture(GL_TEXTURE0);

	glViewport(0,0,sw,sh);
	drawOverlay(Rect::XYXY(-1,-1,1,1));
}
bool OpenGLgraphics::initFBOs(unsigned int maxSamples)
{
	bool gl3FBOs = openGL3 || GLEW_ARB_framebuffer_object;
	//////////////////////////////////////////////////////////////////////////////////////
	auto checkForFBOErrors = [gl3FBOs]()-> bool
	{
		GLenum status = gl3FBOs ? glCheckFramebufferStatus(GL_FRAMEBUFFER) : glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status == GL_FRAMEBUFFER_COMPLETE)							return true;
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
		return false;
	};
	/////////////////////////////////////////////////////////////////////////////////////
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
	samples = samples >= maxSamples ? maxSamples : samples;
	multisampling = (GLEW_EXT_framebuffer_multisample || gl3FBOs) && samples > 1;
	//////////////////////////////////////////////////////////////////////////////////////
	for(int i = 0; i < 2; i++)
	{
		glGenTextures(1, &FBOs[i].color);
		glBindTexture(GL_TEXTURE_2D, FBOs[i].color);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		//glGenTextures(1, &FBOs[i].normals);
		//glBindTexture(GL_TEXTURE_2D, FBOs[i].normals);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &FBOs[i].depth);
		glBindTexture(GL_TEXTURE_2D, FBOs[i].depth);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT24, sw, sh, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

		if(gl3FBOs)
		{
			glGenFramebuffers(1, &FBOs[i].fboID);
			glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i].fboID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOs[i].color, 0);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, FBOs[i].normals, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBOs[i].depth, 0);
		}
		else
		{
			glGenFramebuffersEXT(1, &FBOs[i].fboID);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i].fboID);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, FBOs[i].color, 0);
			//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, FBOs[i].normals, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, FBOs[i].depth, 0);
		}

		if(!checkForFBOErrors())
			return false;
	}
	//////////////////////////////////////////////////////////////////////////////////////
	if(multisampling)
	{
		if(gl3FBOs)
		{
			glGenRenderbuffers(1, &multisampleFBO.color);
			glBindRenderbuffer(GL_RENDERBUFFER, multisampleFBO.color);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA, sw, sh);

			glGenRenderbuffers(1, &multisampleFBO.depth);
			glBindRenderbuffer(GL_RENDERBUFFER, multisampleFBO.depth);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT24, sw, sh);

			glGenFramebuffers(1, &multisampleFBO.fboID);

			glBindFramebuffer(GL_FRAMEBUFFER, multisampleFBO.fboID);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, multisampleFBO.color);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampleFBO.depth);

			checkErrors();
			if(!checkForFBOErrors())
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
		}
		else
		{
			glGenRenderbuffersEXT(1, &multisampleFBO.color);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, multisampleFBO.color);
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_RGBA, sw, sh);

			glGenRenderbuffersEXT(1, &multisampleFBO.depth);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, multisampleFBO.depth);
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_DEPTH_COMPONENT24, sw, sh);

			glGenFramebuffersEXT(1, &multisampleFBO.fboID);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFBO.fboID);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, multisampleFBO.color);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, multisampleFBO.depth);

			checkErrors();
			if(!checkForFBOErrors())
			{
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				return false;
			}
		}
	}

	if(gl3FBOs)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return true;
}
void OpenGLgraphics::destroyFBOs()
{
	if(FBOs[0].color != 0)		glDeleteTextures(1, &FBOs[0].color);
	if(FBOs[0].depth != 0)		glDeleteTextures(1, &FBOs[0].depth);
	if(FBOs[1].color != 0)		glDeleteTextures(1, &FBOs[1].color);
	if(FBOs[1].depth != 0)		glDeleteTextures(1, &FBOs[1].depth);

	if(openGL3 || GLEW_ARB_framebuffer_object)
	{
		if(FBOs[0].fboID != 0)		glDeleteFramebuffers(1, &FBOs[0].fboID);
		if(FBOs[1].fboID != 0)		glDeleteFramebuffers(1, &FBOs[1].fboID);
		if(multisampling)
		{
			if(multisampleFBO.fboID != 0)	glDeleteFramebuffers(1, &multisampleFBO.fboID);
			if(multisampleFBO.color != 0)	glDeleteRenderbuffers(1, &multisampleFBO.color);
			if(multisampleFBO.depth != 0)	glDeleteRenderbuffers(1, &multisampleFBO.depth);
		}
	}
	else
	{
		if(FBOs[0].fboID != 0)		glDeleteFramebuffersEXT(1, &FBOs[0].fboID);
		if(FBOs[1].fboID != 0)		glDeleteFramebuffersEXT(1, &FBOs[1].fboID);
		if(multisampling)
		{
			if(multisampleFBO.fboID != 0)	glDeleteFramebuffersEXT(1, &multisampleFBO.fboID);
			if(multisampleFBO.color != 0)	glDeleteRenderbuffersEXT(1, &multisampleFBO.color);
			if(multisampleFBO.depth != 0)	glDeleteRenderbuffersEXT(1, &multisampleFBO.depth);
		}
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
	fps= ((double)frameTimes.size()) /  totalTime;

/////////////////////////////////////BIND BUFFER//////////////////////////////////////////
	bindRenderTarget(multisampling ? RT_MULTISAMPLE_FBO : RT_FBO_0);
	setDepthMask(true);
	setColorMask(true);
///////////////////////////////////CLEAR BUFFERS/////////////////////////////////
	glClearColor(0.47f,0.57f,0.63f,1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//////////////////////////////////SET LIGHTING///////////////////////////////////
	//float lightPos[4] = {lightPosition.x, lightPosition.y, lightPosition.z, 1.0};
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
////////////////////////////////////3D RENDERING/////////////////////////////////////
	Vec3f cameraOffset;
	for(int eye=0; eye<(stereo?2:1); eye++)
	{
		//set up stereo
		if(stereo && eye==0)
		{
			leftEye = true;
			glColorMask(true,false,false,true);
			cameraOffset = Vec3f(-interOcularDistance/2,0,0);
		}
		else if(stereo && eye==1)
		{
			leftEye = false;
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
				dataManager.bind("model");
				currentView = shared_ptr<View>(*i);
				glViewport(currentView->viewport().x * sh, currentView->viewport().y * sh, currentView->viewport().width * sh, currentView->viewport().height * sh);

				if(highResScreenshot)	currentView->constrainView(viewConstraint);
				else if(stereo)			currentView->shiftCamera(cameraOffset);

				dataManager.setUniformMatrix("cameraProjection",currentView->projectionMatrix() * currentView->modelViewMatrix());
				dataManager.setUniformMatrix("modelTransform", Mat4f());

				currentView->render();

				//dataManager.setUniformMatrix("cameraProjection",Mat4f());
				//dataManager.setUniformMatrix("modelTransform", Mat4f());
				//drawTriangle(Vec3f(0,0,0), Vec3f(0.5,0.4,0.0), Vec3f(0,0.4,0.0));

				//dataManager.bind("ortho");
				//dataManager.setUniform4f("color",white);
				//drawOverlay(Rect::CWH(0.25,0.25,0.1,0.1), "grass");


				if(highResScreenshot)	currentView->constrainView(Rect::XYXY(0,0,1,1));
				else if(stereo)			currentView->shiftCamera(Vec3f(0,0,0));

				i++;
			}
		}

		//capture depth buffer for reading and bind it to texture unit 1
		if(multisampling && (openGL3 || GLEW_ARB_framebuffer_object))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFBO.fboID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOs[0].fboID);
			glBlitFramebuffer(0, 0, sw, sh, 0, 0, sw, sh, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, multisampleFBO.fboID);
		}
		else if(multisampling)
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, multisampleFBO.fboID);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, FBOs[0].fboID);
			glBlitFramebufferEXT(0, 0, sw, sh, 0, 0, sw, sh, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFBO.fboID);
		}
		else
		{
			setDepthMask(false);
		}
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBOs[0].depth);

		//render particles
		for(auto i = views.begin(); i != views.end();)
		{
			if(i->expired()) //check if the view no longer exists (just to be safe)
			{
				i = views.erase(i);
			}
			else
			{
				currentView = shared_ptr<View>(*i);
				if(currentView->renderParticles())
				{
					glViewport(currentView->viewport().x * sh, currentView->viewport().y * sh, currentView->viewport().width * sh, currentView->viewport().height * sh);

					if(highResScreenshot)	currentView->constrainView(viewConstraint);
					else if(stereo)			currentView->shiftCamera(cameraOffset);

					particleManager.render(currentView);

					if(highResScreenshot)	currentView->constrainView(Rect::XYXY(0,0,1,1));
					else if(stereo)			currentView->shiftCamera(Vec3f(0,0,0));
				}
				i++;
			}
		}
	}

	glDisable(GL_DEPTH_TEST);

	currentView.reset(); //set the current view to null
	if(stereo)
		glColorMask(true,true,true,true);

	sceneManager.endRender(); //do some post render cleanup
/////////////////////////////////////START 2D////////////////////////////////////
	glViewport(0,0,sw,sh);
	if(!highResScreenshot)
	{
		dataManager.bind("ortho");
		dataManager.setUniform4f("color",white);
		menuManager.render();

		#ifdef _DEBUG
			if(fps<59.0)dataManager.setUniform4f("color",red);
			else dataManager.setUniform4f("color",black);
			graphics->drawText(lexical_cast<string>(floor(fps)),Vec2f(sAspect*0.5-0.5*graphics->getTextSize(lexical_cast<string>(floor(fps))).x,0.02));
			dataManager.setUniform4f("color",white);
			Profiler.draw();

			if(errorGlowEndTime > GetTime() && dataManager.assetLoaded("errorGlow"))
			{
				setColor(1,0,0,clamp((errorGlowEndTime-GetTime())/1000.0, 0.0, 1.0));
				drawOverlay(Rect::XYXY(0,0,sAspect,1), "errorGlow");
				setColor(1,1,1,1);
			}
		#endif
	}
///////////////////////////////////////Post Processing//////////////////////////////////
	if(multisampling && (openGL3 || GLEW_ARB_framebuffer_object))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampleFBO.fboID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOs[0].fboID);
		glBlitFramebuffer(0, 0, sw, sh, 0, 0, sw, sh, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	else if(multisampling)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, multisampleFBO.fboID);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, FBOs[0].fboID);
		glBlitFramebufferEXT(0, 0, sw, sh, 0, 0, sw, sh, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
	}

	bindRenderTarget(RT_SCREEN);
	//glClearColor(0.0,0.0,0.0,1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	dataManager.bind("gamma shader");
	dataManager.setUniform1f("gamma",currentGamma);
	dataManager.setUniform1i("tex",0);
	renderFBO(RT_FBO_0);
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
	//TODO: write windows destruction code
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
	XCloseDisplay(x11_display);

#endif
	this->~OpenGLgraphics();
}
#ifdef WINDOWS
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
bool OpenGLgraphics::createWindow(string title, Vec2i screenResolution, unsigned int maxSamples)
{
	bool fullscreenflag=true;

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


	if (!(context->hRC=wglCreateContext(context->hDC)))				// Are We Able To Get A Rendering Context?
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
	fullscreenflag = false;
	sw = screenResolution.x = 1024;
	sh = screenResolution.y = 786;
	sAspect = ((float)sw)/sh;
	
	
	context->fullscreen = fullscreenflag;
	//based on http://content.gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
	static int attrListDbl[] =
	{
		GLX_RGBA, GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 4,
		GLX_GREEN_SIZE, 4,
		GLX_BLUE_SIZE, 4,
		GLX_DEPTH_SIZE, 16,
		None
	};
	
	int glxMajor, glxMinor, vmMajor, vmMinor;
	XF86VidModeModeInfo **modes;
	int modeNum, bestMode=0;
	XSetWindowAttributes winAttr;
	
	XF86VidModeQueryVersion(x11_display, &vmMajor, &vmMinor);
	XF86VidModeGetAllModeLines(x11_display, x11_screen, &modeNum, &modes);
	/* save desktop-resolution before switching modes */
	context->desktopMode = *modes[0];
	/* look for mode with requested resolution */
	for (int i = 0; i < modeNum; i++)
	{
		if ((modes[i]->hdisplay == screenResolution.x) && (modes[i]->vdisplay == screenResolution.y))
			bestMode = i;
	}
	/* get an appropriate visual */
	XVisualInfo* vi = glXChooseVisual(x11_display, x11_screen, attrListDbl);
	if (vi == nullptr)
	{
		//TODO: add cleanup code
		return false;// could not find
	}
	glXQueryVersion(x11_display, &glxMajor, &glxMinor);
	/* create a GLX context */
	context->context = glXCreateContext(x11_display, vi, 0, true);
	/* create a color map */
	Colormap cmap = XCreateColormap(x11_display, RootWindow(x11_display, vi->screen), vi->visual, AllocNone);
	winAttr.colormap = cmap;
	winAttr.border_pixel = 0;

	if (fullscreenflag)
	{
		/* switch to fullscreen */
		XF86VidModeSwitchToMode(x11_display, x11_screen, modes[bestMode]);
		XF86VidModeSetViewPort(x11_display, x11_screen, 0, 0);
		int dpyWidth = modes[bestMode]->hdisplay;
		int dpyHeight = modes[bestMode]->vdisplay;
		XFree(modes);

		/* set window attributes */
		winAttr.override_redirect = True;
		winAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |  StructureNotifyMask;
		x11_window = XCreateWindow(x11_display, RootWindow(x11_display, vi->screen), 0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &winAttr);
		XWarpPointer(x11_display, None, x11_window, 0, 0, 0, 0, 0, 0);
		XMapRaised(x11_display, x11_window);
		XGrabKeyboard(x11_display, x11_window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
		XGrabPointer(x11_display, x11_window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, x11_window, None, CurrentTime);
	}
	else
	{
		/* create a window in window mode*/
		winAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
		x11_window = XCreateWindow(x11_display, RootWindow(x11_display, vi->screen), 0, 0, screenResolution.x, screenResolution.y, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &winAttr);
		/* only set window title and handle wm_delete_events if in windowed mode */
		Atom wmDelete = XInternAtom(x11_display, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(x11_display, x11_window, &wmDelete, 1);
		XSetStandardProperties(x11_display, x11_window, "FighterPilot", "FighterPilot", None, NULL, 0, NULL);
		XMapRaised(x11_display, x11_window);
	}
	free(modes);
	XFree(vi);
	/* connect the glx-context to the window */
	glXMakeCurrent(x11_display, x11_window, context->context);
	if(!glXIsDirect(x11_display, context->context))
	{
		//we don't have hardware acceleration!
		//TODO: display warning message about this issue 
		cout << "No hardware acceleration!!!" << endl;
	}
#endif

	glewExperimental = true; //force glew to attempt to get all function pointers (even for "unsupported" extensions)
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		destroyWindow();
		string s = string("Glew initialization failed with error: \"") + string((const char*)glewGetErrorString(err)) + "\"";
#ifdef WINDOWS
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
#endif
		return false;
	}
	else if(!GLEW_VERSION_2_1)
	{
		destroyWindow();
		const char* version = (const char*)glGetString(GL_VERSION);
		const char* renderer = (const char*)glGetString(GL_RENDERER);
		const char* vender = (const char*)glGetString(GL_VENDOR);

		string s("Your version of OpenGL must be at least 2.1");
		if(version) s += string("\n   OpenGL version: ") + version;
		if(renderer) s += string("\n   Renderer: ") + renderer;
		if(vender) s += string("\n   Vender: ") + vender;
#ifdef WINDOWS
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
#endif
		return false;
	}
	else if(!GL_EXT_framebuffer_object)
	{
		destroyWindow();
		const char* version = (const char*)glGetString(GL_VERSION);
		const char* renderer = (const char*)glGetString(GL_RENDERER);
		const char* vender = (const char*)glGetString(GL_VENDOR);

		string s("Your graphics card must support GL_EXT_framebuffer_object");
		if(version) s += string("\n   OpenGL version: ") + version;
		if(renderer) s += string("\n   Renderer: ") + renderer;
		if(vender) s += string("\n   Vender: ") + vender;
#ifdef WINDOWS
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
#endif
		return false;
	}

#if defined(WINDOWS)
	if(game->hasCommandLineOption("--opengl3") && wglCreateContextAttribsARB)
	{
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};
	
		auto gl3_hrc = wglCreateContextAttribsARB(context->hDC,0, attribs);
		if(gl3_hrc)
		{
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(context->hRC);
			context->hRC = gl3_hrc;
			wglMakeCurrent(context->hDC, context->hRC);
			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			gl2Hacks = false; //we won't be needing any hacks to get FighterPilot to work on old hardware...
			openGL3 = true;
		}
		else
		{
			MessageBoxA(NULL, "OpenGL 3 unsupported. Reverting to OpenGL 2.1","",MB_OK);
		}
	}
	else if(game->hasCommandLineOption("--opengl3"))
	{
		MessageBoxA(NULL, "OpenGL 3 unsupported. Reverting to OpenGL 2.1","",MB_OK);
	}

	ShowWindow(context->hWnd,SW_SHOW);				// Show The Window
	SetForegroundWindow(context->hWnd);				// Slightly Higher Priority
	SetFocus(context->hWnd);						// Sets Keyboard Focus To The Window
	RegisterHotKey(context->hWnd,IDHOT_SNAPWINDOW,0,VK_SNAPSHOT);
	RegisterHotKey(context->hWnd,IDHOT_SNAPDESKTOP,0,VK_SNAPSHOT);
#elif defined(LINUX)
	//GLuint vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);
	//gl2Hacks = false; //we won't be needing any hacks to get FighterPilot to work on old hardware...
	//openGL3 = true;	
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if(gl2Hacks)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	if (!initFBOs(maxSamples))
	{
		destroyWindow();
		return false;
	}

	overlayVBO = genVertexBuffer(vertexBuffer::STREAM);
	overlayVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION2,	0);
	overlayVBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		2*sizeof(float));
	overlayVBO->setTotalVertexSize(sizeof(texturedVertex2D));

	shapesVBO = genVertexBuffer(vertexBuffer::STREAM);
	shapesVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3, 0);
	shapesVBO->setTotalVertexSize(sizeof(vertex3D));

	return true;
}
bool OpenGLgraphics::changeResolution(Vec2i resolution, unsigned int maxSamples) // may invalidate OpenGL function pointers?
{
#if defined(WINDOWS)
	DEVMODE dmScreenSettings;										// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize			= sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= resolution.x;					// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= resolution.y;					// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= 32;							// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
	{
		SetWindowPos(context->hWnd,0,0,0,resolution.x,resolution.y,0);
		resize(resolution.x,resolution.y);
		destroyFBOs();
		initFBOs(maxSamples);
		resize(resolution.x, resolution.y);
		return true;
	}
#elif defined(LINUX)
	//TODO: add window resize code
#endif
	return false;
}
set<Vec2u> OpenGLgraphics::getSupportedResolutions()
{
	set<Vec2u> s;
#if defined(WINDOWS)
	int i=0;
	DEVMODE d;

	d.dmSize = sizeof(DEVMODE);
	d.dmDriverExtra = 0;

	while(EnumDisplaySettings(nullptr, i++, &d))
	{
		s.insert(Vec2u(d.dmPelsWidth, d.dmPelsHeight));
	}

#elif defined(LINUX)
	XF86VidModeModeInfo **modes;
	int numModes;
	XF86VidModeGetAllModeLines(x11_display, x11_screen, &numModes, &modes);

	for(int i = 0; i < numModes; i++)
	{
		s.insert(Vec2u(modes[i]->hdisplay, modes[i]->vdisplay));
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

void OpenGLgraphics::takeScreenshot()
{
	world.time.pause();
#ifdef WINDOWS
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);

	string filename = "screen shots/FighterPilot ";
	filename += lexical_cast<string>(sTime.wYear) + "-" + lexical_cast<string>(sTime.wMonth) + "-" +
				lexical_cast<string>(sTime.wDay) + " " + lexical_cast<string>(sTime.wHour+1) + "-" +
				lexical_cast<string>(sTime.wMinute) + "-" + lexical_cast<string>(sTime.wSecond) + "-" +
				lexical_cast<string>(sTime.wMilliseconds) + ".png";
#else
	string filename = "screen shots/FighterPilot.bmp";
#endif



	fileManager.createDirectory("screen shots");
	highResScreenshot = true;
	const int TILES=2;

	shared_ptr<FileManager::textureFile> file(new FileManager::pngFile(filename));
	file->channels = 3;
	file->width = sw*TILES;
	file->height = sh*TILES;
	file->contents = new unsigned char[3*sw*TILES*sh*TILES];

	memset(file->contents, 0, 3*sw*TILES*sh*TILES);

	unsigned char* tileContents = new unsigned char[3*sw*sh];

	for(int x=0; x<TILES; x++)
	{
		for(int y=0; y<TILES; y++)
		{
			viewConstraint = Rect::XYWH(1.0/TILES*x, 1.0/TILES*y, 1.0/TILES, 1.0/TILES);

			world.frameUpdate();
			render();


			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, sw, sh, GL_RGB, GL_UNSIGNED_BYTE, tileContents);

			for(int row=0;row<sh;row++)
			{
				memcpy(file->contents + (y*sw*sh*TILES + x*sw + row*sw*TILES)*3, tileContents+row*sw*3, 3*sw);
			}
		}
	}

	delete[] tileContents;

	fileManager.writePngFile(file,false);
	viewConstraint = Rect::XYXY(0,0,1,1);
	highResScreenshot = false;
	world.time.unpause();
}
void OpenGLgraphics::drawSphere(Vec3f position, float radius)
{
	drawModel("sphere", position, Quat4f(), Vec3f(radius, radius, radius));
}
void OpenGLgraphics::drawLine(Vec3f start, Vec3f end)
{
	shapes3D[0].position = start;
	shapes3D[1].position = end;

	shapesVBO->setVertexData(2*sizeof(vertex3D), shapes3D);
	shapesVBO->drawBuffer(LINES,0,2);
}
void OpenGLgraphics::drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3)
{
	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;

	shapesVBO->setVertexData(3*sizeof(vertex3D), shapes3D);
	shapesVBO->drawBuffer(TRIANGLES,0,3);
}
void OpenGLgraphics::drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4)
{
	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;
	shapes3D[3].position = p4;

	shapesVBO->setVertexData(4*sizeof(vertex3D), shapes3D);
	shapesVBO->drawBuffer(TRIANGLE_FAN,0,4);
}
void OpenGLgraphics::drawModel(string model, Vec3f position, Quat4f rotation, Vec3f scale)
{
	if(!currentView)
		return;

	auto m = dataManager.getModel(model);

	float s;
	if(abs(scale.x) >= abs(scale.y) && abs(scale.x) >= abs(scale.z))
		s = scale.x;
	else if(abs(scale.y) >= abs(scale.z))
		s = scale.y;
	else
		s = scale.z;

	if(m == nullptr || !currentView->sphereInFrustum(m->boundingSphere * s + position))
		return;

	dataManager.bind("white");
	dataManager.bind("model");
	dataManager.setUniform1i("tex",0);
	dataManager.setUniform3f("lightPosition", getLightPosition());
	dataManager.setUniformMatrix("modelTransform", Mat4f(rotation, position, scale));

	bool dMask = true;
	for(auto material = m->materials.begin(); material!=m->materials.end(); material++)
	{
		dataManager.bind(material->tex == "" ? "white" : material->tex);

		dataManager.setUniform4f("diffuse", material->diffuse);
		dataManager.setUniform3f("specular", material->specular);
		dataManager.setUniform1f("hardness", material->hardness);

		if(material->diffuse.a > 0.999 && !dMask)
		{
			dMask = true;
			glDepthMask(true);
		}
		else if(material->diffuse.a <= 0.999 && dMask)
		{
			dMask = false;
			glDepthMask(false);
		}
		material->indexBuffer->drawBuffer(GraphicsManager::TRIANGLES, m->VBO);
	}

	if(!dMask)	glDepthMask(true);

	dataManager.setUniform4f("color", 1,1,1,1);
}
void OpenGLgraphics::drawModelCustomShader(string model, Vec3f position, Quat4f rotation, Vec3f scale)
{
	if(!currentView)
		return;

	auto m = dataManager.getModel(model);
	float s;
	if(abs(scale.x) >= abs(scale.y) && abs(scale.x) >= abs(scale.z))
		s = scale.x;
	else if(abs(scale.y) >= abs(scale.z))
		s = scale.y;
	else
		s = scale.z;
	if(m == nullptr || !currentView->sphereInFrustum(m->boundingSphere * s + position))
		return;
	Mat4f cameraProjectionMat = currentView->projectionMatrix() * currentView->modelViewMatrix();
	dataManager.setUniformMatrix("cameraProjection",cameraProjectionMat);

	dataManager.setUniformMatrix("modelTransform", Mat4f(rotation, position, scale));

	bool dMask = true;
	for(auto material = m->materials.begin(); material!=m->materials.end(); material++)
	{
		dataManager.setUniform4f("diffuse", material->diffuse);
		dataManager.setUniform3f("specular", material->specular);
		dataManager.setUniform1f("hardness", material->hardness);

		if(material->diffuse.a > 0.999 && !dMask)
		{
			dMask = true;
			setDepthMask(true);
		}
		else if(material->diffuse.a <= 0.999 && dMask)
		{
			dMask = false;
			setDepthMask(false);
		}
		material->indexBuffer->drawBuffer(GraphicsManager::TRIANGLES, m->VBO);
	}
	if(!dMask)	glDepthMask(true);
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

#endif /*OPENGL*/
