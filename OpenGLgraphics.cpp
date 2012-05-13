
#include "engine.h"
#include <Windows.h>
#include "GL/glee.h"
#include <GL/glu.h>
#include "png/png.h"


struct OpenGLgraphics::Context
{
	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance; // not initialized?
	Context(): hDC(NULL),hRC(NULL),hWnd(NULL){}
};
OpenGLgraphics::vertexBufferGL::vertexBufferGL(UsageFrequency u): vertexBuffer(u), vBufferID(0)
{
	glGenBuffers(1, &vBufferID);
}
OpenGLgraphics::vertexBufferGL::~vertexBufferGL()
{
	glBindBuffer(GL_ARRAY_BUFFER, vBufferID);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBufferID);
}
void OpenGLgraphics::vertexBufferGL::setVertexData(unsigned int size, void* data)
{
	vertexBuffer::bindBuffer();
	if(usageFrequency == STATIC)		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
}
void OpenGLgraphics::vertexBufferGL::bindBuffer(unsigned int offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, vBufferID);

	dynamic_cast<OpenGLgraphics*>(graphics)->setClientStates(texCoordDataSize != 0, normalDataSize == 3, colorDataSize != 0);

	if(positionDataSize != 0)
	{
		glVertexPointer(positionDataSize, GL_FLOAT, totalVertexSize, (void*)(positionDataOffset+offset));
	}
	if(texCoordDataSize != 0)
	{
		glTexCoordPointer(texCoordDataSize, GL_FLOAT, totalVertexSize, (void*)(texCoordDataOffset+offset));
	}
	if(normalDataSize == 3)
	{
		glNormalPointer(GL_FLOAT, totalVertexSize, (void*)(normalDataOffset+offset));
	}
	if(colorDataSize != 0)
	{
		glColorPointer(colorDataSize, GL_FLOAT, totalVertexSize, (void*)(colorDataOffset+offset));
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
}
void OpenGLgraphics::indexBufferGL::setData(unsigned short* data, unsigned int count)
{
	dataType = USHORT;
	dataCount = count;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	if(usageFrequency == STATIC)		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*2, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*2, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*2, data, GL_STREAM_DRAW);
}
void OpenGLgraphics::indexBufferGL::setData(unsigned int* data, unsigned int count)
{
	dataType = UINT;
	dataCount = count;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	if(usageFrequency == STATIC)		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_STATIC_DRAW);
	else if(usageFrequency == DYNAMIC)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_DYNAMIC_DRAW);
	else if(usageFrequency == STREAM)	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*4, data, GL_STREAM_DRAW);
}
void OpenGLgraphics::indexBufferGL::drawBuffer(Primitive primitive, shared_ptr<vertexBuffer> buffer, unsigned int offset)
{
	if(dataCount == 0 || dataType == NO_TYPE)
		return;

	GLenum type;
	if(dataType == UCHAR) type = GL_UNSIGNED_BYTE;
	else if(dataType == USHORT) type = GL_UNSIGNED_SHORT;
	else if(dataType == UINT) type = GL_UNSIGNED_INT;

	bindVertexBuffer(buffer, offset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);

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
void OpenGLgraphics::texture2DGL::setData(unsigned int Width, unsigned int Height, Format f, unsigned char* data, bool tileable)
{	graphics->checkErrors();
	if(f != LUMINANCE && f != LUMINANCE_ALPHA && f != RGB && f != RGBA)
	{
		debugBreak();
		return;
	}

	format = f;
	width = Width;
	height = Height;
	
	//extention check needed since GeForce FX graphics cards do not support NPOT textures (even though they support OpenGL 2.0)
	bool NPOT = GLEE_ARB_texture_non_power_of_two && ((width & (width-1)) || (height & (height-1)));
	
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
	
	if(GLEE_ARB_texture_non_power_of_two || (!(width & (width-1)) && !(height & (height-1)))) //if we have support for NPOT textures, or texture is power of 2
	{
		if(format == LUMINANCE)				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		else if(format == LUMINANCE_ALPHA)	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		else if(format == RGB)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(format == RGBA)				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		glEnable(GL_TEXTURE_2D); //required for some ATI drivers
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else // we need to resize to a power of 2 (should only occur on GeForce FX graphics cards)
	{
		if(format == LUMINANCE)				gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		else if(format == LUMINANCE_ALPHA)	gluBuild2DMipmaps(GL_TEXTURE_2D, 2, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		else if(format == RGB)				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(format == RGBA)				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);	
	}
	graphics->checkErrors();
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
	if(f != LUMINANCE && f != LUMINANCE_ALPHA && f != RGB && f != RGBA)
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

	glGenerateMipmap(GL_TEXTURE_3D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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
	glBindTexture(GL_TEXTURE_3D, 0);
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
	if(f != LUMINANCE && f != LUMINANCE_ALPHA && f != RGB && f != RGBA)
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
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*4 * 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*4 * 1);	
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*4 * 2);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*4 * 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*4 * 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data + width*height*4 * 5);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

	glShaderSource(v, 1, &vert, NULL);
	glShaderSource(f, 1, &frag, NULL);

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

	//glGetShaderiv(v,GL_INFO_LOG_LENGTH,&i);
	//glGetShaderiv(f,GL_INFO_LOG_LENGTH,&i);


	shaderId = glCreateProgram();
	glAttachShader(shaderId,f);
	glAttachShader(shaderId,v);
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
	if(vertErrorLog != "" && fragErrorLog != "")
	{
		return string("vert:") + vertErrorLog + "frag:" + fragErrorLog;
	}
	else if(vertErrorLog != "")
	{
		return string("vert:") + vertErrorLog;
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
	ShowWindow(context->hWnd, SW_MINIMIZE);
}
OpenGLgraphics::OpenGLgraphics():multisampling(false),samples(0),renderTarget(RT_SCREEN), colorMask(true), depthMask(true), texCoord_clientState(false), normal_clientState(false), color_clientState(false)
{
#ifdef _DEBUG
	errorGlowEndTime = 0;
#endif

	context = new Context;
	//useAnagricStereo(true);
	//setInterOcularDistance(0.75);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	setClientStates(true,false,false);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//if(tex != "") dataManager.unbind(tex);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	setClientStates(true,false,false);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//if(tex != "") dataManager.unbind(tex);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	setClientStates(true,false,false);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//if(tex != "") dataManager.unbind(tex);
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
	string boundShader = dataManager.getBoundShader();
	if(boundShader == "ortho")
		dataManager.setUniform4f("color", r, g, b, a);
	else
		glColor4f(r,g,b,a);
}
void OpenGLgraphics::setColorMask(bool mask)
{
	if(renderTarget == RT_SCREEN && mask != colorMask)
	{
		colorMask = mask;
		glColorMask(mask, mask, mask, mask);
	}
	else if(renderTarget == RT_FBO_0 && mask != FBOs[0].colorBound)
	{
		FBOs[0].colorBound = mask;
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? FBOs[0].color : 0, 0);
	}
	else if(renderTarget == RT_FBO_1 && mask != FBOs[1].colorBound)
	{
		FBOs[1].colorBound = mask;
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? FBOs[1].color : 0, 0);		
	}
	else if(renderTarget == RT_MULTISAMPLE_FBO && mask != multisampleFBO.colorBound)
	{
		multisampleFBO.colorBound = mask;
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? multisampleFBO.color : 0, 0);
	}
}
void OpenGLgraphics::setDepthMask(bool mask)
{
	if(renderTarget == RT_SCREEN && mask != depthMask)
	{
		depthMask = mask;
		glDepthMask(depthMask);
	}
	else if(renderTarget == RT_FBO_0 && mask != FBOs[0].depthBound)
	{
		FBOs[0].depthBound = mask;
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? FBOs[0].depth : 0, 0);
	}
	else if(renderTarget == RT_FBO_1 && mask != FBOs[1].depthBound)
	{
		FBOs[1].depthBound = mask;
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? FBOs[1].depth : 0, 0);		
	}
	else if(renderTarget == RT_MULTISAMPLE_FBO && mask != multisampleFBO.depthBound)
	{
		multisampleFBO.depthBound = mask;
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mask ? multisampleFBO.depth : 0, 0);
	}
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
	if(blend == TRANSPARENCY)		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	else if(blend == ADDITIVE)		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	else if(blend == ALPHA_ONLY)	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
}
void OpenGLgraphics::setClientStates(bool texCoord, bool normal, bool color)
{
	if(!texCoord_clientState && texCoord)  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if(texCoord_clientState && !texCoord)  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	texCoord_clientState = texCoord;

	if(!normal_clientState && normal)  glEnableClientState(GL_NORMAL_ARRAY);
	if(normal_clientState && !normal)  glDisableClientState(GL_NORMAL_ARRAY);
	normal_clientState = normal;

	if(!color_clientState && color)  glEnableClientState(GL_COLOR_ARRAY);
	if(color_clientState && !color)  glDisableClientState(GL_COLOR_ARRAY);
	color_clientState = color;
}
void OpenGLgraphics::setVSync(bool enabled)
{
	if(wglSwapIntervalEXT)
		wglSwapIntervalEXT(enabled ? 1 : 0);//turn on/off vsync (0 = off and 1 = on)
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
	if(t == RT_FBO_0)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[0].fboID);
	}
	else if(t == RT_FBO_1)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[1].fboID);
	}
	else if(t == RT_MULTISAMPLE_FBO)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFBO.fboID);
	}
	else if(t == RT_SCREEN)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
}
void OpenGLgraphics::renderFBO(RenderTarget src) //right now can only be RT_FBO
{
	glActiveTexture(GL_TEXTURE0);
	if(src == RT_FBO_0)			glBindTexture(GL_TEXTURE_2D, FBOs[0].fboID);
	else if(src == RT_FBO_1)	glBindTexture(GL_TEXTURE_2D, FBOs[1].fboID);
	else return;

	glViewport(0,0,sw,sh);
	drawOverlay(Rect::XYXY(-1,-1,1,1));
}
bool OpenGLgraphics::initFBOs(unsigned int maxSamples)
{
	//////////////////////////////////////////////////////////////////////////////////////
	auto checkForFBOErrors = []()-> bool
	{
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status == GL_FRAMEBUFFER_COMPLETE_EXT)							return true;
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)			MessageBoxA(NULL, "Framebuffer incomplete: Attachment is NOT complete.","ERROR",MB_OK);
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)	MessageBoxA(NULL, "Framebuffer incomplete: No image is attached to FBO.","ERROR",MB_OK);
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)			MessageBoxA(NULL, "Framebuffer incomplete: Attached images have different dimensions.","ERROR",MB_OK);
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)			MessageBoxA(NULL, "Framebuffer incomplete: Color attached images have different internal formats.","ERROR",MB_OK);
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Draw buffer.","ERROR",MB_OK);
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)		MessageBoxA(NULL, "Framebuffer incomplete: Read buffer.","ERROR",MB_OK);
		else if(status == GL_FRAMEBUFFER_UNSUPPORTED_EXT)					MessageBoxA(NULL, "Unsupported by FBO implementation.","ERROR",MB_OK);
		else 																MessageBoxA(NULL, "Unknow frame buffer error.","ERROR",MB_OK);
		return false;
	};
	/////////////////////////////////////////////////////////////////////////////////////
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
	samples = samples >= maxSamples ? maxSamples : samples;
	multisampling = GLEE_EXT_framebuffer_multisample && samples > 1;
	//////////////////////////////////////////////////////////////////////////////////////
	for(int i = 0; i < 2; i++)
	{
		glGenTextures(1, &FBOs[i].color);
		glBindTexture(GL_TEXTURE_2D, FBOs[i].color);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &FBOs[i].depth);
		glBindTexture(GL_TEXTURE_2D, FBOs[i].depth);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, sw, sh, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

		glGenFramebuffersEXT(1, &FBOs[i].fboID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i].fboID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, FBOs[i].color, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, FBOs[i].depth, 0);

		if(!checkForFBOErrors())
			return false;
	}
	//////////////////////////////////////////////////////////////////////////////////////
	if(multisampling)
	{
		glGenRenderbuffersEXT(1, &multisampleFBO.color);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, multisampleFBO.color);
		glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_RGBA/*GL_RGBA16F*/, sw, sh);

		glGenRenderbuffersEXT(1, &multisampleFBO.depth);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, multisampleFBO.depth);
		glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_DEPTH_COMPONENT, sw, sh);

		glGenFramebuffersEXT(1, &multisampleFBO.fboID);


		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisampleFBO.fboID);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, multisampleFBO.color);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, multisampleFBO.depth);
		//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		checkErrors();
		if(!checkForFBOErrors())
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			return false;
		}
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return true;
}
void OpenGLgraphics::destroyFBOs()
{
	if(FBOs[0].fboID != 0)		glDeleteFramebuffersEXT(1, &FBOs[0].fboID);
	if(FBOs[0].color != 0)		glDeleteTextures(1, &FBOs[0].color);
	if(FBOs[0].depth != 0)		glDeleteTextures(1, &FBOs[0].depth);

	if(FBOs[1].fboID != 0)		glDeleteFramebuffersEXT(1, &FBOs[1].fboID);
	if(FBOs[1].color != 0)		glDeleteTextures(1, &FBOs[1].color);
	if(FBOs[1].depth != 0)		glDeleteTextures(1, &FBOs[1].depth);

	if(multisampleFBO.fboID != 0)	glDeleteFramebuffersEXT(1, &multisampleFBO.fboID);
	if(multisampleFBO.color != 0)	glDeleteRenderbuffers(1, &multisampleFBO.color);
	if(multisampleFBO.depth != 0)	glDeleteRenderbuffers(1, &multisampleFBO.depth);
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

///////////////////////////////////CLEAR BUFFERS/////////////////////////////////
	glClearColor(0.47f,0.57f,0.63f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

//////////////////////////////////SET LIGHTING///////////////////////////////////
	float lightPos[4] = {lightPosition.x, lightPosition.y, lightPosition.z, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
////////////////////////////////////START 3D/////////////////////////////////////

	/*if(stereo)
	{
		glViewport(0, 0, sw, sh);				// stereo disabled for now
	
		leftEye = true;
		glColorMask(true,false,false,true);
		menuManager.render3D(0); // only the first view is drawn with stereo rendering
	
		glClear(GL_DEPTH_BUFFER_BIT);
	
		leftEye = false;
		glColorMask(false,true,true,true);
		menuManager.render3D(0);
	
		glColorMask(true,true,true,true);
	}
	else*/
	{
		for(auto i = views_new.begin(); i != views_new.end();)
		{
			if(i->expired()) //if the view no longer exists
			{
				i = views_new.erase(i);
			}
			else
			{
				dataManager.bind("model");
				currentView = shared_ptr<View>(*i);
				glViewport(currentView->viewport().x * sh, currentView->viewport().y * sh, currentView->viewport().width * sh, currentView->viewport().height * sh);

				dataManager.setUniformMatrix("cameraProjection",currentView->projectionMatrix() * currentView->modelViewMatrix());
				dataManager.setUniformMatrix("modelTransform", Mat4f());

				currentView->render();

				i++;
			}
		}
		currentView.reset();
	}
///////////////////////////////////START PARTICLES///////////////////////

	if(multisampling)
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
	
	for(auto i = views_new.begin(); i != views_new.end();)
	{
		if(i->expired()) //check if the view no longer exists (just to be safe)
		{
			i = views_new.erase(i);
		}
		else
		{
			currentView = shared_ptr<View>(*i);
			if(currentView->renderParticles())
			{
				glViewport(currentView->viewport().x * sh, currentView->viewport().y * sh, currentView->viewport().width * sh, currentView->viewport().height * sh);			

				particleManager.render(currentView);
			}
			i++;
		}
	}
	currentView.reset(); //set the current view to null

	sceneManager.endRender(); //do some post render cleanup

	if(!multisampling)
	{
		setDepthMask(true);
	}
/////////////////////////////////////START 2D////////////////////////////////////
	glViewport(0,0,sw,sh);
	//depth testing was already disabled for particles

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




///////////////////////////////////////Post Processing//////////////////////////////////
	if(multisampling)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, multisampleFBO.fboID);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, FBOs[0].fboID);
		glBlitFramebufferEXT(0, 0, sw, sh, 0, 0, sw, sh, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
	}

	bindRenderTarget(RT_SCREEN);
	glClearColor(0.0,0.0,0.0,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	dataManager.bind("gamma shader");
	dataManager.setUniform1f("gamma",currentGamma);
	dataManager.setUniform1i("tex",0);
	renderFBO(RT_FBO_0);
///////////////////////////////////////////////////////////////////////////////////////

	checkErrors();
}
//void OpenGLgraphics::viewport(int x,int y,int width,int height)
//{
//	GraphicsManager::viewport(x,y,width,height);
//	glViewport(x,y,width,height);
//}
//void OpenGLgraphics::perspective(float fovy, float aspect, float zNear, float zFar)
//{
//	GraphicsManager::perspective(fovy, aspect, zNear, zFar);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(fovy, aspect, zNear, zFar);
//}
//void OpenGLgraphics::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
//{
//	GraphicsManager::ortho(left, right, bottom, top, zNear, zFar);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(left, right, bottom, top, zNear, zFar);
//}
//void OpenGLgraphics::lookAt(Vec3f eye, Vec3f center, Vec3f up)
//{
//	if(stereo)
//	{
//		Vec3f f = (center - eye).normalize();
//		Vec3f r = (up.normalize()).cross(f);
//		if(leftEye)
//		{
//			eye += r * interOcularDistance * 0.5;
//			center += r * interOcularDistance * 0.5;
//		}
//		else
//		{
//			eye -= r * interOcularDistance * 0.5;
//			center -= r * interOcularDistance * 0.5;
//		}
//	}
//	GraphicsManager::lookAt(eye, center, up);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(eye.x,eye.y,eye.z,center.x,center.y,center.z,up.x,up.y,up.z);
//}
void OpenGLgraphics::destroyWindow()
{
	destroyFBOs();

	//glDeleteRenderbuffersEXT(2, depthRenderBuffers);

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
}
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool OpenGLgraphics::createWindow(string title, Vec2i screenResolution, unsigned int maxSamples)
{
	static bool arbMultisampleSupported=false;
	static int arbMultisampleFormat=0;
	bool fullscreenflag=true;

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

	if(!GLEE_VERSION_2_0)
	{
		destroyWindow();
		string s = string("Your version of OpenGL must be at least 2.0\n   OpenGL version: ") + (char*)glGetString(GL_VERSION) + "\n   Renderer: " + (char*)glGetString(GL_RENDERER) + "\n   Vender: " + (char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}
	else if(!GLEE_EXT_framebuffer_blit)
	{
		destroyWindow();
		string s = string("Your graphics card must support GL_EXT_framebuffer_blit\n   OpenGL version: ") + (char*)glGetString(GL_VERSION) + "\n   Renderer: " + (char*)glGetString(GL_RENDERER) + "\n   Vender: " + (char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}

	ShowWindow(context->hWnd,SW_SHOW);				// Show The Window
	SetForegroundWindow(context->hWnd);				// Slightly Higher Priority
	SetFocus(context->hWnd);						// Sets Keyboard Focus To The Window
	//graphics->resize(WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top);	// Set Up Our Perspective GL Screen

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	glDisable(GL_CULL_FACE);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE4_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0_ARB);	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);

	if (!initFBOs(maxSamples))
	{
		destroyWindow();
		return false;
	}

	RegisterHotKey(context->hWnd,IDHOT_SNAPWINDOW,0,VK_SNAPSHOT);
	RegisterHotKey(context->hWnd,IDHOT_SNAPDESKTOP,0,VK_SNAPSHOT);

	return true;
}
bool OpenGLgraphics::changeResolution(Vec2i resolution, unsigned int maxSamples)
{
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
	return false;
}
set<Vec2u> OpenGLgraphics::getSupportedResolutions()
{
	int i=0;
	DEVMODE d;
	set<Vec2u> s;

	d.dmSize = sizeof(DEVMODE);
	d.dmDriverExtra = 0;

	while(EnumDisplaySettings(nullptr, i, &d))
	{
		s.insert(Vec2u(d.dmPelsWidth, d.dmPelsHeight));
		i++;
	}
	return s;
}
void OpenGLgraphics::swapBuffers()
{
	SwapBuffers(context->hDC);
}
void OpenGLgraphics::takeScreenshot()
{
#ifdef WINDOWS
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);

	string filename = "screen shots/FighterPilot ";
	filename += lexical_cast<string>(sTime.wYear) + "-" + lexical_cast<string>(sTime.wMonth) + "-" + 
				lexical_cast<string>(sTime.wDay) + " " + lexical_cast<string>(sTime.wHour+1) + "-" + 
				lexical_cast<string>(sTime.wMinute) + "-" + lexical_cast<string>(sTime.wSecond) + "-" + 
				lexical_cast<string>(sTime.wMilliseconds) + ".png";
#elif
	string filename = "screen shots/FighterPilot.png";
#endif

	fileManager.createDirectory("screen shots");

	shared_ptr<FileManager::textureFile> file(new FileManager::pngFile(filename));

	file->channels = 3;
	file->width = sw;
	file->height = sh;
	file->contents = new unsigned char[3*sw*sh];

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, sw, sh, GL_RGB, GL_UNSIGNED_BYTE, file->contents);

	fileManager.writePngFile(file,true);
}
void OpenGLgraphics::drawSphere(Vec3f position, float radius)
{
	drawModel("sphere", position, Quat4f(), Vec3f(radius, radius, radius));
}
void OpenGLgraphics::drawLine(Vec3f start, Vec3f end)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	setClientStates(false,false,false);

	shapes3D[0].position = start;
	shapes3D[1].position = end;
	glVertexPointer(3, GL_FLOAT, sizeof(vertex3D), &shapes3D[0].position.x);
	glDrawArrays(GL_LINES, 0, 2);
}
void OpenGLgraphics::drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	setClientStates(false,false,false);

 	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;

	glVertexPointer(3, GL_FLOAT, sizeof(vertex3D), &shapes3D[0].position.x);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
