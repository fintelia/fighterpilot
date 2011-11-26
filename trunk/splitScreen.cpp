

#include "game.h"

namespace gui{
splitScreen::splitScreen(std::shared_ptr<LevelFile> lvl): dogFight(lvl)
{
	graphics->resetViews(2);
	graphics->viewport(0,0.5, sAspect,0.5,	0);
	graphics->viewport(0,0.0, sAspect,0.5,	1);
	graphics->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0,	0);
	graphics->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0,	1);
}
int splitScreen::update()
{
//	if(input.getKey(VK_F1))	{	players[0].toggleFirstPerson(); input.up(VK_F1);}
//	if(input.getKey(VK_F2))	{	players[1].toggleFirstPerson(); input.up(VK_F2);}

	if(input.getKey(VK_ESCAPE))
	{
		menuManager.setPopup(new gui::inGame);
		input.up(VK_ESCAPE);
	}

#ifdef _DEBUG
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
			graphics->drawOverlay(Rect::XYXY(0,sh/2*acplayer,sw,sh/2*(acplayer+1)),"cockpit");
			targeter(400,150+300*acplayer,50,p->roll);
			radar(-0.56, 0.13-acplayer, 0.16, 0.2133, true, p);
			healthBar(-0.65, 0.2-acplayer, 0.5, 0.666, p->health/p->maxHealth,true);
		}
		else if(!p->dead)
		{
			radar(730, 230+300*acplayer, 64, 64, false, p);	
			healthBar(614, 25+300*acplayer, 150, 25, p->health/p->maxHealth,false);
		}
	}
}
void splitScreen::render3D(unsigned int view)
{
//	glClearColor(0.5f,0.8f,0.9f,1.0f);
//	glViewport(0, sh/2, sw, sh/2);
//	graphics->perspective(80.0, (double)sw / ((double)sh/2),1.0, 160000.0);
	drawScene(view);
	if(players[view]->firstPersonView && !((nPlane*)players[view]->getObject())->controled && !players[view]->getObject()->dead)
		sceneManager.renderScene(players[view]->getObject()->meshInstance);
	else
		sceneManager.renderScene();
//	glViewport(0, 0, sw, sh/2);
//	drawScene(1);
}
}