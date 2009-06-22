


class modes
{
protected:
	void ViewOrtho(int x, int y)				// Set Up An Ortho View
	{
		glMatrixMode(GL_PROJECTION);			// Select Projection
		glPushMatrix();							// Push The Matrix
		glLoadIdentity();						// Reset The Matrix
		glOrtho( 0, x , y , 0, -1, 1 );			// Select Ortho Mode
		glMatrixMode(GL_MODELVIEW);				// Select Modelview Matrix
		glPushMatrix();							// Push The Matrix
		glLoadIdentity();						// Reset The Matrix
	}
	void ViewPerspective(void)					// Set Up A Perspective View
	{
		glMatrixMode( GL_PROJECTION );			// Select Projection
		glPopMatrix();							// Pop The Matrix
		glMatrixMode( GL_MODELVIEW );			// Select Modelview
		glPopMatrix();							// Pop The Matrix
	}
	Terrain* loadTerrain(const char* filename, float height) {
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
	//void groundTex()//unused (no performance benefit)
	//{
	//	vector<vector<int> > t;
	//	SRenderTexture s;
	//	//s.create(8*size,8*size);
	//	glPushMatrix();
	//	glColor3f(1,1,1);
	//	glScalef(0.5,0.5,0.5);
	//
	//	glUseProgram(shaders[0]);
	//	int texture_location = glGetUniformLocation(shaders[0], "heightRange");
	//	glUniform1f(texture_location++, 600);

	//	glActiveTextureARB(GL_TEXTURE0_ARB);
	//	glEnable(GL_TEXTURE_2D);
	//	glBindTexture(GL_TEXTURE_2D,tex[15]);	
	//	glActiveTextureARB(GL_TEXTURE1_ARB);
	//	glEnable(GL_TEXTURE_2D);
	//	glBindTexture(GL_TEXTURE_2D,tex[16]);
	//	glActiveTextureARB(GL_TEXTURE2_ARB);
	//	glEnable(GL_TEXTURE_2D);
	//	glBindTexture(GL_TEXTURE_2D,tex[1]);	
	//	glActiveTextureARB(GL_TEXTURE3_ARB);
	//	glEnable(GL_TEXTURE_2D);
	//	glBindTexture(GL_TEXTURE_2D,tex[14]);

	//	int textures[4]={0,1,2,3};
	//	texture_location = glGetUniformLocation(shaders[0], "myTexture[0]");
	//	glUniform1i(texture_location++, textures[0]);
	//	glUniform1i(texture_location++, textures[1]);
	//	glUniform1i(texture_location++, textures[2]);
	//	glUniform1i(texture_location++, textures[3]);

	//	int tileSize=8,z,x;
	//	for(z = 0; z < terrain->length(); z+=tileSize) {
	//		vector<int>tmp;
	//		for(x = 0; x < terrain->width(); x+=tileSize) {
	//			s.create(8*size,8*size);
	//			s.beginRender();
	//			glOrtho(x,x+tileSize,z,z+tileSize,-400,400);
	//			for(int lz=0;lz<tileSize;lz++)
	//			{
	//				glBegin(GL_TRIANGLE_STRIP);
	//				for(int lx=0;lx<tileSize;lx++)
	//				{
	//					int mx=x+lx;
	//					int mz=z+lz;

	//					Vec3f normal = terrain->getNormal(mx, mz);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx)/tileSize,double(lz)/tileSize);
	//					glVertex3f(mx * size, terrain->getHeight(mx, mz), mz * size);

	//					normal = terrain->getNormal(mx, mz+1);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx)/tileSize,double(lz+1)/tileSize);
	//					glVertex3f(mx * size, terrain->getHeight(mx, mz+1), (mz+1) * size);

	//					normal = terrain->getNormal(mx+1, mz);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx+1)/tileSize,double(lz)/tileSize);
	//					glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz), mz * size);

	//					normal = terrain->getNormal(mx+1, mz+1);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx+1)/tileSize,double(lz+1)/tileSize);
	//					glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz+1), (mz+1) * size);
	//				}
	//				glEnd();
	//			}
	//			tmp.push_back(s.finishRender());
	//		}
	//		t.push_back(tmp);
	//	}
	//	glPopMatrix();
	//	glUseProgram(0);
	//	glActiveTextureARB(GL_TEXTURE0_ARB);
	//	glDisable(GL_TEXTURE_2D);
	//	glActiveTextureARB(GL_TEXTURE1_ARB);
	//	glDisable(GL_TEXTURE_2D);
	//	glActiveTextureARB(GL_TEXTURE2_ARB);
	//	glDisable(GL_TEXTURE_2D);	
	//	glActiveTextureARB(GL_TEXTURE3_ARB);
	//	glDisable(GL_TEXTURE_2D);
	//	glActiveTextureARB(GL_TEXTURE0_ARB);
	//	disp[0]= glGenLists(1);
	//	glNewList(disp[0],GL_COMPILE);
	//	for(z = 0; z < terrain->length(); z+=tileSize) {
	//		for(x = 0; x < terrain->width(); x+=tileSize) {
	//			glBindTexture(GL_TEXTURE_2D,t[z/tileSize][x/tileSize]);
	//			for(int lz=0;lz<tileSize;lz++)
	//			{
	//				glBegin(GL_TRIANGLE_STRIP);
	//				for(int lx=0;lx<tileSize;lx++)
	//				{
	//					int mx=x+lx;
	//					int mz=z+lz;

	//					Vec3f normal = terrain->getNormal(mx, mz);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx)/tileSize,double(lz)/tileSize);
	//					glVertex3f(mx * size, terrain->getHeight(mx, mz), mz * size);

	//					normal = terrain->getNormal(mx, mz+1);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx)/tileSize,double(lz+1)/tileSize);
	//					glVertex3f(mx * size, terrain->getHeight(mx, mz+1), (mz+1) * size);

	//					normal = terrain->getNormal(mx+1, mz);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx+1)/tileSize,double(lz)/tileSize);
	//					glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz), mz * size);

	//					normal = terrain->getNormal(mx+1, mz+1);
	//					glNormal3f(normal[0], normal[1], normal[2]);
	//					glTexCoord2f(double(lx+1)/tileSize,double(lz+1)/tileSize);
	//					glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz+1), (mz+1) * size);
	//				}
	//				glEnd();
	//			}
	//		}
	//	}
	//	glEndList();
	//}
	void DisplayLists()
	{
		int x;
		int z;
		disp[0]= glGenLists(1);
		glNewList(disp[0],GL_COMPILE);
			glPushMatrix();
			//glColor3f(1,1,1);
		
			glUseProgram(shaders[0]);
			int texture_location = glGetUniformLocation(shaders[0], "heightRange");
			glUniform1f(texture_location++, 300);

			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,tex[15]);	
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,tex[16]);
			glActiveTextureARB(GL_TEXTURE2_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,tex[1]);	
			glActiveTextureARB(GL_TEXTURE3_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,tex[14]);

