
#include "engine.h"
#include "game.h"

namespace gui{
dogFight::dogFight(shared_ptr<const LevelFile> lvl): level(lvl), triggers(level->triggers)
{

}
dogFight::~dogFight()
{
	world.reset();
}
void dogFight::healthBar(float x, float y, float width, float height, float health) const
{
	//if(!firstPerson)
	//{
	//	graphics->setColor(1,1,1);
	//
	//	graphics->drawOverlay(Rect::XYWH(x,y,width,height),"health bar");
	//
	//	Vec2f v1 = Vec2f((x + width/150*14)*(1.0-health)+(x + width/150*125)*(health), y + height/25*8.0);
	//	Vec2f v2 = Vec2f(x + width/150*125, y + height/25*16.0);
	//
	//	graphics->drawOverlay(Rect::XYXY(v1, v2),"white");
	//}
	//else
	//{
		auto healthShader = shaders.bind("health");
		healthShader->setUniform1f("health",health);
		healthShader->setUniform1f("angle",1.24f);
		healthShader->setUniform4f("viewConstraint", graphics->getViewContraint());
		healthShader->setUniform3f("HUD_color_back", level->info.night ? Vec3f(0.1, 0.6, 0.2) : Vec3f(0.11,0.35,0.52));
		healthShader->setUniform3f("HUD_color", level->info.night ? Vec3f(0.15, 0.7, 0.25) : Vec3f(0.19,0.58,0.78));
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
		shaders.bind("ortho");
	//}
}
void dogFight::tiltMeter(float x1,float y1,float x2,float y2,float degrees) const
{
	x1 *=	0.00125*sw;
	y1 *=	0.00167*sh;
	x2 *=	0.00125*sw;
	y2 *=	0.00167*sh;

	graphics->drawRotatedOverlay(Rect::XYXY(x1,y2,x2,y2),degrees * PI/180,"tilt back");
	graphics->drawOverlay(Rect::XYXY(x1,y2,x2,y2),"tilt front");
}
void dogFight::radar(float x, float y, float width, float height,bool firstPerson, shared_ptr<plane> p) const
{
	float radarAng = 45.0*world->time()/1000;
	radarAng = (radarAng/360.0 - floor(radarAng/360.0)) * 360;

	if(firstPerson)
	{
		//////////////////////////////////////////////////////////////////////////////
		auto radarShader = shaders.bind("radar");
		radarShader->setUniform4f("viewConstraint", graphics->getViewContraint());
		radarShader->setUniform1f("radarAng", radarAng);
		radarShader->setUniform3f("HUD_color_back", level->info.night ? Vec3f(0.1, 0.6, 0.2) : Vec3f(0.11,0.35,0.52));
		radarShader->setUniform3f("HUD_color", level->info.night ? Vec3f(0.15, 0.75, 0.25) : Vec3f(0.19, 0.58, 0.87));
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));

		////////////////////////////////////////////////////////////////
		Vec3f nC((x+width/2),(y+height/2),0);
		float radius = width/2;

		auto radarPlaneShader = shaders.bind("radar plane shader");
		radarPlaneShader->setUniform4f("viewConstraint", graphics->getViewContraint());
		radarPlaneShader->setUniform2f("center",nC.x,nC.y);
		radarPlaneShader->setUniform1f("radius",radius);
		//radarShader->setUniform3f("HUD_color_back", level->info.night ? Vec3f(0.1, 0.6, 0.2) : Vec3f(0.11,0.35,0.52));
		if(level->info.night)	radarPlaneShader->setUniform4f("color", 0.15, 0.75, 0.25, 1.0);
		else					radarPlaneShader->setUniform4f("color", 0.19, 0.58, 0.87, 1.0);

	//	graphics->setColor(0.19,0.58,0.87);

