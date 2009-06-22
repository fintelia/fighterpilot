
void callLoad(void* );

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
	Terrain* loadTerrain(const char* filename, float height) {
		Image* image = loadBMP(filename);
		Terrain* t = new Terrain(image->width, image->height);
		for(int y = 0; y < image->height; y++) {
			for(int x = 0; x < image->width; x++) {
				unsigned char color = (unsigned char)image->pixels[3 * (y * image->width + x)];
				//if( x==0 || y==0 || y==image->height-1 || x==image->width-1){
				//	color = 0;
				//}
				float h = height * ((color / 255.0f) - 0.5f);
				t->setHeight(x, y, h);
			}
		}
		
		delete image;
		t->computeNormals();
		return t;
	}
	//void DisplayLists()
	//{
	//	int x;
	//	int z;
	//	disp[0]= glGenLists(1);
	//	

	//	glNewList(disp[0],GL_COMPILE);
	//		//glColor4f(0.1f, 0.1f, 0.9f,1);
	//		//glPushMatrix();
	//		glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
	//		glColor3f(1,1,1);
	//		glBindTexture(GL_TEXTURE_2D, tex[1]); // We set the active texture
	//		for(z = 0; z < terrain->length() - 1; z++) {
	//			//Makes OpenGL draw a triangle at every three consecutive vertices
	//			glBegin(GL_TRIANGLE_STRIP);
	//			
	//				for(x = 0; x < terrain->width(); x+=2) {
	//						Vec3f normal = terrain->getNormal(x, z);
	//						glNormal3f(normal[0], normal[1], normal[2]);
	//						glTexCoord2f(0,0);
	//						glVertex3f(x * size, terrain->getHeight(x, z), z * size);
	//						
	//						normal = terrain->getNormal(x, z+1);
	//						glNormal3f(normal[0], normal[1], normal[2]);
	//						glTexCoord2f(0,1);
	//						glVertex3f(x * size, terrain->getHeight(x, z+1), (z+1) * size);

	//						normal = terrain->getNormal(x+1, z);
	//						glNormal3f(normal[0], normal[1], normal[2]);
	//						glTexCoord2f(1,0);
	//						glVertex3f((x+1) * size, terrain->getHeight(x+1, z), z * size);
	//						
	//						normal = terrain->getNormal(x+1, z+1);
	//						glNormal3f(normal[0], normal[1], normal[2]);
	//						glTexCoord2f(1,1);
	//						glVertex3f((x+1) * size, terrain->getHeight(x+1, z+1), (z+1) * size);
	//				}
	//			glEnd();
	//		}
	//		glDisable(GL_TEXTURE_2D);
	//		//glPopMatrix();
	//	glEndList();


	//}
	void setShaders() 
	{
		GLuint v=0,f=0,p;
		char *vs = NULL,*fs = NULL,*fs2 = NULL;
		v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

		vs = textFileRead("media2/toon.vert");
		fs = textFileRead("media2/toon.frag");

		const char * ff = fs;
		const char * vv = vs;
		
		int fff[1];
		fff[0]=sizeof(ff);
		int vvv[1];
		vvv[0]=sizeof(vs);


		glShaderSource(v, 1, &vv, NULL);
		glShaderSource(f, 1, &ff, NULL);
		free(vs);free(fs);

		glCompileShader(v);
		glCompileShader(f);
		
		char cv[256];
		char cf[256];
		glGetShaderInfoLog(v,64,NULL,cv);
		cout << "-" << cv << endl;
		glGetShaderInfoLog(f,64,NULL,cf);
		cout << "-" << cf << endl;
		p = glCreateProgram();
		glAttachShader(p,f);
		glAttachShader(p,v);

		glLinkProgram(p);
		shaders.push_back(p);
		glUseProgram(0);
	}
	void loadShader(char* vert,char* frag)
	{
		GLuint v=0,f=0,p;
		v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

		const char * ff = textFileRead(frag);
		const char * vv = textFileRead(vert);
		glShaderSource(v, 1, &vv, NULL);
		glShaderSource(f, 1, &ff, NULL);
		glCompileShader(v);
		glCompileShader(f);

		char cv[256];
		char cf[256];
		glGetShaderInfoLog(v,64,NULL,cv);//<---|
		glGetShaderInfoLog(f,64,NULL,cf);//<---|
							//                 |
		p = glCreateProgram();//               |
		glAttachShader(p,f);//       breakpoints from here
		glAttachShader(p,v);

		glLinkProgram(p);
		shaders.push_back(p);
		glUseProgram(0);
	}
	void load()
	{
		//model.push_back(load_texture("media2/plane3.raw"));
		model.push_back(load_model("media2/f18hornet.obj"));
		model.push_back(load_texture("media2/missile3.raw"));
		//model.push_back(load_texture("media2/turret.raw"));

		tex.push_back(LoadBitMap("media2/cockpit.tga"));//0
		tex.push_back(LoadBitMap("media2/grass3.tga"));
		tex.push_back(LoadBitMap("media2/fire.tga"));
		tex.push_back(LoadBitMap("media2/aimer.tga"));
		tex.push_back(LoadBitMap("media2/dial front.tga"));
		tex.push_back(LoadBitMap("media2/dial back.tga"));//5
		tex.push_back(LoadBitMap("media2/speed.tga"));
		tex.push_back(LoadBitMap("media2/altitude.tga"));
		tex.push_back(LoadBitMap("media2/needle.tga"));//8
		tex.push_back(LoadBitMap("media2/radar2.tga"));
		tex.push_back(LoadBitMap("media2/particle.tga"));//10
		tex.push_back(LoadBitMap("media2/plane radar2.tga"));
		tex.push_back(LoadBitMap("media2/key.tga"));
		tex.push_back(LoadBitMap("media2/letters.tga"));
		tex.push_back(LoadBitMap("media2/rock.tga"));
		tex.push_back(LoadBitMap("media2/water.tga"));//15
		tex.push_back(LoadBitMap("media2/sand.tga"));//16

		menusTx.push_back(LoadBitMap("media2/mainMenu.tga"));
		menusTx.push_back(LoadBitMap("media2/controlsMenu.tga"));

		//particleTex=LoadBitMap("media2/smoke2.tga");
		//if(particleTex==-1)
		//	exitProgram();
		setShaders();
		loadShader("media2/radar.vert","media2/radar.frag");

		for(int i=0;i< (signed int)tex.size();i++)
		{
			if (tex[i]==-1)
			{
				assert (0);
				exitProgram();
			}
		}
		for(int i=0;i< (signed int)menusTx.size();i++)
		{
			if (menusTx[i]==-1)
			{
				assert (0);
				exitProgram();
			}
		}
	}

	//static unsigned __stdcall ThreadStaticEntryPoint(void * pThis)
	//{
	//  ThreadX * pthX = (ThreadX*)pThis;   // the tricky cast

	//  pthX->load();    // now call the true entry-point-function

	//  // A thread terminates automatically if it completes execution,
	//  // or it can terminate itself with a call to _endthread().

	//  return 1;          // the thread exit code
	//}
	

	void draw()
	{
		glClearColor(0.5f,0.5f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0 , sw, sh);
		glLoadIdentity();
		//gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 10000.0);
		ViewOrtho(sw,sh);
		glPushMatrix();
		glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
		glEnable(GL_BLEND);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(0,0);
			glTexCoord2f(0,0);	glVertex2f(0,600);
			glTexCoord2f(1,0);	glVertex2f(800,600);		
			glTexCoord2f(1,1);	glVertex2f(800,0);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		ViewPerspective();
	}
	int update(int value)
	{
		if(toLoad==0)
		{
			menuBack=-1;
			menuBack=LoadBitMap("media2/menu background.tga");
			if(menuBack==-1)
				exitProgram();
			Redisplay=true;
		}
		else if(toLoad==1)
		{
			wglMakeCurrent(0,0);
			_beginthread( callLoad, 0, (void*)this );
			hasContext=false;
			Cmenu=new mainMenu;
			newMode=2;
		}

		toLoad++;
		return 30;
	}
	loading()
	{
		newMode=0;
		Cmenu=new closedMenu;
		toLoad=0;
	}
};

void callLoad(void* v)
{
	loading* l=(loading*)v;
	wglMakeCurrent(hDC,hRC);
	l->load();
	wglMakeCurrent(0,0);
	getContext=true;
}