			int textures[4]={0,1,2,3};
			texture_location = glGetUniformLocation(shaders[0], "myTexture[0]");
			glUniform1i(texture_location++, textures[0]);
			glUniform1i(texture_location++, textures[1]);
			glUniform1i(texture_location++, textures[2]);
			glUniform1i(texture_location++, textures[3]);

			int tileSize=8;
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
							glTexCoord2f(double(lx)/tileSize,double(lz)/tileSize);
							glVertex3f(mx * size, terrain->getHeight(mx, mz), mz * size);

							normal = terrain->getNormal(mx, mz+1);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(lx)/tileSize,double(lz+1)/tileSize);
							glVertex3f(mx * size, terrain->getHeight(mx, mz+1), (mz+1) * size);

							normal = terrain->getNormal(mx+1, mz);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(lx+1)/tileSize,double(lz)/tileSize);
							glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz), mz * size);

							normal = terrain->getNormal(mx+1, mz+1);
							glNormal3f(normal[0], normal[1], normal[2]);
							glTexCoord2f(double(lx+1)/tileSize,double(lz+1)/tileSize);
							glVertex3f((mx+1) * size, terrain->getHeight(mx+1, mz+1), (mz+1) * size);
						}
						glEnd();
					}

				}
			}
			glPopMatrix();
			glUseProgram(0);

			glActiveTextureARB(GL_TEXTURE3_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_2D);	
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glDisable(GL_TEXTURE_2D);
		glEndList();

	}
	void prepareMap(char *filename)
	{
		settings.loadMap(filename);
		terrain = loadTerrain(settings.MAP_FILE.c_str(), 300);
		DisplayLists();
	}

