
#include "game.h"

namespace gui{
dogFight::dogFight(std::shared_ptr<LevelFile> lvl): level(lvl)
{

}
dogFight::~dogFight()
{
	world.destroy();
}
void dogFight::healthBar(float x, float y, float width, float height, float health, bool firstPerson)
{
	if(!firstPerson)
	{
		graphics->setColor(1,1,1);

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

	float radarAng = 45.0*world.time()/1000;
	radarAng = (radarAng/360.0 - floor(radarAng/360.0)) * 360;

	if(firstPerson)
	{
		dataManager.bind("radar");
		dataManager.setUniform1f("radarAng", radarAng);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height));

		Vec3f nC((x+width/2),(y+height/2),0);
		float radius = width/2;

		dataManager.bind("radar plane shader");
		dataManager.setUniform2f("center",nC.x,nC.y);
		dataManager.setUniform1f("radius",radius);
		graphics->setColor(0.19,0.58,0.87);

		Vec3f n;
		Vec3f cent, u, v;
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(n.x,n.z) - p->direction - 18.0 * PI/180 + PI;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = -p->direction + ((nPlane*)i->second.get())->direction - 18.0 * PI/180 + PI;
				u = Vec3f(sin(ang),cos(ang),0);
				v = Vec3f(sin(ang+PI/2),cos(ang+PI/2),0);

				graphics->drawTriangle(cent + u * 0.004, cent - u * 0.004 + v * 0.003, cent - u * 0.004 - v * 0.003);
			}

		}

		graphics->setColor(1,1,1);
		dataManager.bind("ortho");
	}
	else
	{
		dataManager.bind("radar2");
		dataManager.bind("radarTex",0);

		dataManager.setUniform1f("radarAng", radarAng);
		dataManager.setUniform1i("backgroundTexture", 0);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height));


