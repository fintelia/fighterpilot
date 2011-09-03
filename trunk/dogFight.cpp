
#include "main.h"

modeDogFight::modeDogFight(Level* lvl)
{
	world.create(lvl);

	////RADAR FRAME BUFFER OBJECTS
	//glGenTextures(1, &radarTexture);
	//glGenFramebuffersEXT(1, &radarFBO);

	//glBindTexture(GL_TEXTURE_2D, radarTexture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 128, 128, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	//
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, radarFBO);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, radarTexture, 0);

	//GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//switch(status)
	//{
	//	case GL_FRAMEBUFFER_COMPLETE_EXT:							break;
	//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Attachment is NOT complete.",L"",0);
	//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:		MessageBox(NULL,L"[ERROR] Framebuffer incomplete: No image is attached to FBO.",L"",0);
	//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Attached images have different dimensions.",L"",0);
	//	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:					MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Color attached images have different internal formats.",L"",0);
	//	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Draw buffer.",L"",0);
	//	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:				MessageBox(NULL,L"[ERROR] Framebuffer incomplete: Read buffer.",L"",0);
	//	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:						MessageBox(NULL,L"[ERROR] Unsupported by FBO implementation.",L"",0);
	//		return;

	//	default:
	//		MessageBox(NULL,L"[ERROR] Unknow FBO error.",L"",0);
	//		return;
	//}
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
	//	return;

}
modeDogFight::~modeDogFight()
{
	world.destroy();
}

void modeDogFight::healthBar(float x, float y, float width, float height, float health, bool firstPerson)
{
	if(!firstPerson)
	{
		//x *=	0.00125*sw;
		//y *=	0.00167*sh;
		//width*= 0.00125*sw;
		//height*=0.00167*sh;

		glColor3f(1,1,1);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height),"health bar");


		//graphics->drawOverlay(Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*7.25), 
		//					Vec2f(x + width/150*125 - ((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health)), height/25*8.5), "noTexture");
		Vec2f v1 = Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*8.0);
		Vec2f v2 = Vec2f(x + width/150*125, y + height/25*16.0);

		graphics->drawOverlay(Rect::XYXY(v1, v2),"white");

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
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
		dataManager.bind("ortho");
	}
}
void modeDogFight::tiltMeter(float x1,float y1,float x2,float y2,float degrees)
{
	x1 *=	0.00125*sw;
	y1 *=	0.00167*sh;
	x2 *=	0.00125*sw;
	y2 *=	0.00167*sh;

	graphics->drawRotatedOverlay(Rect::XYXY(x1,y2,x2,y2),degrees * PI/180,"tilt back");
	graphics->drawOverlay(Rect::XYXY(x1,y2,x2,y2),"tilt front");
}
void modeDogFight::radar(float x, float y, float width, float height,bool firstPerson, nPlane* p)
{
	//plane p = *(plane*)planes[players[acplayer].planeNum()];
	int xc=x+width/2,yc=y+height/2;

	float radarAng = 45.0*world.time()/1000;
	radarAng = (radarAng/360.0 - floor(radarAng/360.0)) * 360;

	if(firstPerson)
	{
	//	static int radarAng = glGetUniformLocation(dataManager.getId("radar"), "radarAng");
		
		dataManager.bind("radar");
		dataManager.bindTex(radarTexture);

		dataManager.setUniform1f("radarAng", radarAng);

		//Vec2f mCenter((world.ground()->sizeX()/2-p->position.x)/160.0,(world.ground()->sizeZ()/2-p->position.z)/160.0);
		//dataManager.setUniform2f("mapCenter", mCenter.x + 0.5, mCenter.y + 0.5);
		//dataManager.setUniform1f("mapRadius", world.ground()->sizeX()/160.0);

		//dataManager.setUniform1i("radarTexture", 0);
	//	glUniform1f(radarAng, radarAng);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
		//dataManager.unbind("radar");
		dataManager.unbindTextures();

		Vec3f nC((x+width/2),(y+height/2),0);
		float radius = width/2;

		dataManager.bind("radar plane shader");
		dataManager.setUniform2f("center",nC.x,nC.y);
		dataManager.setUniform1f("radius",radius);
		glColor3f(0.19,0.58,0.87);

		Vec3f n;
		Vec3f cent, u, v;
		for(auto i = world.planes().begin(); i != world.planes().end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(-n.x,n.z) + p->direction + 18.0 * PI/180;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = p->direction + i->second->direction + 18.0 * PI/180;
				u = Vec3f(sin(ang),cos(ang),0);
				v = Vec3f(sin(ang+PI/2),cos(ang+PI/2),0);

				graphics->drawTriangle(cent + u * 0.004, cent - u * 0.004 + v * 0.003, cent - u * 0.004 - v * 0.003);
			}
		
		}

		glColor3f(1,1,1);
		dataManager.bind("ortho");
	}
	else
	{
		dataManager.bind("radar2");
		dataManager.bind("radarTex",0);
		dataManager.bindTex(radarTexture,1);

		dataManager.setUniform1f("radarAng", radarAng);
		dataManager.setUniform1i("backgroundTexture", 0);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height));

		dataManager.unbindTextures();
		//dataManager.unbindShader();

