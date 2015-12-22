
#include "engine.h"
#include "game.h"

namespace gui{
	campaign::campaign(shared_ptr<const LevelFile> lvl): dogFight(lvl), countdown(0.0), restart(false), levelup(false), victory(false)
#ifdef _DEBUG
    ,slow(false), wireframe(false)
#endif
{
	view = graphics->genView();
	view->viewport(0, 0, sAspect, 1.0);
	view->perspective(50.0, (double)sw / ((double)sh), 1.0, 2000000.0);
	view->setRenderFunc(bind(&campaign::render3D, this, std::placeholders::_1));
	view->setTransparentRenderFunc(bind(&campaign::renderTransparency, this, std::placeholders::_1));
	//graphics->setLightPosition(Vec3f(0.0, 1600000.0, 0.0));
	graphics->setLightPosition(Vec3f(0.0, 100000.0, 100000.0).normalize());
	//ephemeris.setTime(0.0, 1, 1, 2000.0);
	//graphics->setLightPosition(ephemeris.getSunDirection() * 100000.0);
}
bool campaign::init()
{
	world = unique_ptr<WorldManager>(new WorldManager(level->clipMap));
	level->initializeWorld(1);

	return true;
}
bool campaign::menuKey(int mkey)
{
	if(mkey == MENU_BACK)
	{
		menuManager.setPopup(new gui::inGame);
		return true;
	}
	return false;
}
void campaign::updateFrame()
{
	//static double timeSinceSkyUpdate = 0;
	//ephemeris.setTime(world->time()/3000, 1, 1, 2000.0);
	//Vec3f sun = ephemeris.getSunDirection();
	//graphics->setLightPosition(sun* 100000.0);
	//if(timeSinceSkyUpdate > 200.0)
	//{
	//	world->generateSky(acos(sun.y), atan2(sun.z,sun.x), 1.8);
	//	timeSinceSkyUpdate -= 200.0;
	//}
	//timeSinceSkyUpdate +=world->time.length();


	//auto& controller = input.getXboxController(0);

	//check whether to toggle first person view
	if(input.getKey(VK_F1))
	{
		players[0]->toggleFirstPerson();
		input.up(VK_F1);
	}

//	graphics->setLightPosition(Vec3f(	0.0 , 10000.0* cos(world->time() *0.001 * 0.1), 
//										100000.0 * sin(world->time() *0.001 * 0.1)));
//	graphics->setLightPosition(Vec3f(0.0,10000.0,100000.0));

	//set camera position
	shared_ptr<plane> p=players[0]->getObject();
	auto camera = players[0]->getCamera(p->controled || p->dead);
	view->lookAt(camera.eye, camera.center, camera.up);
	players[0]->setVibrate(p->cameraShake);

#ifdef _DEBUG
	//slow down game speed to 10%
	if(input.getKey(0x54))
	{
		input.up(0x54);
		if(!slow)
		{
			world->time.changeSpeed(0.1, 300.0);
			slow = true;
		}
		else
		{
			world->time.changeSpeed(1.0, 300.0);
			slow = false;
		}
	}
	if(input.getKey(0x55))
	{
		input.up(0x55);
		wireframe = !wireframe;
		world->setWireframe(wireframe);
	}
	//test death animation
	if(input.getKey(0x4c))
	{
		players[0]->getObject()->loseHealth(world->time.length()/30.0);
	}
#endif

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
				menuManager.setMenu(new gui::campaign(l));
			}
		}
	}
	else if(restart)
	{
		countdown-=world->time.length();
		if(countdown<=0)
		{
			menuManager.setMenu(new gui::campaign(level));
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
		else if(players[0]->getObject()->dead)
		{
			restart=true;
			countdown=5000;
		}
	}
}
void campaign::render()
{
	shared_ptr<plane> p = players[0]->getObject();

	if(players[0]->firstPersonView && !p->controled && !p->dead)
	{
		graphics->drawOverlay(Rect::XYXY(0,0.0,sAspect,1.0),"cockpit square");

		targeter(0.5*sAspect, 0.5, 0.08, -p->roll);
		radar(0.2 * sAspect, 0.567, 0.125, 0.125, true, p);

		healthBar(0.175*sAspect, 0.35, 0.25*sAspect, 0.333, p->health/100.0);
	}
	else if(!p->dead && !p->controled)
	{
		//radar(sAspect-0.167, 0.833, 0.1333, 0.1333, false, p);
		if(!graphics->isHighResScreenshot())
		{
			auto planes = world->getAllOfType(PLANE);
			for(auto i = planes.begin(); i != planes.end();i++)
			{
				if(i->second->id != p->id && i->second->team == p->team && !i->second->dead)
					drawHudIndicator(view, p, i->second, Color(0,1,0,0.5), Color4(0.66*0.2989,0.34+0.66*0.2989,0.66*0.2989,0.5));
			}
			
			if(p->target != 0 && !graphics->isHighResScreenshot())
			{
				auto targetPtr = world->getObjectById(p->target);
				drawHudIndicator(view, p, targetPtr, p->targetLocked ? Color4(1,0,0) : Color4(0,0,1), p->targetLocked ? Color4(0.34+0.66*0.2989,0.66*0.2989,0.66*0.2989) : Color4(0.66*0.1140,0.66*0.1140,0.34+0.66*0.1140,0.5));
			}
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

void campaign::render3D(unsigned int v)
{
	drawScene(view, 0);
	if(players[v]->firstPersonView && !(players[v]->getObject())->controled && !players[v]->getObject()->dead)
	{
		sceneManager.renderScene(view, players[v]->getObject()->meshInstance);
		world->renderFoliage(view);
		sceneManager.renderSceneTransparency(view, players[v]->getObject()->meshInstance);
	}
	else
	{
		sceneManager.renderScene(view);
		world->renderFoliage(view);
		sceneManager.renderSceneTransparency(view);
	}
}
void campaign::renderTransparency(unsigned int v)
{
	particleManager.render(view);
}
}
