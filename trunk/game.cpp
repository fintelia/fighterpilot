
#include "game.h"
#include <Windows.h>
ObjectStats settings;
Game* game = new Game;
objId bullets;

bool Game::init()
{
	if(!fileManager.directoryExists("media"))
	{
		MessageBox(NULL,L"Media folder not found. Fighter-Pilot will now close.", L"Error",MB_ICONERROR);
		return false;
	}
	else if(!dataManager.loadAssetList())
	{
		MessageBox(NULL,L"Error reading media/assetList.xml. Fighter-Pilot will now close.", L"Error",MB_ICONERROR);
		return false;
	}

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,L"Would you like to run with high graphics settings?", L"Start High Quality?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		lowQuality=true;
	}

	Vec2i r;
	if(!lowQuality)
	{
		r.x = GetSystemMetrics(SM_CXSCREEN);
		r.y = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		r.x = 800;
		r.y = 600;
	}


	// Create Our OpenGL Window
	if (!graphics->createWindow("Fighter Pilot",r))
	{
		return false;									// Quit If Window Was Not Created
	}
	//if(wglSwapIntervalEXT)
	//	wglSwapIntervalEXT(0);//turn on/off vsync (0 = off and 1 = on)
//	ShowHideTaskBar(false);
//////
	srand ((unsigned int)time(NULL));

	menuManager.setMenu(new gui::loading);
	return true;
}

void Game::update()
{

	/*                 COLLISION CHECKING GOES HERE						*/

	//while(world.time.needsUpdate())
	//{
	//	world.time.nextUpdate();
	//}


	while(world.time.needsUpdate())
	{
		world.time.nextUpdate();

		world.simulationUpdate();
		particleManager.update();

		input->update();
	}
	menuManager.update();
	world.frameUpdate();

	world.time.nextFrame();

//	input->update();		//takes 2-11 ms
//	menuManager.update();	//takes almost no time
}