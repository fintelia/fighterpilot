
//void callLoad(void* );

class loading:public modes
{
public:
	int toLoad;
	void exitProgram()
	{
		cout << "texture file not found" << endl; 
		//sleep(1000);
		assert (0);
		exit (0);		
	}
	//void loadShader(char* vert,char* frag)
	//{
	//	GLuint v=0,f=0,p;
	//	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	//	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	//	char * ff = textFileRead(frag);
	//	char * vv = textFileRead(vert);
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
	//}

	//void load()
	//{
	//	double t=GetTime();
	//	//model.push_back(load_texture("media/plane3.raw"));
	//	model.push_back(load_model("media/f18hornet.obj"));
	//	model.push_back(load_texture("media/missile3.raw"));
	//	model.push_back(load_model("media/f18hornet(low poly).obj"));
	//	model.push_back(load_model("media/dome3.obj"));
	//	readModelData("media/modelData.txt");

	//	tex.push_back(LoadBitMap("media/cockpit.tga",true));//0
	//	tex.push_back(LoadBitMap("media/grass3.tga"));
	//	tex.push_back(0);//was fire
	//	tex.push_back(LoadBitMap("media/aimer.tga"));
	//	tex.push_back(LoadBitMap("media/dial front.tga"));
	//	tex.push_back(LoadBitMap("media/dial back.tga"));//5
	//	tex.push_back(LoadBitMap("media/speed2.tga"));
	//	tex.push_back(LoadBitMap("media/altitude.tga"));
	//	tex.push_back(LoadBitMap("media/needle2.tga"));//8
	//	tex.push_back(LoadBitMap("media/radar.tga"));
	//	tex.push_back(LoadBitMap("media/particle4.tga"));//10
	//	tex.push_back(LoadBitMap("media/plane radar2.tga"));
	//	tex.push_back(LoadBitMap("media/key.tga",true));
	//	tex.push_back(LoadBitMap("media/letters.tga",true));
	//	tex.push_back(LoadBitMap("media/rock4.tga"));///////////////////////
	//	tex.push_back(LoadBitMap("media/water.tga"));//15
	//	tex.push_back(LoadBitMap("media/sand.tga"));
	//	tex.push_back(LoadBitMap("media/nextlevel.tga"));//17
	//	tex.push_back(LoadBitMap("media/radar2.tga"));
	//	tex.push_back(LoadBitMap("media/radar_frame.tga"));
	//	tex.push_back(LoadBitMap("media/particle6.tga"));//20
	//	tex.push_back(0);//LoadBitMap("media/shadow.tga"));
	//	tex.push_back(LoadBitMap("media/sky.tga"));
	//	tex.push_back(LoadBitMap("media/water plane.tga"));
	//	tex.push_back(LoadBitMap("media/health bar.tga",true));
	//	tex.push_back(0);//tex.push_back(LoadBitMap("media/water plane.tga"));//25
	//	tex.push_back(LoadBitMap("media/LCnoise.tga"));
	//	tex.push_back(LoadBitMap("media/explosion/core.tga"));
	//	tex.push_back(LoadBitMap("media/explosion/fireball2.tga"));
	//	tex.push_back(LoadBitMap("media/explosion/flash.tga"));
	//	tex.push_back(LoadBitMap("media/explosion/smoke2.tga"));

	//	menusTx.push_back(LoadBitMap("media/mainMenu.tga",true));
	//	menusTx.push_back(LoadBitMap("media/controlsMenu.tga",true));
	//	menusTx.push_back(LoadBitMap("media/menu/start.tga",true));
	//	menusTx.push_back(LoadBitMap("media/menu/slot.tga",true));
	//	menusTx.push_back(LoadBitMap("media/menu/mode choices.tga",true));