#ifdef RADAR_MAP_BOUNDS
		dataManager.bind("radar bounds");

		Vec3f cCenter = (Vec3f(world.ground()->sizeX()/2,0,world.ground()->sizeZ()/2) - p->position) / 16000.0;
		float cR = sqrt(cCenter.x*cCenter.x + cCenter.z*cCenter.z);
		float cA = atan2(cCenter.x, cCenter.z) - p->direction;
		cCenter = Vec3f(sin(cA)*cR, 0, cos(cA)*cR);


		double cRadius = world.ground()->sizeX() / 16000.0;

		dataManager.setUniform2f("mapCenter",cCenter.x,cCenter.z);
		dataManager.setUniform1f("mapRadius",cRadius);
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
#endif



		Vec3f nC((x+width/2),(y+height/2),0);
		float radius = width/2;

		dataManager.bind("radar plane shader");
		dataManager.setUniform2f("center",nC.x,nC.y);
		dataManager.setUniform1f("radius",radius);
		dataManager.setUniform1f("sAspect",sAspect);
		glColor3f(0.05,0.79,0.04);

		Vec3f n;
		Vec3f cent, u, v;
		for(auto i = world.planes().begin(); i != world.planes().end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(-n.x,n.z) + p->direction;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = p->direction + i->second->direction;
				u = Vec3f(sin(ang),cos(ang),0);
				v = Vec3f(sin(ang+PI/2),cos(ang+PI/2),0);

				graphics->drawTriangle(cent + u * 0.004, cent - u * 0.004 + v * 0.003, cent - u * 0.004 - v * 0.003);
			}
		
		}

		glColor3f(1,1,1);
		dataManager.bind("ortho");

	//	graphics->drawOverlay(Rect::XYWH(x,y,width,height),"radar frame");
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

					graphics->drawOverlay(Rect::XYXY(vX + (s.x - 0.006) * vWidth,vY + s.y * vHeight - 0.006 * vWidth,vX + (s.x + 0.006) * vWidth, vY + s.y * vHeight + 0.006 * vWidth),"target ring");
				}
			}
		}
		glColor3f(1,1,1);
	}
}
void modeDogFight::targeter(float x, float y, float apothem, Angle tilt)
{
	//float width = apothem*0.00125*sw;
	//float height = apothem*0.00167*sh;
	//x *=	0.00125*sw;
	//y *=	0.00167*sh;
	//dataManager.bind("ortho");
	graphics->drawRotatedOverlay(Rect::CWH(x,y,apothem*2,-apothem*2),tilt,"tilt");
	graphics->drawOverlay(Rect::CWH(x,y,apothem*2,-apothem*2),"targeter");
	//dataManager.unbindShader();
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
			//graphics->drawLine(a,a+(*i).second->rotation * Vec3f(0,0,100));
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

				int ml=0;
				for(int m = cPlane->rockets.max - cPlane->rockets.left; m < cPlane->rockets.max; m++)
				{
					glPushMatrix();
					glTranslatef(cPlane->rockets.ammoRounds[m].offset.x,cPlane->rockets.ammoRounds[m].offset.y,cPlane->rockets.ammoRounds[m].offset.z);
					dataManager.draw(cPlane->rockets.ammoRounds[m].type);
					glPopMatrix();
				}
				for(int m = cPlane->bombs.roundsMax - cPlane->bombs.roundsLeft; m < cPlane->bombs.roundsMax; m++)
				{
					glPushMatrix();
					glTranslatef(cPlane->bombs.ammoRounds[m].offset.x,cPlane->bombs.ammoRounds[m].offset.y,cPlane->bombs.ammoRounds[m].offset.z);
					dataManager.draw(cPlane->bombs.ammoRounds[m].type);
					glPopMatrix();
				}
			glPopMatrix();
		}
	}
}
void modeDogFight::drawBullets()
{
	double time = world.time();
	double lTime = time - 20.0;//world.time.getLastTime();

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

				Vec3f dir = i->velocity.normalize();
				float a1 = dir.dot(up);
				float a2 = dir.dot(right);
				float len = 0.3/sqrt(a1*a1+a2*a2);

				v[0] = start + (right*a1 - up*a2)*len;
				v[1] = start - (right*a1 - up*a2)*len;
				v[2] = end2 - (right*a1 - up*a2)*len;
				v[3] = end2 + (right*a1 - up*a2)*len;

				glTexCoord2f(1,1);	glVertex3fv(&v[0].x);
				glTexCoord2f(1,0);	glVertex3fv(&v[1].x);
				glTexCoord2f(0,0);	glVertex3fv(&v[2].x);
				glTexCoord2f(0,1);	glVertex3fv(&v[3].x);
			}
		}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
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
	dataManager.setUniform4f("color",c.r,c.g,c.b,c.a);

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
	double interp = 1.0;//world.time.interpolate();

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
	Vec3f e;
	Vec3f c;
	Vec3f u;
	if(!players[acplayer].firstPerson() || p->controled || p->dead)
	{
		e = lerp(p->camera.lastPosition,p->camera.position,interp);
		c = lerp(p->camera.lastCenter,p->camera.center,interp);
		u = lerp(p->camera.lastUp,p->camera.up,interp);

		graphics->lookAt(e, c, u);
		frustum.setCamDef(e,c,u);
	}
	else
	{
		Quat4f rot = slerp(p->rotation,p->lastRotation,interp);

		e = lerp(p->lastPosition,p->position,interp);
		c = rot * Vec3f(0,0,1) + e;//(p->x + sin(p->angle * DegToRad),p->y + p->climb,p->z + cos(p->angle * DegToRad));
		u = rot * Vec3f(0,1,0);
		
		graphics->lookAt(e, c, u);
		frustum.setCamDef(e,c,u);
	}
	//sky dome
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);

	glTranslatef(e.x,0,e.z);
	glScalef(30000,10000,30000);
	//dataManager.bind("sky shader");
	dataManager.draw("sky dome");
	glScalef(1,-1,1);
	dataManager.draw("sky dome");
	dataManager.unbindShader();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();


	if(world.level != NULL)
		world.level->render(e);

	dataManager.bind("model");
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
		}
	}

	const map<objId,bomb*>& bombs = world.bombs();
	for(auto i=bombs.begin();i != bombs.end();i++)
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
		}
	}

	for(auto i = world.aaGuns().begin(); i != world.aaGuns().end();i++)
	{
		glPushMatrix();	
		glTranslatef(i->second->position.x,i->second->position.y,i->second->position.z);
		Angle ang = acosA(i->second->rotation.w);
		glRotatef((ang*2.0).degrees(), i->second->rotation.x/sin(ang),i->second->rotation.y/sin(ang),i->second->rotation.z/sin(ang));
		glRotatef(90,1,0,0);
		dataManager.draw("AA gun");
		glPopMatrix();
	}

	drawPlanes(acplayer,false,true);
