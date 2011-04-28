
#include "main.h"

//modeSplitScreen::modeSplitScreen(): modeDogFight(new Level("media/heightmap5.bmp",1000))
//{
//	//settings.ENEMY_PLANES=8;
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
//	world.level->ground()->setSize(world.level->ground()->size()*size);
//	((Level::heightmapGL*)world.level->ground())->setShader(dataManager.getId("grass new terrain"));
//
//	LevelFile::Object obj;
//	obj.controlType = CONTROL_HUMAN;
//	obj.startRot = Quat4f();
//	obj.type = defaultPlane;
//	obj.team = TEAM0;
//	for(int i = 0; i < 2; i++)
//	{
//		obj.startloc = Vec3f(rand()%1000,50,rand()%1000);
//		world.objectList.newObject(obj);
//	}
//	obj.controlType = CONTROL_COMPUTER;
//	for(int i = 0; i < 8; i++)
//	{
//		obj.team = TEAM0<<(i+1);
//		obj.startloc = Vec3f(rand()%1000,50,rand()%1000);
//		world.objectList.newObject(obj);
//	}
//}
//modeSplitScreen::modeSplitScreen(Level* lvl): modeDogFight(lvl)
//{
//	//settings.ENEMY_PLANES=world.level->objects().size();
//	//settings.GAME_TYPE=TEAMS;
//	//settings.HEIGHT_RANGE=1000;
//	//settings.KILL_PERCENT_NEEDED=100;
//	//settings.LEVEL_NAME="unnamed";
//	//settings.MAP_TYPE=WATER;
//	//settings.MAX_X=world.level->ground()->sizeX();
//	//settings.MAX_Y=world.level->ground()->sizeZ();
//	//settings.MIN_X=0;
//	//settings.MIN_Y=0;
//	//settings.ON_AI_HIT=RESPAWN;
//	//settings.ON_HIT=RESPAWN;
//	//settings.SEA_FLOOR_TYPE=ROCK;
//	//settings.SEA_LEVEL=world.level->water().seaLevel;
//
//	((Level::heightmapGL*)world.level->ground())->setShader(dataManager.getId("grass new terrain"));
// }
int modeSplitScreen::update()
{
	if(input->getKey(F1))	{	players[0].toggleFirstPerson(); input->up(F1);}
	if(input->getKey(F2))	{	players[1].toggleFirstPerson(); input->up(F2);}
#ifdef _DEBUG
	if(input->getKey(0x31))
	{
		menuManager.setMenu("menuInGame");
		input->up(0x31);
	}
#else if
	if(input->getKey(VK_ESCAPE))
	{
		menuManager.setMenu("menuInGame");
		input->up(VK_ESCAPE);
	}
#endif

	static bool slow = false;
	if(input->getKey(0x54))
	{
		input->up(0x54);
		slow = !slow;
		world.time.ChangeSpeed(slow ? 0.1 : 1.0, 5.0);
	}

	//((nPlane*)world.objectList[players[0].objectNum()])->setControlState(players[0].getControlState());
	//((nPlane*)world.objectList[players[1].objectNum()])->setControlState(players[1].getControlState());

	world.update();
	radarAng+=45.0*world.time.getLength()/1000;
	if(radarAng>=360)
		radarAng-=360;

	return 7;
}
void modeSplitScreen::draw2D()
{
	for(int acplayer=0; acplayer <= 1; acplayer++)
	{
		nPlane* p=(nPlane*)world.objectList[players[acplayer].objectNum()];
		if(players[acplayer].firstPerson() && !p->controled)
		{
			//planeIdBoxes(p, 0.0, sh/2*acplayer, sw, sh/2);
			graphics->drawOverlay(Vec2f(0,sh/2*acplayer),Vec2f(sw,sh/2+sh/2*acplayer),"cockpit");
			targeter(400,150+300*acplayer,50,p->turn);
			radar(176, 200+300*acplayer, 64, 64, true);
			healthBar(340, 40+300*acplayer, -200, 200, p->health/p->maxHealth,true);

			//speedMeter(16,16,80,80,p.accel.magnitude()*30.5+212);
			//altitudeMeter(368,215,432,279,p.altitude);
		}
		else
		{
			radar(730, 230+300*acplayer, 64, 64, false);	
			healthBar(614, 25+300*acplayer, 150, 25, p->health/p->maxHealth,false);
		}
	}
}
void modeSplitScreen::draw3D()
{
	glClearColor(0.5f,0.8f,0.9f,1.0f);
	glViewport(0, sh/2, sw, sh/2);
	gluPerspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	frustum.setCamInternals(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	drawScene(0);

	glViewport(0, 0, sw, sh/2);
	drawScene(1);
}