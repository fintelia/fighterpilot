
class loading:public modes
{
public:
	bool toLoad;
	void exitProgram()
	{
		cout << "texture file not found" << endl; 
		//sleep(1000);
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
	void DisplayLists()
	{
		int x;
		int z;
		disp[0]= glGenLists(1);
		glNewList(disp[0],GL_COMPILE);
			//glColor4f(0.1f, 0.1f, 0.9f,1);
			glPushMatrix();
			glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
			glColor3f(1,1,1);
			glBindTexture(GL_TEXTURE_2D, tex[1]); // We set the active texture
			for(z = 0; z < _terrain->length() - 1; z++) {
				//Makes OpenGL draw a triangle at every three consecutive vertices
				glBegin(GL_TRIANGLE_STRIP);
				
					for(x = 0; x < _terrain->width(); x+=2) {
							Vec3f normal = _terrain->getNormal(x, z);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(0,0);
							glVertex3f(x * size, _terrain->getHeight(x, z), z * size);
							
							normal = _terrain->getNormal(x, z+1);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(0,1);
							glVertex3f(x * size, _terrain->getHeight(x, z+1), (z+1) * size);

							normal = _terrain->getNormal(x+1, z);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(1,0);
							glVertex3f((x+1) * size, _terrain->getHeight(x+1, z), z * size);
							
							normal = _terrain->getNormal(x+1, z+1);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(1,1);
							glVertex3f((x+1) * size, _terrain->getHeight(x+1, z+1), (z+1) * size);
					}
				glEnd();
			}
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
		glEndList();

	}
	void load()
	{
		model.push_back(load_texture("media2/plane3.raw"));
		model.push_back(load_texture("media2/missile3.raw"));

		tex.push_back(LoadBitMap("media2/cockpit.tga"));//0
		tex.push_back(LoadBitMap("media2/grass2.tga"));
		tex.push_back(LoadBitMap("media2/fire.tga"));
		tex.push_back(LoadBitMap("media2/aimer.tga"));//5
		tex.push_back(LoadBitMap("media2/dial front.tga"));
		tex.push_back(LoadBitMap("media2/dial back.tga"));
		tex.push_back(LoadBitMap("media2/speed.tga"));
		tex.push_back(LoadBitMap("media2/altitude.tga"));
		tex.push_back(LoadBitMap("media2/needle.tga"));//8
		tex.push_back(LoadBitMap("media2/radar2.tga"));
		tex.push_back(LoadBitMap("media2/radar tail2.tga"));//10
		tex.push_back(LoadBitMap("media2/plane radar2.tga"));
		tex.push_back(LoadBitMap("media2/key.tga"));
		tex.push_back(LoadBitMap("media2/letters.tga"));//13

		menus.push_back(LoadBitMap("media2/mainMenu.tga"));
		menus.push_back(LoadBitMap("media2/controlsMenu.tga"));

		//particleTex=LoadBitMap("media2/smoke2.tga");
		//if(particleTex==-1)
		//	exitProgram();

		for(int i=0;i< (signed int)tex.size();i++)
		{
			if (tex[i]==-1)
				exitProgram();
		}
		for(int i=0;i< (signed int)menus.size();i++)
		{
			if (menus[i]==-1)
				exitProgram();
		}

	}
	void draw()
	{
		glClearColor(0.5f,0.5f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0 , sw, sh);
		glLoadIdentity();
		gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 10000.0);
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
		glutSwapBuffers();
	}
	int update(int value)
	{
		if(toLoad)
		{
			load();
			_terrain = loadTerrain("media2/heightmap2.bmp", 100);
			DisplayLists();
			newMode=2;
		}
		else
		{
			menuBack=4;
			menuBack=LoadBitMap("media2/menu background.tga");
			if(menuBack==-1)
				exitProgram();
			//glutGameModeString("800x600:32");
			//// enter full screen
			//if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) 
			//	glutEnterGameMode();
			//else {
			//	cout << "error";
			//	glutInitWindowSize(600, 800);
			//	glutCreateWindow("dog fights!" );
			//}
		}
		glutPostRedisplay();
		toLoad=true;
		return 1;
	}
	loading()
	{
		newMode=0;
		toLoad=false;
	}
};