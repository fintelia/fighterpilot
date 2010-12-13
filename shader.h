
class shader
{
protected:
	virtual void setInput1f(string name,float v0)=0;
	virtual void setInput2f(string name,float v0,float v1)=0;
	virtual void setInput3f(string name,float v0,float v1,float v2)=0;
	virtual void setInput4f(string name,float v0,float v1,float v2,float v3)=0;
	void setInput2f(string name,Vec2f v){setInput2f(name,v.x,v.y);}
	void setInput3f(string name,Vec3f v){setInput3f(name,v.x,v.y,v.z);}
	void setInput4f(string name,Quat4f q){setInput4f(name,q.x,q.y,q.z,q.w);}

	virtual void useShader()=0;

public:
	virtual void setValue1f(string name,float v0){}
	virtual void setValue2f(string name,float v0,float v1){}
	virtual void setValue3f(string name,float v0,float v1,float v2){}
	virtual void setValue4f(string name,float v0,float v1,float v2,float v3){}
	void setValue2f(string name,Vec2f v){setValue2f(name,v.x,v.y);}
	void setValue3f(string name,Vec3f v){setValue3f(name,v.x,v.y,v.z);}
	void setValue4f(string name,Quat4f q){setValue4f(name,q.x,q.y,q.z,q.w);}

	virtual void loadString(char* vert, char* frag)=0;
	virtual void loadFile(string vertFile, string fragFile)=0;


	virtual void bind()=0;
	virtual void unbind()=0;
};
class shaderGL: public virtual shader
{
protected:
	int id;
	virtual void setInput1f(string name,float v0);
	virtual void setInput2f(string name,float v0,float v1);
	virtual void setInput3f(string name,float v0,float v1,float v2);
	virtual void setInput4f(string name,float v0,float v1,float v2,float v3);
	void useShader(){glUseProgram(id);}

public:
	void loadString(char* vert, char* frag);
	void loadFile(string vertFile, string fragFile);
	void unbind(){glUseProgram(0);}

};

class shaderRadar: public virtual shader
{
public:
	void bind();
};
class shaderRadarGL: public shaderRadar, public shaderGL
{
	shaderRadarGL(string vert, string frag){loadFile(vert, frag);}
	shaderRadarGL(){}
};

class shaderHealth: public virtual shader
{
protected:
	float health;
public:
	void bind();
	void setValue1f(string name,float v0);
};
class shaderHealthGL: public shaderHealth, public shaderGL
{
	shaderHealthGL(string vert, string frag){loadFile(vert, frag);}
	shaderHealthGL(){}
};
//
//extern shader* hShader;