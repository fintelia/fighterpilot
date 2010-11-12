
extern int sh,sw;

class modes
{
protected:
	Terrain* loadTerrainOLD(const char* filename, float height) {
		Image* image = loadBMP(filename);
		Terrain* t = new Terrain(image->width, image->height);
		for(int y = 0; y < image->height; y++) {
			for(int x = 0; x < image->width; x++) {
				unsigned char color = (unsigned char)image->pixels[3 * (y * image->width + x)];
				float h = height * ((color / 255.0f) - 0.5f);
				t->setHeight(x, y, h);
			}
		}
		
		delete image;
		t->computeNormals();
		return t;
	}
	Terrain* loadTerrain(const char* filename, float height) 
	{

		Image* image = loadBMP(filename);
		Terrain* t = new Terrain(image->width, image->height);

		glGenTextures(1,(GLuint*)&t->textureId);
		glBindTexture(GL_TEXTURE_2D, t->textureId); // Bind the ID texture specified by the 2nd parameter
		glTexImage2D(GL_TEXTURE_2D, 0, 3 , image->width, image->height, 0 ,GL_RGB, GL_UNSIGNED_BYTE, (void*)image->pixels);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // We don't combine the color with the original surface color, use only the texture map.
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3 , image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, (void*)image->pixels);
				
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
	//Terrain* loadTerrainNEW(const char* filename) 
	//{
	//	Terrain* t = new gridFloatTerrain("media/ned_29518377");
	//	t->computeNormals();
	//	return t;
	//}
	float waterOpacity(float height)
	{
		float tmpVal=(settings.SEA_LEVEL-height)/200+0.015;	
		if(height>settings.SEA_LEVEL || tmpVal<=0.0)	return 0.0;
		else if(tmpVal>=0.8) 							return 0.8; 
		else											return tmpVal;
	}
	void DisplayLists()
	{
		int x;
		int z;
		int landShader;
		int texture_location;

		if(settings.MAP_TYPE == LAND) landShader = dataManager.getId("grass terrain");
		else if(settings.MAP_TYPE == WATER) landShader = dataManager.getId("island terrain");
		else if(settings.MAP_TYPE == SNOW) landShader = dataManager.getId("rock terrain");

		disp[0]= glGenLists(1);
		glNewList(disp[0],GL_COMPILE);
			glPushMatrix();
			//glColor3f(1,1,1);
			glUseProgram(landShader);
			glUniform1f(glGetUniformLocation(landShader, "maxHeight"), 250);
			glUniform1f(glGetUniformLocation(landShader, "minHeight"), settings.SEA_LEVEL);

			if(settings.MAP_TYPE == LAND || settings.MAP_TYPE == WATER)
			{
				//glActiveTextureARB(GL_TEXTURE0_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[14]);	//should be 15
				//glActiveTextureARB(GL_TEXTURE1_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[16]);
				//glActiveTextureARB(GL_TEXTURE2_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[1]);
				//glActiveTextureARB(GL_TEXTURE3_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[14]);

				//glActiveTextureARB(GL_TEXTURE4_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[26]);


				dataManager.bind("rock",0);
				dataManager.bind("sand",1);
				dataManager.bind("grass",2);
				dataManager.bind("rock",3);
				dataManager.bind("LCnoise",4);

				int textures[5]={0,1,2,3,4};
				texture_location = glGetUniformLocation(landShader, "myTexture[0]");
				glUniform1i(texture_location++, textures[0]);
				glUniform1i(texture_location++, textures[1]);
				glUniform1i(texture_location++, textures[2]);
				glUniform1i(texture_location++, textures[3]);
				glUniform1i(texture_location++, textures[4]);
			}
			else if(settings.MAP_TYPE == SNOW)
			{
				//glActiveTextureARB(GL_TEXTURE0_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[32]);
				//glActiveTextureARB(GL_TEXTURE1_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[14]);
				//glActiveTextureARB(GL_TEXTURE2_ARB);
				//glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,tex[26]);
				dataManager.bind("snow",0);
				dataManager.bind("rock",1);
				dataManager.bind("LCnoise",2);
				int textures[3]={0,1,2};
				texture_location = glGetUniformLocation(landShader, "myTexture[0]");
				glUniform1i(texture_location++, textures[0]);
				glUniform1i(texture_location++, textures[1]);
				glUniform1i(texture_location++, textures[2]);
			}
			int tileSize=16;
			int mapW=terrain->width();//map width and height
			int mapH=terrain->length();

			for(z = 0; z < terrain->length(); z+=tileSize) {
				//Makes OpenGL draw a triangle at every three consecutive vertices
				for(x = 0; x < terrain->width(); x+=tileSize) {
					for(int lz=0;lz<tileSize;lz++)
					{
						glBegin(GL_TRIANGLE_STRIP);
						for(int lx=0;lx<tileSize;lx++)
						{
							int mx=x+lx;
							int mz=z+lz;

							Vec3f normal = terrain->getNormal(mx, mz);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(mx)/tileSize,double(mz)/tileSize);
							glMultiTexCoord2fARB(GL_TEXTURE4_ARB,float(mx)/mapW,float(mz)/mapH);
							glVertex3f(mx * size, terrain->getHeight(mx, mz), mz * size);

							normal = terrain->getNormal(mx, mz+1);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(mx)/tileSize,double(mz+1)/tileSize);
							glMultiTexCoord2fARB(GL_TEXTURE4_ARB,float(mx)/mapW,(float(mz)+1)/mapH);
							glVertex3f(mx * size, terrain->getHeight(mx, mz+1), (mz+1) * size);

							normal = terrain->getNormal(mx+1, mz);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(mx+1)/tileSize,double(mz)/tileSize);
							glMultiTexCoord2fARB(GL_TEXTURE4_ARB,(float(mx)+1)/mapW,float(mz)/mapH);
							glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz), mz * size);

							normal = terrain->getNormal(mx+1, mz+1);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(mx+1)/tileSize,double(mz+1)/tileSize);
							glMultiTexCoord2fARB(GL_TEXTURE4_ARB,(float(mx)+1)/mapW,(float(mz)+1)/mapH);
							glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz+1), (mz+1) * size);
						}
						glEnd();
					}
				}
			}
			glPopMatrix();
			glUseProgram(0);

			//glActiveTextureARB(GL_TEXTURE4_ARB);
			//glBindTexture(GL_TEXTURE_2D,0);
			//glActiveTextureARB(GL_TEXTURE3_ARB);
			//glBindTexture(GL_TEXTURE_2D,0);
			//glActiveTextureARB(GL_TEXTURE2_ARB);
			//glBindTexture(GL_TEXTURE_2D,0);	
			//glActiveTextureARB(GL_TEXTURE1_ARB);
			//glBindTexture(GL_TEXTURE_2D,0);
			//glActiveTextureARB(GL_TEXTURE0_ARB);
			//glBindTexture(GL_TEXTURE_2D,0);
			dataManager.bindTex(0,4);
			dataManager.bindTex(0,3);
			dataManager.bindTex(0,2);
			dataManager.bindTex(0,1);
			dataManager.bindTex(0,0);

			//glEnable(GL_BLEND);
			//glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D,tex[23]);
			//glColor4f(1.0,1.0,1.0,0.6);
			//glBegin(GL_QUADS);
			//	glVertex3f(0,-100,0);
			//	glVertex3f(terrain->length()* size,-100,0);
			//	glVertex3f(terrain->length()* size,-100,terrain->width()* size);
			//	glVertex3f(0,-100,terrain->width()* size);
			//glEnd();
			//glBindTexture(GL_TEXTURE_2D,0);
			//glDisable(GL_BLEND);
		glEndList();




		//	float sl=settings.SEA_LEVEL;
		//	tileSize=16;
		//	glPushMatrix();
		//	glBindTexture(GL_TEXTURE_2D,tex[23]);
		//	//for(z = 0; z < terrain->length(); z+=tileSize) {
		//	//	for(x = 0; x < terrain->width(); x+=tileSize) {
		//	//		for(int lz=0;lz<tileSize;lz++){
		//	//			glBegin(GL_TRIANGLE_STRIP);
		//	//			for(int lx=0;lx<=tileSize;lx++)
		//	//			{
		//	//				int mx=x+lx;
		//	//				int mz=z+lz;
		//	//				glColor4f(1,1,1,waterOpacity(terrain->getHeight(mx, mz)));
		//	//				glTexCoord2f(double(lx)/tileSize,double(lz)/tileSize);
		//	//				glVertex3f(mx*size,sl,mz*size);

		//	//				glColor4f(1,1,1,waterOpacity(terrain->getHeight(mx, mz+1)));
		//	//				glTexCoord2f(double(lx)/tileSize,double(lz+1)/tileSize);
		//	//				glVertex3f(mx*size,sl,(mz+1.0)*size);

		//	//				//glColor4f(1,1,1,waterOpacity(terrain->getHeight(mx+1, mz)));
		//	//				//glTexCoord2f(double(lx+1)/tileSize,double(lz)/tileSize);
		//	//				//glVertex3f((mx+1.0)*size,sl,mz*size);

		//	//				//glColor4f(1,1,1,waterOpacity(terrain->getHeight(mx+1, mz+1)));
		//	//				//glTexCoord2f(double(lx+1)/tileSize,double(lz+1)/tileSize);
		//	//				//glVertex3f((mx+1.0)*size,sl,(mz+1.0)*size);
		//	//			}
		//	//			glEnd();
		//	//		}
		//	//	}
		//	//}
		//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//	for(float z = 0; z <= terrain->length(); z+=1) 
		//	{
		//		glBegin(GL_TRIANGLE_STRIP);
		//		for(float x = 0; x <= terrain->width(); x+=1) 
		//		{
		//			glColor4f(1,1,1,waterOpacity(terrain->getHeight(x, z)));
		//			glTexCoord2f(x/tileSize,z/tileSize);
		//			glVertex3f(x*size,sl,z*size);

		//			glColor4f(1,1,1,waterOpacity(terrain->getHeight(x, z+1)));
		//			glTexCoord2f(x/tileSize,(z+1)/tileSize);
		//			glVertex3f(x*size,sl,(z+1.0)*size);
		//		}
		//		glEnd();
		//	}
		//	glPopMatrix();
		//glEndList();
		
		//disp[2]= glGenLists(1);
		//glNewList(disp[2],GL_COMPILE);
		//	glUseProgram(shaders[3]);
		//	glBindTexture(GL_TEXTURE_2D,tex[26]);
		//	texture_location = glGetUniformLocation(shaders[3], "noise");
		//	glUniform1i(texture_location++, 0);
		//	
		//	texture_location = glGetUniformLocation(shaders[3], "time");
		//	float t=gameTime();
		//	glUniform1f(texture_location, t);

		//	tileSize=16;
		//	glPushMatrix();
		//	
		//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//	for(float z = 0; z <= terrain->length(); z+=1) 
		//	{
		//		glBegin(GL_TRIANGLE_STRIP);
		//		for(float x = 0; x <= terrain->width(); x+=1) 
		//		{
		//			//glColor4f(1,1,1,waterOpacity(terrain->getHeight(x, z)));
		//			glTexCoord2f(x/tileSize,z/tileSize);
		//			glVertex3f(x*size,sl,z*size);

		//			//glColor4f(1,1,1,waterOpacity(terrain->getHeight(x, z+1)));
		//			glTexCoord2f(x/tileSize,(z+1)/tileSize);
		//			glVertex3f(x*size,sl,(z+1.0)*size);
		//		}
		//		glEnd();
		//	}
		//	glPopMatrix();
		//	glUseProgram(0);
		//glEndList();
	}
	void prepareMap(char *filename)
	{
		settings.loadMap(filename);
		terrain = loadTerrain(settings.MAP_FILE.c_str(), settings.HEIGHT_RANGE);
		//sea = loadTerrain("media/sea.bmp", 300);
		DisplayLists();
	}