		Vec3f n;
		Vec3f cent, u, v;
		auto planes = world->getAllOfType(PLANE);
		for(auto i = planes.begin(); i != planes.end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(n.x,n.z) - p->direction - 18.0 * PI/180 + PI;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = -p->direction + dynamic_pointer_cast<plane>(i->second)->direction - 18.0 * PI/180 + PI;
				//u = Vec3f(sin(ang),cos(ang),0);
				//v = Vec3f(sin(ang+PI/2),cos(ang+PI/2),0);
				graphics->drawRotatedOverlay(Rect::CWH(cent.x,cent.y,0.008,0.008),ang);
				//graphics->drawTriangle(cent + u * 0.004, cent - u * 0.004 + v * 0.003, cent - u * 0.004 - v * 0.003);
			}
		}
		dataManager.bind("ortho");
	}
	else
	{
		auto radarShader = shaders.bind("radar2");
		dataManager.bind("radarTex",0);
		radarShader->setUniform4f("viewConstraint", graphics->getViewContraint());
		radarShader->setUniform1f("radarAng", radarAng);
		radarShader->setUniform1i("backgroundTexture", 0);

		graphics->drawOverlay(Rect::XYWH(x,y,width,height));


#ifdef RADAR_MAP_BOUNDS
		auto radarBoundsShader = shaders.bind("radar bounds");

		Vec2f cCenter =world->bounds().center;
		float cR = sqrt(cCenter.x*cCenter.x + cCenter.y*cCenter.y);
		float cA = atan2(cCenter.x, cCenter.y) - p->direction;
		cCenter = Vec3f(sin(cA)*cR, cos(cA)*cR);


		double cRadius = world->bounds().radius / 8000.0;

		radarBoundsShader->setUniform2f("mapCenter",cCenter);
		radarBoundsShader->setUniform1f("mapRadius",cRadius);
		radarBoundsShader->setUniform4f("viewConstraint", graphics->getViewContraint());
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
#endif



		Vec3f nC((x+width/2),(y+height/2),0);
		float radius = width/2;

		auto radarPlane = shaders.bind("radar plane shader");
		radarPlane->setUniform4f("viewConstraint", graphics->getViewContraint());
		radarPlane->setUniform2f("center",nC.x,nC.y);
		radarPlane->setUniform1f("radius",radius);
		radarPlane->setUniform1f("sAspect",sAspect);
		graphics->setColor(0.05,0.69,0.04);
		Vec3f n;
		Vec3f cent, u, v;
		auto planes = world->getAllOfType(PLANE);
		for(auto i = planes.begin(); i != planes.end(); i++)
		{
			n = (i->second->position - p->position) / (16000.0);
			if(p->id != i->second->id && !i->second->dead /*&& n.magnitudeSquared() < 1.0*/)
			{
				float mag = n.magnitude();
				Angle ang = atan2A(n.x,n.z) + PI - p->direction;
				cent = Vec3f(sin(ang)*mag*radius,cos(ang)*mag*radius,0) + nC;
				ang = -p->direction + dynamic_pointer_cast<plane>(i->second)->direction;
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
//void dogFight::planeIdBoxes(shared_ptr<plane> p, float vX, float vY, float vWidth, float vHeight, shared_ptr<GraphicsManager::View> v) //must get 'eye' location instead of plane location to work in 3rd person
//{
//	if(!p->dead)
//	{
//		auto planes = world->getAllOfType(PLANE);
//		for(auto i = planes.begin(); i != planes.end();i++)
//		{
//			if(p->id!=i->second->id && !i->second->dead)
//			{
//				Vec2f s = v->project(i->second->position);
//				double distSquared = i->second->position.distanceSquared(p->position);
//				if(s.x > 0.0 && s.x < 1.0 && s.y > 0.0 && s.y < 1.0)
//				{
//					if(i->second->team==p->team)		graphics->setColor(0,1,0);
//					else if(distSquared > 2000*2000)	graphics->setColor(0.6,0.5,0.5);
//					else								graphics->setColor(0.5,0,0);
//
//					graphics->drawOverlay(Rect::XYXY(vX + (s.x - 0.006) * vWidth,vY + s.y * vHeight - 0.006 * vWidth,vX + (s.x + 0.006) * vWidth, vY + s.y * vHeight + 0.006 * vWidth),"target ring");
//				}
//			}
//		}
//		graphics->setColor(1,1,1);
//	}
//}
void dogFight::targeter(float x, float y, float apothem, Angle tilt) const
{
	graphics->drawRotatedOverlay(Rect::CWH(x,y,apothem*2,-apothem*2),tilt,"tilt");
	graphics->drawOverlay(Rect::CWH(x,y,apothem*2,-apothem*2),"targeter");
}
void dogFight::drawHexCylinder(shared_ptr<GraphicsManager::View> view, Vec3f center, float radius, float height, Color c) const
{
	auto hexGrid = shaders("hex grid shader");
    hexGrid->bind();
	dataManager.bind("hex grid", 0);

	hexGrid->setUniform1i("tex",0);
	hexGrid->setUniform1f("minHeight",center.y);
	hexGrid->setUniform1f("maxHeight",center.y+height);
	hexGrid->setUniform1f("radius",radius);
	hexGrid->setUniform4f("color",c.r,c.g,c.b,c.a);

	//graphics->drawModelCustomShader("cylinder", Vec3f(center), ,Vec3f(radius,height,radius));
	sceneManager.drawMesh(view, dataManager.getModel("cylinder"), Mat4f(Quat4f(), center, radius), hexGrid);
	dataManager.bind("model");
}
void dogFight::drawHudIndicator(shared_ptr<GraphicsManager::View> view, shared_ptr<plane> p, shared_ptr<object> targetPtr, Color4 color, Color4 nightColor) const
{
	if(targetPtr && targetPtr->meshInstance != nullptr)
	{
		float interpolate = world->time.interpolate();
		Vec3f planePos = lerp(p->lastPosition, p->position, interpolate);
		Vec3f targetPos = lerp(targetPtr->lastPosition, targetPtr->position, interpolate);
		Quat4f targetRot = slerp(targetPtr->lastRotation, targetPtr->rotation, interpolate);

		Vec3f targetOffset = -targetRot * targetPtr->meshInstance->getBoundingSphere().center;
		Vec3f proj = (view->projectionMatrix() * view->modelViewMatrix()) * (targetPos+targetOffset);
		if(proj.z < 1.0 && (proj.x > -1.02 && proj.x < 1.02) && (proj.y > -1.02 && proj.y < 1.02))
		{
			auto circleShader = shaders.bind("circle shader");
			circleShader->setUniform4f("viewConstraint", graphics->getViewContraint());

			graphics->setColor(level->info.night ? nightColor : color);
			graphics->drawOverlay(Rect::CWH(view->project(targetPos+targetOffset), Vec2f(0.02,0.02)));

			if(p->team != targetPtr->team && targetPtr->type & PLANE && p->position.distanceSquared(targetPos+targetOffset) <= 2000.0*2000.0)
			{
				auto targetPlanePtr = dynamic_pointer_cast<plane>(targetPtr);
				float s = bullet::bulletSpeed; //speed of bullets
				Vec3f r = (targetPos+targetOffset) - planePos;
				Vec3f v = targetRot * Vec3f(0,0,targetPlanePtr->speed);
	
				float a = v.dot(v) - s*s;
				float b = 2.0 * v.dot(r);
				float c = r.dot(r);
				float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);
				if(b*b - 4.0*a*c >= 0.0 && t >= 0)
				{
					graphics->setColor(level->info.night ? Color4(0.4,0.4,0.4,0.3) : Color4(1,1,1,0.3));
					graphics->drawOverlay(Rect::CWH(view->project(targetPos+targetOffset + v * t), Vec2f(0.015,0.015)));
				}
			}
			graphics->setColor(1,1,1);
			shaders.bind("ortho");
		}
		else
		{
			//Vec3f fwd = view->camera().fwd;
			Vec3f up = view->camera().up;
			Vec3f right = view->camera().right;
			Vec3f direction = (targetPos - planePos).normalize();
			Vec2f screenDirection(direction.dot(right), direction.dot(up));
			screenDirection = screenDirection.normalize();

			float w = view->viewport().width;
			float h = view->viewport().height;
			if(screenDirection.x/(w-0.1) > abs(screenDirection.y/(h-0.1)))
				screenDirection = Vec2f(w-0.1, screenDirection.y * (w-0.1) / screenDirection.x);
			else if(-screenDirection.x/(w-0.1) > abs(screenDirection.y/(h-0.1)))
				screenDirection = Vec2f(0.1-w, screenDirection.y * (0.1-w) / screenDirection.x);
			else if(screenDirection.y/(h-0.1) > abs(screenDirection.x/(w-0.1)))
				screenDirection = Vec2f(screenDirection.x * (h-0.1) / screenDirection.y, h-0.1);
			else
				screenDirection = Vec2f(screenDirection.x * (0.1-h) / screenDirection.y, 0.1-h);

			Angle ang = atan2A(screenDirection.y,screenDirection.x);
			float radius = (w-0.1)*(h-0.1) / sqrt( (h-0.1)*(h-0.1)*cos(ang)*cos(ang) + (w-0.1)*(w-0.1)*sin(ang)*sin(ang) );//min(screenDirection.magnitude(), (w-0.1)*abs(cos(ang))+(h-0.1)*abs(sin(ang)));

			screenDirection.x = radius*cos(ang)*0.5 + 0.5*w + view->viewport().x;
			screenDirection.y = -radius*sin(ang)*0.5 + 0.5*h + view->viewport().y;

			graphics->setColor(level->info.night ? nightColor : color);
			graphics->drawRotatedOverlay(Rect::CWH(screenDirection, Vec2f(0.08,0.08)),ang, "arrow");
			graphics->setColor(1,1,1);
		}
	}
}
void dogFight::drawScene(shared_ptr<GraphicsManager::View> view, int acplayer)
{
	static map<int,double> lastDraw;
	double time=world->time();
//	double interp = 1.0 - world->time.interpolate();

	auto p = players[acplayer]->getObject();
	if(!p)
	{
		return;
	}

//	auto camera = players[acplayer]->getCamera(p->controled || p->dead);
//	Vec3f e = camera.eye;
	//graphics->lookAt(camera.eye, camera.center, camera.up);

	world->renderTerrain(view);

	graphics->setDepthMask(false);

	Vec3f cCenter(world->bounds().center.x,0,world->bounds().center.y);

	//drawHexCylinder(cCenter,cRadius,20000, white);

#ifdef _DEBUG
	players.debugDraw();
#endif

	graphics->setDepthMask(true);

	lastDraw[acplayer] = time;

	graphics->checkErrors(); //calls debugBreak() if openGL has previously encountered an error (can be placed after any line to pinpoint the error)
}
void dogFight::updateSimulation()
{
	players.update();
	world->simulationUpdate();

	auto planes = world->getAllOfType(PLANE);
	auto AAA = world->getAllOfType(ANTI_AIRCRAFT_ARTILLARY);
	auto ships = world->getAllOfType(SHIP);
	auto missiles = world->getAllOfType(MISSILE);
	auto& bulletsRef = dynamic_pointer_cast<bulletCloud>(world->getObjectById(bullets))->bullets;

	float bulletLineLength = bullet::bulletSpeed * world->time.length() * 0.001;
	
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		shared_ptr<plane> p = dynamic_pointer_cast<plane>(i->second); //probably should replace all the 'i->second' with 'p'
		float damageMultiplier = p->controlType == plane::CONTROL_TYPE_AI ? 1.0f : 0.25f;
		if(!i->second->dead && i->second->collisionInstance)
		{
			Sphere<float> boundingSphere = i->second->collisionInstance->boundingSphere();
			float bulletTestRadiusSquared = (bulletLineLength+boundingSphere.radius)*(bulletLineLength+boundingSphere.radius);
			for(auto l=0;l<(signed int)bulletsRef.size();l++)
			{
				bullet& bulletRef = bulletsRef[l];
				if(bulletRef.owner != i->second->id && bulletRef.startTime < world->time.lastTime() && bulletRef.startTime + bulletRef.life > world->time())
				{
					Vec3f lineStart = bulletRef.startPos+bulletRef.velocity*(world->time()-bulletRef.startTime)*0.001f;
					if(lineStart.distanceSquared(boundingSphere.center) < bulletTestRadiusSquared)
					{
						Vec3f lineEnd = bulletRef.startPos+bulletRef.velocity*(world->time.lastTime()-bulletRef.startTime)*0.001f;
						if(collisionManager.testSphereSegment(boundingSphere,lineStart,lineEnd))
						{
							i->second->loseHealth(14.4f * damageMultiplier);

							if((*i).second->dead)
							{
								if(players.numPlayers() >= 1 && bulletRef.owner==players[0]->objectNum()) players[0]->addKill();
								if(players.numPlayers() >= 2 && bulletRef.owner==players[1]->objectNum()) players[1]->addKill();
							}
							bulletsRef.erase(bulletsRef.begin()+l);
							l--;
						}
					}
				}
			}
			for(auto l=missiles.begin();l!=missiles.end();l++)
			{
				objId owner = dynamic_pointer_cast<missileBase>(l->second)->owner;
				if(owner != i->second->id &&  owner != (*i).first && !l->second->awaitingDelete && collisionManager(i->second,l->second))
				{
					i->second->loseHealth(105 * damageMultiplier);

					if((*i).second->dead)
					{
						if(players.numPlayers() >= 1 && owner==players[0]->objectNum()) players[0]->addKill();
						if(players.numPlayers() >= 2 && owner==players[1]->objectNum()) players[1]->addKill();
					}
					else
					{
						particleManager.addEmitter(new particle::explosion(), l->second->position, 5.0);
					//	particleManager.addEmitter(new particle::explosionSmoke(), l->second->position, 2.0);
						particleManager.addEmitter(new particle::explosionFlash(), l->second->position, 5.0);
					//	particleManager.addEmitter(new particle::explosionFlash2(), l->second->position, 2.0);
					}
					l->second->awaitingDelete = true;
				}
			}
			if(collisionManager.groundCollsion(i->second))
			{
				p->die(world->isLand(i->second->position.x,i->second->position.z) ? plane::DEATH_HIT_GROUND : plane::DEATH_HIT_WATER);
			}
		}
	}
	for(auto i = AAA.begin(); i != AAA.end();i++)
	{
		if(!i->second->dead && i->second->collisionInstance)
		{
			Sphere<float> boundingSphere = i->second->collisionInstance->boundingSphere();
			float bulletTestRadiusSquared = (bulletLineLength+boundingSphere.radius)*(bulletLineLength+boundingSphere.radius);
			for(auto l=0;l<(signed int)bulletsRef.size();l++)
			{
				bullet& bulletRef = bulletsRef[l];
				if(bulletRef.owner != i->second->id && bulletRef.startTime < world->time.lastTime() && bulletRef.startTime + bulletRef.life > world->time())
				{
					Vec3f lineStart = bulletRef.startPos+bulletRef.velocity*(world->time()-bulletRef.startTime)*0.001f;
					if(lineStart.distanceSquared(boundingSphere.center) < bulletTestRadiusSquared)
					{
						Vec3f lineEnd = bulletRef.startPos+bulletRef.velocity*(world->time.lastTime()-bulletRef.startTime)*0.001f;
						if(collisionManager.testSphereSegment(boundingSphere,lineStart,lineEnd))
						{
							i->second->loseHealth(14.4f);

							if((*i).second->dead)
							{
								if(players.numPlayers() >= 1 && bulletRef.owner==players[0]->objectNum()) players[0]->addKill();
								if(players.numPlayers() >= 2 && bulletRef.owner==players[1]->objectNum()) players[1]->addKill();
							}
							bulletsRef.erase(bulletsRef.begin()+l);
							l--;
						}
					}
				}
			}
			for(auto l=missiles.begin();l!=missiles.end();l++)
			{
				objId owner = dynamic_pointer_cast<missileBase>(l->second)->owner;
				if(owner != i->second->id &&  owner != (*i).first && !l->second->awaitingDelete && collisionManager(i->second,l->second))
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
	for(auto i = ships.begin(); i != ships.end();i++)
	{
		if(!i->second->dead && i->second->collisionInstance)
		{
			Sphere<float> boundingSphere = i->second->collisionInstance->boundingSphere();
			float bulletTestRadiusSquared = (bulletLineLength+boundingSphere.radius)*(bulletLineLength+boundingSphere.radius);
			for(auto l=0;l<(signed int)bulletsRef.size();l++)
			{
				bullet& bulletRef = bulletsRef[l];
				if(bulletRef.owner != i->second->id && bulletRef.startTime < world->time.lastTime() && bulletRef.startTime + bulletRef.life > world->time())
				{
					Vec3f lineStart = bulletRef.startPos+bulletRef.velocity*(world->time()-bulletRef.startTime)*0.001f;
					if(lineStart.distanceSquared(boundingSphere.center) < bulletTestRadiusSquared)
					{
						Vec3f lineEnd = bulletRef.startPos+bulletRef.velocity*(world->time.lastTime()-bulletRef.startTime)*0.001f;
						if(collisionManager.testSphereSegment(boundingSphere,lineStart,lineEnd))
						{
							i->second->loseHealth(14.4f);

							if((*i).second->dead)
							{
								if(players.numPlayers() >= 1 && bulletRef.owner==players[0]->objectNum()) players[0]->addKill();
								if(players.numPlayers() >= 2 && bulletRef.owner==players[1]->objectNum()) players[1]->addKill();
							}
							bulletsRef.erase(bulletsRef.begin()+l);
							l--;
						}
					}
				}
			}
			for(auto l=missiles.begin();l!=missiles.end();l++)
			{
				objId owner = dynamic_pointer_cast<missileBase>(l->second)->owner;
				if(owner != i->second->id &&  owner != (*i).first && !l->second->awaitingDelete && collisionManager(i->second,l->second))
				{
					i->second->loseHealth(35);
					if((*i).second->dead)
					{
						if(players.numPlayers() >= 1 && owner==players[0]->objectNum()) players[0]->addKill();
						if(players.numPlayers() >= 2 && owner==players[1]->objectNum()) players[1]->addKill();
					}
					else
					{
						particleManager.addEmitter(new particle::explosion(), l->second->position, 5.0);
						particleManager.addEmitter(new particle::explosionFlash(), l->second->position, 5.0);
					}
					l->second->awaitingDelete = true;
				}
			}
		}
	}

	//Vec3f bulletPosition;				//PARTICLE EFFECTS FOR GROUND/WATER COLLISIONS
	//for(auto i=0;i<(signed int)bulletRef.size();i++)
	//{
	//	bulletPosition = bulletRef[i].startPos+bulletRef[i].velocity*(world->time()-bulletRef[i].startTime)/1000;
	//	if(world->altitude(bulletPosition) < 0.0)
	//	{
	//		if(world->isLand(bulletPosition.x, bulletPosition.z))
	//		{
	//			world->addDecal(Vec2f(bulletPosition.x, bulletPosition.z), 3.0, 3.0, "scorch", 100.0); 
	//		}
	//		bulletRef[i].life = world->time()-bulletRef[i].startTime; //makes the bullet die at the current time
	//	}
	//}

	int numObjects=0;
	map<int,int> objectsPerTeam;

	for(auto i = planes.begin(); i != planes.end();i++)
	{
		if(!(*i).second->dead)
		{
			numObjects++;

			if(objectsPerTeam.find(i->second->team) != objectsPerTeam.end())
				objectsPerTeam[i->second->team]++;
			else
				objectsPerTeam[i->second->team] = 1;
		}
	}
	for(auto i = AAA.begin(); i != AAA.end();i++)
	{
		if(!(*i).second->dead)
		{
			numObjects++;
			objectsPerTeam[i->second->team]++;
		}
	}
	for(auto i = ships.begin(); i != ships.end();i++)
	{
		if(!(*i).second->dead)
		{
			numObjects++;
			objectsPerTeam[i->second->team]++;
		}
	}

	for(auto i = triggers.begin(); i != triggers.end();)
	{
		bool conditionTrue = false;
		if(i->condition && i->condition->conditionType == LevelFile::Trigger::Condition::NUM_OBJECTS)
		{
			conditionTrue = i->checkComparison(numObjects);
		}
		else if(i->condition && i->condition->conditionType == LevelFile::Trigger::Condition::NUM_OBJECTS_ON_TEAM)
		{
			conditionTrue = i->checkComparison(objectsPerTeam[static_pointer_cast<LevelFile::Trigger::ConditionNumObjectsOnTeam>(i->condition)->teamNumber]);
		}
		else if(i->comparison == LevelFile::Trigger::ALWAYS)
		{
			conditionTrue = true;
		}


		if(conditionTrue)
		{
			for(auto a = i->actions.begin(); a != i->actions.end(); a++)
			{
				if((*a) && (*a)->actionType == LevelFile::Trigger::Action::START_PATH)
				{
					auto startPath = static_pointer_cast<LevelFile::Trigger::ActionStartPath>(*a);
					shared_ptr<object> objPtr = world->getObjectById(startPath->objectNumber+1); //hack since object 1 is bullet cloud!!!
					if(objPtr && (objPtr->type & PLANE) && startPath->pathNumber > 0 && level->paths.size() >= startPath->pathNumber)
					{
						shared_ptr<plane> planePtr = dynamic_pointer_cast<plane>(objPtr);

						Vec3f lastPoint, nextPoint;
						auto waypoints = level->paths[startPath->pathNumber-1].waypoints;
						for(auto w = waypoints.begin(); w != waypoints.end(); w++)
						{
							Vec3f lastPoint = w != waypoints.begin() ? (w-1)->position : w->position;
							Vec3f nextPoint = w+1 != waypoints.end() ? (w+1)->position : w->position;
							planePtr->wayPoints.push_back(plane::wayPoint(world->time() + w->time, w->position, Quat4f((nextPoint-lastPoint)*0.5)));
						}
						planePtr->controled = true;
					}
					else
					{
						debugBreak(); //invalid path or object number given
					}
				}
			}

			if(i->destroyTrigger)
			{
				i = triggers.erase(i);
			}
		}
		else
		{
			i++;
		}
	}
}
}
