
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
	if(levelup)
	{
		countdown-=world.time.length();
		if(countdown<0)
		{
			//need to add code to mode onto the next level
			return 30;
		}
	}
	if(input->getKey(VK_F1))
	{
		players[0].toggleFirstPerson();
		input->up(VK_F1);
	}
#ifdef _DEBUG
	if(input->getKey(0x31))
	{
		menuManager.setMenu(new menu::inGame);
		input->up(0x31);
	}
	if(input->getKey(0x4c))
	{
		((nPlane*)world.objectList[players[0].objectNum()])->loseHealth(world.time.length()/10.0);
	}
#else if
	if(input->getKey(VK_ESCAPE))
	{
		menuManager.setMenu(new menu::inGame);
		input->up(VK_ESCAPE);
	}
#endif
	//((plane*)world.objectList[players[0].objectNum()])->setControlState(players[0].getControlState());
	world.update();

	int enemies_left=0;
	const map<objId,nPlane*>& planes = world.planes();
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		if((*i).second->team != world.objectList[players[0].objectNum()]->team)
			enemies_left++;
	}

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

#ifdef _DEBUG
	static bool slow = false;
	if(input->getKey(0x54))
	{
		input->up(0x54);
		slow = !slow;
		world.time.changeSpeed(slow ? 0.1 : 1.0, 5.0);
	}
#endif
	return 30;
}
void modeCampaign::draw2D()
{
	nPlane* p=(nPlane*)world.objectList[players[0].objectNum()];
	
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
	

	//if(levelup)
	//{
	//	float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
	//	graphics->drawOverlay(Vec2f(sw/2-v*sw/2,sh-v*sh),Vec2f(sw,sh)*v,"next level");
	//}
}
void modeCampaign::draw3D()
{
	//static int n = 0;	n++;
	//if(n <= 1) return;

	gluPerspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	frustum.setCamInternals(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	drawScene(0);
}