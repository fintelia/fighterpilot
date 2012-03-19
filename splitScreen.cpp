

#include "game.h"

namespace gui{
splitScreen::splitScreen(std::shared_ptr<LevelFile> lvl): dogFight(lvl)
{
	views[0] = graphics->genView();
	views[0]->viewport(0,0.5, sAspect,0.5);
	views[0]->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	views[0]->setRenderFunc(bind(&splitScreen::render3D, this, placeholders::_1), 0);

	views[1] = graphics->genView();
	views[1]->viewport(0,0.0, sAspect,0.5);
	views[1]->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	views[1]->setRenderFunc(bind(&splitScreen::render3D, this, placeholders::_1), 1);

	graphics->setLightPosition(Vec3f(-500.0, 800.0, 100.0));
}
bool splitScreen::init()
{
	world.create();
	level->initializeWorld(2);
	return true;
}
int splitScreen::update()
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

	checkCollisions();

	return 7;
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
			healthBar(0.175*sAspect, 0.6-0.5*acplayer, 0.25*sAspect, 0.333, p->health/p->maxHealth,true);
		}
		else if(!p->dead)
		{
			radar(sAspect-0.11, 0.389+0.5*acplayer, 0.094, 0.094, false, p);	
			healthBar(sAspect-0.024-0.146, 0.024+0.5*acplayer, 0.146, 0.024, p->health/p->maxHealth,false);
		}
	}
}
void splitScreen::render3D(unsigned int v)
{
//	glClearColor(0.5f,0.8f,0.9f,1.0f);
//	glViewport(0, sh/2, sw, sh/2);
//	graphics->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	drawScene(views[v], v);
	if(players[v]->firstPersonView && !((nPlane*)players[v]->getObject())->controled && !players[v]->getObject()->dead)
		sceneManager.renderScene(views[v], players[v]->getObject()->meshInstance);
	else
		sceneManager.renderScene(views[v]);
//	glViewport(0, 0, sw, sh/2);
//	drawScene(1);
}
}