	//	//particleTex=LoadBitMap("media/smoke2.tga");
	//	//if(particleTex==-1)
	//	//	exitProgram();
	//	loadShader("media/toon.vert","media/toon.frag");
	//	loadShader("media/radar.vert","media/radar.frag");
	//	loadShader("media/radar2.vert","media/radar2.frag");
	//	loadShader("media/water.vert","media/water.frag");
	//	loadShader("media/sea floor.vert","media/sea floor.frag");
	//	//loadShader("media/fire.vert","media/fire.frag");
	//	for(int i=0;i< (signed int)tex.size();i++)
	//	{
	//		if (tex[i]==-1)
	//		{
	//			MessageBox(NULL,"texture loading failed on texture # " + i, "Error",MB_ICONEXCLAMATION);
	//			__debugbreak();
	//			exitProgram();
	//		}
	//	}
	//	for(int i=0;i< (signed int)menusTx.size();i++)
	//	{
	//		if (menusTx[i]==-1)
	//		{
	//			MessageBox(NULL,"texture loading failed on menu texture # " + i, "Error",MB_ICONEXCLAMATION);
	//			__debugbreak();
	//			exitProgram();
	//		}
	//	}
	//	BuildFont();
	//	Cmenu=new m_start;
	//	newMode=BLANK_MODE;
	//	//PlaySound(TEXT("media/07_Creedence_Clearwater_Revival_Fortunate_Son.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
	//}

	//static void ThreadStaticEntryPoint(void * pThis)
	//{
	//	wglMakeCurrent(hDC,hRC);
	//	((loading*)pThis)->load();
	//	//loading* l=(loading*)v;
	//	wglMakeCurrent(0,0);
	//	getContext=true;