public:
	//modeType newMode;
		modeType newMode;
	menuType newMenu;
	virtual int update(float value)=0;
	virtual void draw()=0;
	virtual void init(){}
	virtual ~modes(){}
};
class blankMode: public modes
{
public:
	void draw()
	{
		glClearColor(0.5f,0.5f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0 , sw, sh);
		glLoadIdentity();
		viewOrtho(sw,sh);
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glColor3f(1,1,1);
		dataManager.bind("menu background");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(0,0);
			glTexCoord2f(0,0);	glVertex2f(0,sh);
			glTexCoord2f(1,0);	glVertex2f(sw,sh);		
			glTexCoord2f(1,1);	glVertex2f(sw,0);
		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
		dataManager.bindTex(0);
		viewPerspective();
	}
	int update(float value)
	{
		//Redisplay=true;
		return 50;
	}
	blankMode()
	{
		Redisplay=true;
		newMode=(modeType)0;
	}
};
class dogFight: public modes
{
public:
	dogFight():level(NULL){}

protected:
	FrustumG frustum;
	//void drawWater()
	//{
	//	float sl=settings.SEA_LEVEL;
	//	int tileSize=16;
	//	float t=gameTime()*0.0001;

	//	glBindTexture(GL_TEXTURE_2D,tex[25]);
	//	for(float z = 0; z <= terrain->length(); z+=1) 
	//	{
	//		glBegin(GL_TRIANGLE_STRIP);
	//		for(float x = 0; x <= terrain->width(); x+=1) 
	//		{
	//			glColor4f(1,1,1,0.7*waterOpacity(terrain->getHeight(x, z)));
	//			glTexCoord2f(x/tileSize,z/tileSize);
	//			glVertex3f(x*size,sl,z*size);

	//			glColor4f(1,1,1,0.7*waterOpacity(terrain->getHeight(x, z+1)));
	//			glTexCoord2f(x/tileSize,(z+1)/tileSize);
	//			glVertex3f(x*size,sl,(z+1.0)*size);
	//		}
	//		glEnd();
	//	}

	//	glActiveTextureARB(GL_TEXTURE0_ARB);
	//	glBindTexture(GL_TEXTURE_2D,tex[23]);
	//	glActiveTextureARB(GL_TEXTURE1_ARB);
	//	glBindTexture(GL_TEXTURE_2D,tex[23]);
	//	for(float z = 0; z <= terrain->length(); z+=1) 
	//	{
	//		glBegin(GL_TRIANGLE_STRIP);
	//		for(float x = 0; x <= terrain->width(); x+=1) 
	//		{
	//			glColor4f(1,1,1,4*waterOpacity(terrain->getHeight(x, z)));
	//			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,x/tileSize+t,z/tileSize);
	//			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,x/tileSize,z/tileSize+t);
	//			glVertex3f(x*size,sl,z*size);

	//			glColor4f(1,1,1,4*waterOpacity(terrain->getHeight(x, z+1)));
	//			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,x/tileSize+t,(z+1)/tileSize);
	//			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,x/tileSize,(z+1)/tileSize+t);
	//			glVertex3f(x*size,sl,(z+1.0)*size);
	//		}
	//		glEnd();
	//	}
	//	glActiveTextureARB(GL_TEXTURE1_ARB);
	//	glBindTexture(GL_TEXTURE_2D,0);
	//	glActiveTextureARB(GL_TEXTURE0_ARB);
	//	glBindTexture(GL_TEXTURE_2D,0);
	//	glColor4f(1,1,1,1);
	//}
	//void drawWater2(Vec3f eye)
	//{
	//	float sl=settings.SEA_LEVEL;
	//	glUseProgram(shaders[3]);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D,tex[26]);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D,terrain->textureId);
	//	glActiveTexture(GL_TEXTURE2);
	//	glBindTexture(GL_TEXTURE_2D,tex[23]);

	//	glUniform1i(glGetUniformLocation(shaders[3], "noise"), 0);
	//	glUniform1i(glGetUniformLocation(shaders[3], "ground"), 1);
	//	glUniform1i(glGetUniformLocation(shaders[3], "tex"), 2);
	//	glUniform1f(glGetUniformLocation(shaders[3], "time"), gameTime());
	//	glUniform1f(glGetUniformLocation(shaders[3], "heightRange"), settings.HEIGHT_RANGE*2);

	//	glUniform3f(glGetUniformLocation(shaders[3], "eyePos"), eye.x, eye.y, eye.z);

	//	float tileSize=16;
	//	//glEnable(GL_POLYGON_SMOOTH);
	//	glPushMatrix();
	//	
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	
	//	glBegin(GL_QUADS);
	//		glTexCoord2f(-4.0,-4.0);		glVertex3f(-4*terrain->width()*size,sl,-4*terrain->length()*size);
	//		glTexCoord2f( 5.0,-4.0);		glVertex3f(5*terrain->width()*size,sl,-4*terrain->length()*size);
	//		glTexCoord2f( 5.0, 5.0);		glVertex3f(5*terrain->width()*size,sl,5*terrain->length()*size);
	//		glTexCoord2f(-4.0, 5.0);		glVertex3f(-4*terrain->width()*size,sl,5*terrain->length()*size);
	//	glEnd();
	//	glPopMatrix();

	//	glEnable(GL_LINE_SMOOTH);
	//	Vec3f verts[4];
	//	float t;
	//	t=(sl-frustum.fbl.y)/(frustum.ftl.y-frustum.fbl.y);		verts[0]=frustum.fbl*(1.0-t)+frustum.ftl*t;
 //		t=(sl-frustum.fbr.y)/(frustum.ftr.y-frustum.fbr.y);		verts[1]=frustum.fbr*(1.0-t)+frustum.ftr*t;
	//	t=(sl-frustum.nbl.y)/(frustum.ntl.y-frustum.nbl.y);		verts[2]=frustum.nbl*(1.0-t)+frustum.ntl*t;
	//	t=(sl-frustum.nbr.y)/(frustum.ntr.y-frustum.nbr.y);		verts[3]=frustum.nbr*(1.0-t)+frustum.ntr*t;

	//	glBegin(GL_LINE_LOOP);
 //			glVertex3f(verts[0].x,verts[0].y,verts[0].z);
	//		glVertex3f(verts[1].x,verts[1].y,verts[1].z);
	//		glVertex3f(verts[2].x,verts[2].y,verts[2].z);
	//		glVertex3f(verts[3].x,verts[3].y,verts[3].z);
	//	glEnd();

	//	glActiveTexture(GL_TEXTURE2);
	//	glBindTexture(GL_TEXTURE_2D,0);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D,0);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D,0);

	//	glUseProgram(0);


