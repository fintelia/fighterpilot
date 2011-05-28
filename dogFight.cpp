
#include "main.h"

modeDogFight::modeDogFight(Level* lvl)
{
	world.create(lvl);
}
modeDogFight::~modeDogFight()
{
	world.destroy();
}

void modeDogFight::healthBar(float x, float y, float width, float height, float health, bool firstPerson)
{
	x *=	0.00125*sw;
	y *=	0.00167*sh;
	width*= 0.00125*sw;
	height*=0.00167*sh;

	if(!firstPerson)
	{
		dataManager.bind("health bar");
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(x,y);
		glTexCoord2f(0,1);	glVertex2f(x,y+height);
		glTexCoord2f(1,1);	glVertex2f(x+width,y+height);
		glTexCoord2f(1,0);	glVertex2f(x+width,y);
		glEnd();
		//graphics->drawOverlay(Vec2f(x,y),Vec2f(width,height),"health bar");
		graphics->drawOverlay(Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*7.25), Vec2f(x + width/150*125 - ((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health)), height/25*8.5), "noTexture");
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
		//hShader->setValue1f("health",health);
		//hShader->bind();
		//graphics->drawOverlay(Vec2f(x,y),Vec2f(width,height));
		//hShader->unbind();
	}
}
void modeDogFight::tiltMeter(float x1,float y1,float x2,float y2,float degrees)
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
		glTexCoord2f(0,0);	glVertex2f(x1,y1);
		glTexCoord2f(0,1);	glVertex2f(x1,y2);
		glTexCoord2f(1,1);	glVertex2f(x2,y2);
		glTexCoord2f(1,0);	glVertex2f(x2,y1);
	glEnd();
	glPopMatrix();

	dataManager.bind("tilt front");
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(x1,y1);
		glTexCoord2f(0,1);	glVertex2f(x1,y2);
		glTexCoord2f(1,1);	glVertex2f(x2,y2);
		glTexCoord2f(1,0);	glVertex2f(x2,y1);
	glEnd();
}
void modeDogFight::radar(float x, float y, float width, float height,bool firstPerson)
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
		glTexCoord2f(0,0);	glVertex2f(x,y);
		glTexCoord2f(0,1);	glVertex2f(x,y+height);
		glTexCoord2f(1,1);	glVertex2f(x+width,y+height);
		glTexCoord2f(1,0);	glVertex2f(x+width,y);
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
			glTexCoord2f(0,0);	glVertex2f(x,y);
			glTexCoord2f(0,1);	glVertex2f(x,y+height);
			glTexCoord2f(1,1);	glVertex2f(x+width,y+height);
			glTexCoord2f(1,0);	glVertex2f(x+width,y);
		glEnd();
	}
}
void modeDogFight::planeIdBoxes(nPlane* p, float vX, float vY, float vWidth, float vHeight) //must get 'eye' location instead of plane location to work in 3rd person
{
	if(!p->dead)
	{
 		dataManager.bind("target ring");
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

					glBegin(GL_QUADS);
						glTexCoord2f(0.0,1.0);	glVertex2f(vX + (s.x - 0.006) * vWidth, vY + s.y * vHeight + 0.006 * vWidth);
						glTexCoord2f(1.0,1.0);	glVertex2f(vX + (s.x + 0.006) * vWidth, vY + s.y * vHeight + 0.006 * vWidth);
						glTexCoord2f(1.0,0.0);	glVertex2f(vX + (s.x + 0.006) * vWidth, vY + s.y * vHeight - 0.006 * vWidth);
						glTexCoord2f(0.0,0.0);	glVertex2f(vX + (s.x - 0.006) * vWidth, vY + s.y * vHeight - 0.006 * vWidth);
					glEnd();
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
	graphics->drawOverlay(Vec2f(x-width,y-height),Vec2f(width*2,height*2),"targeter");
}
void modeDogFight::drawExaust()
{
	int numSprites=0;
	float time = world.time();
	if(world.exaust.getSize()!=0)
	{
		for(int i=0;i<world.exaust.getSize();i++)
		{
			if(world.exaust.startTime[i] < time && time < world.exaust.endTime[i])
			{
				graphics->drawParticle(exaustParticleEffect,world.exaust.positions[i]+world.exaust.velocity[i]*(world.exaust.endTime[i]-world.exaust.startTime[i])/1000,0.5*(world.exaust.endTime[i]-time)/(world.exaust.endTime[i]-world.exaust.startTime[i]));
				numSprites++;
			}
		}
	}
}
void modeDogFight::drawPlanes(int acplayer,bool showBehind,bool showDead)
{
	nPlane* p=(nPlane*)world.objectList[players[acplayer].objectNum()];
	const map<objId,nPlane*>& planes = world.planes();
	nPlane* cPlane;

	glColor3f(1,1,1);
	//Vec3f axis;
	Angle roll;
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		cPlane=(*i).second;
		Vec3f a=(*i).second->position;
		if((cPlane->id!=players[acplayer].objectNum() || !players[acplayer].firstPerson() ||  p->controled) && frustum.sphereInFrustum(a,8) != FrustumG::OUTSIDE && (showDead || !cPlane->dead))
		{
			//glColor3f(0,0,0);
			//Vec3f fwd = (*i).second->rotation * Vec3f(0,0,100);
			//glBegin(GL_LINES);
			//glVertex3f(a.x,a.y,a.z);
			//glVertex3f(a.x+fwd.x,a.y+fwd.y,a.z+fwd.z);
			//glEnd();
			//glColor3f(1,1,1);
			glPushMatrix(); 
				glTranslatef(a.x,a.y,a.z);

				Angle ang = acosA(cPlane->rotation.w);
				glRotatef((ang*2.0).degrees(), cPlane->rotation.x/sin(ang),cPlane->rotation.y/sin(ang),cPlane->rotation.z/sin(ang));

				//if(dist_squared(a,e)<2000*2000)	glCallList(model[0]);
				//else if(!(*i).second->dead)		glCallList(model[2]);
				
				//m.draw();
				dataManager.draw(cPlane->type);

				int ml=1;
				glEnable(GL_LIGHTING);
				for(auto m = settings.planeStats[cPlane->type].hardpoints.rbegin(); m!= settings.planeStats[cPlane->type].hardpoints.rend(); m++, ml++)
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
	double lTime = time - 5.0;//world.time.getLastTime();
	//float length;
	Vec3f start, end, end2;
	glLineWidth(3);
	glBegin(GL_LINES);
		for(vector<bullet>::iterator i=world.bullets.begin();i!=world.bullets.end();i++)
		{
			if(time > i->startTime)
			{
				start=i->startPos+i->velocity*(time-i->startTime)/1000;
				//length=max(min(frameLength,time-i->startTime)*i->velocity.magnitude()/1000,120)+20;
				end=i->startPos+i->velocity*(time-i->startTime)/1000-i->velocity.normalize()*2;
				end2=i->startPos+i->velocity*max(lTime-i->startTime,0.0)/1000;

				glColor4f(1.0,0.9,0.6,1.0);		glVertex3f(start.x,start.y,start.z);//was (0.6,0.6,0.6,0.7)...
				glColor4f(0.6,0.6,0.6,1.0);		glVertex3f(end.x,end.y,end.z);
				glColor4f(0.6,0.6,0.6,1.0);		glVertex3f(start.x,start.y,start.z);
				glColor4f(0.6,0.6,0.6,0.0);		glVertex3f(end2.x,end2.y,end2.z);


			}
		}
	glEnd();
	glLineWidth(1);
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
	if(!players[acplayer].firstPerson() || p->controled)
	{
 		Vec3f vel2D = p->rotation * Vec3f(0,0,1); vel2D.y=0;vel2D = vel2D.normalize();
		e = p->position - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
		if(p->controled) e = p->camera;
		c = p->position + vel2D * 45.0;
		if(p->controled) c=p->center;
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
	glColor3f(1,1,1);
	//sky dome
	glDepthRange(0.99,1.0);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glTranslatef(e[0],e[1],e[2]);
	glScalef(3000,800,3000);
	glTranslatef(0,-0.7,0);
	dataManager.draw("sky dome");
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glDepthRange(0,1);

	//if(settings.MAP_TYPE==WATER && world.level == NULL)
	//{
	//	//if(lowQuality)
	//		//drawWater3(e);
	//		drawWater(e);
	//	//else
	//	//	drawWater4(e);
	//		
	//}
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0,1.0);
	if(world.level != NULL)
		world.level->render(e);
	//satellite();

	//glPolygonOffset(0.0,0.0);
	//glDisable(GL_POLYGON_OFFSET_FILL);
	glError();	//missiles
	glColor3f(1,0,0);
	Vec3f axis;
	const map<objId,missile*>& missiles = world.missiles();
	for(auto i=missiles.begin();i != missiles.end();i++)
	{
		if(!i->second->awaitingDelete && !i->second->dead)
		{
			glPushMatrix();
				glTranslatef(i->second->position.x,i->second->position.y,i->second->position.z);
				axis=(i->second->velocity.normalize()).cross(Vec3f(0,0,1)).normalize();
				Angle a=acosA((i->second->velocity.normalize()).dot(Vec3f(0,0,1)));
				glRotatef(-a.degrees(),axis.x,axis.y,axis.z);
				glCallList(i->second->displayList);
			glPopMatrix();
		}
	}

	for(auto i = world.aaGuns().begin(); i != world.aaGuns().end();i++)
	{
		glPushMatrix();	
		glTranslatef(i->second->position.x,i->second->position.y+50.0,i->second->position.z);
		glScalef(5,5,5);


		//dataManager.draw("AA gun");       WE DO NOT HAVE A MODEL YET!!!!
		glPopMatrix();
	}

	//glBindTexture(GL_TEXTURE_2D,0);
	//glBegin(GL_LINES);
	//for(auto i = world.aaGuns().begin(); i != world.aaGuns().end();i++)
	//{
	//	glColor3f(1,0,0);
	//	glVertex3f(i->second->position.x,i->second->position.y,i->second->position.z);
	//	glVertex3f(i->second->position.x+cos(i->second->angle)*1000,i->second->position.y+sin(i->second->elevation)*1000,i->second->position.z+sin(i->second->angle)*1000);
	//	//glColor3f(0,1,0);
	//	//glVertex3f(i->second->position.x,i->second->position.y+300,i->second->position.z);
	//	//glVertex3f(i->second->position.x+1000,i->second->position.y+300,i->second->position.z);
	////	glColor3f(0,1,0);
	//}
	//glEnd();
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
#endif


	glDepthMask(false);
	glDisable(GL_LIGHTING);
	glError();

	//glCallList(treeDisp);

		
	const map<objId,nPlane*>& planes = world.planes();
	for(auto i=planes.begin();i!=planes.end();i++)
		i->second->drawExplosion(p->id==i->second->id);
	//vertexArrayParticles();


	
	drawExaust();
	graphics->render3D();
	particleManager.render();

	//drawOrthoView(acplayer,e);

	drawBullets();

	//////////////2D/////////////
	//glDisable(GL_DEPTH_TEST);
	//glColor4f(1,1,1,1);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//drawHUD(acplayer,e,c,u);
	//glBindTexture(GL_TEXTURE_2D,0);
	//#ifdef _DEBUG
	//	if(fps<29.0)glColor4f(1,0,0,1);
	//	else glColor4f(0,0,0,1);

 	//	if(acplayer==0) textManager->renderText(lexical_cast<string>(floor(fps)),sw/2-25,25);
	//		//textManager->renderText(string((char*)gluErrorString(glGetError())),sw/2-25,25);
	//	if(acplayer==0) Profiler.draw();
	//#endif
	//glEnable(GL_DEPTH_TEST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);glError();
	//viewPerspective();
	///////////2D end//////////////	
	glDepthMask(true);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D,0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1.0/frustum.ratio, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	planeIdBoxes(p, 0.0, 0.0, 1.0, 1.0/frustum.ratio);
		
	glMatrixMode( GL_PROJECTION );			// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );			// Select Modelview

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	
	lastDraw[acplayer] = time;glError();



}