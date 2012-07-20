
#include "game.h"
#ifdef WINDOWS
#include <Windows.h>
#endif
PlayerManager& players = PlayerManager::getInstance();
SettingsManager& settings = SettingsManager::getInstance();


Game* game = new Game;

objId bullets;
planeType defaultPlane;

bool Game::init()
{
#if defined WINDOWS && defined _DEBUG
	MessageBox(NULL,L"Fighter-Pilot is Currently Running in Debug Mode. Click OK to Proceed.",L"Fighter Pilot",0);
#endif

	if(!fileManager.directoryExists("media"))
	{
#ifdef WINDOWS
		MessageBox(NULL,L"Media folder not found. Fighter-Pilot will now close.", L"Error",MB_ICONERROR);
#endif
		return false;
	}
	else if(!assetLoader.loadAssetList())
	{
#ifdef WINDOWS
		MessageBox(NULL,L"Error reading media/assetList.xml. Fighter-Pilot will now close.", L"Error",MB_ICONERROR);
#endif
		return false;
	}
	string appData = fileManager.getAppDataDirectory();
	if(!fileManager.fileExists(appData + "settings.ini"))
	{
		shared_ptr<FileManager::iniFile> settingsFile(new FileManager::iniFile(appData + "settings.ini"));
		settingsFile->bindings["graphics"]["maxFrameRate"] = "60.0";
#ifdef WINDOWS
		settingsFile->bindings["graphics"]["resolutionX"] = lexical_cast<string>(GetSystemMetrics(SM_CXSCREEN));
		settingsFile->bindings["graphics"]["resolutionY"] = lexical_cast<string>(GetSystemMetrics(SM_CYSCREEN));
#else
		settingsFile->bindings["graphics"]["resolutionX"] = "0";
		settingsFile->bindings["graphics"]["resolutionY"] = "0";

#endif
		settingsFile->bindings["graphics"]["samples"] = "4";
		settingsFile->bindings["graphics"]["gamma"] = "1.0";
		if(!fileManager.writeIniFile(settingsFile))
			debugBreak();
	}

	shared_ptr<FileManager::textFile> shortcut(new FileManager::textFile("media/settings.ini.url"));
	shortcut->contents = string("[InternetShortcut]\r\nURL=") + appData + "settings.ini\r\nIconIndex=0\r\nIconFile=" + appData + "settings.ini\r\n";
	fileManager.writeTextFile(shortcut);


	auto s = fileManager.loadIniFile(appData + "settings.ini");
	settings.load(s->bindings);

	maxFrameRate = settings.get<float>("graphics", "maxFrameRate");

	

	Vec2u r, rWanted;
#ifdef WINDOWS
	r.x = GetSystemMetrics(SM_CXSCREEN);
	r.y = GetSystemMetrics(SM_CYSCREEN);
#else
	r.x = 0;
	r.y = 0;
#endif
	rWanted.x = settings.get<int>("graphics","resolutionX");
	rWanted.y = settings.get<int>("graphics","resolutionY");

	auto resolutions = graphics->getSupportedResolutions();
	for(auto i=resolutions.begin(); i!=resolutions.end(); i++)
	{
		if(*i == rWanted)
		{
			r = rWanted;
		}
	}
	if(r.x == 0 || r.y == 0) //if we failed to get the resolution we wanted and we don't know the screen resolution
	{
		for(auto i=resolutions.begin(); i!=resolutions.end(); i++)
		{
			if((*i).y > r.y || ((*i).y == r.y && (*i).x > r.x)) //pick the highest vertical resolution
			{
				r = *i;
			}
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
	
	graphics->setVSync(maxFrameRate <= 75.0);

//	ShowHideTaskBar(false);
//////

	menuManager.setMenu(new gui::loading);

//	controlManager.addController(shared_ptr<controller>(new controller(0)));
//	controlManager.addController(shared_ptr<controller>(new controller(1)));

	return true;
}

void Game::update()
{

	/*                 COLLISION CHECKING GOES HERE						*/

	int n=0;
	input.update();
	while(world.time.needsUpdate())
	{
		n++;

		world.time.nextUpdate();

		players.update();
		world.simulationUpdate();
		particleManager.update();

		if(n==10)
		{
			//we can no-longer keep up with the simulation
			break;
		}
	}	

	world.time.nextFrame();	
	world.frameUpdate();
	menuManager.update();
}
