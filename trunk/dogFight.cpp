
#include "main.h"

modeDogFight::modeDogFight(Level* lvl)
{
	world.create(lvl);

	//RADAR FRAME BUFFER OBJECTS
	glGenTextures(1, &radarTexture);
	glGenFramebuffersEXT(1, &radarFBO);

	glBindTexture(GL_TEXTURE_2D, radarTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 128, 128, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, radarFBO);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, radarTexture, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:							break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Attachment is NOT complete.",L"",0);
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:		MessageBox(NULL,L"[ERROR] Framebuffer incomplete: No image is attached to FBO.",L"",0);
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Attached images have different dimensions.",L"",0);
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:					MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Color attached images have different internal formats.",L"",0);
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Draw buffer.",L"",0);
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Read buffer.",L"",0);
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:						MessageBox(NULL,L"[ERROR] Unsupported by FBO implementation.",L"",0);
			return;

		default:
			MessageBox(NULL,L"[ERROR] Unknow FBO error.",L"",0);
			return;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
		return;

}
modeDogFight::~modeDogFight()
{
	world.destroy();
}

void modeDogFight::healthBar(float x, float y, float width, float height, float health, bool firstPerson)
{


	if(!firstPerson)
	{
		x *=	0.00125*sw;
		y *=	0.00167*sh;
		width*= 0.00125*sw;
		height*=0.00167*sh;
		graphics->drawOverlay(x,y,x+width,y+height,"health bar");



		//graphics->drawOverlay(Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*7.25), 
		//					Vec2f(x + width/150*125 - ((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health)), height/25*8.5), "noTexture");
		graphics->drawOverlay(Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*8.0), 
							Vec2f(x + width/150*125, y + height/25*16.0), "noTexture");
	}
	else
	{
		dataManager.bind("health");
		//static int uniform_health = glGetUniformLocation(dataManager.getId("health"), "health");
		//static int uniform_angle = glGetUniformLocation(dataManager.getId("health"), "angle");
		//glUniform1f(uniform_health, health);
		//glUniform1f(uniform_angle, 1.24f);
		dataManager.setUniform1f("health",health);
		dataManager.setUniform1f("angle",1.24f);
		graphics->drawOverlay(x,y,x+width,y+height,"noTexture");
		dataManager.unbindShader();
	}
}
void modeDogFight::tiltMeter(float x1,float y1,float x2,float y2,float degrees)
{
	x1 *=	0.00125*sw;
	y1 *=	0.00167*sh;
	x2 *=	0.00125*sw;
	y2 *=	0.00167*sh;

	graphics->drawRotatedOverlay(Vec2f(x1,y2),Vec2f(x2-x1,y2-y1),degrees * PI/180,"tilt back");
	graphics->drawOverlay(x1,y2,x2,y2,"tilt front");
}
void modeDogFight::radar(float x, float y, float width, float height,bool firstPerson, nPlane* p)
{
	//plane p = *(plane*)planes[players[acplayer].planeNum()];
	int xc=x+width/2,yc=y+height/2;

	//////////////////FBO/////////////////////////////////////
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, radarFBO);
	//glClearColor(0,0,0,0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glViewport(0,0,128,128);

	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//glOrtho( 0, 1 , 1 , 0, -1, 1 );
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();


	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glEnable(GL_POLYGON_SMOOTH);

	//Vec3f n;
	//glTranslatef(0.5,0.5,0);
	//glRotatef(p->direction.degrees(),0,0,-1);
	//glTranslatef(-0.5,-0.5,0);
	//
	//static float filter[] = {	0.09,	0.11,	0.09,
	//							0.11,	0.19,	0.11,
	//							0.09,	0.11,	0.09};

	//for(auto i = world.planes().begin(); i != world.planes().end(); i++)
	//{
	//	if(p->id != i->second->id && !i->second->dead)
	//	{
	//		n = (i->second->position - p->position) / 32000.0;

	//		glPushMatrix();
	//		glTranslatef(n.x,n.y,0);
	//		glRotatef(i->second->direction.degrees(),0,0,1);
	//		glTranslatef(-n.x,-n.y,0);

	//		glBegin(GL_TRIANGLES);
	//		for(int x = -1; x <= 1; x++)
	//		{
	//			for(int y = -1; y <= 1; y++)
	//			{
	//				glColor4f(1,1,1,filter[(x+1)+(y+1)*3]);
	//				glVertex2f(0.5 + (float)x/256 - n.x,		0.5 + (float)y/256 + n.z + 0.02);
	//				glVertex2f(0.5 + (float)x/256 - n.x - 0.02,	0.5 + (float)y/256 + n.z - 0.02);
	//				glVertex2f(0.5 + (float)x/256 - n.x + 0.02,	0.5 + (float)y/256 + n.z - 0.02);
	//			}
	//		}
	//		glEnd();

	//		glPopMatrix();
	//	}
	//}
	//glColor3f(1,1,1);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//glMatrixMode(GL_PROJECTION);
	//glPopMatrix();
	////glLoadIdentity();
	////glOrtho( 0, sw , sh , 0, -1, 1 );
	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
	////glLoadIdentity();
	//glViewport(0,0,sw,sh);
	/////////////////////////////////////////////////////////

	float radarAng = 45.0*world.time()/1000;
	radarAng = (radarAng/360.0 - floor(radarAng/360.0)) * 360;

	if(firstPerson)
	{
	//	static int radarAng = glGetUniformLocation(dataManager.getId("radar"), "radarAng");
		
		dataManager.bind("radar");
		dataManager.bindTex(radarTexture);

		dataManager.setUniform1f("radarAng", radarAng);
		//dataManager.setUniform1i("radarTexture", 0);
	//	glUniform1f(radarAng, radarAng);

		graphics->drawOverlay(x,y,x+width,y+height);
		dataManager.unbind("radar");
		dataManager.unbindTextures();

		glPushMatrix();
		glTranslatef(((1.0+x)+abs(width)/2)/2*sw,((-y+1.0)+abs(height)/2)/2*sh,0);
		//glScalef(abs(width)*sw,abs(height)*sh,1.0);
		//glScalef(-1,1,1.0);
		glRotatef(p->direction.degrees()+18.0,0,0,1);
	}
	else
	{
		x *=	0.00125*sw;
		y *=	0.00167*sh;
		width*= 0.00125*sw;
		height*=0.00167*sh;
	//	static int radarTexture = glGetUniformLocation(dataManager.getId("radar2"), "radarTexture");
	//	static int radarAng = glGetUniformLocation(dataManager.getId("radar2"), "radarAng");
		dataManager.bind("radar2");
		dataManager.bind("radarTex",0);
		dataManager.bindTex(radarTexture,1);

		dataManager.setUniform1f("radarAng", radarAng);
		dataManager.setUniform1i("backgroundTexture", 0);
		//dataManager.setUniform1i("radarTexture", 1);

		//glUniform1f(radarAng, radarAng);
		//glUniform1i(radarTexture, 0);

		graphics->drawOverlay(x,y,x+width,y+height);

		dataManager.unbindTextures();
		dataManager.unbindShader();

		glPushMatrix();

		glTranslatef(x+width/2,y+height/2,0);
		//glScalef(width,height,1.0);
		glRotatef(p->direction.degrees(),0,0,1);
	}

	if(firstPerson)	glColor3f(0.19,0.58,0.87);
	else			glColor3f(0.05,0.79,0.04);

	//glBegin(GL_TRIANGLES);
	//	glVertex2f(0,	-20);
	//	glVertex2f(-20,	20);
	//	glVertex2f(20,	20);
	//glEnd();

	Vec3f n;
	for(auto i = world.planes().begin(); i != world.planes().end(); i++)
	{
		n = (i->second->position - p->position) / 160.0;
		if(p->id != i->second->id && !i->second->dead && n.magnitude() < width/2)
		{

			glPushMatrix();
		//	glTranslatef(n.x,n.y,0);
			glRotatef(i->second->direction.degrees(),0,0,1);
			glTranslatef(-n.x,-n.z,0);

			glBegin(GL_TRIANGLES);
				glVertex2f(0.0,  -4.0);
				glVertex2f(-3.0, 4.0);
				glVertex2f(3.0,	 4.0);
			glEnd();

			glPopMatrix();
		}
		
	}
	glPopMatrix();
	glColor3f(1,1,1);


	if(!firstPerson)
	{
		graphics->drawOverlay(x,y,x+width,y+height,"radar frame");
	}
}
void modeDogFight::planeIdBoxes(nPlane* p, float vX, float vY, float vWidth, float vHeight) //must get 'eye' location instead of plane location to work in 3rd person
{
	if(!p->dead)
	{
		for(auto i = world.planes().begin(); i != world.planes().end();i++)
		{
			if(p->id!=i->second->id && !i->second->dead &&    frustum.sphereInFrustum(i->second->position,8) != FrustumG::OUTSIDE)
			{
				Vec2f s = frustum.project(i->second->position);
				double distSquared = i->second->position.distanceSquared(p->position);
				if(s.x > 0.0 && s.x < 1.0 && s.y > 0.0 && s.y < 1.0)
				{
					if(i->second->team==p->team)		glColor3f(0,1,0);
					else if(distSquared > 2000*2000)	glColor3f(0.6,0.5,0.5);
					else								glColor3f(0.5,0,0);

					graphics->drawOverlay(vX + (s.x - 0.006) * vWidth,vY + s.y * vHeight - 0.006 * vWidth,vX + (s.x + 0.006) * vWidth, vY + s.y * vHeight + 0.006 * vWidth,"target ring");
				}
			}
		}
		glColor3f(1,1,1);
	}
}
void modeDogFight::targeter(float x, float y, float apothem, Angle tilt)
{
	float width = apothem*0.00125*sw;
	float height = apothem*0.00167*sh;
	x *=	0.00125*sw;
	y *=	0.00167*sh;

	graphics->drawRotatedOverlay(Vec2f(x-width,y-height),Vec2f(width*2,height*2),tilt,"tilt");
	graphics->drawOverlay(x-width,y-height,x+width,y+height,"targeter");
}
//modes
void modeDogFight::drawPlanes(int acplayer,bool showBehind,bool showDead)
{
	nPlane* p=(nPlane*)world.objectList[players[acplayer].objectNum()];
	const map<objId,nPlane*>& planes = world.planes();
	nPlane* cPlane;

	//Vec3f axis;
	Angle roll;
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		cPlane=(*i).second;
		Vec3f a=(*i).second->position;
		if((cPlane->id!=players[acplayer].objectNum() || !players[acplayer].firstPerson() ||  p->controled) && frustum.sphereInFrustum(a,8) != FrustumG::OUTSIDE && (showDead || !cPlane->dead) && cPlane->death != nPlane::DEATH_EXPLOSION)
		{
			//Vec3f fwd = (*i).second->rotation * Vec3f(0,0,100);
			//glBegin(GL_LINES);
			//glVertex3f(a.x,a.y,a.z);
			//glVertex3f(a.x+fwd.x,a.y+fwd.y,a.z+fwd.z);
			//glEnd();
			glPushMatrix(); 
				glTranslatef(a.x,a.y,a.z);

				Angle ang = acosA(cPlane->rotation.w);
				glRotatef((ang*2.0).degrees(), cPlane->rotation.x/sin(ang),cPlane->rotation.y/sin(ang),cPlane->rotation.z/sin(ang));

				//if(dist_squared(a,e)<2000*2000)	glCallList(model[0]);
				//else if(!(*i).second->dead)		glCallList(model[2]);
				
				//m.draw();
				//glScalef(20,20,20);
				dataManager.draw(cPlane->type);
				//dataManager.draw("sphere");

				int ml=1;
				for(auto m = settings.planeStats[cPlane->type].hardpoints.rbegin(); m!= settings.planeStats[cPlane->type].hardpoints.rend(); m++, ml++)
				{
					if(i->second->rockets.left>=ml)
					{
						glPushMatrix();
						glTranslatef(m->offset.x,m->offset.y,m->offset.z);
						dataManager.draw(m->mType);
						glPopMatrix();
					}
				}
			glPopMatrix();
			//Vec3f p;
			//glLineWidth(3);
			//glColor4f(1,1,1,0.4);
			//glBegin(GL_LINE_STRIP);
			//int n=0;
			//for(double n = 0.0; n < 300.0 && i->second->planePath.startTime() < world.time()-n; n+=10.0)
			//{
			//	p = i->second->planePath(world.time()-n).position + i->second->planePath(world.time()-n).rotation * Vec3f(7.3,0,-2);
			//	glColor4f(1,1,1,0.4*(300.0-n)/300.0);
			//	glVertex3f(p.x,p.y,p.z);
			//}
			//glEnd();
			//glBegin(GL_LINE_STRIP);
			//n=0;
			//for(double n = 0.0; n < 300.0 && i->second->planePath.startTime() < world.time()-n; n+=10.0)
			//{
			//	p = i->second->planePath(world.time()-n).position + i->second->planePath(world.time()-n).rotation * Vec3f(-7.3,0,-2);
			//	glColor4f(1,1,1,0.4*(300.0-n)/300.0);
			//	glVertex3f(p.x,p.y,p.z);
			//}
			//glEnd();
			//glLineWidth(1);
		}
	}
}
void modeDogFight::drawBullets()
{
	double time = world.time();
	double lTime = time - 7.0;//world.time.getLastTime();

	Vec3f up, right;

	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	right.x = modelview[0];
	right.y = modelview[4];
	right.z = modelview[8];
	up.x = modelview[1];
	up.y = modelview[5];
	up.z = modelview[9];


	dataManager.bind("bullet");

	//float length;
	Vec3f start, end, end2;
	Vec3f v[4];

	glBegin(GL_QUADS);
		for(vector<bullet>::iterator i=world.bullets.begin();i!=world.bullets.end();i++)
		{
			if(time > i->startTime)
			{
				start=i->startPos+i->velocity*(time-i->startTime)/1000;
				//length=max(min(frameLength,time-i->startTime)*i->velocity.magnitude()/1000,120)+20;
				end=i->startPos+i->velocity*(time-i->startTime)/1000-i->velocity.normalize()*2;
				end2=i->startPos+i->velocity*max(lTime-i->startTime,0.0)/1000;

				v[0] = start + right*0.3;
				v[1] = start - right*0.3;
				v[2] = end2 + right*0.3;
				v[3] = end2 - right*0.3;
				glTexCoord2f(1,1);	glVertex3fv(&v[0].x);
				glTexCoord2f(1,0);	glVertex3fv(&v[1].x);
				glTexCoord2f(0,1);	glVertex3fv(&v[2].x);
				glTexCoord2f(0,0);	glVertex3fv(&v[3].x);
			}
		}
	glEnd();
	//glColor3f(1,1,1);

	dataManager.unbind("bullet");
}
void modeDogFight::drawHexCylinder(Vec3f center, float radius, float height, Color c)
{
	//Vec3f cn(world.ground()->sizeX()/2,0,world.ground()->sizeZ()/2);
	//float h=10000.0;
	//float radius = world.ground()->sizeX()*2.0;

 	dataManager.bind("hex grid shader");
	dataManager.setUniform1i("tex",0);
	dataManager.setUniform1f("minHeight",center.y);
	dataManager.setUniform1f("maxHeight",center.y+height);
	dataManager.setUniform1f("radius",radius);
	dataManager.setUniform3f("color",c.r,c.g,c.b);

	glPushMatrix();

	glTranslatef(center.x,center.y,center.z);
	glScalef(radius,height,radius);

	dataManager.drawCustomShader("cylinder");

	glPopMatrix();

	dataManager.unbindShader();
}
void modeDogFight::drawScene(int acplayer) 
{

	static map<int,double> lastDraw;
	double time=world.time();


	nPlane* p=(nPlane*)world.objectList[players[acplayer].objectNum()];
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat ambientColor[] = {0.85f, 0.85f, 0.85f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 0.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	float ly= 0.5;
	Vec3f cam;
	Vec3f e;
	Vec3f c;
	Vec3f u;
	if(!players[acplayer].firstPerson() || p->controled || p->dead)
	{
 		//Vec3f vel2D = p->rotation * Vec3f(0,0,1); vel2D.y=0;vel2D = vel2D.normalize();
		//e = p->position - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
		e = p->camera;
		//c = p->position + vel2D * 45.0;
		c=p->center;
		u = Vec3f(0,1,0);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//glOrtho(-40000, 40000 , 15000 , -15000, -100, 20000 );
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		//u=Vec3f(1,0,0);
		//e=Vec3f(64*size,15000,64*size);
		//c=Vec3f(64*size,900,64*size);

		//e = p->pos + Vec3f(250,400,250);
		//c = p->pos;
		//u= Vec3f(0,1,0);




		gluLookAt(e.x,e.y,e.z, c.x,c.y,c.z, u.x,u.y,u.z);
		frustum.setCamDef(e,c,u);
		cam=e;
	}
	else
	{
		e=p->position;
		c=p->rotation * Vec3f(0,0,1) + e;//(p->x + sin(p->angle * DegToRad),p->y + p->climb,p->z + cos(p->angle * DegToRad));
		u=p->rotation * Vec3f(0,1,0);
		//upAndRight(p->rotation * Vec3f(0,0,1),p->roll,u,Vec3f());
			
		gluLookAt(e.x,e.y,e.z, c.x,c.y,c.z, u.x,u.y,u.z);
		frustum.setCamDef(e,c,u);
		cam=e;
	}
	//sky dome
	glDepthRange(0.99,1.0);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glTranslatef(e[0],e[1],e[2]);
	glScalef(3000,800,3000);
	glTranslatef(0,-0.7,0);
	dataManager.draw("sky dome");
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glDepthRange(0,1);

	if(world.level != NULL)
		world.level->render(e);


	Vec3f axis;
	const map<objId,missile*>& missiles = world.missiles();
	for(auto i=missiles.begin();i != missiles.end();i++)
	{
		if(!i->second->awaitingDelete && !i->second->dead)
		{
			glPushMatrix();
				glTranslatef(i->second->position.x,i->second->position.y,i->second->position.z);

				Angle ang = acosA(i->second->rotation.w);
				glRotatef((ang*2.0).degrees(), i->second->rotation.x/sin(ang),i->second->rotation.y/sin(ang),i->second->rotation.z/sin(ang));
			//	axis=(i->second->velocity.normalize()).cross(Vec3f(0,0,1)).normalize();
			//	Angle a=acosA((i->second->velocity.normalize()).dot(Vec3f(0,0,1)));
			//	glRotatef(-a.degrees(),axis.x,axis.y,axis.z);
				dataManager.draw(i->second->type);
			glPopMatrix();

			nPlane* enemy = (nPlane*)world.objectList[i->second->target];
			if(enemy != NULL)
			{
				Vec3f destVec = (enemy->position - i->second->position).normalize();
				Vec3f fwd = i->second->rotation * Vec3f(0,0,1);
				Angle ang = acosA(destVec.dot(fwd));

				Vec3f v = i->second->position + i->second->rotation * Vec3f(0,0,1000);

				Vec3f w = i->second->position + destVec * 1000;

				glBegin(GL_LINES);
					glColor3f(0,1,0);
					glVertex3f(i->second->position.x,i->second->position.y,i->second->position.z);
					glVertex3f(v.x,v.y,v.z);

					glColor3f(1,0,0);
					glVertex3f(i->second->position.x,i->second->position.y,i->second->position.z);
					glVertex3f(w.x,w.y,w.z);
				glEnd();
				glColor3f(1,1,1);
			}
		}
	}

	//for(auto i = world.aaGuns().begin(); i != world.aaGuns().end();i++)
	//{
	//	glPushMatrix();	
	//	glTranslatef(i->second->position.x,i->second->position.y+50.0,i->second->position.z);
	//	glScalef(5,5,5);
	//	dataManager.draw("AA gun");       WE DO NOT HAVE A MODEL YET!!!!
	//	glPopMatrix();
	//}

	drawPlanes(acplayer,false,true);
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
	glColor3f(1,1,1);
#endif
	 glError();

	drawBullets();

	glDepthMask(false);

	drawHexCylinder(Vec3f(world.ground()->sizeX()/2,0,world.ground()->sizeZ()/2),world.ground()->sizeX(),20000, white);

	glDepthMask(false);//needed since drawHexCylinder sets depthMask to true

	graphics->render3D();
	particleManager.render();

	


	glDepthMask(true);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D,0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1.0/frustum.ratio, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//planeIdBoxes(p, 0.0, 0.0, 1.0, 1.0/frustum.ratio);
		
	glMatrixMode( GL_PROJECTION );			// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );			// Select Modelview

	glEnable(GL_DEPTH_TEST);
	
	lastDraw[acplayer] = time;
	
	glError();
}