public:
	int newMode;
	virtual int update(int value)=0;
	virtual void draw()=0;
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
		//gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 1000.0);
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
		//Redisplay=true;
		return 50;
	}
	blankMode()
	{
		Redisplay=true;
		newMode=0;
	}
};
class dogFight: public modes
{
protected:
	FrustumG frustum;
	int acplayer;
	void render_tex_quad(int t,float x1,float x2,float y1,float y2)
	{
		glBindTexture(GL_TEXTURE_2D, t); // We set the active texture
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
	}
	void drawgauge(float x1,float y1,float x2,float y2,int texBack,int texFront,float rBack,float rFront)
	{
		glColor4f(1,1,1,0.22);
		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(rBack,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		glBindTexture(GL_TEXTURE_2D, tex[texBack]);//dial back
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
		glBindTexture(GL_TEXTURE_2D, tex[texFront]);//dial front
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
	void drawCocpitView(plane p)
	{
		if(sw==800 && sh==600)
		{
			render_tex_quad(tex[0],0,800,0,298);//cockpit
			render_tex_quad(tex[3],384,416,134,166);//targeter		
			drawgauge(16,215,80,279,5,4,p.turn,0);
			drawgauge(16,16,80,80,6,8,0,p.acceleration*45-20);
			drawgauge(368,215,432,279,7,8,0,p.altitude);//(p.y-_terrain->getHeight(int(p.x/size), int(p.z/size)) * 10));
			glColor3f(1,1,1);
			if(p.dead)
				render_tex_quad(tex[2],0,800,0,298);
		}
		else if(sw==1024 && sh==768)
		{
			render_tex_quad(tex[0],0,1024,0,384);//cockpit
			render_tex_quad(tex[3],491,532,171,212);//targeter		
			drawgauge(20,275,102,377,5,4,p.turn,0);
			drawgauge(20,20,102,102,6,8,0,p.acceleration*45-20);
			drawgauge(471,275,553,377,7,8,0,p.altitude);//(p.y-_terrain->getHeight(int(p.x/size), int(p.z/size)) * 10));
			glColor3f(1,1,1);
			if(p.dead)
				render_tex_quad(tex[2],0,1024,0,384);
		}
		else if(sw==1280 && sh==1024)
		{
			render_tex_quad(tex[0],0,1280,0,510);//cockpit
			render_tex_quad(tex[3],615,665,214,265);//targeter		
			drawgauge(26,344,128,446,5,4,p.turn,0);
			drawgauge(26,26,128,128,6,8,0,p.acceleration*45-20);
			drawgauge(589,344,691,446,7,8,0,p.altitude);//(p.y-_terrain->getHeight(int(p.x/size), int(p.z/size)) * 10));
			glColor3f(1,1,1);
			if(p.dead)
				render_tex_quad(tex[2],0,1280,0,510);
		}
	}
	void renderPointSprites()
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,tex[10]);
		glEnable( GL_BLEND );
		glEnable( GL_POINT_SPRITE );
		//renderPointSprites();
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		//float quadratic[] =  { 0.15f, 0.0f, 0.0000006f };
		float quadratic[] =  { 0.007f, 0.00045f, 0.0000009f };
		glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, quadratic );
		float maxSize = 0.0f;
		glGetFloatv( GL_POINT_SIZE_MAX, &maxSize );
		if( maxSize > 25.0f ) maxSize = 25.0f;
		//glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, 10.0f );
		glPointSize( maxSize );
		glPointParameterf( GL_POINT_SIZE_MAX, maxSize );
		glPointParameterf( GL_POINT_SIZE_MIN, 1.0f );
		glTexEnvf( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
		for(int i=0;i<(signed int )fire.size();i++)
		{
			glColor4f(fire[i].r,fire[i].g,fire[i].b,0.5);
			glBegin(GL_POINTS);
				for(int l=0;l<10;l++)
				{
					glVertex3f(fire[i].x+rand()%200/10-10,fire[i].y+rand()%200/10-10,fire[i].z+rand()%200/10-10);
				}
			glEnd();
		}
		glPointParameterf( GL_POINT_SIZE_MAX, 4 );
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		for(int i=0;i<(signed int )smoke.size();i++)
		{
			glColor4f(smoke[i].r,smoke[i].g,smoke[i].b,0.4);
			glBegin(GL_POINTS);
				for(int l=0;l<15;l++)
					glVertex3f(smoke[i].x+rand()%200/10-10,smoke[i].y+rand()%200/10-10,smoke[i].z+rand()%200/10-10);
			glEnd();
		}
		glDisable(GL_POINT_SPRITE);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);

	}

	void drawScene2() {
		plane p = *(plane*)planes[acplayer];
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
		if(firstP[acplayer]==false)
		{
			Vec3f e(p.x - sin(p.angle * DegToRad)*175, p.y + sin(45.0)*175, p.z - cos(p.angle * DegToRad)*175);
			Vec3f c(p.x + sin(p.angle * DegToRad)*175, p.y, p.z + cos(p.angle * DegToRad)*175);
			Vec3f u(0,1,0);
			
			gluLookAt(e[0],e[1],e[2],c[0],c[1],c[2],u[0],u[1],u[2]);
			frustum.setCamDef(e,c,u);

			cam[0]=p.x - sin(p.angle * DegToRad)*100;
			cam[1]=p.y + sin(45.0)*100;
			cam[2]=p.z - cos(p.angle * DegToRad)*100;
		}
		else
		{
			Vec3f e(p.x, p.y, p.z);
			Vec3f c(p.x + sin(p.angle * DegToRad),p.y + p.climb,p.z + cos(p.angle * DegToRad));
			Vec3f u(p.normal[0],p.normal[1],p.normal[2]);
			
			gluLookAt(e[0],e[1],e[2],c[0],c[1],c[2],u[0],u[1],u[2]);
			frustum.setCamDef(e,c,u);

			cam[0]=p.x;cam[1]=p.y;cam[2]=p.z;
		}
		glCallList(disp[0]);//draw ground
		renderPointSprites();//draw particles


		glColor3f(0.1f, 0.1f, 0.1f);
		glPointSize(1);
		glBegin(GL_POINTS);
			for(int i=0;i<(signed int )bullets.size();i++)
			{
				glVertex3fv(&bullets[i].x);
			}
		glEnd();
		glColor3f(1,0,0);
		for(int i=0;i<(signed int )missiles.size();i++)
		{

			glPushMatrix();
				glTranslatef(missiles[i].x,missiles[i].y,missiles[i].z);
				glRotatef(missiles[i].angle+180,0,1,0);
				glRotatef(missiles[i].climbAngle,1,0,0);
				glScalef(0.25f,0.25f,0.15f);
				glRotatef(-90,1,0,0);
				glCallList(model[1]);
			glPopMatrix();
		}
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			Vec3f a((*i).second->x, (*i).second->y, (*i).second->z);
			if(((*i).second->player!=p.player || firstP[acplayer]==false) && (frustum.sphereInFrustum(a,8) != FrustumG::OUTSIDE))
			{
				glPushMatrix(); 
					//glRotatef(90,1,0,0);
					glTranslatef((*i).second->x, (*i).second->y, (*i).second->z);
					glRotatef((*i).second->angle-(*i).second->turn/3,0,1,0);
					glRotatef((*i).second->turn,0,0,1);
					glRotatef(atan(-(*i).second->climb/(*i).second->acceleration)*50,1,0,0);
					glScalef(5,5,5);
					glCallList(model[0]);
				glPopMatrix();
			}
		}

		//for(map<int,turretBase*>::iterator i = turrets.begin(); i != turrets.end();i++)
		//{
		//		glColor3f(0.4f, 0.4f, 0.4f);
		//		if((*i).second->dead)
		//			glColor3f(1,0,0);
		//		glPushMatrix(); 
		//			glTranslatef((*i).second->x, (*i).second->y, (*i).second->z);
		//			glRotatef((*i).second->angle,0,1,0);
		//			glRotatef(-90,1,0,0);
		//			//glRotatef(atan(-(*i).second->climb/(*i).second->acceleration)*50,1,0,0);
		//			glScalef(25,25,25);
		//			glCallList(model[2]);
		//		glPopMatrix();
		//}

		ViewOrtho(sw,sh/2-2); //set view to ortho - start 2d

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glColor4f(1,1,1,1);

		if(firstP[acplayer]==true)
			drawCocpitView(p);
		int x1,x2,y1,y2,xc,yc;
		if(sw==800 && sh==600)
			x1=720,x2=784,y1=215,y2=279,xc=(x1+x2)/2,yc=(y1+y2)/2;
		else if(sw==1024 && sh==768)
			x1=720*1.28,x2=784*1.28,y1=215*1.28,y2=279*1.28,xc=(x1+x2)/2,yc=(y1+y2)/2;
		else if(sw==1280 && sh==1024)
			x1=720*1.6,x2=784*1.6,y1=215*1.6,y2=279*1.6,xc=(x1+x2)/2,yc=(y1+y2)/2;

		glBindTexture(GL_TEXTURE_2D, tex[9]);

		glUseProgram(shaders[1]);
		int location = glGetUniformLocation(shaders[1], "radarTexture");
		glUniform1i(location, 0);
		location = glGetUniformLocation(shaders[1], "radarAng");
		glUniform1f(location, radarAng);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glUseProgram(0);
		glColor4f(1,1,1,0.25);
		//glPushMatrix();
		//	glColor4f(1,1,1,0.25);
		//	glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		//	glRotatef(radarAng,0,0,1);
		//	glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		//	glBindTexture(GL_TEXTURE_2D, tex[10]);//dial front
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
		//glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, tex[11]);
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			int d=sqrt((p.x-(*i).second->x)*(p.x-(*i).second->x)+(p.z-(*i).second->z)*(p.z-(*i).second->z));
			if(d<2000 && acplayer!=(*i).first && !(*i).second->dead)
			{
				glPushMatrix();
				glTranslatef(xc,xc,0);
				glRotatef(p.angle-180,0,0,1);
				glTranslatef(((*i).second->x-p.x)*32/(2000),((*i).second->z-p.z)*32/(2000),0);
				glRotatef((*i).second->angle-180,0,0,1);

				for(float x=-0.5;x<0.9;x+=0.5)
				{
					for(float y=-0.5;y<0.9;y+=0.5)
					{
						glBegin(GL_QUADS);
							glTexCoord2f(0,1);	glVertex2f(4+x,4+y);
							glTexCoord2f(0,0);	glVertex2f(4+x,-4+y);
							glTexCoord2f(1,0);	glVertex2f(-4+x,-4+y);		
							glTexCoord2f(1,1);	glVertex2f(-4+x,4+y);
						glEnd();
					}
				}
				glPopMatrix(); //radar end
			}
		}