void OpenGLgraphics::drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	setClientStates(false,false,false);

	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;
	shapes3D[3].position = p4;

	glVertexPointer(3, GL_FLOAT, sizeof(vertex3D), &shapes3D[0].position.x);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

	//Mat4f matV = views[currentView].modelViewMat  * Mat4f(rotation,position,scale);
	//Mat4f matP = views[currentView].projectionMat;
	//Mat4f mat =  matP * matV;
	//Mat4f matV2;	glGetFloatv(GL_MODELVIEW_MATRIX,matV2.ptr());
	//Mat4f matP2;	glGetFloatv(GL_PROJECTION_MATRIX,matP2.ptr());
	//dataManager.setUniformMatrix("modelviewProjection",mat);
	Mat4f cameraProjectionMat = currentView->projectionMatrix() * currentView->modelViewMatrix();
	dataManager.setUniformMatrix("cameraProjection",cameraProjectionMat);

	dataManager.setUniformMatrix("modelTransform", Mat4f(rotation, position, scale));

	//glBindBuffer(GL_ARRAY_BUFFER, m->id);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glVertexPointer(3,	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)0);
	//glNormalPointer(	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(3*sizeof(float)));
	//glTexCoordPointer(2,GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(6*sizeof(float)));

	bool dMask = true;

	for(auto i = m->materials.begin(); i!=m->materials.end(); i++)
	{
		dataManager.bind(i->tex == "" ? "white" : i->tex);
		glColor4f(i->color.r,i->color.g,i->color.b, i->color.a);

		if(i->color.a > 0.999 && !dMask)
		{
			dMask = true;
			glDepthMask(true);
		}
		else if(i->color.a <= 0.999 && dMask)
		{
			dMask = false;
			glDepthMask(false);
		}
		m->VBO->drawBuffer(TRIANGLES, i->indicesOffset, i->numIndices);
		//glDrawArrays(GL_TRIANGLES,i->indicesOffset, i->numIndices);
	}

	if(!dMask)	glDepthMask(true);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glColor3f(1,1,1);
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
	//glBindBuffer(GL_ARRAY_BUFFER, m->id);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glVertexPointer(3,	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)0);
	//glNormalPointer(	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(3*sizeof(float)));
	//glTexCoordPointer(2,GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(6*sizeof(float)));

	bool dMask = true;

	for(auto i = m->materials.begin(); i!=m->materials.end(); i++)
	{
		glColor4f(i->color.r,i->color.g,i->color.b, i->color.a);

		if(i->color.a > 0.999 && !dMask)
		{
			dMask = true;
			glDepthMask(true);
		}
		else if(i->color.a <= 0.999 && dMask)
		{
			dMask = false;
			glDepthMask(false);
		}
		m->VBO->drawBuffer(TRIANGLES, i->indicesOffset, i->numIndices);checkErrors();
		//glDrawArrays(GL_TRIANGLES,i->indicesOffset, i->numIndices);
	}
	if(!dMask)	glDepthMask(true);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glColor3f(1,1,1);
}
void OpenGLgraphics::checkErrors()
{
#ifdef _DEBUG
	GLenum error = glGetError();
	const char* errorString = (const char*)gluErrorString(error);
	if(strcmp(errorString, "no error") != 0)
	{
		//debugBreak();
		errorGlowEndTime = GetTime() + 1000.0;
	}
#endif
}