
class mapBuilder: public modes
{
private:
	Angle rot;
	editLevel* level;
	//int currentShader;
	float maxHeight;
	float minHeight;

	float getHeight(int x, int z)
	{
		//while(x>=(int)level->ground()->getSize()-1)
		//	x -= (int)level->ground()->getSize()-1;
		//while(z>=(int)level->ground()->getSize()-1)
		//	z -= (int)level->ground()->getSize()-1;
		//while(z<0)
		//	z += (int)level->ground()->getSize()-1;
		//while(x<0)
		//	x += (int)level->ground()->getSize()-1;

		return level->ground()->getHeight(x,z);
	}
	void diamondSquare(float h)
	{
		//set corners

		int x, z, numSquares, squareSize;

		float c[4];

		h=pow(2.0f,-h);
		float rVal=10*(1.0-h), y;
		

		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				level->ground()->setHeight(x,0,y);
			}
		}

		level->ground()->setHeight(0,0,0);
		level->ground()->setHeight(0,0,level->ground()->getSize()-1);
		level->ground()->setHeight(level->ground()->getSize()-1,0,level->ground()->getSize()-1);
		level->ground()->setHeight(level->ground()->getSize()-1,0,0);

		for(int itt=0; (0x1 << itt) < (level->ground()->getSize()-1);itt++, rVal*=1.0-h)
		{
			numSquares = 0x1 << itt;
			squareSize = (level->ground()->getSize()-1)/numSquares;
			//diamond
			for(x = 0; x < numSquares; x++)
			{
				for(z = 0; z < numSquares; z++)
				{
					y = (getHeight(x*squareSize,z*squareSize)+
						getHeight(x*squareSize+squareSize,z*squareSize)+
						getHeight(x*squareSize+squareSize,z*squareSize+squareSize)+
						getHeight(x*squareSize,z*squareSize+squareSize))/4;
					y += float(rand()%2000-1000)/500.0f*rVal;

					level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize+squareSize/2);
				}
			}
			//square
			for(x = 0; x < numSquares; x++)
			{
				for(z = 0; z < numSquares; z++)
				{
					//left
					c[0]=getHeight(x*squareSize,z*squareSize);
					c[1]=getHeight(x*squareSize,z*squareSize+squareSize);
					c[2]=getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					c[3]=getHeight(x*squareSize-squareSize/2,z*squareSize+squareSize/2);
					if(x==0)			y=(c[0]+c[1]+c[2])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
					else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
					level->ground()->setHeight(x*squareSize,y,z*squareSize+squareSize/2);


					//top
					c[0]=getHeight(x*squareSize,z*squareSize);
					c[1]=getHeight(x*squareSize+squareSize,z*squareSize);
					c[2]=getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					c[3]=getHeight(x*squareSize+squareSize/2,z*squareSize-squareSize/2);
					if(z==0)			y=(c[0]+c[1]+c[2])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
					else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
					level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize);

					if(x == numSquares-1)//right
					{
						c[0]=getHeight((x+1)*squareSize,z*squareSize);
						c[1]=getHeight((x+1)*squareSize,z*squareSize+squareSize);
						//c[2]=getHeight((x+1)*squareSize+squareSize/2,z*squareSize+squareSize/2);
						c[3]=getHeight((x+1)*squareSize-squareSize/2,z*squareSize+squareSize/2);
						y=(c[0]+c[1]+c[3])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
						//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
						level->ground()->setHeight(x*squareSize,y,z*squareSize+squareSize/2);
					}
					if(z == numSquares-1)//buttom
					{
						c[0]=getHeight(x*squareSize,z*squareSize);
						c[1]=getHeight(x*squareSize+squareSize,z*squareSize);
						//c[2]=getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
						c[3]=getHeight(x*squareSize+squareSize/2,z*squareSize-squareSize/2);
						y=(c[0]+c[1]+c[3])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
						//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
						level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize);
					}
				}
			}
			//rVal*=h;
		}
		//for(x = 0; x < level->ground()->getSize(); x++)
		//	level->ground()->setHeight(x,getHeight(x,0),level->ground()->getSize()-2);
		//for(z = 0; z < level->ground()->getSize(); z++)
		//	level->ground()->setHeight(level->ground()->getSize()-2,getHeight(x,z),z);
		maxHeight=-9999;
		minHeight=9999;
		float h;
		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				h = level->ground()->getHeight(x,y);
				if(h>maxHeight) maxHeight=h;
				if(h<minHeight) minHeight=h;
			}
		}
	}
	void faultLine()
	{
		float pd;
		float a,b,c;
		float disp=0.5;
		float x,y;
		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				level->ground()->setHeight(x,0,y);
			}
		}
		for(int i=0;i<100;i++)
		{
			a = float(rand()%1000)/500.0-1.0;
			b = float(rand()%1000)/500.0-1.0;
			c = rand()%level->ground()->getSize()-level->ground()->getSize()/2;
			for(x=0;x<level->ground()->getSize();x++)
			{
				for(y=0;y<level->ground()->getSize();y++)
				{
					pd = ((x-level->ground()->getSize()/2) * a + (y-level->ground()->getSize()/2) * b + c)/(128.0/30)/2;//  ***/2500
					if (pd > 1.57) pd = 1.57;
					else if (pd < 0.05) pd = 0.05;
					level->ground()->increaseHeight(x,-disp/2 + sin(pd)*disp,y);
				}
			}		
		}
		maxHeight=-9999;
		minHeight=9999;
		float h;
		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				h = level->ground()->getHeight(x,y);
				if(h>maxHeight) maxHeight=h;
				if(h<minHeight) minHeight=h;
			}
		}
		level->ground()->setMinMaxHeights();
	}
	//int loadShader(char* vert,char* frag)
	//{
	//	char * ff = textFileRead(frag);
	//	char * vv = textFileRead(vert);
	//	if(ff == NULL || vv == NULL)
	//		return 0;

	//	GLuint v=0,f=0,p;
	//	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	//	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	//	glShaderSource(v, 1, (const char **)&vv, NULL);
	//	glShaderSource(f, 1, (const char **)&ff, NULL);
	//	glCompileShader(v);
	//	glCompileShader(f);
	//	free(ff);
	//	free(vv);

	//	char* cv=(char*)malloc(512); memset(cv,0,512);
	//	char* cf=(char*)malloc(512); memset(cf,0,512);
	//	int lv,lf;
	//	glGetShaderInfoLog(v,512,&lv,cv);
	//	glGetShaderInfoLog(f,512,&lf,cf);

	//	p = glCreateProgram();
	//	glAttachShader(p,f);
	//	glAttachShader(p,v);

	//	glLinkProgram(p);
	//	shaders.push_back(p);
	//	glUseProgram(0);

	//	if(lv != 0) MessageBoxA(NULL,(string(vert)+": "+string(cv)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
	//	if(lf != 0) MessageBoxA(NULL,(string(frag)+": "+string(cf)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
	//	return p;
	//}


	//struct shaderButton{
	//	guiBase::handle id;
	//	int shaderId;
	//	shaderButton(guiBase::handle Id, int ShaderId): id(Id), shaderId(ShaderId) {}
	//};
	vector<int> shaderButtons;
	friend class menuLevelEditor;