	//}
	void drawWater3(Vec3f eye)
	{
		double sl=settings.SEA_LEVEL;
		
		int s=dataManager.getId("horizon");
		dataManager.bind("horizon");

		dataManager.bind("hardNoise",0);
		dataManager.bindTex(terrain->textureId,1);
		if(settings.SEA_FLOOR_TYPE==ROCK)	dataManager.bind("rock",2);
		else								dataManager.bind("sand",2);

		glUniform1i(glGetUniformLocation(s, "bumpMap"), 0);
		glUniform1i(glGetUniformLocation(s, "ground"), 1);
		glUniform1i(glGetUniformLocation(s, "tex"), 2);
		glUniform1f(glGetUniformLocation(s, "time"), gameTime());
		glUniform1f(glGetUniformLocation(s, "heightRange"), settings.HEIGHT_RANGE*2);
		glUniform3f(glGetUniformLocation(s, "center"),terrain->width()/2*size,settings.SEA_LEVEL-1000,terrain->width()/2*size);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(-4.0,-4.0);		glVertex3d(-4.0*terrain->width()*size,sl,-4.0*terrain->length()*size);
			glTexCoord2f( 5.0,-4.0);		glVertex3d(5.0*terrain->width()*size,sl,-4.0*terrain->length()*size);
			glTexCoord2f( 5.0, 5.0);		glVertex3d(5.0*terrain->width()*size,sl,5.0*terrain->length()*size);
			glTexCoord2f(-4.0, 5.0);		glVertex3d(-4.0*terrain->width()*size,sl,5.0*terrain->length()*size);
		glEnd();
		glPopMatrix();

		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		dataManager.bindShader(0);
	}
	double PoS(double x,double z,Vec3f center, double radius) const
	{
		return sqrt(radius*radius-(x-center.x)*(x-center.x)-(z-center.z)*(z-center.z))+center.y;
	}
	void waterDisp()
	{
		

		int tilesize = 32;

		double sl=settings.SEA_LEVEL;
		double m=terrain->width(), im=1.0/m;
		double r=sqrt((float)terrain->width()*terrain->width()+terrain->width()*terrain->width())/2;
		double r2=400000;
		double sqrt2=1.41421;
		Vec3f center(m/2*size,sl-r2*cos(asin(r/r2)),m/2*size);
		int i;
		double f;
		double x,z;

		disp[1] = glGenLists(1);
		glNewList(disp[1],GL_COMPILE);
		//	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);		glVertex3d(0,sl,0);
			glTexCoord2f(1.0,0.0);		glVertex3d(m*size,sl,0);
			glTexCoord2f(1.0,1.0);		glVertex3d(m*size,sl,m*size);
			glTexCoord2f(0.0,1.0);		glVertex3d(0,sl,m*size);
		glEnd();



		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(1.0,0);	glVertex3f(m*size,sl,0);
			glTexCoord2f(0.0,0.0);	glVertex3f(0,sl,0);
			for(i = 0; i <= m; i+=tilesize){
				glTexCoord2f(im*i,im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))));
				glVertex3f(i*size,sl,(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size);
			}
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(1.0,1.0);	glVertex3f(m*size,sl,m*size);
			glTexCoord2f(0,1.0);	glVertex3f(0,sl,m*size);
			for(i = 0; i <= m; i+=tilesize){
				glTexCoord2f(im*i,im*(-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m));
				glVertex3f(i*size,sl,(-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m)*size);
			}
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(0,1.0);	glVertex3f(0,sl,m*size);
			glTexCoord2f(0,0);		glVertex3f(0,sl,0);
			for(i = 0; i <= m; i+=tilesize){
				glTexCoord2f(im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))),im*i);
				glVertex3f((sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size,sl,i*size);
			}
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(1.0,1.0);	glVertex3f(m*size,sl,m*size);
			glTexCoord2f(1.0,0);	glVertex3f(m*size,sl,0);
			for(i = 0; i <= m; i+=tilesize){
				glTexCoord2f(im*(-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m),im*i);
				glVertex3f((-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m)*size,sl,i*size);
			}
		glEnd();



		//glEnable(GL_BLEND);
		//glEnable(GL_LINE_SMOOTH);
		//glLineWidth(1);

		//glVertex3f(x*size,sl,-3000);

		//for(ang=0;ang<PI*2;ang+=PI/256)
		//	glVertex3f((l*cos(ang)+size*m/2),sl-0.5,(l*sin(ang)+size*m/2));
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		//glBegin(GL_POLYGON);
		//for(ang=PI*3/4;ang<PI*5/4;ang+=PI/256)
		//	glVertex3f((l*cos(ang)+size*m/2),sl,(l*sin(ang)+size*m/2));
		//glEnd();
		//glBegin(GL_POLYGON);
		//for(ang=PI*5/4;ang<PI*7/4;ang+=PI/256)
		//	glVertex3f((l*cos(ang)+size*m/2),sl,(l*sin(ang)+size*m/2));
		//glEnd();
		//glBegin(GL_POLYGON);
		//for(ang=PI*7/4;ang<PI*9/4;ang+=PI/256)
		//	glVertex3f((l*cos(ang)+size*m/2),sl,(l*sin(ang)+size*m/2)); 
		//glEnd();

		//r=sqrt((float)m*m+m*m)/2;


		for(i=0;i<m;i+=tilesize)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(im*i,im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))));				glVertex3f(i*size,sl,(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size);
			glTexCoord2f(im*(i+tilesize),im*(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2))));	glVertex3f((i+tilesize)*size,sl,(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2)))*size);
			for(f=(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))-16;f>-m*10;f-=10)
			{
				glTexCoord2f(im*i,im*f);			glVertex3f(i*size,PoS(i*size,f*size,center,r2),f*size);
				glTexCoord2f(im*(i+tilesize),im*f);	glVertex3f((i+tilesize)*size,PoS((i+tilesize)*size,f*size,center,r2),f*size);
			}
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(im*i,im*-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m));				glVertex3f(i*size,sl,-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m)*size);
			glTexCoord2f(im*(i+tilesize),im*-(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2))-m));	glVertex3f((i+tilesize)*size,sl,-(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2))-m)*size);
			for(f=-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))+16+m;f<m*10;f+=10)
			{
				glTexCoord2f(im*i,im*f);		glVertex3f(i*size,PoS(i*size,f*size,center,r2),f*size);
				glTexCoord2f(im*(i+tilesize),im*f);	glVertex3f((i+tilesize)*size,PoS((i+tilesize)*size,f*size,center,r2),f*size);
			}
			glEnd();


			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))),im*i);				glVertex3f((sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size,sl,i*size);
			glTexCoord2f(im*(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2))),im*(i+tilesize));	glVertex3f((sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2)))*size,sl,(i+tilesize)*size);
			for(f=(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))-16;f>-m*10;f-=10)
			{
				glTexCoord2f(im*f,im*i);		glVertex3f(f*size,PoS(i*size,f*size,center,r2),i*size);
				glTexCoord2f(im*f,im*(i+tilesize));	glVertex3f(f*size,PoS((i+tilesize)*size,f*size,center,r2),(i+tilesize)*size);
			}
			glEnd();

			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(im*-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m),im*i);				glVertex3f(-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m)*size,sl,i*size);
			glTexCoord2f(im*-(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2))-m),im*(i+tilesize));	glVertex3f(-(sqrt2/2*r-sqrt(r*r-((i+tilesize)-m/2)*((i+tilesize)-m/2))-m)*size,sl,(i+tilesize)*size);
			for(f=-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))+16+m;f<m*10;f+=10)
			{
				glTexCoord2f(im*f,im*i);		glVertex3f(f*size,PoS(i*size,f*size,center,r2),i*size);
				glTexCoord2f(im*f,im*(i+tilesize));	glVertex3f(f*size,PoS((i+tilesize)*size,f*size,center,r2),(i+tilesize)*size);
			}
			glEnd();

		}

		for(z=-tilesize;z>-m*10;z-=tilesize)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for(x=0;x>-m*10;x-=tilesize)
			{
				glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
				glTexCoord2f(im*x,im*(z+tilesize));	glVertex3f(x*size,PoS(x*size,(z+tilesize)*size,center,r2),(z+tilesize)*size);
			}
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);
			for(x=m;x<m*10;x+=tilesize)
			{
				glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
				glTexCoord2f(im*x,im*(z+tilesize));	glVertex3f(x*size,PoS(x*size,(z+tilesize)*size,center,r2),(z+tilesize)*size);
			}
			glEnd();
		}
		for(z=m;z<m*10;z+=tilesize)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for(x=0;x>-m*10;x-=tilesize)
			{
				glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
				glTexCoord2f(im*x,im*(z+tilesize));	glVertex3f(x*size,PoS(x*size,(z+tilesize)*size,center,r2),(z+tilesize)*size);
			}
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);
			for(x=m;x<m*10;x+=tilesize)
			{
				glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
				glTexCoord2f(im*x,im*(z+tilesize));	glVertex3f(x*size,PoS(x*size,(z+tilesize)*size,center,r2),(z+tilesize)*size);
			}
			glEnd();
		}
		glPopMatrix();	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glEndList();

	}
	void drawWater4(Vec3f eye)
	{
		double sl=settings.SEA_LEVEL;
		double m=terrain->width(), im=1.0/m;
		double r=sqrt((float)terrain->width()*terrain->width()+terrain->width()*terrain->width())/2;
		double r2=400000;
		double sqrt2=1.41421;
		Vec3f center(m/2*size,sl-r2*cos(asin(r/r2)),m/2*size);

		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		//glEnable(GL_BLEND);

		int s=dataManager.getId("horizon");
		dataManager.bind("horizon");
			
		dataManager.bind("hardNoise",0);
		dataManager.bindTex(terrain->textureId,1);
		if(settings.SEA_FLOOR_TYPE==ROCK)	dataManager.bind("rock",2);
		else								dataManager.bind("sand",2);

		glUniform1i(glGetUniformLocation(s, "bumpMap"), 0);
		glUniform1i(glGetUniformLocation(s, "ground"), 1);
		glUniform1i(glGetUniformLocation(s, "tex"), 2);
		glUniform1f(glGetUniformLocation(s, "time"), gameTime());
		glUniform1f(glGetUniformLocation(s, "heightRange"), settings.HEIGHT_RANGE*2);
		glUniform3f(glGetUniformLocation(s, "center"),center.x,center.y,center.z);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCallList(disp[1]);

		//glPushMatrix();
		//glBegin(GL_QUADS);
		//	glTexCoord2f(0.0,0.0);		glVertex3d(0,sl,0);
		//	glTexCoord2f(1.0,0.0);		glVertex3d(m*size,sl,0);
		//	glTexCoord2f(1.0,1.0);		glVertex3d(m*size,sl,m*size);
		//	glTexCoord2f(0.0,1.0);		glVertex3d(0,sl,m*size);
		//glEnd();



		//glBegin(GL_TRIANGLE_FAN);
		//	glTexCoord2f(1.0,0);	glVertex3f(m*size,sl,0);
		//	glTexCoord2f(0.0,0.0);	glVertex3f(0,sl,0);
		//	for(i = 0; i <= m; i+=8){
		//		glTexCoord2f(im*i,im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))));
		//		glVertex3f(i*size,sl,(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size);
		//	}
		//glEnd();
		//glBegin(GL_TRIANGLE_FAN);
		//	glTexCoord2f(1.0,1.0);	glVertex3f(m*size,sl,m*size);
		//	glTexCoord2f(0,1.0);	glVertex3f(0,sl,m*size);
		//	for(i = 0; i <= m; i+=8){
		//		glTexCoord2f(im*i,im*(-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m));
		//		glVertex3f(i*size,sl,(-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m)*size);
		//	}
		//glEnd();
		//glBegin(GL_TRIANGLE_FAN);
		//	glTexCoord2f(0,1.0);	glVertex3f(0,sl,m*size);
		//	glTexCoord2f(0,0);		glVertex3f(0,sl,0);
		//	for(i = 0; i <= m; i+=8){
		//		glTexCoord2f(im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))),im*i);
		//		glVertex3f((sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size,sl,i*size);
		//	}
		//glEnd();
		//glBegin(GL_TRIANGLE_FAN);
		//	glTexCoord2f(1.0,1.0);	glVertex3f(m*size,sl,m*size);
		//	glTexCoord2f(1.0,0);	glVertex3f(m*size,sl,0);
		//	for(i = 0; i <= m; i+=8){
		//		glTexCoord2f(im*(-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m),im*i);
		//		glVertex3f((-sqrt2/2*r+sqrt(r*r-(i-m/2)*(i-m/2))+m)*size,sl,i*size);
		//	}
		//glEnd();
		//glPopMatrix();


		////glEnable(GL_BLEND);
		////glEnable(GL_LINE_SMOOTH);
		////glLineWidth(1);

		////glVertex3f(x*size,sl,-3000);

		////for(ang=0;ang<PI*2;ang+=PI/256)
		////	glVertex3f((l*cos(ang)+size*m/2),sl-0.5,(l*sin(ang)+size*m/2));
		////glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		////glBegin(GL_POLYGON);
		////for(ang=PI*3/4;ang<PI*5/4;ang+=PI/256)
		////	glVertex3f((l*cos(ang)+size*m/2),sl,(l*sin(ang)+size*m/2));
		////glEnd();
		////glBegin(GL_POLYGON);
		////for(ang=PI*5/4;ang<PI*7/4;ang+=PI/256)
		////	glVertex3f((l*cos(ang)+size*m/2),sl,(l*sin(ang)+size*m/2));
		////glEnd();
		////glBegin(GL_POLYGON);
		////for(ang=PI*7/4;ang<PI*9/4;ang+=PI/256)
		////	glVertex3f((l*cos(ang)+size*m/2),sl,(l*sin(ang)+size*m/2)); 
		////glEnd();

		////r=sqrt((float)m*m+m*m)/2;


		//for(i=0;i<m;i+=8)
		//{
		//	glBegin(GL_TRIANGLE_STRIP);
		//	glTexCoord2f(im*i,im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))));				glVertex3f(i*size,sl,(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size);
		//	glTexCoord2f(im*(i+8),im*(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2))));	glVertex3f((i+8)*size,sl,(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2)))*size);
		//	for(f=(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))-16;f>-m*10;f-=10)
		//	{
		//		glTexCoord2f(im*i,im*f);		glVertex3f(i*size,PoS(i*size,f*size,center,r2),f*size);
		//		glTexCoord2f(im*(i+8),im*f);	glVertex3f((i+8)*size,PoS((i+8)*size,f*size,center,r2),f*size);
		//	}
		//	glEnd();

		//	glBegin(GL_TRIANGLE_STRIP);
		//	glTexCoord2f(im*i,im*-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m));				glVertex3f(i*size,sl,-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m)*size);
		//	glTexCoord2f(im*(i+8),im*-(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2))-m));	glVertex3f((i+8)*size,sl,-(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2))-m)*size);
		//	for(f=-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))+16+m;f<m*10;f+=10)
		//	{
		//		glTexCoord2f(im*i,im*f);		glVertex3f(i*size,PoS(i*size,f*size,center,r2),f*size);
		//		glTexCoord2f(im*(i+8),im*f);	glVertex3f((i+8)*size,PoS((i+8)*size,f*size,center,r2),f*size);
		//	}
		//	glEnd();


		//	glBegin(GL_TRIANGLE_STRIP);
		//	glTexCoord2f(im*(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))),im*i);				glVertex3f((sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))*size,sl,i*size);
		//	glTexCoord2f(im*(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2))),im*(i+8));	glVertex3f((sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2)))*size,sl,(i+8)*size);
		//	for(f=(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))-16;f>-m*10;f-=10)
		//	{
		//		glTexCoord2f(im*f,im*i);		glVertex3f(f*size,PoS(i*size,f*size,center,r2),i*size);
		//		glTexCoord2f(im*f,im*(i+8));	glVertex3f(f*size,PoS((i+8)*size,f*size,center,r2),(i+8)*size);
		//	}
		//	glEnd();

		//	glBegin(GL_TRIANGLE_STRIP);
		//	glTexCoord2f(im*-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m),im*i);				glVertex3f(-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2))-m)*size,sl,i*size);
		//	glTexCoord2f(im*-(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2))-m),im*(i+8));	glVertex3f(-(sqrt2/2*r-sqrt(r*r-((i+8)-m/2)*((i+8)-m/2))-m)*size,sl,(i+8)*size);
		//	for(f=-(sqrt2/2*r-sqrt(r*r-(i-m/2)*(i-m/2)))+16+m;f<m*10;f+=10)
		//	{
		//		glTexCoord2f(im*f,im*i);		glVertex3f(f*size,PoS(i*size,f*size,center,r2),i*size);
		//		glTexCoord2f(im*f,im*(i+8));	glVertex3f(f*size,PoS((i+8)*size,f*size,center,r2),(i+8)*size);
		//	}
		//	glEnd();

		//}

		//for(z=-8;z>-m*10;z-=8)
		//{
		//	glBegin(GL_TRIANGLE_STRIP);
		//	for(x=0;x>-m*10;x-=8)
		//	{
		//		glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
		//		glTexCoord2f(im*x,im*(z+8));	glVertex3f(x*size,PoS(x*size,(z+8)*size,center,r2),(z+8)*size);
		//	}
		//	glEnd();
		//	glBegin(GL_TRIANGLE_STRIP);
		//	for(x=m;x<m*10;x+=8)
		//	{
		//		glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
		//		glTexCoord2f(im*x,im*(z+8));	glVertex3f(x*size,PoS(x*size,(z+8)*size,center,r2),(z+8)*size);
		//	}
		//	glEnd();
		//}
		//for(z=m;z<m*10;z+=8)
		//{
		//	glBegin(GL_TRIANGLE_STRIP);
		//	for(x=0;x>-m*10;x-=8)
		//	{
		//		glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
		//		glTexCoord2f(im*x,im*(z+8));	glVertex3f(x*size,PoS(x*size,(z+8)*size,center,r2),(z+8)*size);
		//	}
		//	glEnd();
		//	glBegin(GL_TRIANGLE_STRIP);
		//	for(x=m;x<m*10;x+=8)
		//	{
		//		glTexCoord2f(im*x,im*z);		glVertex3f(x*size,PoS(x*size,z*size,center,r2),z*size);
		//		glTexCoord2f(im*x,im*(z+8));	glVertex3f(x*size,PoS(x*size,(z+8)*size,center,r2),(z+8)*size);
		//	}
		//	glEnd();
		//}
		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		dataManager.bindShader(0);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}
	//void drawWater4(Vec3f eye)
	//{
	//	for(float z = -0.5f; z <= 0.5; z+=0.01) 
	//	{
	//		glBegin(GL_TRIANGLE_STRIP);
	//		for(float x = -0.5; x <= 0.5; x+=0.01) 
	//		{
	//			glColor4f(1,1,1,4*waterOpacity(terrain->getHeight(x, z)));
	//			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,x/tileSize+t,z/tileSize);
	//			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,x/tileSize,z/tileSize+t);
	//			glVertex3f(x*size,sl,z*size);

	//			glColor4f(1,1,1,4*waterOpacity(terrain->getHeight(x, z+1)));
	//			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,x/tileSize+t,(z+1)/tileSize);
	//			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,x/tileSize,(z+1)/tileSize+t);
	//			glVertex3f(x*size,sl,(z+1.0)*size);
	//		}
	//		glEnd();
	//	}
	//}
	void seaFloor()
	{
		dataManager.bind("sea floor");

		dataManager.bind("rock");
		glUniform1i(glGetUniformLocation(dataManager.getId("sea floor"), "tex"), 0);
		glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);		glVertex3f( -terrain->width()*size,-settings.HEIGHT_RANGE/2, -terrain->length()*size);
			glTexCoord2f(5,0);		glVertex3f(2*terrain->width()*size,-settings.HEIGHT_RANGE/2, -terrain->length()*size);
			glTexCoord2f(5,5);		glVertex3f(2*terrain->width()*size,-settings.HEIGHT_RANGE/2,2*terrain->length()*size);
			glTexCoord2f(0,5);		glVertex3f( -terrain->width()*size,-settings.HEIGHT_RANGE/2,2*terrain->length()*size);
		glEnd();
		glPopMatrix();
		dataManager.bindTex(0);
		dataManager.bindShader(0);
	}
	//void texturedQuad(int t,float x1,float x2,float y1,float y2)
	//{
	//	glBindTexture(GL_TEXTURE_2D, t);
	//	glBegin(GL_QUADS);
	//		glTexCoord2f(0,1);	glVertex2f(x1,y1);
	//		glTexCoord2f(0,0);	glVertex2f(x1,y2);
	//		glTexCoord2f(1,0);	glVertex2f(x2,y2);
	//		glTexCoord2f(1,1);	glVertex2f(x2,y1);
	//	glEnd();
	//}
	void texturedQuad(string t,float x,float y,float width,float height)
	{
		dataManager.bind(t);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x,y);
			glTexCoord2f(0,0);	glVertex2f(x,y+height);
			glTexCoord2f(1,0);	glVertex2f(x+width,y+height);
			glTexCoord2f(1,1);	glVertex2f(x+width,y);
		glEnd();
	}
	//void drawgauge(float x1,float y1,float x2,float y2,int texBack,int texFront,float rBack,float rFront)
	//{
	//	glColor4f(1,1,1,0.22);
	//	glPushMatrix();
	//	glTranslatef((x1+x2)/2,(y1+y2)/2,0);
	//	glRotatef(rBack,0,0,1);
	//	glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
	//	glBindTexture(GL_TEXTURE_2D, tex[texBack]);//dial back
	//	for(float x=-0.5;x<0.9;x+=0.5)
	//	{
	//		for(float y=-0.5;y<0.9;y+=0.5)
	//		{
	//			glBegin(GL_QUADS);
	//				glTexCoord2f(0,1);	glVertex2f(x1+x,y1+y);
	//				glTexCoord2f(0,0);	glVertex2f(x1+x,y2+y);
	//				glTexCoord2f(1,0);	glVertex2f(x2+x,y2+y);
	//				glTexCoord2f(1,1);	glVertex2f(x2+x,y1+y);
	//			glEnd();
	//		}
	//	}
	//	glPopMatrix();

	//	glPushMatrix();
	//	glTranslatef((x1+x2)/2,(y1+y2)/2,0);
	//	glRotatef(rFront,0,0,1);
	//	glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
	//	glBindTexture(GL_TEXTURE_2D, tex[texFront]);//dial front
	//	for(float x=-0.5;x<0.9;x+=0.5)
	//	{
	//		for(float y=-0.5;y<0.9;y+=0.5)
	//		{
	//			glBegin(GL_QUADS);
	//				glTexCoord2f(0,1);	glVertex2f(x1+x,y1+y);
	//				glTexCoord2f(0,0);	glVertex2f(x1+x,y2+y);
	//				glTexCoord2f(1,0);	glVertex2f(x2+x,y2+y);
	//				glTexCoord2f(1,1);	glVertex2f(x2+x,y1+y);
	//			glEnd();
	//		}
	//	}
	//	glPopMatrix();
	//}
	void drawgauge(float x1,float y1,float x2,float y2,string texBack,string texFront,float rBack,float rFront)
	{
		glColor4f(1,1,1,0.22);
		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(rBack,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		dataManager.bind(texBack);
		for(float x=-0.5;x<0.9;x+=0.5)
		{
			for(float y=-0.5;y<0.9;y+=0.5)
			{
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2f(x1+x,y1+y);
					glTexCoord2f(0,0);	glVertex2f(x1+x,y2+y);
					glTexCoord2f(1,0);	glVertex2f(x2+x,y2+y);
					glTexCoord2f(1,1);	glVertex2f(x2+x,y1+y);
				glEnd();
			}
		}
		glPopMatrix();

		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(rFront,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		dataManager.bind(texFront);
		for(float x=-0.5;x<0.9;x+=0.5)
		{
			for(float y=-0.5;y<0.9;y+=0.5)
			{
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2f(x1+x,y1+y);
					glTexCoord2f(0,0);	glVertex2f(x1+x,y2+y);
					glTexCoord2f(1,0);	glVertex2f(x2+x,y2+y);
					glTexCoord2f(1,1);	glVertex2f(x2+x,y1+y);
				glEnd();
			}
		}
		glPopMatrix();
	}
	void healthBar(float x, float y, float width, float height, float health, bool firstPerson)
	{
		x *=	0.00125*sw;
		y *=	0.00167*sh;
		width*= 0.00125*sw;
		height*=0.00167*sh;

		if(!firstPerson)
		{
			dataManager.bind("health bar");
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);	glVertex2f(x,y);
				glTexCoord2f(0,0);	glVertex2f(x,y+height);
				glTexCoord2f(1,0);	glVertex2f(x+width,y+height);
				glTexCoord2f(1,1);	glVertex2f(x+width,y);
			glEnd();
			dataManager.bindTex(0);
			glBegin(GL_QUADS);
				glVertex2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health),	y + height/25*7.25);
				glVertex2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health),	y + height/25*15.75);
				glVertex2f(x + width/150*125,												y + height/25*15.75);
				glVertex2f(x + width/150*125,												y + height/25*7.25);
			glEnd();
		}
		else
		{
			dataManager.bind("health");
			glUniform1f(glGetUniformLocation(dataManager.getId("health"), "health"), health);
			glUniform1f(glGetUniformLocation(dataManager.getId("health"), "angle"), 1.24f);
			glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x,y);
			glTexCoord2f(0,0);	glVertex2f(x,y+height);
			glTexCoord2f(1,0);	glVertex2f(x+width,y+height);
			glTexCoord2f(1,1);	glVertex2f(x+width,y);
			glEnd();
			dataManager.bindShader(0);
		}
	}
	void tiltMeter(float x1,float y1,float x2,float y2,float degrees)
	{
		x1 *=	0.00125*sw;
		y1 *=	0.00167*sh;
		x2*=	0.00125*sw;
		y2*=	0.00167*sh;

		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(degrees,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		dataManager.bind("tilt back");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glPopMatrix();

		dataManager.bind("tilt front");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
	}
	void speedMeter(float x1,float y1,float x2,float y2,float degrees)
	{
		x1 *=	0.00125*sw;
		y1 *=	0.00167*sh;
		x2*=	0.00125*sw;
		y2*=	0.00167*sh;

		dataManager.bind("speed");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();

		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(degrees,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		dataManager.bind("needle");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glPopMatrix();
	}
	void altitudeMeter(float x1,float y1,float x2,float y2,float degrees)
	{
		x1 *=	0.00125*sw;
		y1 *=	0.00167*sh;
		x2*=	0.00125*sw;
		y2*=	0.00167*sh;

		dataManager.bind("altitude");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();

		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(degrees,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		dataManager.bind("needle");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glPopMatrix();
	}
	virtual void drawRadar(int acplayer,float viewX=1.0,float viewY=0.5)
	{
		plane p = *(plane*)planes[players[acplayer].planeNum()];
		int x1,x2,y1,y2,xc,yc;
		float s=(float)sw/800.0;
		//x1=720*s*viewX,x2=x1+64*s,y1=430*s*viewY,y2=y1+64*s;

		//glColor4f(0.05,0.79,0.04,1);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		//glPushMatrix();
		//glTranslatef(-p.x/size+xc,-p.z/size+yc,0);
		//for(int z = p.z/size-8; z < terrain->length() && p.z/size+8; z++) {
		//	glBegin(GL_QUAD_STRIP);
		//		for(int x = p.x/size-8; x < terrain->width() && p.x/size+8; x++) {
		//			glVertex3f(x*4,		z * 4,		terrain->getHeight(x,z)/300.0);
		//			glVertex3f(x*4,		(z+1) * 4,	terrain->getHeight(x,z+1)/300.0);
		//			glVertex3f((x+1)*4,	z * 4,		terrain->getHeight(x+1,z)/300.0);
		//			glVertex3f((x+1)*4,	(z+1) * 4,	terrain->getHeight(x+1,z+1)/300.0);
		//		}
		//	glEnd();
		//}
		//glPopMatrix();
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		//glOrtho( 0, sw*viewX , sh*viewY-(1.0/viewY-1)*2 , 0, -1, 1 );

		int shader;
		if(players[acplayer].firstPerson()==true && !p.controled)
		{
			shader=dataManager.getId("radar");
			x2=(sw/2-s*160)*viewX,x1=x2-64*s,y2=(sh-s*80)*viewY,y1=y2-64*s;
			xc=(x1+x2)/2,yc=(y1+y2)/2;
		}
		else
		{
			shader=dataManager.getId("radar2");
			dataManager.bind("radarTex");
			x2=(sw-32)*viewX,x1=x2-64*s,y2=(sh-32)*viewY,y1=y2-64*s;
			xc=(x1+x2)/2,yc=(y1+y2)/2;
		}
		glUseProgram(shader);
		int location = glGetUniformLocation(shader, "radarTexture");
		glUniform1i(location, 0);
		location = glGetUniformLocation(shader, "radarAng");
		glUniform1f(location, radarAng);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glUseProgram(0);
		glColor4f(1,1,1,0.25);

		//dataManager.bind("radar plane");
		//Vec3f trans;
		//for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		//{
		//	int d=p.pos.distance((*i).second->pos);
		//	if(d<3733 && acplayer!=(*i).first && !(*i).second->dead)
		//	{
		//		glPushMatrix();
		//		glTranslatef(xc,yc,0);
		//		glRotatef(acosA(Vec3f(p.velocity.x,0,p.velocity.z).dot(Vec3f(0,0,1))).degrees()-180,0,0,1);
		//		glTranslatef(((*i).second->pos.x-p.pos.x)*(x2-xc)/(16384),((*i).second->pos.z-p.pos.z)*(x2-xc)/(16384),0);
		//		glRotatef(acosA(Vec3f((*i).second->velocity.x,0,(*i).second->velocity.z).dot(Vec3f(0,0,1))).degrees()-180,0,0,1);

		//		for(float x=-0.5;x<0.9;x+=0.5)
		//		{
		//			for(float y=-0.5;y<0.9;y+=0.5)
		//			{
		//				glBegin(GL_QUADS);
		//					glTexCoord2f(0,1);	glVertex2f(4+x,4+y);
		//					glTexCoord2f(0,0);	glVertex2f(4+x,-4+y);
		//					glTexCoord2f(1,0);	glVertex2f(-4+x,-4+y);
		//					glTexCoord2f(1,1);	glVertex2f(-4+x,4+y);
		//				glEnd();
		//			}
		//		}
		//		glPopMatrix();
		//	}
		//}
		if(!players[acplayer].firstPerson() || p.controled)
		{
			glColor4f(1,1,1,1);
			dataManager.bind("radar frame");
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);	glVertex2f(x1,y1);
				glTexCoord2f(0,0);	glVertex2f(x1,y2);
				glTexCoord2f(1,0);	glVertex2f(x2,y2);
				glTexCoord2f(1,1);	glVertex2f(x2,y1);
			glEnd();
		}
	}

	void drawRadar(float x, float y, float width, float height,bool firstPerson)
	{
		x *=	0.00125*sw;
		y *=	0.00167*sh;
		width*= 0.00125*sw;
		height*=0.00167*sh;

		//plane p = *(plane*)planes[players[acplayer].planeNum()];
		int xc=x+width/2,yc=y+height/2;

		int shader;
		if(firstPerson)
		{
			shader=dataManager.getId("radar");
		}
		else
		{
			shader=dataManager.getId("radar2");
			dataManager.bind("radarTex");
		}

		glUseProgram(shader);
		glUniform1i(glGetUniformLocation(shader, "radarTexture"), 0);
		glUniform1f(glGetUniformLocation(shader, "radarAng"), radarAng);

		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x,y);
			glTexCoord2f(0,0);	glVertex2f(x,y+height);
			glTexCoord2f(1,0);	glVertex2f(x+width,y+height);
			glTexCoord2f(1,1);	glVertex2f(x+width,y);
		glEnd();
		glUseProgram(0);

		////dataManager.bind("radar plane");
		////Vec3f trans;
		////for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		////{
		////	int d=p.pos.distance((*i).second->pos);
		////	if(d<3733 && acplayer!=(*i).first && !(*i).second->dead)
		////	{															///NEEDS TO BE REDONE!!!
		////		glPushMatrix();
		////		glTranslatef(xc,yc,0);
		////		glRotatef(acosA(Vec3f(p.velocity.x,0,p.velocity.z).dot(Vec3f(0,0,1))).degrees()-180,0,0,1);
		////		glTranslatef(((*i).second->pos.x-p.pos.x)*(x2-xc)/(16384),((*i).second->pos.z-p.pos.z)*(x2-xc)/(16384),0);
		////		glRotatef(acosA(Vec3f((*i).second->velocity.x,0,(*i).second->velocity.z).dot(Vec3f(0,0,1))).degrees()-180,0,0,1);
		////		for(float px=-0.5;px<0.9;px+=0.5)
		////		{
		////			for(float py=-0.5;py<0.9;py+=0.5)
		////			{
		////				glBegin(GL_QUADS);
		////					glTexCoord2f(0,1);	glVertex2f(4+px,4+py);
		////					glTexCoord2f(0,0);	glVertex2f(4+px,-4+py);
		////					glTexCoord2f(1,0);	glVertex2f(-4+px,-4+py);
		////					glTexCoord2f(1,1);	glVertex2f(-4+px,4+py);
		////				glEnd();
		////			}
		////		}
		////		glPopMatrix();
		////	}
		////}

		if(!firstPerson)
		{
			glColor4f(1,1,1,1);
			dataManager.bind("radar frame");
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);	glVertex2f(x,y);
				glTexCoord2f(0,0);	glVertex2f(x,y+height);
				glTexCoord2f(1,0);	glVertex2f(x+width,y+height);
				glTexCoord2f(1,1);	glVertex2f(x+width,y);
			glEnd();
		}
	}
	void targeter(float x, float y, float apothem, Angle tilt)
	{
		float width = apothem*0.00125*sw;
		float height = apothem*0.00167*sh;
		x *=	0.00125*sw;
		y *=	0.00167*sh;

		glPushMatrix();
		glTranslatef(x,y,0);
		glRotatef(tilt.degrees(),0,0,-1);
		glTranslatef(-x,-y,0);
		dataManager.bind("tilt");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x-width,y-height);
			glTexCoord2f(0,0);	glVertex2f(x-width,y+height);
			glTexCoord2f(1,0);	glVertex2f(x+width,y+height);
			glTexCoord2f(1,1);	glVertex2f(x+width,y-height);
		glEnd();
		glPopMatrix();

		dataManager.bind("targeter");
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x-width,y+height);
			glTexCoord2f(0,0);	glVertex2f(x-width,y-height);
			glTexCoord2f(1,0);	glVertex2f(x+width,y-height);
			glTexCoord2f(1,1);	glVertex2f(x+width,y+height);
		glEnd();
	}
	//void targeter(float x,float y,float width,float height,const planeBase& p)
	//{
	//	Vec3f fwd=(frustum.ntl+frustum.ntr+frustum.nbl+frustum.nbr)/4.0-p.pos;
	//	float length=1.0/fwd.dot(p.targeter);
	//	Vec3f tar=p.targeter*length;
	//	tar+=p.pos;
	//	float tarX=dist_Point_to_Segment(tar,frustum.ntl,frustum.nbl)/frustum.ntl.distance(frustum.ntr);
	//	float tarY=dist_Point_to_Segment(tar,frustum.ntl,frustum.ntr)/frustum.ntl.distance(frustum.nbl);
	//	//texturedQuad("aimer",x+tarX*width-16,y+tarY*height-16,32,32);
	//	dataManager.bind("aimer");
	//	glBegin(GL_QUADS);
	//		glTexCoord2f(0,1);	glVertex2f(x+tarX*width-16,y+tarY*height-16);
	//		glTexCoord2f(0,0);	glVertex2f(x+tarX*width-16,y+tarY*height+16);
	//		glTexCoord2f(1,0);	glVertex2f(x+tarX*width+16,y+tarY*height+16);
	//		glTexCoord2f(1,1);	glVertex2f(x+tarX*width+16,y+tarY*height-16);
	//	glEnd();
	//}
	void planeIdBoxes(plane p, Vec3f eye) //must get 'eye' location instead of plane location to work in 3rd person
	{
		#ifdef PLANE_ID_BOXES
		glLineWidth(3);
		glBindTexture(GL_TEXTURE_2D,0);
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if(p.id!=(*i).second->id && 	frustum.sphereInFrustum( (*i).second->loc(),8) != FrustumG::OUTSIDE)
			{
				Vec3f tar=((*i).second->loc()-eye).normalize();
				Vec3f fwd=(frustum.ntl+frustum.ntr+frustum.nbl+frustum.nbr)/4.0-eye;
				float length=1.0/fwd.dot(tar);
				tar=tar*length;
				tar+=eye;//Vec3f(p.x,p.y,p.z);
				float tX=dist_Point_to_Segment(tar,frustum.ntl,frustum.nbl)/dist(frustum.ntl,frustum.ntr);
				float tY=dist_Point_to_Segment(tar,frustum.ntl,frustum.ntr)/dist(frustum.ntl,frustum.nbl);
				if(i->second->id<=1)
					glColor4f(0,1,0,1);
				else if(i->second->dead)
					glColor4f(0.5,0.5,0.5,1);
				else
					glColor4f(0.5,0,0,1);
				glBegin(GL_LINE_LOOP);
					glVertex2f(tX*sw-7,tY*sh*0.5+7);
					glVertex2f(tX*sw+7,tY*sh*0.5+7);
					glVertex2f(tX*sw+7,tY*sh*0.5-7);
					glVertex2f(tX*sw-7,tY*sh*0.5-7);
				glEnd();
			}
		}
		glLineWidth(1);
		glColor4f(1,1,1,1);

		#endif
	}
	virtual void drawHUD(int acplayer, Vec3f eye, Vec3f center, Vec3f up)=0;
	//virtual void drawOrthoView(int acplayer, Vec3f eye)
	//{
	//	plane p=*(plane*)planes[players[acplayer].planeNum()];
	//	float s=(float)sw/800.0;
	//	viewOrtho(sw,sh/2); //set view to ortho - start 2d

	//	glDisable(GL_DEPTH_TEST);

	//	glColor4f(1,1,1,1);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//

	//	if(players[acplayer].firstPerson() && !p.controled)
	//	{
	//		//targeter start
	//		Vec3f fwd=(frustum.ntl+frustum.ntr+frustum.nbl+frustum.nbr)/4.0-p.pos;
	//		float length=1.0/fwd.dot(p.targeter);
	//		Vec3f tar=p.targeter*length;
	//		tar+=p.pos;
	//		float tarX=dist_Point_to_Segment(tar,frustum.ntl,frustum.nbl)/frustum.ntl.distance(frustum.ntr);
	//		float tarY=dist_Point_to_Segment(tar,frustum.ntl,frustum.ntr)/frustum.ntl.distance(frustum.nbl);
	//		//targeter end

	//		texturedQuad("cockpit",0,0,sw,sh/2);//cockpit
	//		texturedQuad("aimer",tarX*sw-16,tarY*((float)sh*0.5-2)-16,32,32);
	//		drawgauge(16*s,215*s,80*s,279*s,"tilt back","tilt front",p.roll.degrees(),0);
	//		drawgauge(16*s,16*s,80*s,80*s,"speed","needle",0,p.accel.magnitude()*30.5+212);
	//		drawgauge(368*s,215*s,432*s,279*s,"altitude","needle",0,p.altitude);//(p.y-_terrain->getHeight(int(p.x/size), int(p.z/size)) * 10));

	//		//  ---
	//		/*glBindTexture(GL_TEXTURE_2D,0);
	//		float x1,y1,y2;
	//		if(frustum.ntl[1]<-75 && frustum.ntr[1]<-75)
	//		{	
	//			glColor4f(0,0,1,0.2);
	//			glBegin(GL_QUADS);
	//				glVertex2f(0,0);
	//				glVertex2f(0,sh/2);
	//				glVertex2f(sw,sh/2);
	//				glVertex2f(sw,0);
	//			glEnd();
	//		}
	//		else if(frustum.nbl[1]<-75 && frustum.nbr[1]<-75)
	//		{
	//			y1=(frustum.ntl[1]-frustum.nbl[1])/(-75-frustum.ntl[1])*sh/2;
	//			y2=(frustum.ntr[1]-frustum.nbr[1])/(-75-frustum.ntr[1])*sh/2;
	//			glColor4f(0,0,1,0.2);
	//			glBegin(GL_QUADS);
	//				glVertex2f(0,sh/2);
	//				glVertex2f(0,y1);
	//				glVertex2f(sw,y2);
	//				glVertex2f(sw,sh/2);
	//			glEnd();
	//		}
	//		else if(frustum.nbl[1]<-75)
	//		{
	//			y1=(frustum.ntl[1]-frustum.nbl[1])/(-75-frustum.ntl[1])*sh/2;
	//			x1=(frustum.nbr[1]-frustum.nbl[1])/(-75-frustum.nbr[1])*sh/2;
	//			glColor4f(0,0,1,0.2);
	//			glBegin(GL_TRIANGLES);
	//				glVertex2f(x1,sh/2);
	//				glVertex2f(0,y1);
	//				glVertex2f(0,sh/2);
	//			glEnd();
	//		}
	//		else if(frustum.nbr[1]<-75)
	//		{
	//			float y1=(frustum.ntr[1]-frustum.nbr[1])/(-75-frustum.ntr[1])*sh/2;
	//			float x1=(frustum.nbl[1]-frustum.nbr[1])/(-75-frustum.nbl[1])*sh/2;
	//			glColor4f(0,0,1,0.2);
	//			glBegin(GL_TRIANGLES);
	//				glVertex2f(x1,sh/2);
	//				glVertex2f(sw,y1);
	//				glVertex2f(sw,sh/2);
	//			glEnd();
	//		}
	//	}*/
	//	}

	//	planeIdBoxes(p,eye);

	//	texturedQuad("health bar",600*s,750*s,25*s,50*s);
	//	texturedQuad(0,(614.0+111.0*(p.health/p.maxHealth))*s,725.0*s,(25+8.5)*s,(25+8.5+7.25)*s);
	//	drawRadar(acplayer);

	//	//#ifdef _DEBUG
	//		if(fps<29.0)glColor4f(1,0,0,1);
	//		else glColor4f(0,0,0,1);
	//		glBindTexture(GL_TEXTURE_2D,0);
 //			//if(acplayer==0) RenderText(375,25,fps);
	//		//if(acplayer==0) Profiler.draw();
	//		//RenderText(175,25,(float)gameTime());
	//	//#endif


	//	glEnable(GL_DEPTH_TEST);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	viewPerspective();//end 2d
	//}
	void l3dBillboardGetUpRightVector(Vec3f &up,Vec3f &right) 
	{ //http://www.lighthouse3d.com/opengl/billboarding/
		
		float modelview[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		right.set(	modelview[0],modelview[4],modelview[8]);
		up.set(		modelview[1],modelview[5],modelview[9]);
	}
	void vertexArrayParticles()
	{
		if(newSmoke.getSize()==0) return;
		Vec3f up,right;
		Vec3f a,b,c,d;
		Vec3f p;
		l3dBillboardGetUpRightVector(up,right);
		SVertex *s=(SVertex*)malloc(newSmoke.getSize()*sizeof(SVertex)*4);

		for(int i=0;i<newSmoke.getSize();i++)
		{
			a = newSmoke.positions[i] - (right + up) * 65.0;
			b = newSmoke.positions[i] + (right - up) * 65.0;
			c = newSmoke.positions[i] + (right + up) * 65.0;
			d = newSmoke.positions[i] - (right - up) * 65.0;

			s[i*4+0].x=a[0];	s[i*4+0].y=a[1];	s[i*4+0].z=a[2];
			s[i*4+1].x=b[0];	s[i*4+1].y=b[1];	s[i*4+1].z=b[2];
			s[i*4+2].x=c[0];	s[i*4+2].y=c[1];	s[i*4+2].z=c[2];
			s[i*4+3].x=d[0];	s[i*4+3].y=d[1];	s[i*4+3].z=d[2];

			s[i*4+0].r=0.25;	s[i*4+0].g=0.25;	s[i*4+0].b=0.25;	s[i*4+0].a=0.5*newSmoke.life[i]*newSmoke.life[i];
			s[i*4+1].r=0.25;	s[i*4+1].g=0.25;	s[i*4+1].b=0.25;	s[i*4+1].a=0.5*newSmoke.life[i]*newSmoke.life[i];
			s[i*4+2].r=0.25;	s[i*4+2].g=0.25;	s[i*4+2].b=0.25;	s[i*4+2].a=0.5*newSmoke.life[i]*newSmoke.life[i];
			s[i*4+3].r=0.25;	s[i*4+3].g=0.25;	s[i*4+3].b=0.25;	s[i*4+3].a=0.5*newSmoke.life[i]*newSmoke.life[i];

			s[i*4+0].s=0;	s[i*4+0].t=1;
			s[i*4+1].s=1;	s[i*4+1].t=1;
			s[i*4+2].s=1;	s[i*4+2].t=0;
			s[i*4+3].s=0;	s[i*4+3].t=0;
		}
		unsigned short *indexes=(unsigned short*)malloc(newSmoke.getSize()*sizeof(unsigned short)*4);
		for(int i=0;i<newSmoke.getSize()*4;i++) indexes[i]=i;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(SVertex), &s[0].x);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex), &s[0].s);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT, sizeof(SVertex), &s[0].r);

		glEnable(GL_CULL_FACE);
		dataManager.bind("missile smoke");
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);

		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		glDrawRangeElements(GL_QUADS, 0, newSmoke.getSize()*4-1, newSmoke.getSize()*4, GL_UNSIGNED_SHORT, indexes);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glEnable(GL_NORMALIZE);
	//	glEnable(GL_LIGHTING);   Is it used?
		glDisable(GL_CULL_FACE); 

		free(s);
		//const GLubyte *i= gluErrorString(glGetError());
	}
	void vertexArrayExaust()
	{
		if(newExaust.getSize()==0) return;
		Vec3f up,right;
		Vec3f a,b,c,d;
		Vec3f p;
		l3dBillboardGetUpRightVector(up,right);
		SVertex *s=newExaust.getMem();

		for(int i=0;i<newExaust.getSize();i++)
		{
			a = newExaust.positions[i] - (right + up) * 15.0;
			b = newExaust.positions[i] + (right - up) * 15.0;
			c = newExaust.positions[i] + (right + up) * 15.0;
			d = newExaust.positions[i] - (right - up) * 15.0;

			s[i*4+0].x=a[0];	s[i*4+0].y=a[1];	s[i*4+0].z=a[2];
			s[i*4+1].x=b[0];	s[i*4+1].y=b[1];	s[i*4+1].z=b[2];
			s[i*4+2].x=c[0];	s[i*4+2].y=c[1];	s[i*4+2].z=c[2];
			s[i*4+3].x=d[0];	s[i*4+3].y=d[1];	s[i*4+3].z=d[2];

			s[i*4+0].r=0.5;	s[i*4+0].g=0.5;	s[i*4+0].b=0.5;	s[i*4+0].a=0.5*newExaust.life[i]*newExaust.life[i];
			s[i*4+1].r=0.5;	s[i*4+1].g=0.5;	s[i*4+1].b=0.5;	s[i*4+1].a=0.5*newExaust.life[i]*newExaust.life[i];
			s[i*4+2].r=0.5;	s[i*4+2].g=0.5;	s[i*4+2].b=0.5;	s[i*4+2].a=0.5*newExaust.life[i]*newExaust.life[i];
			s[i*4+3].r=0.5;	s[i*4+3].g=0.5;	s[i*4+3].b=0.5;	s[i*4+3].a=0.5*newExaust.life[i]*newExaust.life[i];

			s[i*4+0].s=0;	s[i*4+0].t=1;
			s[i*4+1].s=1;	s[i*4+1].t=1;
			s[i*4+2].s=1;	s[i*4+2].t=0;
			s[i*4+3].s=0;	s[i*4+3].t=0;
		}
		unsigned short *indexes=(unsigned short*)malloc(newExaust.getSize()*sizeof(unsigned short)*4);
		for(int i=0;i<newExaust.getSize()*4;i++) indexes[i]=i;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(SVertex), &s[0].x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex), &s[0].s);
		glColorPointer(4,GL_FLOAT, sizeof(SVertex), &s[0].r);

		glEnable(GL_CULL_FACE);
		dataManager.bind("missile smoke");
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);

		glDrawRangeElements(GL_QUADS, 0, newExaust.getSize()*4-1, newExaust.getSize()*4, GL_UNSIGNED_SHORT, indexes);
		glEnable(GL_NORMALIZE);
		glDisable(GL_CULL_FACE);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	//void billboardParticles()
	//{
	//	//							GL_VERTEX_PROGRAM_POINT_SIZE may be of use...
	//	Vec3f up,right;
	//	Vec3f a,b,c,d;
	//	Vec3f p;
	//	l3dBillboardGetUpRightVector(up,right);

	//	glEnable(GL_CULL_FACE); 
	//	glBindTexture(GL_TEXTURE_2D,tex[20]);

	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//	glBegin(GL_QUADS);		
	//	//for(int i=0;i<(signed int )smoke.size();i++)
	//	//{
	//	//	p=Vec3f(smoke[i].x,smoke[i].y,smoke[i].z);

	//	//	glColor4f(smoke[0].r,smoke[i].g,smoke[i].b,0.25*smoke[i].life/smoke[i].maxLife);
	//	//	a = p - (right + up) * 15.0;
	//	//	b = p + (right - up) * 15.0;
	//	//	c = p + (right + up) * 15.0;
	//	//	d = p - (right - up) * 15.0;

	//	//	glTexCoord2f(0,1);	glVertex3fv((float*)&a);
	//	//	glTexCoord2f(1,1);	glVertex3fv((float*)&b);
	//	//	glTexCoord2f(1,0);	glVertex3fv((float*)&c);
	//	//	glTexCoord2f(0,0);	glVertex3fv((float*)&d);
	//	//}

	//	for(int i=0;i<newSmoke.getSize();i++)
	//	{
	//		a = newSmoke.positions[i] - (right + up) * 15.0;
	//		b = newSmoke.positions[i] + (right - up) * 15.0;
	//		c = newSmoke.positions[i] + (right + up) * 15.0;
	//		d = newSmoke.positions[i] - (right - up) * 15.0;

	//		glColor4f(0.25,0.25,0.25,0.25*newSmoke.life[i]);

	//		glTexCoord2f(0,1);	glVertex3fv((float*)&a);
	//		glTexCoord2f(1,1);	glVertex3fv((float*)&b);
	//		glTexCoord2f(1,0);	glVertex3fv((float*)&c);
	//		glTexCoord2f(0,0);	glVertex3fv((float*)&d);
	//	}
	//	glEnd();
	//	
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//	glDisable(GL_CULL_FACE); 
	//}

	//void renderPointSprites(Vec3f e)
	//{
	//	glDisable(GL_DEPTH_TEST);
	//	//glDepthFunc(GL_ALWAYS);
	//	glBindTexture(GL_TEXTURE_2D,tex[10]);
	//	




	//	glEnable( GL_POINT_SPRITE );
	//	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	//	glEnable( GL_BLEND );	
	//	//glBlendFunc(GL_ONE,GL_ONE);

	//	float quadratic[] =  { 0.0f, 0.01f, 0.0f };
	//	float normal[] =  { 1.0f, 0.0f, 0.0f };
	//	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, quadratic );
	//	glPointSize( 2048 );
	//	glTexEnvf( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
	//	//glBegin(GL_POINTS);
	//	//for(int i=0;i<(signed int )fire.size();i++)
	//	//{
	//	//	glColor4f(0,0,0,0.3);
	//	//	glVertex3f(fire[i].x,fire[i].y,fire[i].z);
	//	//}
	//	//glEnd();

	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE );
	//	//glBegin(GL_POINTS);
	//	//for(int i=0;i<(signed int )fire.size();i++)
	//	//{
	//	//	glColor4f(fire[i].r,fire[i].g,fire[i].b,0.3);
	//	//		//int num=200/dist(e,Vec3f(fire[i].x,fire[i].y,fire[i].z))+5;
	//	//		//for(int l=0;l<num;l++)
	//	//		//{
	//	//		//	//location = glGetAttribLocation(shaders[2], "camDist");
	//	//		//	//glVertexAttrib1f(location, );
	//	//		//	glVertex3f(fire[i].x+rand()%200/10-10,fire[i].y+rand()%200/10-10,fire[i].z+rand()%200/10-10);
	//	//		//}
	//	//		glVertex3f(fire[i].x,fire[i].y,fire[i].z);
	//	//}
	//	//glEnd();
	//	////glPointParameterf( GL_POINT_SIZE_MAX, 4 );
	//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION,normal);

	//	glPointSize( 4 );
	//	for(int i=0;i<(signed int )exhaust.size();i++)
	//	{
	//		glColor4f(exhaust[i].r,exhaust[i].g,exhaust[i].b,0.4);
	//		glBegin(GL_POINTS);
	//			for(int l=0;l<4;l++)
	//				glVertex3f(exhaust[i].x+rand()%200/10-10,exhaust[i].y+rand()%200/10-10,exhaust[i].z+rand()%200/10-10);
	//		glEnd();
	//	}
	//	

	//	glPointSize(60);
	//	
	//	float quadratic2[] =  { 0.0f, 0.025f, 0.0f };
 //   	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic2 );
	//	//glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 300.0f );

	//	glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1  );
	//	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 64  );
	//	glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );



	//	glBindTexture(GL_TEXTURE_2D,tex[20]);
	//	for(int i=0;i<(signed int )smoke.size();i++)
	//	{
	//		glColor4f(smoke[i].r,smoke[i].g,smoke[i].b,0.25*smoke[i].life/smoke[i].maxLife);
	//		//glPointSize(1/(dist(e,(Vec3f(smoke[i].x,smoke[i].y,smoke[i].z)))/10));
	//		//glColor3f(smoke[i].r,smoke[i].g,smoke[i].b);
	//		glBegin(GL_POINTS);
	//			for(int l=0;l<1;l++)
	//				//glVertex3f(smoke[i].x+rand()%200/10-10,smoke[i].y+rand()%200/10-10,smoke[i].z+rand()%200/10-10);
	//				glVertex3f(smoke[i].x,smoke[i].y,smoke[i].z);
	//		glEnd();
	//	}
	//	//glUseProgram(0);
	//	glDisable(GL_POINT_SPRITE);
	//	glEnable(GL_DEPTH_TEST);
	//	glBindTexture(GL_TEXTURE_2D,0);
	//	glDisable(GL_BLEND);
	//	glPointSize(1);
	//	glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
	//	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 1.0f );
	//	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION,normal);
	//	//glDepthFunc(GL_LESS);
	//}
	void drawPlanes(int acplayer,Vec3f e,bool showBehind=false,bool showDead=false)
	{
		plane p=*(plane*)planes[players[acplayer].planeNum()];
		planeBase* cPlane;
		//if(showBehind)
		//{
		//	glDisable(GL_DEPTH_TEST);
		//	glDisable(GL_TEXTURE_2D);//one exeption!!!
		//	glDisable(GL_LIGHTING);

		//	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		//	{
		//		cPlane=(*i).second;
		//		Vec3f a=(*i).second->pos;
		//		if( (cPlane->player!=p.player || firstP[acplayer]==false || p.controled) && frustum.sphereInFrustum(a,8) != FrustumG::OUTSIDE && !cPlane->dead)
		//		{
		//			glPushMatrix(); 
		//				glTranslatef(a[0],a[1],a[2]);
		//				if(cPlane->controled) 	glRotatef(cPlane->angle,0,1,0);
		//				else 					glRotatef(cPlane->angle-cPlane->turn/3,0,1,0);
		//										glRotatef(cPlane->turn,0,0,1);
		//				if(cPlane->controled)	glRotatef(cPlane->climbAng,1,0,0);
		//				else 					glRotatef(atan(-cPlane->climb/cPlane->acceleration)*50,1,0,0);
		//				glScalef(5,5,5);

		//				if(cPlane->team!=p.team)		glColor3f(1,0,0);
		//				else 						glColor3f(0,1,0);

		//				if(dist_squared(a,e)<2500000)
		//					glCallList(model[0]);
		//				else if(!(*i).second->dead)
		//					glCallList(model[2]);

		//			glPopMatrix();
		//		}
		//	}
		//	glEnable(GL_LIGHTING);
		//	glEnable(GL_TEXTURE_2D);
		//	glEnable(GL_DEPTH_TEST);
		//}

		glColor3f(1,1,1);
		//Vec3f axis;
		Angle roll;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			cPlane=(*i).second;
			Vec3f a=(*i).second->pos;
			if((cPlane->id!=p.id || !players[acplayer].firstPerson() || p.controled) && frustum.sphereInFrustum(a,8) != FrustumG::OUTSIDE && (showDead || !cPlane->dead))
			{
				glPushMatrix(); 
					glTranslatef(a.x,a.y,a.z);

					//axis=cPlane->velocity.normalize();
					//roll=acosA((cPlane->normal.normalize()).dot(Vec3f(0,1,0)));
					//glRotatef(-ang.degrees(),axis.x,axis.y,axis.z);

					//axis=   cPlane->velocity.normalize() .cross(Vec3f(0,0,1)) .normalize();
					//ang=acosA((cPlane->velocity.normalize()).dot(Vec3f(0,0,1)));
					//glRotatef(-ang.degrees(),axis.x,axis.y,axis.z);
					
					//glRotatef(atan2A(cPlane->velocity.x,cPlane->velocity.z).degrees()	,0,1,0);
					//glRotatef(-asinA(cPlane->velocity.y).degrees()						,1,0,0);
					//glRotatef(roll.degrees()											,0,0,1);

					//						glTranslatef(a[0],a[1],a[2]);
					//if(cPlane->controled) 	glRotatef(cPlane->angle,0,1,0);
					//else 					glRotatef(cPlane->angle-cPlane->turn/3,0,1,0);
					//						glRotatef(cPlane->turn,0,0,1);
					//if(cPlane->controled)	glRotatef(cPlane->climbAng,1,0,0);
					//else 					glRotatef(atan(-cPlane->climb/cPlane->acceleration)*50,1,0,0);
					
					
					//glRotatef(atan2A(cPlane->velocity.x,cPlane->velocity.z).degrees(),0,1,0);
					//glRotatef(asinA(-cPlane->velocity.y/cPlane->velocity.magnitude()).degrees(),1,0,0);
					//glRotatef(cPlane->roll.degrees(),0,0,1);


					Angle ang = acosA(cPlane->rotation.w);
					glRotatef((ang*2.0).degrees(), cPlane->rotation.x/sin(ang),cPlane->rotation.y/sin(ang),cPlane->rotation.z/sin(ang));

					glScalef(5,5,5);



					//if(dist_squared(a,e)<2000*2000)	glCallList(model[0]);
					//else if(!(*i).second->dead)		glCallList(model[2]);
					
					
					//m.draw();
					glCallList(planeModels[defaultPlane]);
					int ml=1;
					glEnable(GL_LIGHTING);
					for(vector<Settings::planeStat::hardpoint>::reverse_iterator m = settings.planeStats[defaultPlane].hardpoints.rbegin(); m!= settings.planeStats[defaultPlane].hardpoints.rend(); m++, ml++)
					{
						if(i->second->rockets.left>=ml)
						{
							glPushMatrix();
							glTranslatef(m->offset.x,m->offset.y,m->offset.z);
							glCallList(settings.missileStats[m->missileNum].dispList);
							glPopMatrix();
						}
					}
				glPopMatrix();
				//glPushMatrix();
				//glBegin(GL_LINES);
				//	glVertex3f(a.x,a.y,a.z);
				//	glVertex3f(a.x+cPlane->forward.x*100,a.y+cPlane->forward.y*100,a.z+cPlane->forward.z*100);

				//	glVertex3f(a.x,a.y,a.z);
				//	glVertex3f(a.x+cPlane->normal.x*100,a.y+cPlane->normal.y*100,a.z+cPlane->normal.z*100);
				//glEnd();
				//glPopMatrix();
			}
		}
	}
	void bulletTrails()
	{
		double time=gameTime();
		//float length;
		Vec3f start, end, end2;
		glLineWidth(2);
		glBegin(GL_LINES);
			for(vector<bullet>::iterator i=bullets.begin();i!=bullets.end();i++)
			{
				if(time > i->startTime)
				{
					start=i->startPos+i->velocity*(time-i->startTime)/1000;
					//length=max(min(frameLength,time-i->startTime)*i->velocity.magnitude()/1000,120)+20;
					end=i->startPos+i->velocity*(time-i->startTime)/1000-i->velocity.normalize()*40;
					end2=i->startPos+i->velocity*(time-i->startTime)/1000-i->velocity.normalize()*140;

					glColor4f(1.0,0.9,0.6,1.0);		glVertex3f(start.x,start.y,start.z);//was (0.6,0.6,0.6,0.7)...
					glColor4f(0.6,0.6,0.6,0.7);		glVertex3f(end.x,end.y,end.z);
					glColor4f(0.6,0.6,0.6,0.7);		glVertex3f(end.x,end.y,end.z);
					glColor4f(0.6,0.6,0.6,0.0);		glVertex3f(end2.x,end2.y,end2.z);
				}
			}
		glEnd();
		glLineWidth(1);
	}
	void drawScene2(int acplayer) {
		Profiler.startElement("beginRender");

		static map<int,double> lastDraw;
		double time=gameTime();


		plane p = *(plane*)planes[players[acplayer].planeNum()];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		GLfloat ambientColor[] = {0.85f, 0.85f, 0.85f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
		
		GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
		GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		
		float ly= 0.5;
		Vec3f cam;
		Vec3f e;
		Vec3f c;
		Vec3f u;
		if(!players[acplayer].firstPerson() || p.controled)
		{
			Vec3f vel2D = p.rotation * Vec3f(0,0,1); vel2D.y=0;
			e=Vec3f(p.pos.x - vel2D.normalize().x*175, p.pos.y + sin(45.0)*175 , p.pos.z - vel2D.normalize().z*175);
			if(p.controled) e=p.camera;
			c=Vec3f(p.pos.x + vel2D.normalize().x*175, p.pos.y, p.pos.z + vel2D.normalize().z*175);
			if(p.controled) c=p.center;
			u=Vec3f(0,1,0);

			//glMatrixMode(GL_PROJECTION);
			//glLoadIdentity();
			//glOrtho(-40000, 40000 , 15000 , -15000, -100, 20000 );
			//glMatrixMode(GL_MODELVIEW);
			//glLoadIdentity();
			//u=Vec3f(1,0,0);
			//e=Vec3f(64*size,15000,64*size);
			//c=Vec3f(64*size,900,64*size);

			//e = p.pos + Vec3f(250,400,250);
			//c = p.pos;
			//u= Vec3f(0,1,0);




			gluLookAt(e[0],e[1],e[2],c[0],c[1],c[2],u[0],u[1],u[2]);
			frustum.setCamDef(e,c,u);
			cam=e;
		}
		else
		{
			e=p.pos;//
			c=p.rotation * Vec3f(0,0,1)+e;//(p.x + sin(p.angle * DegToRad),p.y + p.climb,p.z + cos(p.angle * DegToRad));
			u=p.rotation * Vec3f(0,1,0);
			//upAndRight(p.rotation * Vec3f(0,0,1),p.roll,u,Vec3f());
			
			gluLookAt(e[0],e[1],e[2],c[0],c[1],c[2],u[0],u[1],u[2]);
			frustum.setCamDef(e,c,u);
			cam=e;
		}
		Profiler.endElement("beginRender");
		Profiler.startElement("dome");
		glEnable(GL_BLEND);		
		glColor3f(1,1,1);
		//sky dome
		glDepthRange(0.99,1.0);
		glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glTranslatef(e[0],e[1],e[2]);
		glScalef(3000,800,3000);
		glTranslatef(0,-0.7,0);
		dataManager.drawModel("sky dome");
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glDepthRange(0,1);
		Profiler.endElement("dome");
		Profiler.startElement("water");
		if(settings.MAP_TYPE==WATER && level == NULL)
		{
			//if(lowQuality)
				drawWater3(e);
			//else
			//	drawWater4(e);
		}
		Profiler.endElement("water");

		Profiler.startElement("ground");
		if(level != NULL)
			level->render();
		else
			glCallList(disp[0]);
		Profiler.endElement("ground");

		Profiler.startElement("models");
		//missiles
		glColor3f(1,0,0);
		Vec3f axis;
		for(int i=0;i<(signed int )missiles.size();i++)
		{
			glPushMatrix();
				glTranslatef(missiles[i].pos.x,missiles[i].pos.y,missiles[i].pos.z);
				axis=(missiles[i].velocity.normalize()).cross(Vec3f(0,0,1)).normalize();
				Angle a=acosA((missiles[i].velocity.normalize()).dot(Vec3f(0,0,1)));
				glRotatef(-a.degrees(),axis.x,axis.y,axis.z);
				//glScalef(0.25f,0.25f,0.15f);
				glScalef(5,5,5);
				//if(missiles[i].target!=-1)glColor3f(0,0,1);
				//else glColor3f(0,1,0);
				//glCallList(model[1]);
				glCallList(missiles[i].displayList);
			glPopMatrix();
		}
		glColor3f(0,1,0);
		for(int i=0;i<(signed int )turrets.size();i++)
		{
			glPushMatrix();
				glTranslatef(turrets[i]->pos.x,turrets[i]->pos.y,turrets[i]->pos.z);
				glScalef(100,100,100);
				//glCallList(model[4]);   need to load model
			glPopMatrix();
		}

		//planes
		drawPlanes(acplayer,e,false);
		Profiler.endElement("models");
#ifdef AI_TARGET_LINES 
		glBegin(GL_LINES);
		int t;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if(i->second->planeType & AI_PLANE)
			{
				t=((AIplane*)(i->second))->target;
				if(planes.find(t)!=planes.end())
				{
					glColor3f(1,0,0);
 					glVertex3f(i->second->x,i->second->y,i->second->z);
					glColor3f(0,1,0);
					glVertex3f(planes[t]->x,planes[t]->y,planes[t]->z);
				}
			}
		}
		glEnd();
#endif


		Profiler.startElement("trans");	
		glDepthMask(false);
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);

		bulletTrails();

		for(map<int,planeBase*>::iterator i=planes.begin();i!=planes.end();i++)
			i->second->drawExplosion(p.id==i->second->id);
		//vertexArrayParticles();

		vertexArrayExaust();

		//drawOrthoView(acplayer,e);


		//////////////2D/////////////
		glDisable(GL_DEPTH_TEST);
		glColor4f(1,1,1,1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		drawHUD(acplayer,e,c,u);
		glBindTexture(GL_TEXTURE_2D,0);
		#ifdef _DEBUG
			if(fps<29.0)glColor4f(1,0,0,1);
			else glColor4f(0,0,0,1);

 			if(acplayer==0) textManager->renderText(lexical_cast<string>(floor(fps)),sw/2-25,25);
				//textManager->renderText(string((char*)gluErrorString(glGetError())),sw/2-25,25);
			//if(acplayer==0) Profiler.draw();
		#endif
		glEnable(GL_DEPTH_TEST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		viewPerspective();
		///////////2D end//////////////

		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D,0);
		glDepthMask(true);
		Profiler.endElement("trans");

		lastDraw[acplayer] = time;
	}
	void updateWorld(float ms)
	{
		double time=gameTime();
		vector<int> toDelete;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if(!(*i).second->Update(ms))
				toDelete.push_back((*i).first);
		}
		for(vector<int>::iterator i=toDelete.begin();i!=toDelete.end();i++)
		{
			int l=planes.erase(*i);
			l=l;
		}
	//------------------------------------------------------------------------//
		Vec3f sp;
		Vec3f v;
		//Vec3f b2;
		Vec3f p;
		int l;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if(!(*i).second->dead)
			{
				p=(*i).second->pos;
				for(l=0;l<(signed int)bullets.size();l++)
				{
					sp=bullets[l].startPos;
					v=bullets[l].velocity;
					if(SegmentSphereIntersect(sp+v*(time-bullets[l].startTime-ms)/1000,sp+v*(time-bullets[l].startTime)/1000,p,12) && bullets[l].owner != (*i).first)
					{
						(*i).second->loseHealth(5.0);
						if((*i).second->dead)
						{
							if(bullets[l].owner==players[0].planeNum() && players[0].active()) players[0].addKill();
							if(bullets[l].owner==players[1].planeNum() && players[1].active()) players[1].addKill();
						}
						bullets.erase(bullets.begin()+l);
						l--;
					}
				}
				for(l=0;l<(signed int)missiles.size();l++)
				{
					if(p.distance(missiles[l].pos)<32 && missiles[l].owner != (*i).first)
					{
						(*i).second->loseHealth(35.0);
						if((*i).second->dead) 
						{
							if(missiles[l].owner==players[0].planeNum() && players[0].active()) players[0].addKill();
							if(missiles[l].owner==players[1].planeNum() && players[1].active()) players[1].addKill();
						}
						missiles.erase(missiles.begin()+l);
						l--;
					}
				}
			}
		}
		for(int i=0;i<(signed int)turrets.size();i++)
		{
			turrets[i]->Update(ms);
		}
		for(int i=0;i<(signed int)bullets.size();i++)
		{
			if(!bullets[i].update(ms))
				bullets.erase(bullets.begin()+i);
		}
		for(int i=0;i<(signed int)missiles.size();i++)
		{
			if(!missiles[i].Update(ms))
				missiles.erase(missiles.begin()+i);
		}
		//for(int i=0;i<(signed int)fire.size();i++)
		//{
		//	if(!fire[i].update(ms))
		//		fire.erase(fire.begin()+i);
		//}
		//for(int i=0;i<(signed int)smoke.size();i++)
		//{
		//	if(!smoke[i].update(ms))
		//		smoke.erase(smoke.begin()+i);
		//}
		//for(int i=0;i<(signed int)exhaust.size();i++)
		//{
		//	if(!exhaust[i].update(ms))
		//		exhaust.erase(exhaust.begin()+i);
		//}
		newSmoke.update(ms);
		newExaust.update(ms);
	}

	Level* level;
};

#include "twoPlayerVs.h"
#include "loading.h"
#include "onePlayer.h"
#include "mapBuilder.h"
extern modes *mode;