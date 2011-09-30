
#include "main.h"

namespace menu{
dogFight::dogFight(std::shared_ptr<Level> lvl): level(lvl)
{
	
}
bool dogFight::init()
{
	world.create(level);
	level->initializeWorld();
	return true;
}
dogFight::~dogFight()
{
	world.destroy();
}
void dogFight::healthBar(float x, float y, float width, float height, float health, bool firstPerson)
{
	if(!firstPerson)
	{
		glColor3f(1,1,1);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height),"health bar");
	
		Vec2f v1 = Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*8.0);
		Vec2f v2 = Vec2f(x + width/150*125, y + height/25*16.0);

		graphics->drawOverlay(Rect::XYXY(v1, v2),"white");

	}
	else
	{
		dataManager.bind("health");
		dataManager.setUniform1f("health",health);
		dataManager.setUniform1f("angle",1.24f);
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
		dataManager.bind("ortho");
	}
}
void dogFight::tiltMeter(float x1,float y1,float x2,float y2,float degrees)
{
	x1 *=	0.00125*sw;
	y1 *=	0.00167*sh;
	x2 *=	0.00125*sw;
	y2 *=	0.00167*sh;

	graphics->drawRotatedOverlay(Rect::XYXY(x1,y2,x2,y2),degrees * PI/180,"tilt back");
	graphics->drawOverlay(Rect::XYXY(x1,y2,x2,y2),"tilt front");
}
void dogFight::radar(float x, float y, float width, float height,bool firstPerson, nPlane* p)
{
	//plane p = *(plane*)planes[players[acplayer].planeNum()];
	int xc=x+width/2,yc=y+height/2;

	float radarAng = 45.0*world.time()/1000;
	radarAng = (radarAng/360.0 - floor(radarAng/360.0)) * 360;

	if(firstPerson)
	{
	//	static int radarAng = glGetUniformLocation(dataManager.getId("radar"), "radarAng");
		
		dataManager.bind("radar");
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
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(-n.x,n.z) + p->direction + 18.0 * PI/180;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = p->direction + ((nPlane*)i->second.get())->direction + 18.0 * PI/180;
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
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(-n.x,n.z) + p->direction;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = p->direction + ((nPlane*)i->second.get())->direction;
				u = Vec3f(sin(ang),cos(ang),0);
				v = Vec3f(sin(ang+PI/2),cos(ang+PI/2),0);

				graphics->drawTriangle(cent + u * 0.004, cent - u * 0.004 + v * 0.003, cent - u * 0.004 - v * 0.003);
			}
		
		}

		glColor3f(1,1,1);
		dataManager.bind("ortho");

		graphics->drawOverlay(Rect::XYWH(x,y,width,height),"radar frame");
	}
}
void dogFight::planeIdBoxes(nPlane* p, float vX, float vY, float vWidth, float vHeight) //must get 'eye' location instead of plane location to work in 3rd person
{
	if(!p->dead)
	{
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			if(p->id!=i->second->id && !i->second->dead)
			{
				Vec2f s = graphics->project(i->second->position);
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
void dogFight::targeter(float x, float y, float apothem, Angle tilt)
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
void dogFight::drawPlanes(int acplayer,bool showBehind,bool showDead)
{
	nPlane* p=(nPlane*)world[players[acplayer].objectNum()].get();
	auto planes = world(PLANE);
	nPlane* cPlane;

	//Vec3f axis;
	Angle roll;
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		cPlane=(nPlane*)((*i).second.get());
		Vec3f a=(*i).second->position;
		if((cPlane->id!=players[acplayer].objectNum() || !players[acplayer].firstPerson() ||  p->controled) && (showDead || !cPlane->dead) && cPlane->death != nPlane::DEATH_EXPLOSION)
		{
			//graphics->drawLine(a,a+(*i).second->rotation * Vec3f(0,0,100));
			glPushMatrix();
				glTranslatef(a.x,a.y,a.z);

				Angle ang = acosA(cPlane->rotation.w);
				glRotatef((ang*2.0).degrees(), cPlane->rotation.x/sin(ang),cPlane->rotation.y/sin(ang),cPlane->rotation.z/sin(ang));

				graphics->drawModel(cPlane->type, (*i).second->position, Quat4f());

				int ml=0;
				for(int m = cPlane->rockets.max - cPlane->rockets.left; m < cPlane->rockets.max; m++)
				{
					glPushMatrix();
					glTranslatef(cPlane->rockets.ammoRounds[m].offset.x,cPlane->rockets.ammoRounds[m].offset.y,cPlane->rockets.ammoRounds[m].offset.z);
					graphics->drawModel(cPlane->rockets.ammoRounds[m].type, (*i).second->position + cPlane->rotation * cPlane->rockets.ammoRounds[m].offset, Quat4f());
					glPopMatrix();
				}
				for(int m = cPlane->bombs.roundsMax - cPlane->bombs.roundsLeft; m < cPlane->bombs.roundsMax; m++)
				{
					glPushMatrix();
					glTranslatef(cPlane->bombs.ammoRounds[m].offset.x,cPlane->bombs.ammoRounds[m].offset.y,cPlane->bombs.ammoRounds[m].offset.z);
					graphics->drawModel(cPlane->bombs.ammoRounds[m].type, (*i).second->position + cPlane->rotation * cPlane->bombs.ammoRounds[m].offset, Quat4f());
					glPopMatrix();
				}
			glPopMatrix();
		}
	}
}
void dogFight::drawHexCylinder(Vec3f center, float radius, float height, Color c)
{
	dataManager.bind("hex grid shader");
	dataManager.setUniform1i("tex",0);
	dataManager.setUniform1f("minHeight",center.y);
	dataManager.setUniform1f("maxHeight",center.y+height);
	dataManager.setUniform1f("radius",radius);
	dataManager.setUniform4f("color",c.r,c.g,c.b,c.a);

	glPushMatrix();

	glTranslatef(center.x,center.y,center.z);
	glScalef(radius,height,radius);

	graphics->drawModelCustomShader("cylinder", Vec3f(center), Quat4f(),Vec3f(radius,height,radius));

	glPopMatrix();

	dataManager.unbindShader();
}
void dogFight::drawScene(int acplayer) 
{
	static map<int,double> lastDraw;
	double time=world.time();
	double interp = 1.0;//world.time.interpolate();

	nPlane* p=(nPlane*)world[players[acplayer].objectNum()].get();
	if(!p)
	{
		return;
	}

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
	}
	else
	{
		Quat4f rot = slerp(p->rotation,p->lastRotation,interp);

		e = lerp(p->lastPosition,p->position,interp);
		c = rot * Vec3f(0,0,1) + e;//(p->x + sin(p->angle * DegToRad),p->y + p->climb,p->z + cos(p->angle * DegToRad));
		u = rot * Vec3f(0,1,0);
		
		graphics->lookAt(e, c, u);
	}
	//sky dome
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);

	glTranslatef(e.x,0,e.z);
	glScalef(30000,10000,30000);
	//dataManager.bind("sky shader");
	graphics->drawModel("sky dome",Vec3f(e.x,0,e.z), Quat4f(), Vec3f(30000,10000,30000));
	glScalef(1,-1,1);
	graphics->drawModel("sky dome",Vec3f(e.x,0,e.z), Quat4f(), Vec3f(30000,-10000,30000));
	dataManager.unbindShader();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();


	if(world.level != NULL)
		world.level->render(e);

	dataManager.bind("model");
	Vec3f axis;
	auto missiles = world(MISSILE);
	for(auto i=missiles.begin();i != missiles.end();i++)
	{
		if(!i->second->awaitingDelete && !i->second->dead)
		{
			glPushMatrix();
				glTranslatef(i->second->position.x,i->second->position.y,i->second->position.z);

				Angle ang = acosA(i->second->rotation.w);
				glRotatef((ang*2.0).degrees(), i->second->rotation.x/sin(ang),i->second->rotation.y/sin(ang),i->second->rotation.z/sin(ang));
				graphics->drawModel(i->second->type,i->second->position, i->second->rotation);
			glPopMatrix();
		}
	}

	auto bombs = world(BOMB);
	for(auto i=bombs.begin();i != bombs.end();i++)
	{
		if(!i->second->awaitingDelete && !i->second->dead)
		{
			glPushMatrix();
				glTranslatef(i->second->position.x,i->second->position.y,i->second->position.z);

				Angle ang = acosA(i->second->rotation.w);
				glRotatef((ang*2.0).degrees(), i->second->rotation.x/sin(ang),i->second->rotation.y/sin(ang),i->second->rotation.z/sin(ang));
				graphics->drawModel(i->second->type,i->second->position, i->second->rotation);
			glPopMatrix();
		}
	}

	auto aaGuns = world(AA_GUN);
	for(auto i = aaGuns.begin(); i != aaGuns.end();i++)
	{
		glPushMatrix();	
		glTranslatef(i->second->position.x,i->second->position.y,i->second->position.z);
		Angle ang = acosA(i->second->rotation.w);
		glRotatef((ang*2.0).degrees(), i->second->rotation.x/sin(ang),i->second->rotation.y/sin(ang),i->second->rotation.z/sin(ang));
		glRotatef(90,1,0,0);
		graphics->drawModel("AA gun",i->second->position, i->second->rotation);
		glPopMatrix();
	}

	drawPlanes(acplayer,false,true);

	glError();

	glDepthMask(false);

	((bulletCloud*)world[bullets].get())->draw();

	Vec3f cCenter = Vec3f(world.ground()->sizeX()/2,0,world.ground()->sizeZ()/2);
	double cRadius = world.ground()->sizeX();

	drawHexCylinder(cCenter,cRadius,20000, white);
	glDepthMask(true);
	
	lastDraw[acplayer] = time;
	
	glError();
}
void dogFight::checkCollisions()
{
	std::shared_ptr<CollisionChecker::triangleList> trl1, trl2;

	auto planes = world(PLANE);
	auto missiles = world(MISSILE);
	auto& bulletRef = ((bulletCloud*)world[bullets].get())->bullets;



	for(auto i = planes.begin(); i != planes.end();i++)
	{
		if(!i->second->dead)
		{
			for(auto l=0;l<(signed int)bulletRef.size();l++)
			{
				if(bulletRef[l].owner != i->second->id && bulletRef[l].startTime < world.time.lastTime() && bulletRef[l].startTime + bulletRef[l].life > world.time())
				{
					if(collisionCheck(i->second->type,bulletRef[l].startPos+bulletRef[l].velocity*(world.time()-bulletRef[l].startTime)/1000-i->second->position, bulletRef[l].startPos+bulletRef[l].velocity*(world.time.lastTime()-bulletRef[l].startTime)/1000-i->second->position))
					{
						((nPlane*)(*i).second.get())->loseHealth(25);
						if((*i).second->dead)
						{
							if(bulletRef[l].owner==players[0].objectNum() && players[0].active()) players[0].addKill();
							if(bulletRef[l].owner==players[1].objectNum() && players[1].active()) players[1].addKill();
						}
						bulletRef.erase(bulletRef.begin()+l);
						l--;
					}
				}
			}
			for(auto l=missiles.begin();l!=missiles.end();l++)
			{
				trl1 = dataManager.getModel(objectTypeString(i->second->type))->trl;
				trl2 = dataManager.getModel(objectTypeString(l->second->type))->trl;
				objId owner = ((missile*)l->second.get())->owner;
				if(owner != i->second->id &&  owner != (*i).first && 
					(i->second->position + i->second->rotation*(trl1!=NULL?trl1->getCenter():Vec3f(0,0,0))).distance(l->second->position + l->second->rotation*(trl2!=NULL?trl2->getCenter():Vec3f(0,0,0))) < (trl1!=NULL?trl1->getRadius():0)+(trl2!=NULL?trl2->getRadius():0) )
					//collisionCheck(i->second,l->second))
				{
					((nPlane*)(*i).second.get())->loseHealth(105);
					if((*i).second->dead) 
					{
						if(owner==players[0].objectNum() && players[0].active()) players[0].addKill();
						if(owner==players[1].objectNum() && players[1].active()) players[1].addKill();
					}
					l->second->awaitingDelete = true;
				}
			}
		}
	}
}
}