#ifdef RADAR_MAP_BOUNDS
		dataManager.bind("radar bounds");

		Vec2f cCenter =world.bounds().center;
		float cR = sqrt(cCenter.x*cCenter.x + cCenter.y*cCenter.y);
		float cA = atan2(cCenter.x, cCenter.y) - p->direction;
		cCenter = Vec3f(sin(cA)*cR, cos(cA)*cR);


		double cRadius = world.bounds().radius / 8000.0;

		dataManager.setUniform2f("mapCenter",cCenter);
		dataManager.setUniform1f("mapRadius",cRadius);
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
#endif



		Vec3f nC((x+width/2),(y+height/2),0);
		float radius = width/2;

		dataManager.bind("radar plane shader");
		dataManager.setUniform2f("center",nC.x,nC.y);
		dataManager.setUniform1f("radius",radius);
		dataManager.setUniform1f("sAspect",sAspect);
		graphics->setColor(0.05,0.69,0.04);
		Vec3f n;
		Vec3f cent, u, v;
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(n.x,n.z) + PI - p->direction;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = -p->direction + ((nPlane*)i->second.get())->direction;
				u = Vec3f(sin(ang),cos(ang),0);
				v = Vec3f(sin(ang+PI/2),cos(ang+PI/2),0);

				graphics->drawTriangle(cent - u * 0.004, cent + u * 0.004 + v * 0.003, cent + u * 0.004 - v * 0.003);
			}
		}

		graphics->setColor(1,1,1);
		dataManager.bind("ortho");

		graphics->drawOverlay(Rect::XYWH(x,y,width,height),"radar frame");
	}
}
void dogFight::planeIdBoxes(nPlane* p, float vX, float vY, float vWidth, float vHeight, shared_ptr<GraphicsManager::View> v) //must get 'eye' location instead of plane location to work in 3rd person
{
	if(!p->dead)
	{
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			if(p->id!=i->second->id && !i->second->dead)
			{
				Vec2f s = v->project(i->second->position);
				double distSquared = i->second->position.distanceSquared(p->position);
				if(s.x > 0.0 && s.x < 1.0 && s.y > 0.0 && s.y < 1.0)
				{
					if(i->second->team==p->team)		graphics->setColor(0,1,0);
					else if(distSquared > 2000*2000)	graphics->setColor(0.6,0.5,0.5);
					else								graphics->setColor(0.5,0,0);

					graphics->drawOverlay(Rect::XYXY(vX + (s.x - 0.006) * vWidth,vY + s.y * vHeight - 0.006 * vWidth,vX + (s.x + 0.006) * vWidth, vY + s.y * vHeight + 0.006 * vWidth),"target ring");
				}
			}
		}
		graphics->setColor(1,1,1);
	}
}
void dogFight::targeter(float x, float y, float apothem, Angle tilt)
{
	graphics->drawRotatedOverlay(Rect::CWH(x,y,apothem*2,-apothem*2),tilt,"tilt");
	graphics->drawOverlay(Rect::CWH(x,y,apothem*2,-apothem*2),"targeter");
}
void dogFight::drawHexCylinder(Vec3f center, float radius, float height, Color c)
{
	dataManager.bind("hex grid shader");
	dataManager.bind("hex grid", 0);

	dataManager.setUniform1i("tex",0);
	dataManager.setUniform1f("minHeight",center.y);
	dataManager.setUniform1f("maxHeight",center.y+height);
	dataManager.setUniform1f("radius",radius);
	dataManager.setUniform4f("color",c.r,c.g,c.b,c.a);

	graphics->drawModelCustomShader("cylinder", Vec3f(center), Quat4f(),Vec3f(radius,height,radius));

	dataManager.bind("model");
}
void dogFight::drawScene(shared_ptr<GraphicsManager::View> view, int acplayer)
{
	static map<int,double> lastDraw;
	double time=world.time();
//	double interp = 1.0 - world.time.interpolate();

	nPlane* p=(nPlane*)players[acplayer]->getObject();
	if(!p)
	{
		return;
	}

//	auto camera = players[acplayer]->getCamera(p->controled || p->dead);
//	Vec3f e = camera.eye;
	//graphics->lookAt(camera.eye, camera.center, camera.up);

	world.renderTerrain(view);

	graphics->setDepthMask(false);

	((bulletCloud*)world[bullets].get())->draw();

	Vec3f cCenter(world.bounds().center.x,0,world.bounds().center.y);
	double cRadius = world.bounds().radius;

	drawHexCylinder(cCenter,cRadius,20000, white);

#ifdef _DEBUG
	players.debugDraw();
#endif

	graphics->setDepthMask(true);

	lastDraw[acplayer] = time;

	graphics->checkErrors(); //calls debugBreak() if openGL has previously encountered an error (can be placed after any line to pinpoint the error)
}
void dogFight::checkCollisions()
{
	std::shared_ptr<CollisionChecker::triangleList> trl1, trl2;

	auto planes = world(PLANE);
	auto AAA = world(ANTI_AIRCRAFT_ARTILLARY);
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
						i->second->loseHealth(11.2);

						if((*i).second->dead)
						{
							if(players.numPlayers() >= 1 && bulletRef[l].owner==players[0]->objectNum()) players[0]->addKill();
							if(players.numPlayers() >= 2 && bulletRef[l].owner==players[1]->objectNum()) players[1]->addKill();
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
				objId owner = dynamic_pointer_cast<missileBase>(l->second)->owner;
				if(owner != i->second->id &&  owner != (*i).first && !l->second->awaitingDelete &&
					(i->second->position + i->second->rotation*(trl1!=NULL?trl1->getCenter():Vec3f(0,0,0))).distance(l->second->position + l->second->rotation*(trl2!=NULL?trl2->getCenter():Vec3f(0,0,0))) < (trl1!=NULL?trl1->getRadius():0)+(trl2!=NULL?trl2->getRadius():0) )
					//collisionCheck(i->second,l->second))
				{
					if(l->second->type == SAM_MISSILE)
						i->second->loseHealth(27);
					else
						i->second->loseHealth(105);

					if((*i).second->dead)
					{
						if(players.numPlayers() >= 1 && owner==players[0]->objectNum()) players[0]->addKill();
						if(players.numPlayers() >= 2 && owner==players[1]->objectNum()) players[1]->addKill();
					}
					l->second->awaitingDelete = true;
				}
			}
		}
	}
	for(auto i = AAA.begin(); i != AAA.end();i++)
	{
		if(!i->second->dead)
		{
			for(auto l=missiles.begin();l!=missiles.end();l++)
			{
				trl1 = dataManager.getModel(objectTypeString(i->second->type))->trl;
				trl2 = dataManager.getModel(objectTypeString(l->second->type))->trl;
				objId owner = dynamic_pointer_cast<missileBase>(l->second)->owner;
				if(owner != i->second->id &&  owner != (*i).first && !l->second->awaitingDelete &&
					(i->second->position + i->second->rotation*(trl1!=NULL?trl1->getCenter():Vec3f(0,0,0))).distance(l->second->position + l->second->rotation*(trl2!=NULL?trl2->getCenter():Vec3f(0,0,0))) < (trl1!=NULL?trl1->getRadius():0)+(trl2!=NULL?trl2->getRadius():0) )
					//collisionCheck(i->second,l->second))
				{
					i->second->loseHealth(105);
					if((*i).second->dead)
					{
						if(players.numPlayers() >= 1 && owner==players[0]->objectNum()) players[0]->addKill();
						if(players.numPlayers() >= 2 && owner==players[1]->objectNum()) players[1]->addKill();
					}
					l->second->awaitingDelete = true;
				}
			}
		}
	}

	//Vec3f bulletPosition;				//PARTICLE EFFECTS FOR GROUND/WATER COLLISIONS
	//for(auto i=0;i<(signed int)bulletRef.size();i++)
	//{
	//	bulletPosition = bulletRef[i].startPos+bulletRef[i].velocity*(world.time()-bulletRef[i].startTime)/1000;
	//	if(world.altitude(bulletPosition) < 0.0)
	//	{
	//		if(!world.isLand(bulletPosition.x, bulletPosition.z))
	//		{
	//			particleManager.addEmitter(new particle::splash, Vec3f(bulletPosition.x, 0, bulletPosition.z), 1.0);
	//		}
	//		bulletRef[i].life = world.time()-bulletRef[i].startTime; //makes the bullet die at the current time
	//	}
	//}
	//float altitude;
	//for(auto i=missiles.begin();i!=missiles.end();i++)
	//{
	//	altitude = world.altitude(i->second->position);
	//	if(altitude < 0.0)
	//	{
	//		if(world.isLand(bulletPosition.x, bulletPosition.z))
	//		{
	//			particleManager.addEmitter(new particle::explosion, Vec3f(i->second->position.x, i->second->position.y - altitude, i->second->position.z), 5.0);
	//		}
	//		else
	//		{
	//			particleManager.addEmitter(new particle::splash, Vec3f(i->second->position.x, 0, i->second->position.z), 5.0);
	//		}
	//		i->second->dead = true;
	//	}
	//}
}
}
