
#include "game.h"
#include <Windows.h>
//ObjectStats settings;
Game* game = new Game;
objId bullets;

bool Game::init()
{
#ifdef _DEBUG
	MessageBox(NULL,L"Fighter-Pilot is Currently Running in Debug Mode. Click OK to Proceed.",L"Fighter Pilot",0);
#endif

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

	auto s = fileManager.loadIniFile("media/settings.ini");
	settings.load(s->bindings);



	Vec2i r, rWanted;
	r.x = GetSystemMetrics(SM_CXSCREEN);
	r.y = GetSystemMetrics(SM_CYSCREEN);
	rWanted.x = settings.get<int>("graphics","resolutionX");
	rWanted.y = settings.get<int>("graphics","resolutionY");

	DEVMODE d;
	d.dmSize = sizeof(d);
	d.dmDriverExtra = 0;
	int i=0;
	while(EnumDisplaySettings(NULL, i++, &d) != 0 || (rWanted.x != 0 && rWanted.y != 0))
	{
		if(d.dmPelsWidth == rWanted.x && d.dmPelsHeight == rWanted.y)
		{
			r = rWanted;
			break;
		}
	}
	float gamma = settings.get<float>("graphics","gamma");
	if(gamma > 0.5 && gamma < 5.0)
		graphics->setGamma(gamma);

	unsigned int maxSamples = settings.get<unsigned int>("graphics","samples");

	// Create Our OpenGL Window
	if (!graphics->createWindow("Fighter Pilot",r,maxSamples))
	{
		return false;
	}
	//if(wglSwapIntervalEXT)
	//	wglSwapIntervalEXT(0);//turn on/off vsync (0 = off and 1 = on)
//	ShowHideTaskBar(false);
//////
	srand ((unsigned int)time(NULL));

	menuManager.setMenu(new gui::loading);

//	controlManager.addController(shared_ptr<controller>(new controller(0)));
//	controlManager.addController(shared_ptr<controller>(new controller(1)));

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

		input.update();
		players.update();

		world.simulationUpdate();
		particleManager.update();


	}	

	world.time.nextFrame();	
	world.frameUpdate();
	menuManager.update();
}