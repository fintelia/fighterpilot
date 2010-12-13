
#include "main.h"
void shaderGL::setInput1f(string name,float v0)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), v0);
}
void shaderGL::setInput2f(string name,float v0,float v1)
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), v0, v1);
}
void shaderGL::setInput3f(string name,float v0,float v1,float v2)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), v0, v1, v2);
}
void shaderGL::setInput4f(string name,float v0,float v1,float v2,float v3)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), v0, v1, v2, v3);
}

void shaderGL::loadFile(string vertFile, string fragFile)
{
	char *vert, *frag;

	ifstream file (vertFile, ios::in|ios::ate);
	if (file.is_open())
	{
		int size = file.tellg();
		vert = new char [size];
		memset(vert,0,size);
		file.seekg (0, ios::beg);
		file.read (vert, size);
		file.close();
	}
	file.open(fragFile, ios::in|ios::ate);
	if (file.is_open())
	{
		int size = file.tellg();
		frag = new char [size];
		memset(frag,0,size);
		file.seekg(0, ios::beg);
		file.read(frag, size);
		file.close();
	}

	GLuint v=0,f=0;
	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	glShaderSource(v, 1, (const char **)&vert, NULL);
	glShaderSource(f, 1, (const char **)&frag, NULL);
	delete[] vert;
	delete[] frag;

	glCompileShader(v);
	glCompileShader(f);

	char* cv=(char*)malloc(512); memset(cv,0,512);
	char* cf=(char*)malloc(512); memset(cf,0,512);
	int lv,lf;
	glGetShaderInfoLog(v,512,&lv,cv);
	glGetShaderInfoLog(f,512,&lf,cf);

	id = glCreateProgram();
	glAttachShader(id,f);
	glAttachShader(id,v);

	glLinkProgram(id);
	glUseProgram(0); 

	if(lv != 0)		messageBox(vertFile+": "+string(cv));
	if(lf != 0) 	messageBox(fragFile+": "+string(cf));
}
void shaderGL::loadString(char* vert, char* frag)
{
	GLuint v=0,f=0;
	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	glShaderSource(v, 1, (const char **)&vert, NULL);
	glShaderSource(f, 1, (const char **)&frag, NULL);
	glCompileShader(v);
	glCompileShader(f);

	char* cv=(char*)malloc(512); memset(cv,0,512);
	char* cf=(char*)malloc(512); memset(cf,0,512);
	int lv,lf;
	glGetShaderInfoLog(v,512,&lv,cv);
	glGetShaderInfoLog(f,512,&lf,cf);

	id = glCreateProgram();
	glAttachShader(id,f);
	glAttachShader(id,v);

	glLinkProgram(id);
	glUseProgram(0); 

	if(lv != 0)		messageBox(": "+string(cv));
	if(lf != 0) 	messageBox(": "+string(cf));
}

/////////////////////////////RADAR//////////////////////////
void shaderRadar::bind()
{
	useShader();
	setInput1f("radarAng",radarAng);
}
////////////////////////////HEATH BAR//////////////////////////
void shaderHealth::bind()
{
	bind();
	setInput1f("health",health);
	setInput1f("angle",1.24f);
}
void shaderHealth::setValue1f(string name,float v0)
{
	if(name.compare("health") == 0)
	{
		health = v0;
	}
}

//shader* hShader = (shader*)new shaderHealthGL;