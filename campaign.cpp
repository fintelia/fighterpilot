
#include "main.h"

//modeCampaign::modeCampaign(): modeDogFight(new Level("media/heightmap5.bmp",1000)), levelup(false), countdown(0.0)
//{
//	//settings.ENEMY_PLANES=2;
//	//settings.GAME_TYPE=FFA;
//	//settings.HEIGHT_RANGE=1000;
//	//settings.KILL_PERCENT_NEEDED=100;
//	//settings.LEVEL_NAME="unnamed";
//	//settings.MAP_FILE="media/heightmap5.bmp";
//	//settings.MAP_TYPE=WATER;
//	//settings.MAX_X=world.level->ground()->sizeX();
//	//settings.MAX_Y=world.level->ground()->sizeZ();
//	//settings.MIN_X=0;
//	//settings.MIN_Y=0;
//	//settings.ON_AI_HIT=RESPAWN;
//	//settings.ON_HIT=RESPAWN;
//	//settings.SEA_FLOOR_TYPE=ROCK;
//	//settings.SEA_LEVEL=-150;
//
//	world.level->ground()->setSize(world.level->ground()->size()*128);
//	((Level::heightmapGL*)world.level->ground())->setShader(dataManager.getId("grass new terrain"));
//
//	LevelFile::Object obj;
//	obj.controlType = CONTROL_HUMAN;
//	obj.startRot = Quat4f();
//	obj.type = defaultPlane;
//	obj.team = TEAM0;
//	obj.startloc = Vec3f(rand()%1000,300,rand()%1000);
//	world.objectList.newObject(obj);
//
//	obj.controlType = CONTROL_COMPUTER;
//	for(int i = 0; i < 2; i++)
//	{
//		obj.team = TEAM0<<(i+1);
//		obj.startloc = Vec3f(rand()%1000,300,rand()%1000);
//		world.objectList.newObject(obj);
//	}
//}
int modeCampaign::update()
{
	if(input->getKey(VK_F1))
	{
		players[0].toggleFirstPerson();
		input->up(VK_F1);
	}
	if(input->getKey(VK_ESCAPE))
	{
		menuManager.setMenu(new menu::inGame);
		input->up(VK_ESCAPE);
	}
	world.update();

#ifdef _DEBUG
	if(input->getKey(0x54))
	{
		input->up(0x54);
		if(world.time.getSpeed() > 0.5)
		{
			world.time.changeSpeed(0.1, 5.0);
		}
		else
		{
			world.time.changeSpeed(1.0, 5.0);
		}
	}
	if(input->getKey(0x4c))
	{
		((nPlane*)world[players[0].objectNum()].get())->loseHealth(world.time.length()/10.0);
	}
#endif

	if(levelup)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			string nLevel = world.level->getLevelNext();
			if(nLevel == "") nLevel="media/map file.lvl";

			std::shared_ptr<Level> l(new Level);
			if(l->init(nLevel))
			{
				modeManager.setMode(new modeCampaign(l));
			}
		}
	}
	else if(restart)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			modeManager.setMode(new modeCampaign(world.level));
		}
	}
	else
	{
		int enemies_left=0;
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			if((*i).second->team != world[players[0].objectNum()]->team && !(*i).second->dead)
				enemies_left++;
		}

		if(enemies_left == 0)
		{
			levelup=true;
			countdown=1000;
		}
		else if(world[players[0].objectNum()]->dead)
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
	//if((100-enemies_left*100/settings.ENEMY_PLANES>=settings.KILL_PERCENT_NEEDED || input->getKey(0x4E)) && (levelNum<TOTAL_LEVELS && !levelup))
	//{
	//	levelup=true;//newLevel(level+1);
	//	countdown=1000;
	//}
	return 30;
}
void modeCampaign::draw2D()
{
	nPlane* p = (nPlane*)world[players[0].objectNum()].get();
	
	if(players[0].firstPerson() && !p->controled && !p->dead)
	{
	//	planeIdBoxes(p,0,0,sw,sh);
	//	dataManager.bind("ortho");
		graphics->drawOverlay(Rect::XYXY(0,1.0,sAspect,0.0),"cockpit square");
	//	dataManager.unbindShader();

		targeter(0.5*sAspect, 0.5, 0.08, p->roll);
		radar(0.2 * sAspect, 0.433, 0.125, -0.125, true, p);
		
		healthBar(0.175*sAspect, 1.0-0.35, 0.25*sAspect, -0.333, p->health/p->maxHealth,true);

		//speedMeter(280,533,344,597,p.accel.magnitude()*30.5+212);
		//altitudeMeter(456,533,520,597,p.altitude);
	}
	else if(!p->dead  && !p->controled)
	{
		radar(sAspect-0.167, 0.167, 0.1333, -0.1333, false, p);
		healthBar(0.768*sAspect, 0.958, 0.188*sAspect, -0.042, p->health/p->maxHealth,false);
	}
	

	if(levelup)
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, sAspect*v, v), "next level");
	}
}
void modeCampaign::draw3D()
{
	//static int n = 0;	n++;
	//if(n <= 1) return;

	graphics->perspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	drawScene(0);
}
void modeCampaign::drawParticles()
{
	graphics->perspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	drawSceneParticles(0);
	//particleManager.render();
}