	//	return; 
	//}
	void interleavedLoad()
	{
		double t=GetTime();

		static int callNum=0;
		callNum++;
		int n=1;

		//if(callNum==n++)		model.push_back(0);
		//else if(callNum==n++)	model.push_back(0);//load_texture("media/missile3.raw"));
		//else if(callNum==n++)	model.push_back(0);
		//else if(callNum==n++)	model.push_back(dataManager.loadModel("media/dome4.obj"));
		if(callNum==n++)	settings.loadModelData("media/modelData.txt");

		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/cockpit.tga",true));		//0
		//else if(callNum==n++)	tex.push_back(loadMMP("media/grass3.mmp"));
		//else if(callNum==n++)	tex.push_back(0);//was fire
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/aimer.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/dial front.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/dial back.tga"));			//5
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/speed2.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/altitude.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/needle2.tga"));				//8
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/radar.tga"));
		//else if(callNum==n++)	tex.push_back(loadMMP("media/particle4.mmp"));				//10
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/plane radar2.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/key.tga",true));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/letters.tga",true));
		//else if(callNum==n++)	tex.push_back(loadMMP("media/rock4.mmp"));
		//else if(callNum==n++)	tex.push_back(0);//loadMMP("media/water.mmp"));			//15
		//else if(callNum==n++)	tex.push_back(loadMMP("media/sand.mmp"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/nextlevel.tga"));			//17
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/radar2.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/radar_frame.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/particle8.tga"));			//20
		//else if(callNum==n++)	tex.push_back(0);//LoadBitMap("media/shadow.tga"));
		//else if(callNum==n++)	tex.push_back(0);//loadMMP("media/sky.mmp"));
		//else if(callNum==n++)	tex.push_back(0);//loadMMP("media/water plane.mmp"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/health bar.tga",true));
		//else if(callNum==n++)	tex.push_back(0);//"media/water plane.tga"));				//25
		//else if(callNum==n++)	tex.push_back(loadMMP("media/LCnoise.mmp"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/explosion/core.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/explosion/fireball2.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/explosion/flash.tga"));
		//else if(callNum==n++)	tex.push_back(LoadBitMap("media/explosion/smoke5.tga"));	//30
		//else if(callNum==n++)	tex.push_back(loadMMP("media/hardNoiseDOT3.mmp"));
		//else if(callNum==n++)	tex.push_back(loadMMP("media/snow.mmp"));

		//else if(callNum==n++)	menusTx.push_back(LoadBitMap("media/mainMenu.tga",true));
		//else if(callNum==n++)	menusTx.push_back(LoadBitMap("media/controlsMenu.tga",true));
		//else if(callNum==n++)	menusTx.push_back(LoadBitMap("media/menu/start.tga",true));
		//else if(callNum==n++)	menusTx.push_back(LoadBitMap("media/menu/slot.tga",true));
		//else if(callNum==n++)	menusTx.push_back(LoadBitMap("media/menu/mode choices.tga",true));

		//particleTex=LoadBitMap("media/smoke2.tga");
		//if(particleTex==-1)
		//	exitProgram();

		//else if(callNum==n++)	loadShader("media/toon.vert","media/toon.frag");
		//else if(callNum==n++)	loadShader("media/radar.vert","media/radar.frag");
		//else if(callNum==n++)	loadShader("media/radar2.vert","media/radar2.frag");
		//else if(callNum==n++)	loadShader("media/water.vert","media/water.frag");
		//else if(callNum==n++)	loadShader("media/sea floor.vert","media/sea floor.frag");
		//else if(callNum==n++)	loadShader("media/horizon.vert","media/horizon.frag");//was water2
		//else if(callNum==n++)	loadShader("media/island.vert","media/island.frag");
		//else if(callNum==n++)	loadShader("media/rock.vert","media/rock.frag");

		else if(callNum==n++)	textManager->init("media/ascii");
		//loadShader("media/fire.vert","media/fire.frag");
		//else if(callNum==n++)
		//{
		//	for(int i=0;i< (signed int)tex.size();i++)
		//	{
		//		if (tex[i]==-1)
		//		{
		//			MessageBoxA(NULL,"texture loading failed on texture # " + i, "Error",MB_ICONEXCLAMATION);
		//			__debugbreak();
		//			exitProgram();
		//		}
		//	}
		//}
		//else if(callNum==n++)
		//{
		//	for(int i=0;i< (signed int)menusTx.size();i++)
		//	{
		//		if (menusTx[i]==-1)
		//		{
		//			MessageBoxA(NULL,"texture loading failed on menu texture # " + i, "Error",MB_ICONEXCLAMATION);
		//			__debugbreak();
		//			exitProgram();
		//		}
		//	}
		//}
		t=GetTime()-t;
		t=GetTime();
	}

	void draw()
	{
		static int s = 0;
		s++;
		//if(s<10)return;
		s=0;

		glClearColor(0.5f,0.5f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0 , sw, sh);
		glLoadIdentity();
		//gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 10000.0);
		viewOrtho(sw,sh);
		glPushMatrix();
		glEnable(GL_BLEND);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(0,0);
			glTexCoord2f(0,0);	glVertex2f(0,sh);
			glTexCoord2f(1,0);	glVertex2f(sw,sh);		
			glTexCoord2f(1,1);	glVertex2f(sw,0);
		glEnd();
		glDisable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, 0);
		viewPerspective();
	}
	int update(float value)
	{
		if(toLoad==0)
		{
			menuBack=-1;
			menuBack=dataManager.loadTexture("media/menu/menu background2.tga");
			if(menuBack==-1)
				exitProgram();
			Redisplay=true;
			toLoad++;
		}
		//else if(toLoad==1)
		//{
		//	wglMakeCurrent(0,0);
		//	//_beginthread( callLoad, 0, (void*)this );
		//	_beginthread( ThreadStaticEntryPoint, 0, (void*)this );
		//	hasContext=false;
		//}


		interleavedLoad();
		if(dataManager.registerAssets())
		{
			Cmenu=new m_start;
			newMode=BLANK_MODE;
		}
		Redisplay=true;

		return 0;
	}
	loading()
	{
		newMode=(modeType)0;
		Cmenu=new closedMenu;
		toLoad=0;
	}
};

//void callLoad(void* v)
//{
//	loading* l=(loading*)v;
//	wglMakeCurrent(hDC,hRC);
//	l->load();
//	wglMakeCurrent(0,0);
//	getContext=true;
//}