#ifdef AI_TARGET_LINES 
	int t;
	for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	{
		if(i->second->planeType & AI_PLANE)
		{
			t=((AIplane*)(i->second))->target;
			if(planes.find(t)!=planes.end())
			{
				graphics->drawLine(*i->second,*planes[t]);
			}
		}
	}
#endif
	glError();

	glDepthMask(false);

	drawBullets();

	Vec3f cCenter = Vec3f(world.ground()->sizeX()/2,0,world.ground()->sizeZ()/2);
	double cRadius = world.ground()->sizeX();
	//double d = abs(sqrt((e.x-cCenter.x)*(e.x-cCenter.x) + (e.z-cCenter.z)*(e.z-cCenter.z)) - cRadius);
	drawHexCylinder(cCenter,cRadius,20000, white);

	//glDepthMask(false);//needed since drawHexCylinder sets depthMask to true

	//particleManager.render();

	


	//glDepthMask(true);
	//glDisable(GL_DEPTH_TEST);
	//glBindTexture(GL_TEXTURE_2D,0);

	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//glOrtho(0, 1, 1.0/frustum.ratio, 0, -1, 1);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	////planeIdBoxes(p, 0.0, 0.0, 1.0, 1.0/frustum.ratio);
	//	
	//glMatrixMode( GL_PROJECTION );			// Select Projection
	//glPopMatrix();							// Pop The Matrix
	//glMatrixMode( GL_MODELVIEW );			// Select Modelview

	//glEnable(GL_DEPTH_TEST);
	
	lastDraw[acplayer] = time;
	
	glError();
}
void modeDogFight::drawSceneParticles(int acplayer)
{
	double interp = 1.0;//world.time.interpolate();
	nPlane* p=(nPlane*)world.objectList[players[acplayer].objectNum()];
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(!players[acplayer].firstPerson() || p->controled || p->dead)
	{
		Vec3f e = lerp(p->camera.lastPosition,p->camera.position,interp);
		Vec3f c = lerp(p->camera.lastCenter,p->camera.center,interp);
		Vec3f u = lerp(p->camera.lastUp,p->camera.up,interp);

		graphics->lookAt(e, c, u);
		frustum.setCamDef(e,c,u);
	}
	else
	{
		Quat4f rot = slerp(p->rotation,p->lastRotation,interp);

		Vec3f e = lerp(p->lastPosition,p->position,interp);
		Vec3f c = rot * Vec3f(0,0,1) + e;//(p->x + sin(p->angle * DegToRad),p->y + p->climb,p->z + cos(p->angle * DegToRad));
		Vec3f u = rot * Vec3f(0,1,0);
		
		graphics->lookAt(e, c, u);
		frustum.setCamDef(e,c,u);
	}
	particleManager.render();
}