public:
	//map<guiBase::handle,string> buttons;
	virtual int update(float value)
	{
		if(input->getKey(0x52) && !menuManager.getMenu()->popupActive())//r key
			rot+=value/1000;
		Redisplay=true;
		menuManager.update();
		return 15;
	}
	virtual void draw() 
	{
		glClearColor(0.5f,0.8f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, sw, sh);
		gluPerspective(80.0, (double)sw / ((double)sh),1.0, 25000.0);
		gluLookAt(level->ground()->getSize()/2+level->ground()->getSize()/2*sin(rot),50,level->ground()->getSize()/2+level->ground()->getSize()/2*cos(rot),	level->ground()->getSize()/2,0,level->ground()->getSize()/2,	0,1,0);
		

		glEnable(GL_LIGHTING);
		GLfloat lightPos0[] = {-0.3f, -0.3f, -0.4f, 0.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

		if(((menuLevelEditor*)menuManager.getMenu())->getShader() == -1)
			level->ground()->draw();
		else
			level->ground()->draw(shaderButtons[((menuLevelEditor*)menuManager.getMenu())->getShader()]);
		glDisable(GL_LIGHTING);
		//viewOrtho(sw,sh);
		//menuManager.render();
		//viewPerspective();
	}
	mapBuilder(): rot(0), level(NULL), maxHeight(0), minHeight(0) {}
	void init()
	{
		menuManager.setMenu("menuLevelEditor");
		//if(Cmenu != NULL) delete Cmenu;
		//Cmenu=new closedMenu;

		if(level != NULL) delete level;
		level = new editLevel;

		rot=0;
		Redisplay=true;
		newMode=(modeType)0;
		level->newGround(65);
		faultLine();
	}
	~mapBuilder()
	{
		//ShowCursor(false);
	}
};