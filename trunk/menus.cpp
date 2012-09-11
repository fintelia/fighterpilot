
#include "game.h"
namespace gui{
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::inGame										            			|
// |____________________________________________________________________________________________________________________________|
//
inGame::inGame(): activeChoice(RESUME)
{
	world.time.pause();
}
void inGame::render()
{
	graphics->drawOverlay(Rect::XYXY(0.440*sAspect,0.381,0.559*sAspect,0.620),"menu in game");

	if(activeChoice==RESUME)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.400,0.545*sAspect,0.450),"menu in game select");
	if(activeChoice==OPTIONS)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.472,0.545*sAspect,0.522),"menu in game select");
	if(activeChoice==QUIT)		graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.546,0.545*sAspect,0.596),"menu in game select");
}
bool inGame::keyDown(int vkey)
{
	if(vkey==VK_PAUSE || vkey==VK_ESCAPE)
	{
		input.up(VK_PAUSE);
		input.up(VK_ESCAPE);
		world.time.unpause();
		done = true;
		return true;
	}
	return false;
}
bool inGame::menuKey(int mkey)
{
	if(mkey==MENU_UP)	activeChoice = choice(int(activeChoice)-1);
	if(mkey==MENU_DOWN)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<RESUME) activeChoice=QUIT;
	if(activeChoice>QUIT) activeChoice=RESUME;
	if(mkey == MENU_ENTER && activeChoice == RESUME)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);
		input.up(VK_PAUSE);
		input.up(VK_ESCAPE);
		world.time.unpause();
		done = true;
	}
	else if(mkey == MENU_ENTER && activeChoice == OPTIONS)
	{
		//
	}
	else if(mkey == MENU_ENTER && activeChoice == QUIT)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		world.time.unpause();

		menuManager.setMenu(new gui::chooseMode);
	}
	else
	{
		return false;
	}
	return true;
}
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::chooseMode										            		|
// |____________________________________________________________________________________________________________________________|
//
void chooseMode::render()
{
/*
 	graphics->drawOverlay(-1.0,-1.0,1.0,1.0,"menu background");
	graphics->drawOverlay(-0.75,-0.4,0.75,0.0,"menu pictures");
	for(int i=1;i<=5;i++)
	{
		if(i!=activeChoice+2)
		{
			graphics->drawOverlay(-1.25+0.5*i,-0.4,0.25+0.5*i,0.0,"menu slot");
			//graphics->drawOverlay(Vec2f((-327+i*210)*sx,300*sy),Vec2f(209*sx,150*sy),"menu slot");
		}
	}
	for(int i=1;i<=3;i++)
	{
	//	graphics->drawPartialOverlay(Vec2f((i*210-115)*sx,298*sy),Vec2f(205*sx,25*sy),Vec2f(0,0.33*(i-1)),Vec2f(1,0.33),"menu mode choices");
	}
 */
	graphics->drawOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),"menu background");
	graphics->drawOverlay(Rect::XYXY((-0.409+0.525)*sAspect,0.507,0.903*sAspect,0.698),"menu pictures");
	for(int i=1;i<=5;i++)
	{
		if(i!=activeChoice+2)
		{
			graphics->drawOverlay(Rect::XYXY((-0.409+i*0.263)*sAspect,0.5,(-0.148+i*0.263)*sAspect,0.75),"menu slot");
		}
	}
	for(int i=1;i<=3;i++)
	{
		graphics->drawPartialOverlay(Rect::XYWH((i*0.263-0.144)*sAspect,1.0-0.503,0.256*sAspect,0.0417),Rect::XYWH(0,0.33*(i-1),1,0.33),"menu mode choices");
	}
}
bool chooseMode::menuKey(int mkey)
{
	if(mkey==MENU_LEFT)	activeChoice = choice(int(activeChoice)-1);
	if(mkey==MENU_RIGHT)activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<0) activeChoice=(choice)2;
	if(activeChoice>2) activeChoice=(choice)0;

	if(mkey == MENU_ENTER && input.getKey(VK_CONTROL) && (activeChoice==SINGLE_PLAYER || activeChoice==MULTIPLAYER))//if the control key is pressed
	{
		openFile* p = new openFile;
		p->callback = (functor<void,popup*>*)this;
		p->init(".lvl");
		menuManager.setPopup(p);
		choosingFile = true;
	}
	else if(mkey == MENU_ENTER && activeChoice == SINGLE_PLAYER)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		shared_ptr<LevelFile> l(new LevelFile);
		if(l->loadZIP("media/simpleCampaign/mission1.lvl"))
		{
			menuManager.setMenu(new gui::campaign(l));
		}
	}
	else if(mkey == MENU_ENTER && activeChoice == MULTIPLAYER)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		shared_ptr<LevelFile> l(new LevelFile);
		if(l->loadZIP("media/map file.lvl"))
		{
			menuManager.setMenu(new gui::splitScreen(l));
		}
	}
	else if(mkey == MENU_ENTER && activeChoice==MAP_EDITOR)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);
		menuManager.setMenu(new gui::levelEditor);
	}
	else
	{
		return false;
	}
	return true;
}
bool chooseMode::keyDown(int vkey)
{
	if(vkey==VK_ESCAPE)
	{
		game->done = true;//end the program
	}
	else if(vkey==VK_F3)
	{
		menuManager.setMenu(new gui::options);
	}
	else
	{
		return false;
	}
	return true;
}
void chooseMode::operator() (popup* p)
{
	if(choosingFile && ((openFile*)p)->validFile())
	{
		if(activeChoice==SINGLE_PLAYER)
		{
			input.up(VK_SPACE);
			input.up(VK_RETURN);


			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(((openFile*)p)->getFile()))
			{
				menuManager.setMenu(new gui::campaign(l));
			}
			return;
		}
		else if(activeChoice==MULTIPLAYER)
		{
			input.up(VK_SPACE);
			input.up(VK_RETURN);

			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(((openFile*)p)->getFile()))
			{
				menuManager.setMenu(new gui::splitScreen(l));
			}
			return;
		}
		choosingFile = false;
	}
}
bool chooseMap::init()
{
	//directory_iterator end_itr; // default construction yields past-the-end
	//for(directory_iterator itr("./media/"); itr != end_itr; ++itr)
	//{
	//	if (extension(itr->path()) == ".lvl")
	//	{
	//		mapChoices.push_back(itr->path().leaf().generic_string());
	//	}
	//}
	mapChoices.push_back("media/simpleCampaign");
	currentChoice = 0;
	return true;
}
void chooseMap::render()
{
	graphics->drawOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),"menu background");
}
bool chooseMap::keyDown(int vkey)
{
	if(mapChoices.empty())
		return false;

	if(vkey==VK_ESCAPE)
	{
		input.up(VK_ESCAPE);
		menuManager.setMenu(new chooseMode);
	}
	if(vkey==VK_UP)		currentChoice--;
	if(vkey==VK_DOWN)	currentChoice++;

	if(currentChoice<0)
		currentChoice=max((int)mapChoices.size()-1,0);
	if(currentChoice>=mapChoices.size())
		currentChoice=0;


	return (vkey==VK_UP || vkey==VK_DOWN || vkey==VK_ESCAPE);
}
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::options										            		|
// |____________________________________________________________________________________________________________________________|
//
bool options::init()
{
	initialState.fullscreenChoice = graphics->getFullscreen() ? 0 : 1;
	initialState.gamma = graphics->getGamma();

	initialState.refreshRateChoice = -1;
	initialState.resolutionChoice = -1;

	unsigned int nSamples = graphics->getMultisampling();
	if(nSamples==0 || nSamples==1)	initialState.samplesChoice=0;
	else if(nSamples == 2)			initialState.samplesChoice=1;
	else if(nSamples == 4)			initialState.samplesChoice=2;
	else if(nSamples == 8)			initialState.samplesChoice=3;
	else if(nSamples == 16)			initialState.samplesChoice=4;

	initialState.vSync = graphics->getVSync();
	initialState.textureCompression = graphics->getTextureCompression();



	string appData = fileManager.getAppDataDirectory();
	settingsFile = fileManager.loadIniFile(appData + "settings.ini");
	
	labels["display mode"] = new label(0.34*sAspect-graphics->getTextSize("Display Mode: ").x,0.275, "Display Mode: ");
	listBoxes["display mode"] = new listBox(0.34*sAspect,0.275,0.150,/*initialState.fullscreenChoice==0 ? "Fullscreen" : "Windowed"*/"Fullscreen", black);
	listBoxes["display mode"]->addOption("Fullscreen");
	//listBoxes["display mode"]->addOption("Windowed");

	labels["resolution"] = new label(0.34*sAspect-graphics->getTextSize("Resolution: ").x,0.315, "Resolution: ");
	listBoxes["resolution"] = new listBox(0.34*sAspect,0.315,0.150,lexical_cast<string>(sw) + "x" + lexical_cast<string>(sh),black);

	set<Vec2u> resolutions;
	currentDisplayMode = graphics->getCurrentDisplayMode();
	displayModes = graphics->getSupportedDisplayModes();
	for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
	{
		if(i->resolution.x >= 800 && i->resolution.y >= 600)
		{
			resolutions.insert(i->resolution);
		}
	}
	for(auto i=resolutions.begin(); i!=resolutions.end(); i++)
	{
		resolutionChoices.push_back(*i);
		listBoxes["resolution"]->addOption(lexical_cast<string>(i->x) + "x" + lexical_cast<string>(i->y));
		if(i->x == sw && i->y == sh)
		{		
			initialState.resolutionChoice = resolutionChoices.size()-1;
		}
	}


	labels["multisampling"] = new label(0.34*sAspect-graphics->getTextSize("Multisampling: ").x,0.355, "Multisampling: ");
	listBoxes["multisampling"] = new listBox(0.34*sAspect,0.355,0.150,initialState.samplesChoice==0 ? "Disabled" : lexical_cast<string>(1<<initialState.samplesChoice)+"X", black);
	listBoxes["multisampling"]->addOption("Disabled");
	listBoxes["multisampling"]->addOption("2X");
	listBoxes["multisampling"]->addOption("4X");
	listBoxes["multisampling"]->addOption("8X");
	listBoxes["multisampling"]->addOption("16X");

	labels["vSync"] = new label(0.34*sAspect-graphics->getTextSize("Vertical Sync: ").x,0.395, "Vertical Sync: ");
	checkBoxes["vSync"] = new checkBox(0.34*sAspect, 0.395, "",initialState.vSync);


	labels["renderVersion"] = new label(0.64*sAspect-graphics->getTextSize("Render Version: ").x,0.275, "Render Version: ");
	listBoxes["renderVersion"] = new listBox(0.64*sAspect,0.275,0.150,"OpenGL 2", black);
	listBoxes["renderVersion"]->addOption("OpenGL 2");
	//listBoxes["renderVersion"]->addOption("OpenGL 3");

	labels["refreshRate"] = new label(0.64*sAspect-graphics->getTextSize("Refresh Rate: ").x,0.315, "Refresh Rate: ");
	listBoxes["refreshRate"] = new listBox(0.64*sAspect,0.315,0.150,lexical_cast<string>(currentDisplayMode.refreshRate) + " Hz", black);
	for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
	{
		if(i->resolution.x == currentDisplayMode.resolution.x && i->resolution.y == currentDisplayMode.resolution.y)
			listBoxes["refreshRate"]->addOption(lexical_cast<string>(i->refreshRate) + " Hz");
	}

	labels["textureCompression"] = new label(0.64*sAspect-graphics->getTextSize("Texture Compression: ").x,0.355, "Texture Compression: ");
	listBoxes["textureCompression"] = new listBox(0.64*sAspect,0.355,0.150,initialState.textureCompression ? "Enabled" : "Disabled", black);
	listBoxes["textureCompression"]->addOption("Enabled");
	listBoxes["textureCompression"]->addOption("Disabled");

	labels["gamma"] = new label(0.3*sAspect, 0.7, "brightness:");
	sliders["gamma"] = new slider(0.4*sAspect, 0.7, 0.100, 0.03,3.0,1.0); 
	sliders["gamma"]->setValue(initialState.gamma);

	buttons["save"] = new button(0.85*sAspect-0.11, 0.71, 0.100, 0.030, "Save", lightGray);
	buttons["cancel"] = new button(0.85*sAspect-0.22, 0.71, 0.100, 0.030, "Cancel", lightGray);

	lastResolutionChoice = initialState.resolutionChoice;

	return true;
}
int options::update()
{
	graphics->setGamma(sliders["gamma"]->getValue());

	if(lastResolutionChoice != listBoxes["resolution"]->getOptionNumber())
	{
		listBoxes["refreshRate"]->clearOptions();
		for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
		{
			if(i->resolution.x == currentDisplayMode.resolution.x && i->resolution.y == currentDisplayMode.resolution.y)
			{
				listBoxes["refreshRate"]->addOption(lexical_cast<string>(i->refreshRate) + " Hz");
				if(i->refreshRate == currentDisplayMode.refreshRate)
					listBoxes["refreshRate"]->setOption(listBoxes["refreshRate"]->getNumOptions()-1);
			}
		}
		if(listBoxes["refreshRate"]->getOptionNumber() == -1)
			listBoxes["refreshRate"]->setOption(0);

		lastResolutionChoice = listBoxes["resolution"]->getOptionNumber();
	}

	if(buttons["save"]->checkChanged())
	{
		bool needRestart = false;

		//////////////////fullscreen//////////////////////////////
		if(initialState.fullscreenChoice != listBoxes["display mode"]->getOptionNumber())
		{
			settingsFile->bindings["graphics"]["fullscreen"] = (listBoxes["display mode"]->getOptionNumber()==0) ? "true" : "false";
			needRestart = true;
		}
		//////////////////refreshRate//////////////////////////////
		if(initialState.refreshRateChoice != listBoxes["display mode"]->getOptionNumber() && initialState.refreshRateChoice != -1)
		{
			settingsFile->bindings["graphics"]["refreshRate"] = listBoxes["display mode"]->getOptionNumber()==0 ? "60" : "75";
			graphics->setRefreshRate(listBoxes["display mode"]->getOptionNumber()==0 ? 60 : 75);
		}
		//////////////////resolution//////////////////////////////
		if(initialState.resolutionChoice != listBoxes["resolution"]->getOptionNumber() && initialState.resolutionChoice != -1)
		{
			settingsFile->bindings["graphics"]["resolutionX"] = lexical_cast<string>(resolutionChoices[listBoxes["resolution"]->getOptionNumber()].x);
			settingsFile->bindings["graphics"]["resolutionY"] = lexical_cast<string>(resolutionChoices[listBoxes["resolution"]->getOptionNumber()].y);
			needRestart = true;
		}
		//////////////////multisampling//////////////////////////////
		if(initialState.samplesChoice != listBoxes["multisampling"]->getOptionNumber())
		{
			settingsFile->bindings["graphics"]["samples"] = listBoxes["display mode"]->getOptionNumber()==0 ? 0 : (1<<listBoxes["display mode"]->getOptionNumber());
			needRestart = true;
		}
		//////////////////texture compression//////////////////////////////
		if(initialState.textureCompression != (listBoxes["textureCompression"]->getOptionNumber()==0))
		{
			settingsFile->bindings["graphics"]["textureCompression"] = listBoxes["textureCompression"]->getOptionNumber()==0 ? "enabled" : "disabled";
			needRestart = true;
		}
		//////////////////gamma//////////////////////////////
		settingsFile->bindings["graphics"]["gamma"] = lexical_cast<string>(sliders["gamma"]->getValue());
		//////////////////vSync//////////////////////////////
		if(initialState.vSync != checkBoxes["vSync"]->getChecked())
		{
			settingsFile->bindings["graphics"]["vSync"] = checkBoxes["vSync"]->getChecked() ? "enabled" : "disabled";
			graphics->setVSync(checkBoxes["vSync"]->getChecked());
		}
		/////////////////////////////////////////////////////
		fileManager.writeIniFile(settingsFile);

		menuManager.setMenu(new chooseMode);
		if(needRestart)
		{
			messageBox("Some changes will not take effect until you restart FighterPilot.");
		}
	}
	else if(buttons["cancel"]->checkChanged())
	{
		graphics->setGamma(initialState.gamma);
		menuManager.setMenu(new chooseMode);
	}
	return true;
}
void options::render()
{
	graphics->drawOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),"menu background");
	graphics->drawPartialOverlay(Rect::CWH(sAspect/2,0.5,0.7*sAspect,0.5),Rect::XYWH(0,0,0.8,0.5),"dialog back");

	graphics->setColor(0.00,0.00,0.00);	graphics->drawOverlay(Rect::XYXY(0.2375*sAspect,0.47,0.2625*sAspect,0.53),"white");
	graphics->setColor(0.05,0.05,0.05);	graphics->drawOverlay(Rect::XYXY(0.2625*sAspect,0.47,0.2875*sAspect,0.53),"white");
	graphics->setColor(0.10,0.10,0.10);	graphics->drawOverlay(Rect::XYXY(0.2875*sAspect,0.47,0.3125*sAspect,0.53),"white");
	graphics->setColor(0.15,0.15,0.15);	graphics->drawOverlay(Rect::XYXY(0.3125*sAspect,0.47,0.3375*sAspect,0.53),"white");
	graphics->setColor(0.20,0.20,0.20);	graphics->drawOverlay(Rect::XYXY(0.3375*sAspect,0.47,0.3625*sAspect,0.53),"white");
	graphics->setColor(0.25,0.25,0.25);	graphics->drawOverlay(Rect::XYXY(0.3625*sAspect,0.47,0.3875*sAspect,0.53),"white");
	graphics->setColor(0.30,0.30,0.30);	graphics->drawOverlay(Rect::XYXY(0.3875*sAspect,0.47,0.4125*sAspect,0.53),"white");
	graphics->setColor(0.35,0.35,0.35);	graphics->drawOverlay(Rect::XYXY(0.4125*sAspect,0.47,0.4375*sAspect,0.53),"white");
	graphics->setColor(0.40,0.40,0.40);	graphics->drawOverlay(Rect::XYXY(0.4375*sAspect,0.47,0.4625*sAspect,0.53),"white");
	graphics->setColor(0.45,0.45,0.45);	graphics->drawOverlay(Rect::XYXY(0.4625*sAspect,0.47,0.4875*sAspect,0.53),"white");
	graphics->setColor(0.50,0.50,0.50);	graphics->drawOverlay(Rect::XYXY(0.4875*sAspect,0.47,0.5125*sAspect,0.53),"white");
	graphics->setColor(0.55,0.55,0.55);	graphics->drawOverlay(Rect::XYXY(0.5125*sAspect,0.47,0.5375*sAspect,0.53),"white");
	graphics->setColor(0.60,0.60,0.60);	graphics->drawOverlay(Rect::XYXY(0.5375*sAspect,0.47,0.5625*sAspect,0.53),"white");
	graphics->setColor(0.65,0.65,0.65);	graphics->drawOverlay(Rect::XYXY(0.5625*sAspect,0.47,0.5875*sAspect,0.53),"white");
	graphics->setColor(0.70,0.70,0.70);	graphics->drawOverlay(Rect::XYXY(0.5875*sAspect,0.47,0.6125*sAspect,0.53),"white");
	graphics->setColor(0.75,0.75,0.75);	graphics->drawOverlay(Rect::XYXY(0.6125*sAspect,0.47,0.6375*sAspect,0.53),"white");
	graphics->setColor(0.80,0.80,0.80);	graphics->drawOverlay(Rect::XYXY(0.6375*sAspect,0.47,0.6625*sAspect,0.53),"white");
	graphics->setColor(0.85,0.85,0.85);	graphics->drawOverlay(Rect::XYXY(0.6625*sAspect,0.47,0.6875*sAspect,0.53),"white");
	graphics->setColor(0.90,0.90,0.90);	graphics->drawOverlay(Rect::XYXY(0.6875*sAspect,0.47,0.7125*sAspect,0.53),"white");
	graphics->setColor(0.95,0.95,0.95);	graphics->drawOverlay(Rect::XYXY(0.7125*sAspect,0.47,0.7375*sAspect,0.53),"white");
	graphics->setColor(1.00,1.00,1.00);	graphics->drawOverlay(Rect::XYXY(0.7375*sAspect,0.47,0.7625*sAspect,0.53),"white");

	menuManager.drawCursor();
}
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::loading										            		|
// |____________________________________________________________________________________________________________________________|
//
loading::loading():progress(0.0f)
{

}
bool loading::init()
{
	progress = 0.0f;
	return true;
}
int loading::update()
{
	static double loadStartTime = GetTime();

	static bool toLoad = true;
	if(toLoad)
	{
		assetLoader.loadAsset(); // does preload if this is the first time it is called

		//settings.load("media/modelData.txt");

		//dataManager.registerAsset("menu background", "media/menu/menu background.png");
		//dataManager.registerAsset("progress back", "media/progress back.png");
		//dataManager.registerAsset("progress front", "media/progress front.png");
		//dataManager.registerShader("ortho", "media/ortho.vert", "media/ortho.frag");
		//dataManager.registerShader("gamma shader", "media/gamma.vert", "media/gamma.frag");
		toLoad=false;

		//messageBox("hello, this is a test."); //for testing fonts
	}
	
	static int totalAssets = -1;
	static int totalObjectMeshes = -1;
	static vector<double> times;

	if(totalObjectMeshes == -1) totalObjectMeshes = objectInfo.loadObjectMeshes();

	int assetsLeft = assetLoader.loadAsset();
	int objectMeshesLeft = totalObjectMeshes;

	if(totalAssets == -1) totalAssets = assetsLeft+1;

	if(assetsLeft == 0)
		objectMeshesLeft = objectInfo.loadObjectMeshes();
	

	//graphics->minimizeWindow();
	//MessageBox(NULL,(wstring(L"loaded asset #") + lexical_cast<wstring>(totalAssets - assetsLeft)).c_str(),L"",0);
	progress = 1.0-static_cast<float>(assetsLeft+objectMeshesLeft)/(totalAssets+totalObjectMeshes);
	if(assetsLeft == 0 && objectMeshesLeft == 0)
	{
		shaders.writeErrorLog("media/shaderErrors.txt");
		menuManager.setMenu(new gui::chooseMode); //otherwise just chose the chooseMode menu
	}
	
	return 30;
}
void loading::render()
{
	//static int n = 0;	n++;
	//if(n <= 1) return;

	graphics->drawOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),"menu background");
	graphics->drawOverlay(Rect::XYXY(0.05*sAspect,0.96,0.95*sAspect,0.98),"progress back");

	if(dataManager.assetLoaded("progress front"))
	{
		graphics->drawOverlay(Rect::XYXY(0.05*sAspect,0.96,(0.05+0.9*progress)*sAspect,0.98),"progress front");
	}
	else
	{
		graphics->drawOverlay(Rect::XYXY(0.05*sAspect,0.96,(0.05+0.9*progress)*sAspect,0.98));
	}
}
}
