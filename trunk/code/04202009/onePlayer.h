
class onePlayer: public modes
{
	public:
	void render_tex_quad(int t,float x1,float x2,float y1,float y2)
	{
		glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
		glEnable(GL_BLEND);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, t); // We set the active texture
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);		
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	}
	void drawgauge(float x1,float y1,float x2,float y2,int texBack,int texFront,float rBack,float rFront)
	{
		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(rBack,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		glBindTexture(GL_TEXTURE_2D, tex[texBack]);//dial back
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glTranslatef((x1+x2)/2,(y1+y2)/2,0);
		glRotatef(rFront,0,0,1);
		glTranslatef(-(x1+x2)/2,-(y1+y2)/2,0);
		glBindTexture(GL_TEXTURE_2D, tex[texFront]);//dial front
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(x1,y1);
			glTexCoord2f(0,0);	glVertex2f(x1,y2);
			glTexCoord2f(1,0);	glVertex2f(x2,y2);
			glTexCoord2f(1,1);	glVertex2f(x2,y1);
		glEnd();
		glPopMatrix();
	}
	void drawScene2() {
		plane p = *(plane*)planes[acplayer-1];
		plane o = *(plane*)planes[1];
		if(acplayer == 2){o = *(plane*)planes[0];}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		GLfloat ambientColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
		
		GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
		GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		
		float ly= 0.5;
		if(firstP[acplayer-1]==false)
		{
			gluLookAt(p.x - sin(p.angle * DegToRad)*140, p.y + ly*140, p.z - cos(p.angle * DegToRad)*140,
				p.x, p.y, p.z, 
				0,1,0);//p.normalX,p.normalY,p.normalZ);
		}
		else
		{
			gluLookAt(p.x, p.y, p.z,
				p.x + sin(p.angle * DegToRad),p.y + p.climb,p.z + cos(p.angle * DegToRad), 
				p.normal[0],p.normal[1],p.normal[2]);
		}
		//glColor4f(0.1f, 0.1f, 0.9f,1);
		//glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
		//glColor3f(1,1,1);
		//glBindTexture(GL_TEXTURE_2D, tex[1]); // We set the active texture
		//for(int z = 0; z < _terrain->length() - 1; z++) {
		//	//Makes OpenGL draw a triangle at every three consecutive vertices
		//	glBegin(GL_TRIANGLE_STRIP);
		//	
		//		for(int x = 0; x < _terrain->width(); x+=2) {
		//				Vec3f normal = _terrain->getNormal(x, z);
		//				glNormal3f(normal[0], normal[1], normal[2]);
		//				glTexCoord2f(0,0);
		//				glVertex3f(x * size, _terrain->getHeight(x, z) * 10, z * size);
		//				
		//				normal = _terrain->getNormal(x, z + 1);
		//				glNormal3f(normal[0], normal[1], normal[2]);
		//				glTexCoord2f(0,1);
		//				glVertex3f(x * size, _terrain->getHeight(x, z + 1) * 10, z * size + size);

		//				normal = _terrain->getNormal(x+1, z);
		//				glNormal3f(normal[0], normal[1], normal[2]);
		//				glTexCoord2f(1,0);
		//		     		glVertex3f((x+1) * size, _terrain->getHeight(x+1, z) * 10, z * size);
		//				
		//				normal = _terrain->getNormal(x+1, z + 1);
		//				glNormal3f(normal[0], normal[1], normal[2]);
		//				glTexCoord2f(1,1);
		//				glVertex3f((x+1) * size, _terrain->getHeight(x+1, z + 1) * 10, z * size + size);
		//		}
		//	glEnd();
		//}
		//glDisable(GL_TEXTURE_2D);

		//glPushMatrix();
		glCallList(disp[0]);
		//glPopMatrix();
		//glColor4f(0.3f, 0.3f, 0.3f,0.5f);
		glColor3f(0.1f, 0.1f, 0.1f);
		glEnable(GL_BLEND);
		glBegin(GL_POINTS);
			for(int i=0;i<(signed int )bullets.size();i++)
			{
				glVertex3f(bullets[i].x,bullets[i].y,bullets[i].z);
			}
		glEnd();
		glDisable(GL_BLEND);

		for(int i=0;i<(signed int )missiles.size();i++)
		{
			glColor3f(1,0,0);
			glPushMatrix();
			glTranslatef(missiles[i].x,missiles[i].y,missiles[i].z);
			glRotatef(missiles[i].angle+180,0,1,0);
			glRotatef(missiles[i].climbAngle,1,0,0);
			//glTranslatef(63,0,-10);
			glScalef(15,15,15);
			glCallList(model[1]);
			glPopMatrix();
			if(missiles[i].target!=-1)
			{
			glBegin(GL_LINES);
			glColor3f(0,1,0);
			glVertex3f(missiles[i].x, missiles[i].y, missiles[i].z);
			glVertex3f(planes[missiles[i].target]->x, planes[missiles[i].target]->y, planes[missiles[i].target]->z);
			glColor3f(0,0,1);
			glVertex3f(missiles[i].x, missiles[i].y, missiles[i].z);
			glVertex3f(missiles[i].x+sin(missiles[i].angle/180*PI)*100, missiles[i].y, missiles[i].z+cos(missiles[i].angle/180*PI)*100);
			glEnd();
			}
		}
		glColor3f(0.5,0.5,0.5);
		glBegin(GL_POINTS);
			for(int i=0;i<(signed int )particles.size();i++)
			{
				glVertex3f(particles[i].x,particles[i].y,particles[i].z);
			}
		glEnd();
		//glPushMatrix(); 
		//	glTranslatef(o.x, o.y, o.z);
		//
		//	glRotatef(o.angle-o.turn/3,0,1,0);
		//	glRotatef(o.turn,0,0,1);
		//	glCallList(model[0]);
		//glPopMatrix();
		for(int i=0;i<(signed int)planes.size();i++)
		{
			if(planes[i]->player!=p.player || firstP[acplayer-1]==false)
			{
				glColor3f(0.6f, 0.4f, 0.2f);
				if(planes[i]->hit)
					glColor3f(1,0,0);
				glPushMatrix(); 
					glTranslatef(planes[i]->x, planes[i]->y, planes[i]->z);
					glRotatef(planes[i]->angle-planes[i]->turn/3,0,1,0);
					glRotatef(planes[i]->turn,0,0,1);
					glCallList(model[0]);
				glPopMatrix();
				glColor3f(0, 0, 0);
				//float mX=(float)(int)planes[i]->x-int(planes[i]->x)%(int)size;
				//float mZ=(float)(int)planes[i]->z-int(planes[i]->z)%(int)size;
				//glBegin(GL_LINES);

				//	glVertex3f(planes[i]->x, planes[i]->y, planes[i]->z);
				//	glVertex3f(mX,_terrain->getHeight(int(planes[i]->x/size), int(planes[i]->z/size)) * 10,mZ);

				//	glVertex3f(planes[i]->x, planes[i]->y, planes[i]->z);
				//	glVertex3f(mX+size,_terrain->getHeight(int(planes[i]->x/size)+1, int(planes[i]->z/size)) * 10,mZ);

				//	glVertex3f(planes[i]->x, planes[i]->y, planes[i]->z);
				//	glVertex3f(mX,_terrain->getHeight(int(planes[i]->x/size), int(planes[i]->z/size)+1) * 10,mZ+size);

				//	glVertex3f(planes[i]->x, planes[i]->y, planes[i]->z);
				//	glVertex3f(mX+size,_terrain->getHeight(int(planes[i]->x/size)+1, int(planes[i]->z/size)+1) * 10,mZ+size);

				//glEnd();
			}
		}
		ViewOrtho(sw,sh/2-2); //set view to ortho - start 2d

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glColor3f(1,1,1);

		drawgauge(720,215,784,279,9,10,0,radarAng);  //radar start
		glPushMatrix();
		glTranslatef(752,247,0);
		glRotatef(p.angle-180,0,0,1);
		glTranslatef((o.x-p.x)/(size*2),(o.x-p.x)/(size*2),0);
		glRotatef(o.angle-180,0,0,1);
		glBindTexture(GL_TEXTURE_2D, tex[11]);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(4,4);
			glTexCoord2f(0,0);	glVertex2f(4,-4);
			glTexCoord2f(1,0);	glVertex2f(-4,-4);		
			glTexCoord2f(1,1);	glVertex2f(-4,4);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix(); //radar end

		#ifdef DEBUG
			DrawText(10,20,"x: ");
			DrawText((int)p.x);
			DrawText(10,40,"y: ");
			DrawText((int)p.y);
			DrawText(10,60,"z: ");
			DrawText((int)p.z);
			DrawText(10,80,"climb: ");
			DrawText(p.climb);
			DrawText(10,100,"angle: ");
			DrawText(p.angle);
			DrawText(10,120,"acceleration: ");
			DrawText(p.acceleration);
			DrawText(10,140,"turn: ");
			DrawText(p.turn);
			DrawText(10,160,"elevation: ");
			DrawText(p.altitude);
			glColor3f(0,0,0);
			DrawText(380,20,fps);
		#endif

		if(firstP[acplayer-1]==true){
			//ViewOrtho(sw,sh/2-2); //set view to ortho - start 2d

			glEnable(GL_TEXTURE_2D);
			//glEnable(GL_BLEND);
			glColor3f(1,1,1);
			glBindTexture(GL_TEXTURE_2D, tex[0]);//cockpit
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);	glVertex2f(0,0);
				glTexCoord2f(0,0);	glVertex2f(0,300);
				glTexCoord2f(1,0);	glVertex2f(800,300);		
				glTexCoord2f(1,1);	glVertex2f(800,0);
			glEnd();

			glBindTexture(GL_TEXTURE_2D, tex[3]);//targeter
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);	glVertex2f(384,134);
				glTexCoord2f(0,0);	glVertex2f(384,166);
				glTexCoord2f(1,0);	glVertex2f(416,166);		
				glTexCoord2f(1,1);	glVertex2f(416,134);
			glEnd();
			
			drawgauge(16,215,80,279,5,4,p.turn,0);
			drawgauge(16,16,80,80,6,8,0,p.acceleration*45-20);
			drawgauge(320,215,384,279,7,8,0,p.altitude);//(p.y-_terrain->getHeight(int(p.x/size), int(p.z/size)) * 10));
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			if(planes[acplayer-1]->hit)
			{
				render_tex_quad(tex[2],0,800,0,600);
			}
			
		}
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		ViewPerspective();//end 2d
	}
	void draw()//MAIN DRAW FUNCTION
	{

		frame++;
		Time=glutGet(GLUT_ELAPSED_TIME);
		
		if (Time - timebase > 1000) {
			fps = frame*1000.0/(Time-timebase);
	 		timebase = Time;		
			frame = 0;
		}
		glClearColor(0.5f,0.8f,0.9f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, sw, sh);
		glLoadIdentity();
		gluPerspective(80.0, (double)sw / ((double)sh),0.999, 10000.0);
		acplayer=1;
		drawScene2();
		glutSwapBuffers();
	}

	int update(int value) {
		acplayer = 1;
		if(input->axes[1]<-300 || input->keys[UP]) planes[0]->Accelerate();
		if(input->axes[1]>300 || input->keys[DOWN]) planes[0]->Brake();
		if(input->axes[2]<-300 || input->keys[_5]) planes[0]->Climb();
		if(input->axes[2]>300 || input->keys[_2]) planes[0]->Dive();
		if(input->axes[0]<-300 || input->keys[RIGHT]) planes[0]->Turn_Right();
		if(input->axes[0]>300 || input->keys[LEFT]) planes[0]->Turn_Left();
		if(input->keys[_9]) planes[0]->ShootMissile();
		if(input->joy[0] || input->keys[_0])planes[0]->Shoot();
		if(input->keys[F1]){firstP[0] = !firstP[0];input->keys[F1];}
	//--------------------------------------------------------------------//
		//acplayer = 2;
		//if(KeyPressed[W])planes[1]->Accelerate(); 
		//if(KeyPressed[S])planes[1]->Brake();
		//if(KeyPressed[U]) planes[1]->Climb();
		//if(KeyPressed[J]) planes[1]->Dive();
		//if(KeyPressed[A]) planes[1]->Turn_Left();
		//if(KeyPressed[D]) planes[1]->Turn_Right();
		//if(KeyPressed[B]) planes[1]->ShootMissile();
		//if(KeyPressed[SPACE]){planes[1]->Shoot();}
		//if(KeyPressed[F2]){firstP[1] = !firstP[1];KeyPressed[F2]=false;}
	//------------------------------------------------------------------------//
		for(int i=0;i<(signed int)planes.size();i++)
		{
			planes[i]->Update();
		}
	//------------------------------------------------------------------------//

		Vec3f b1;
		Vec3f b2;
		Vec3f p;
		int l;
		for(int i=0;i<NumPlayers;i++)
		{
			p[0]=planes[i]->x;p[1]=planes[i]->y;p[2]=planes[i]->z;
			for(l=0;l<(signed int)bullets.size();l++)
			{
				b1[0]=bullets[l].x;b1[1]=bullets[l].y;b1[2]=bullets[l].z;
				//b2[0]=bullets[l].x+bullets[l].vx*speed;
				//b2[1]=bullets[l].y+bullets[l].vy*speed;
				//b2[2]=bullets[l].z+bullets[l].vz*speed;
				if(d(p,b1)<64 && bullets[l].owner != i)
				{
					planes[i]->die();
					bullets.erase(bullets.begin()+l);
					l--;
				}
			}
			for(l=0;l<(signed int)missiles.size();l++)
			{
				b1[0]=missiles[l].x;b1[1]=missiles[l].y;b1[2]=missiles[l].z;
				//b2[0]=missiles[l].x+missiles[l].vx*speed;
				//b2[1]=missiles[l].y+missiles[l].vy*speed;
				//b2[2]=missiles[l].z+missiles[l].vz*speed;
				if(d(p,b1)<256 && missiles[l].owner != i)
				{
					planes[i]->die();
					missiles.erase(missiles.begin()+l);
					l--;
				}
			}
		}
		for(int i=0;i<(signed int)bullets.size();i++)
		{
			if(!bullets[i].update(1))
			{
				bullets.erase(bullets.begin()+i);
			}
		}
		for(int i=0;i<(signed int)missiles.size();i++)
		{
			if(!missiles[i].Update())
			{
				missiles.erase(missiles.begin()+i);
			}
		}
		for(int i=0;i<(signed int)particles.size();i++)
		{
			if(!particles[i].update(1))
			{
				particles.erase(particles.begin()+i);
			}
		}
		radarAng+=4;
		if(radarAng>360)
			radarAng-=360;

		//cout << gluErrorString(glGetError()) << "   " << gluErrorString(glGetError()) << "   "<< gluErrorString(glGetError()) << endl;

		glutPostRedisplay();
		glutForceJoystickFunc();
		return 25;
	}
	onePlayer()
	{
		newMode=0;
		planes.push_back(new plane(0,_terrain,&missiles,&bullets,&particles,&planes));	
		for(int i=1;i<10;i++)
		{
			planes.push_back(new AIplane(i,_terrain,&missiles,&bullets,&particles,&planes));	
		}
	}
};