#ifdef DEBUG
		if(acplayer==0)
		{
			glColor4f(0,0,0,1.0);
			RenderText(375,25,fps);
		}
#endif
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		ViewPerspective();//end 2d
	}
	void updateWorld(int value)
	{
		vector<int> toDelete;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if(!(*i).second->Update(value))
			{
				toDelete.push_back((*i).first);
			}
		}
		for(vector<int>::iterator i=toDelete.begin();i!=toDelete.end();i++)
		{
			int l=planes.erase(*i);
			l=l;
		}
	//------------------------------------------------------------------------//
		Vec3f b1;
		Vec3f b2;
		Vec3f p;
		int l;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			p[0]=(*i).second->x;p[1]=(*i).second->y;p[2]=(*i).second->z;
			for(l=0;l<(signed int)bullets.size();l++)
			{
				b1[0]=bullets[l].x;b1[1]=bullets[l].y;b1[2]=bullets[l].z;
				if(dist(p,b1)<32 && bullets[l].owner != (*i).first)
				{
					(*i).second->die();
					bullets.erase(bullets.begin()+l);
					l--;
				}
			}
			for(l=0;l<(signed int)missiles.size();l++)
			{
				b1[0]=missiles[l].x;b1[1]=missiles[l].y;b1[2]=missiles[l].z;
				if(dist(p,b1)<32 && missiles[l].owner != (*i).first)
				{
					(*i).second->die();
					missiles.erase(missiles.begin()+l);
					l--;
				}
			}
		}
		for(int i=0;i<(signed int)bullets.size();i++)
		{
			if(!bullets[i].update((float)value/33.0))
				bullets.erase(bullets.begin()+i);
		}
		for(int i=0;i<(signed int)missiles.size();i++)
		{
			if(!missiles[i].Update((float)value/33.0))
				missiles.erase(missiles.begin()+i);
		}
		for(int i=0;i<(signed int)fire.size();i++)
		{
			if(!fire[i].update((float)value/33.0))
				fire.erase(fire.begin()+i);
		}
		for(int i=0;i<(signed int)smoke.size();i++)
		{
			if(!smoke[i].update((float)value/33.0))
				smoke.erase(smoke.begin()+i);
		}
	}
};

#include "twoPlayerVs.h"
#include "loading.h"
#include "onePlayer.h"
modes *mode;