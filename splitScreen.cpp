

#include "game.h"

namespace gui{
splitScreen::splitScreen(shared_ptr<LevelFile> lvl): dogFight(lvl), countdown(0.0), restart(false), levelup(false), victory(false)
{
	views[0] = graphics->genView();
	views[0]->viewport(0, 0.0, sAspect, 0.5);
	views[0]->perspective(40.0, (double)sw / ((double)sh/2), 1.0, 500000.0);
	views[0]->setRenderFunc(std::bind(&splitScreen::render3D, this, std::placeholders::_1), 0);

	views[1] = graphics->genView();
	views[1]->viewport(0, 0.5, sAspect, 0.5);
	views[1]->perspective(40.0, (double)sw / ((double)sh/2),1.0, 500000.0);
	views[1]->setRenderFunc(std::bind(&splitScreen::render3D, this, std::placeholders::_1), 1);

	graphics->setLightPosition(Vec3f(0.0, 16000.0, 10000.0));
}
bool splitScreen::init()
{
	world.create();
	level->initializeWorld(2);

	if(players[0]->getObject()->team == players[1]->getObject()->team)
		gameType = COOPERATIVE;
	else
		gameType = PLAYER_VS_PLAYER;

	return true;
}
void splitScreen::updateFrame()
{
	//set camera position
	for(int i=0; i<2; i++)
	{
		nPlane* p=(nPlane*)players[i]->getObject();
		auto camera = players[i]->getCamera(p->controled || p->dead);
		views[i]->lookAt(camera.eye, camera.center, camera.up);
	}

	//check whether to toggle first person views
	if(input.getKey(VK_F1))	{	players[0]->toggleFirstPerson(); input.up(VK_F1);}
	if(input.getKey(VK_F2))	{	players[1]->toggleFirstPerson(); input.up(VK_F2);}

	//check whether to bring up the in-game menu
	if(input.getKey(VK_ESCAPE) || input.getXboxController(0).getButton(XINPUT_START) || input.getXboxController(1).getButton(XINPUT_START))
	{
		menuManager.setPopup(new gui::inGame);
		input.up(VK_ESCAPE);
	}

#ifdef _DEBUG
	//slow down the game speed to 10%
	if(input.getKey(0x54))
	{
		input.up(0x54);
		if(world.time.getSpeed() > 0.5)
		{
			world.time.changeSpeed(0.1, 5.0);
		}
		else
		{
			world.time.changeSpeed(1.0, 5.0);
		}
	}
#endif
	//((nPlane*)world.objectList[players[0].objectNum()])->setControlState(players[0].getControlState());
	//((nPlane*)world.objectList[players[1].objectNum()])->setControlState(players[1].getControlState());
	if(gameType == COOPERATIVE)
	{
		if(levelup)
		{
			countdown-=world.time.length();
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
			countdown-=world.time.length();
			if(countdown<=0)
			{
				menuManager.setMenu(new gui::splitScreen(level));
			}
		}
		else if(victory)
		{
			countdown-=world.time.length();
			if(countdown<=0)
			{
				menuManager.setMenu(new gui::chooseMode());
			}
		}
		else
		{
			int enemies_left=0;
			auto planes = world(PLANE);
			auto AAA = world(ANTI_AIRCRAFT_ARTILLARY);
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
		nPlane* p=(nPlane*)players[acplayer]->getObject();
		if(players[acplayer]->firstPersonView && !p->controled)
		{
			graphics->drawOverlay(Rect::XYXY(0,0.5*acplayer,sAspect,0.5*(acplayer+1)),"cockpit");
			targeter(sAspect*0.5,0.25+0.5*acplayer,0.049,-p->roll);
			radar(0.222 * sAspect, 0.437+0.5*acplayer, 0.1, -0.1, true, p);
			healthBar(0.175*sAspect, 0.6-0.5*acplayer, 0.25*sAspect, 0.333, p->health/100.0,true);
		}
		else if(!p->dead)
		{
			if(p->target != 0 && world[p->target] != nullptr)
			{
				Vec3f proj = (views[acplayer]->projectionMatrix() * views[acplayer]->modelViewMatrix()) * world[p->target]->position;
				if(proj.z < 1.0 && (proj.x > -1.02 && proj.x < 1.02) && (proj.y > -1.02 && proj.y < 1.02))
				{
					shaders.bind("circle shader");
					if(p->targetLocked)		graphics->setColor(1,0,0);
					else					graphics->setColor(0,0,1);
					graphics->drawOverlay(Rect::CWH(views[acplayer]->project(world[p->target]->position), Vec2f(0.02,0.02)));

					if(world[p->target]->type & PLANE && p->position.distanceSquared(world[p->target]->position) <= 2000.0*2000.0)
					{
						auto targetPtr = dynamic_pointer_cast<nPlane>(world[p->target]);
						float s = 1000; //speed of bullets
						Vec3f r = targetPtr->position - p->position;
						Vec3f v = targetPtr->rotation * Vec3f(0,0,targetPtr->speed);
		
						float a = v.dot(v) - s*s;
						float b = 2.0 * v.dot(r);
						float c = r.dot(r);
		
						if(b*b - 4.0*a*c >= 0.0)
						{
							float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);
		
							if(t <= 0.0) //can only happen when plane is flying faster than bullets
							{
								t = (-b + sqrt(b*b - 4.0*a*c)) / (2.0 * a);
							}
		
							graphics->setColor(1,1,1,0.3);
							graphics->drawOverlay(Rect::CWH(views[acplayer]->project(targetPtr->position + v * t), Vec2f(0.015,0.015)));
						}
					}
					graphics->setColor(1,1,1);
					shaders.bind("ortho");
				}
				else
				{
					Vec3f fwd = views[acplayer]->camera().fwd;
					Vec3f up = views[acplayer]->camera().up;
					Vec3f right = views[acplayer]->camera().right;
					Vec3f direction = (world[p->target]->position - p->position).normalize();
					Vec3f projectedDirection = direction - fwd * (fwd.dot(direction));
					Vec2f screenDirection(projectedDirection.dot(right), projectedDirection.dot(up));
					screenDirection = screenDirection.normalize();
					Angle ang = atan2A(screenDirection.y,screenDirection.x);
					screenDirection.x = clamp( (screenDirection.x+1.0)*sAspect/2.0, 0.05, sAspect-0.05);
					screenDirection.y = clamp( (-screenDirection.y+1.0)/2.0, 0.05, 0.95) * 0.5 + 0.5 * acplayer;
					if(p->targetLocked)		graphics->setColor(1,0,0);
					else					graphics->setColor(0,0,1);
					graphics->drawRotatedOverlay(Rect::CWH(screenDirection, Vec2f(0.08,0.08)),ang, "arrow");
					graphics->setColor(1,1,1);
				}
			}

	//		radar(sAspect-0.11, 0.389+0.5*acplayer, 0.094, 0.094, false, p);	
	//		healthBar(sAspect-0.024-0.146, 0.024+0.5*acplayer, 0.146, 0.024, p->health/p->maxHealth,false);
		}
	}
	if(levelup)
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, v, 0.25*v), "next level");
	}
	if(victory)
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
	if(players[v]->firstPersonView && !((nPlane*)players[v]->getObject())->controled && !players[v]->getObject()->dead)
		sceneManager.renderScene(views[v], players[v]->getObject()->meshInstance);
	else
		sceneManager.renderScene(views[v]);

	world.renderFoliage(views[v]);

	if(players[v]->firstPersonView && !((nPlane*)players[v]->getObject())->controled && !players[v]->getObject()->dead)
		sceneManager.renderSceneTransparency(views[v], players[v]->getObject()->meshInstance);
	else
		sceneManager.renderSceneTransparency(views[v]);

//	glViewport(0, 0, sw, sh/2);
//	drawScene(1);
}
}