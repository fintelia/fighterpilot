
#include "game.h"

namespace gui{
campaign::campaign(std::shared_ptr<LevelFile> lvl): dogFight(lvl), countdown(0.0), restart(false), levelup(false)
{
	graphics->resetViews(1);
	graphics->viewport(0,0, sAspect,1.0);
	graphics->perspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
}
int campaign::update()
{
	//set camera position
	nPlane* p=(nPlane*)players[0]->getObject();
	auto camera = players[0]->getCamera(p->controled || p->dead);
	graphics->lookAt(camera.eye, camera.center, camera.up);

	//check whether to toggle first person view
	if(input.getKey(VK_F1))
	{
		players[0]->toggleFirstPerson();
		input.up(VK_F1);
	}
	//check whether to bring up the in-game menu
	if(input.getKey(VK_ESCAPE))
	{
		menuManager.setPopup(new gui::inGame);
		input.up(VK_ESCAPE);
	}
	//check for collisions between objects
	checkCollisions();

#ifdef _DEBUG
	//slow down game speed to 10%
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
	//test death animation
	if(input.getKey(0x4c))
	{
		((nPlane*)players[0]->getObject())->loseHealth(world.time.length()/10.0);
	}
#endif
	
	if(levelup)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			string nLevel = level->info->nextLevel;
			if(nLevel == "") nLevel="media/map file.lvl";

			std::shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadPNG(nLevel))
			{
				menuManager.setMenu(new gui::campaign(l));
		//		modeManager.setMode(new modeCampaign(l));
			}
		}
	}
	else if(restart)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			menuManager.setMenu(new gui::campaign(level));
		}
	}
	else
	{
		int enemies_left=0;
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			if((*i).second->team != players[0]->getObject()->team && !(*i).second->dead)
				enemies_left++;
		}

		if(enemies_left == 0)
		{
			levelup=true;
			countdown=1000;
		}
		else if(players[0]->getObject()->dead)
		{
			restart=true;
			countdown=3000;
		}
	}



	//((plane*)world.objectList[players[0].objectNum()])->setControlState(players[0].getControlState());
	


	//if(settings.ON_HIT==RESTART && world.objectList[players[0].objectNum()]->dead)
	//{
	//	//need to add code to restart the level
	//	return 30;
	//}
	//if((100-enemies_left*100/settings.ENEMY_PLANES>=settings.KILL_PERCENT_NEEDED || input.getKey(0x4E)) && (levelNum<TOTAL_LEVELS && !levelup))
	//{
	//	levelup=true;//newLevel(level+1);
	//	countdown=1000;
	//}
	return 30;
}
void campaign::render()
{
	nPlane* p = ((nPlane*)players[0]->getObject());
	
	if(players[0]->firstPersonView && !p->controled && !p->dead)
	{
	//	planeIdBoxes(p,0,0,sw,sh);
	//	dataManager.bind("ortho");
		graphics->drawOverlay(Rect::XYXY(0,0.0,sAspect,1.0),"cockpit square");
	//	dataManager.unbindShader();

		targeter(0.5*sAspect, 0.5, 0.08, p->roll);
		radar(0.2 * sAspect, 0.567, 0.125, 0.125, true, p);
		
		healthBar(0.175*sAspect, 0.35, 0.25*sAspect, 0.333, p->health/p->maxHealth,true);

		//speedMeter(280,533,344,597,p.accel.magnitude()*30.5+212);
		//altitudeMeter(456,533,520,597,p.altitude);
	}
	else if(!p->dead  && !p->controled)
	{
		radar(sAspect-0.167, 0.833, 0.1333, 0.1333, false, p);
		healthBar(0.768*sAspect, 0.042, 0.188*sAspect, 0.042, p->health/p->maxHealth,false);
	}
	

	if(levelup)
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, sAspect*v, v), "next level");
	}
}
void campaign::render3D(unsigned int view)
{
	drawScene(0);
	if(players[view]->firstPersonView && !((nPlane*)players[view]->getObject())->controled && !players[view]->getObject()->dead)
		sceneManager.renderScene(players[view]->getObject()->meshInstance);
	else
		sceneManager.renderScene();
}
}