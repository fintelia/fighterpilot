
#include "engine.h"
#include "game.h"

namespace gui{
splitScreen::splitScreen(shared_ptr<const LevelFile> lvl): dogFight(lvl), countdown(0.0), restart(false), levelup(false), victory(false)
{
	views[0] = graphics->genView();
	views[0]->viewport(0, 0.0, sAspect, 0.5);
	views[0]->perspective(40.0, (double)sw / ((double)sh/2), 1.0, 2000000.0);
	views[0]->setRenderFunc(std::bind(&splitScreen::render3D, this, std::placeholders::_1), 0);
	views[0]->setTransparentRenderFunc(std::bind(&splitScreen::renderTransparency, this, std::placeholders::_1), 0);

	views[1] = graphics->genView();
	views[1]->viewport(0, 0.5, sAspect, 0.5);
	views[1]->perspective(40.0, (double)sw / ((double)sh/2),1.0, 2000000.0);
	views[1]->setRenderFunc(std::bind(&splitScreen::render3D, this, std::placeholders::_1), 1);
	views[1]->setTransparentRenderFunc(std::bind(&splitScreen::renderTransparency, this, std::placeholders::_1), 1);

	graphics->setLightPosition(Vec3f(0.0, 16000.0, 10000.0));
}
bool splitScreen::init()
{
	world = unique_ptr<WorldManager>(new WorldManager(level->clipMap));
	level->initializeWorld(2);

	if(players[0]->getObject()->team == players[1]->getObject()->team)
		gameType = COOPERATIVE;
	else
		gameType = PLAYER_VS_PLAYER;

	return true;
}
bool splitScreen::menuKey(int mkey)
{
	if(mkey == MENU_BACK)
	{
		menuManager.setPopup(new gui::inGame);
		return true;
	}
	return false;
}
void splitScreen::updateFrame()
{
	//check whether to toggle first person views
	if(input.getKey(VK_F1))	{	players[0]->toggleFirstPerson(); input.up(VK_F1);}
	if(input.getKey(VK_F2))	{	players[1]->toggleFirstPerson(); input.up(VK_F2);}

	//set camera position
	for(int i=0; i<2; i++)
	{
		shared_ptr<plane> p = players[i]->getObject();
		auto camera = players[i]->getCamera(p->controled || p->dead);
		views[i]->lookAt(camera.eye, camera.center, camera.up);
	}

#ifdef _DEBUG
	//slow down the game speed to 10%
	if(input.getKey(0x54))
	{
		input.up(0x54);
		if(world->time.getSpeed() > 0.5)
		{
			world->time.changeSpeed(0.1, 5.0);
		}
		else
		{
			world->time.changeSpeed(1.0, 5.0);
		}
	}
#endif
	if(gameType == COOPERATIVE)
	{
		if(levelup)
		{
			countdown-=world->time.length();
			if(countdown<=0)
			{
				string nLevel = level->info.nextLevel;
				//if(nLevel == "") nLevel="media/map file.lvl";

				shared_ptr<LevelFile> l(new LevelFile);
				if(l->loadZIP(nLevel))
				{
					menuManager.setMenu(new gui::splitScreen(l));
				}
			}
		}
		else if(restart)
		{
			countdown-=world->time.length();
			if(countdown<=0)
			{
				menuManager.setMenu(new gui::splitScreen(level));
			}
		}
		else if(victory)
		{
			countdown-=world->time.length();
			if(countdown<=0)
			{
				menuManager.setMenu(new gui::chooseMode);
				menuManager.setPopup(new gui::credits);
			}
		}
		else
		{
			int enemies_left=0;
			auto planes = world->getAllOfType(PLANE);
			auto AAA = world->getAllOfType(ANTI_AIRCRAFT_ARTILLARY);
			for(auto i = planes.begin(); i != planes.end();i++)
			{
				if((*i).second->team != players[0]->getObject()->team && !(*i).second->dead)
					enemies_left++;
			}
			for(auto i = AAA.begin(); i != AAA.end();i++)
			{
				if((*i).second->team != players[0]->getObject()->team && !(*i).second->dead)
					enemies_left++;
			}

			if(enemies_left == 0 && level->info.nextLevel != "")
			{
				levelup=true;
				countdown=1000;
			}
			else if(enemies_left == 0)
			{
				victory=true;
				countdown=1000;
			}
			else if(players[0]->getObject()->dead && players[1]->getObject()->dead)
			{
				restart=true;
				countdown=5000;
			}
		}
	}
	else if(gameType == PLAYER_VS_PLAYER) //should be like cooperative victory instead of just a messageBox
	{
		if(players[1]->getObject()->dead)
		{
			menuManager.setMenu(new gui::chooseMode());	
			messageBox("Player 1 Wins!");
		}
		else if(players[0]->getObject()->dead)
		{
			menuManager.setMenu(new gui::chooseMode());
			messageBox("Player 2 Wins!");
		}
	}
}
void splitScreen::render()
{
	for(int acplayer=0; acplayer <= 1; acplayer++)
	{
		shared_ptr<plane> p = players[acplayer]->getObject();
		if(players[acplayer]->firstPersonView && !p->controled && !p->dead)
		{
			graphics->drawOverlay(Rect::XYXY(0,0.5*acplayer,sAspect,0.5*(acplayer+1)),"cockpit");
			targeter(sAspect*0.5,0.25+0.5*acplayer,0.049,-p->roll);
			radar(0.222 * sAspect, 0.437+0.5*acplayer, 0.1, -0.1, true, p);
			healthBar(0.175*sAspect, 0.1+0.5*acplayer, 0.25*sAspect, 0.333, p->health/100.0);
		}
		else if(!p->dead && !p->controled && !graphics->isHighResScreenshot())
		{

			auto planes = world->getAllOfType(PLANE);
			for(auto i = planes.begin(); i != planes.end();i++)
			{
				if(i->second->id != p->id && i->second->team == p->team && !i->second->dead)
					drawHudIndicator(views[acplayer], p, i->second, Color(0,1,0,0.5), Color4(0.66*0.2989,0.34+0.66*0.2989,0.66*0.2989,0.5));
			}
			
			if(p->target != 0 && !graphics->isHighResScreenshot())
			{
				auto targetPtr = world->getObjectById(p->target);
				drawHudIndicator(views[acplayer], p, targetPtr, p->targetLocked ? Color4(1,0,0) : Color4(0,0,1), p->targetLocked ? Color4(0.34+0.66*0.2989,0.66*0.2989,0.66*0.2989) : Color4(0.66*0.1140,0.66*0.1140,0.34+0.66*0.1140,0.5));
			}

			//if(p->target != 0 && world[p->target] != nullptr)
			//{
			//	auto targetPtr = world[p->target];
			//	if(targetPtr && targetPtr->meshInstance != nullptr)
			//	{
			//		float interpolate = world->time.interpolate();
			//		Vec3f planePos = lerp(p->lastPosition, p->position, interpolate);
			//		//Quat4f planeRot = slerp(p->lastRotation, p->rotation, interpolate);
			//		Vec3f targetPos = lerp(targetPtr->lastPosition, targetPtr->position, interpolate);
			//		Quat4f targetRot = slerp(targetPtr->lastRotation, targetPtr->rotation, interpolate);

			//		Vec3f targetOffset = -targetRot * targetPtr->meshInstance->getBoundingSphere().center;
			//		Vec3f proj = (views[acplayer]->projectionMatrix() * views[acplayer]->modelViewMatrix()) * (targetPos+targetOffset);
			//		if(proj.z < 1.0 && (proj.x > -1.02 && proj.x < 1.02) && (proj.y > -1.02 && proj.y < 1.02))
			//		{
			//			auto circleShader = shaders.bind("circle shader");
			//			circleShader->setUniform4f("viewConstraint", graphics->getViewContraint());

			//			if(level->info.night)
			//			{
			//				if(p->targetLocked)		graphics->setColor(0.04+0.36*0.2989,0.36*0.2989,0.36*0.2989);
			//				else					graphics->setColor(0.36*0.1140,0.36*0.1140,0.04+0.36*0.1140);
			//			}
			//			else
			//			{
			//				if(p->targetLocked)		graphics->setColor(1,0,0);
			//				else					graphics->setColor(0,0,1);
			//			}
			//			graphics->drawOverlay(Rect::CWH(views[acplayer]->project(targetPos+targetOffset), Vec2f(0.02,0.02)));

			//			if(targetPtr->type & PLANE && p->position.distanceSquared(targetPos+targetOffset) <= 2000.0*2000.0)
			//			{
			//				auto targetPlanePtr = dynamic_pointer_cast<plane>(targetPtr);
			//				float s = 1000; //speed of bullets
			//				Vec3f r = (targetPos+targetOffset) - planePos;
			//				Vec3f v = targetRot * Vec3f(0,0,targetPlanePtr->speed);
		
			//				float a = v.dot(v) - s*s;
			//				float b = 2.0 * v.dot(r);
			//				float c = r.dot(r);
			//				float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);
			//				if(b*b - 4.0*a*c >= 0.0 && t > 0.0)
			//				{
			//					if(level->info.night)	graphics->setColor(0.4,0.4,0.4,0.3);
			//					else					graphics->setColor(1,1,1,0.3);
			//					graphics->drawOverlay(Rect::CWH(views[acplayer]->project(targetPos+targetOffset + v * t), Vec2f(0.015,0.015)));
			//				}
			//			}
			//			graphics->setColor(1,1,1);
			//			shaders.bind("ortho");
			//		}
			//		else
			//		{
			//			Vec3f fwd = views[acplayer]->camera().fwd;
			//			Vec3f up = views[acplayer]->camera().up;
			//			Vec3f right = views[acplayer]->camera().right;
			//			Vec3f direction = (targetPos - planePos).normalize();
			//			Vec3f projectedDirection = direction - fwd * (fwd.dot(direction));
			//			Vec2f screenDirection(projectedDirection.dot(right)*0.5, projectedDirection.dot(up));
			//			screenDirection = screenDirection.normalize();
			//			Angle ang = atan2A(screenDirection.y,screenDirection.x);
			//			screenDirection.x = clamp( (screenDirection.x+1.0)*sAspect/2.0, 0.05, sAspect-0.05);
			//			screenDirection.y = clamp( (-screenDirection.y+1.0)/2.0, 0.05, 0.95) * 0.5 + 0.5 * acplayer;
			//			if(level->info.night)
			//			{
			//				if(p->targetLocked)		graphics->setColor(0.04+0.36*0.2989,0.36*0.2989,0.36*0.2989);
			//				else					graphics->setColor(0.36*0.1140,0.36*0.1140,0.04+0.36*0.1140);
			//			}
			//			else
			//			{
			//				if(p->targetLocked)		graphics->setColor(1,0,0);
			//				else					graphics->setColor(0,0,1);
			//			}
			//			graphics->drawRotatedOverlay(Rect::CWH(screenDirection, Vec2f(0.08,0.08)),ang, "arrow");
			//			graphics->setColor(1,1,1);
			//		}
			//	}
			//}

	//		radar(sAspect-0.11, 0.389+0.5*acplayer, 0.094, 0.094, false, p);	
	//		healthBar(sAspect-0.024-0.146, 0.024+0.5*acplayer, 0.146, 0.024, p->health/p->maxHealth,false);
		}
	}
	if(levelup && !graphics->isHighResScreenshot())
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, v, 0.25*v), "next level");
	}
	if(victory && !graphics->isHighResScreenshot())
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, v, 0.25*v), "victory");
	}
}
void splitScreen::render3D(unsigned int v)
{
	//if(firstFrame)
	//{
	//	firstFrame = false;
	//	return;
	//}

//	glClearColor(0.5f,0.8f,0.9f,1.0f);
//	glViewport(0, sh/2, sw, sh/2);
//	graphics->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	drawScene(views[v], v);
	if(players[v]->firstPersonView && !players[v]->getObject()->controled && !players[v]->getObject()->dead)
		sceneManager.renderScene(views[v], players[v]->getObject()->meshInstance);
	else
		sceneManager.renderScene(views[v]);

	world->renderFoliage(views[v]);

	if(players[v]->firstPersonView && !players[v]->getObject()->controled && !players[v]->getObject()->dead)
		sceneManager.renderSceneTransparency(views[v], players[v]->getObject()->meshInstance);
	else
		sceneManager.renderSceneTransparency(views[v]);

//	glViewport(0, 0, sw, sh/2);
//	drawScene(1);
}
void splitScreen::renderTransparency(unsigned int v)
{
	particleManager.render(views[v]);
}
}
