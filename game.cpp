
#include "engine.h"
#include "game.h"
#ifdef WINDOWS
#include <Windows.h>
#endif
PlayerManager& players = PlayerManager::getInstance();
SettingsManager& settings = SettingsManager::getInstance();



unique_ptr<Game> game(new Game);

objId bullets;

bool Game::init()
{
//////////////////////////////////////////////Wait to Enter Full Screen///////////////////////////////////////////////////////////////////////////////
#if defined(WINDOWS) && defined(_DEBUG)
//	MessageBox(NULL,L"FighterPilot is Currently Running in Debug Mode. Click OK to Proceed.",L"FighterPilot",0);
#endif
/////////////////////////////////////////////Check for Required Files/////////////////////////////////////////////////////////////////////////////////
	if(!fileManager.directoryExists("media"))
	{
#ifdef WINDOWS
		MessageBox(NULL,L"Media folder not found. FighterPilot will now close.", L"Error",MB_ICONERROR);
#endif
		return false;
	}
//////////////////////////////////////////////////////load or define settings///////////////////////////////////////////////////////////////////////////////////
	string appData = fileManager.getAppDataDirectory();
	shared_ptr<FileManager::iniFile> settingsFile;

	if(!fileManager.fileExists(appData + "settings.ini"))
		settingsFile = std::make_shared<FileManager::iniFile>(appData + "settings.ini");
	else
		settingsFile = fileManager.loadFile<FileManager::iniFile>(appData + "settings.ini");

#ifdef WINDOWS
	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = 0;

	settingsFile->addValueIfMissing("graphics", "resolutionX", lexical_cast<string>(GetSystemMetrics(SM_CXSCREEN)));
	settingsFile->addValueIfMissing("graphics", "resolutionY", lexical_cast<string>(GetSystemMetrics(SM_CYSCREEN)));

	if(EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&devMode))
		settingsFile->addValueIfMissing("graphics", "refreshRate", lexical_cast<string>(devMode.dmDisplayFrequency));
	else
		settingsFile->addValueIfMissing("graphics", "refreshRate", "60");
#else
	settingsFile->addValueIfMissing("graphics", "resolutionX", "0");
	settingsFile->addValueIfMissing("graphics", "resolutionY", "0");
	settingsFile->addValueIfMissing("graphics", "refreshRate", "60");
#endif
	settingsFile->addValueIfMissing("graphics", "samples", "4");
	settingsFile->addValueIfMissing("graphics", "gamma", "1.0");
	settingsFile->addValueIfMissing("graphics", "vSync", "enabled");
	settingsFile->addValueIfMissing("graphics", "rendererVersion", "3");
	settingsFile->addValueIfMissing("graphics", "fullscreen", "true");
	settingsFile->addValueIfMissing("graphics", "textureCompression", "enabled");

	if(!fileManager.writeFile(settingsFile))
		debugBreak();

	shared_ptr<FileManager::textFile> shortcut(new FileManager::textFile("media/settings.ini.url"));
	shortcut->contents = string("[InternetShortcut]\r\nURL=") + appData + "settings.ini\r\nIconIndex=0\r\nIconFile=" + appData + "settings.ini\r\n";
	fileManager.writeFile(shortcut,false);

//////////////////////////////////////////////////////apply settings///////////////////////////////////////////////////////////////////////////////////
	settings.load(settingsFile->bindings);
	/////////////////////Screen Resolution/////////////////////////
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

	auto displayModes = graphics->getSupportedDisplayModes();
	for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
	{
		if(i->resolution == rWanted)
		{
			r = rWanted;
		}
	}
	if(r.x == 0 || r.y == 0) //if we failed to get the resolution we wanted and we don't know the screen resolution
	{
		for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
		{
			if(i->resolution.y > r.y || (i->resolution.y == r.y && i->resolution.x > r.x)) //pick the highest vertical resolution
			{
				r = i->resolution;
			}
		}
	}
	////////////////////
	sw = r.x;
	sh = r.y;
	sAspect = ((float)sw)/sh;
	/////////////////////Gamma/////////////////////////////////////
	float gamma = settings.get<float>("graphics","gamma");
	if(gamma > 0.5 && gamma < 5.0)
		graphics->setGamma(gamma);

	/////////////////////Multisampling/////////////////////////////
	unsigned int maxSamples = settings.get<unsigned int>("graphics","samples");

	/////////////////////Texture Compression/////////////////////////////
	assetLoader.setTextureCompression(settings.get<string>("graphics", "textureCompression")=="enabled");

////////////////////////////////////////////load asset list and object data///////////////////////////////////////////////////////////////////////////
	if(!assetLoader.loadAssetList())
	{
#ifdef WINDOWS
		MessageBox(NULL,L"Error reading media/assetList.xml. FighterPilot will now close.", L"Error",MB_ICONERROR);
#endif
		return false;
	}
//	else if(!objectInfo.loadObjectData())
//	{
//#ifdef WINDOWS
//		MessageBox(NULL,L"Error reading media/objectData.xml. FighterPilot will now close.", L"Error",MB_ICONERROR);
//#endif
//		return false;
//	}
//////////////////////////////////////////////////////create window///////////////////////////////////////////////////////////////////////////////////
	if(!graphics->createWindow("FighterPilot",r,maxSamples, (settings.get<string>("graphics","fullscreen")=="true"), settings.get<unsigned int>("graphics","rendererVersion")))
	{
		cout << "create windows failed";
		return false;
	}

	graphics->setRefreshRate(settings.get<unsigned int>("graphics", "refreshRate"));
	graphics->setVSync(settings.get<string>("graphics", "vSync")=="enabled");
	graphics->setSampleShadingFraction(1);
	graphics->setSampleShading(false);

	input.initialize();

	menuManager.setMenu(new gui::loading);
    menuManager.update();
	return true;
}

void Game::update()
{
	input.update();
	menuManager.update();
}
