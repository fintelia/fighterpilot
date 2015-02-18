
#include "game.h"
namespace gui{
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::inGame										            			|
// |____________________________________________________________________________________________________________________________|
//
inGame::inGame(): activeChoice(RESUME)
{
	world->time.pause();
}
void inGame::render()
{
	graphics->drawOverlay(Rect::XYXY(0.440*sAspect,0.381,0.559*sAspect,0.620),"menu in game");

	if(activeChoice==RESUME)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.400,0.545*sAspect,0.450),"menu in game select");
	if(activeChoice==OPTIONS)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.472,0.545*sAspect,0.522),"menu in game select");
	if(activeChoice==QUIT)		graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.546,0.545*sAspect,0.596),"menu in game select");
}
bool inGame::keyDown(int vkey, char ascii)
{
//	if(vkey==VK_PAUSE || vkey==VK_ESCAPE)
//	{
//		input.up(VK_PAUSE);
//		input.up(VK_ESCAPE);
//		world->time.unpause();
//		done = true;
//		return true;
//	}
	return false;
}
bool inGame::menuKey(int mkey)
{
	if(mkey==MENU_UP)	activeChoice = choice(int(activeChoice)-1);
	if(mkey==MENU_DOWN)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<RESUME) activeChoice=QUIT;
	if(activeChoice>QUIT) activeChoice=RESUME;
	if((mkey == MENU_ENTER && activeChoice == RESUME) || mkey == MENU_BACK)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);
		input.up(VK_PAUSE);
	//	input.up(VK_ESCAPE);
		world->time.unpause();
		done = true;
	}
	else if(mkey == MENU_ENTER && activeChoice == OPTIONS)
	{
		menuManager.setPopup(new gui::options);
	}
	else if(mkey == MENU_ENTER && activeChoice == QUIT)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		world->time.unpause();

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
	graphics->drawPartialOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),Rect::XYXY(0.5*(1.7778-sAspect),0.0,1.0-0.5*(1.7778-sAspect),1.0),"menu background");
	graphics->drawOverlay(Rect::CWH(0.5*sAspect,0.35,1.111,0.222), "menu game title");
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

	graphics->drawOverlay(Rect::XYWH(0,0,384.0/sh, 32.0/sh), "fuction key tabs");
}
bool chooseMode::menuKey(int mkey)
{
	if(mkey==MENU_LEFT)	activeChoice = choice(int(activeChoice)-1);
	if(mkey==MENU_RIGHT)activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<0) activeChoice=(choice)2;
	if(activeChoice>2) activeChoice=(choice)0;

	if(mkey == MENU_ENTER && input.getKey(VK_CONTROL) && activeChoice==SINGLE_PLAYER)//if the control key is pressed
	{
		openFile* p = new openFile;
		p->callback = [](popup* p){
			input.up(VK_SPACE);
			input.up(VK_RETURN);

			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(((openFile*)p)->getFile()) && l->checkValid())
			{
				menuManager.setMenu(new gui::campaign(l));
			}
		};
		p->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(mkey == MENU_ENTER && input.getKey(VK_CONTROL) && activeChoice==MULTIPLAYER)//if the control key is pressed
	{
		openFile* p = new openFile;
		p->callback = [](popup* p){
			input.up(VK_SPACE);
			input.up(VK_RETURN);

			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(((openFile*)p)->getFile()) && l->checkValid())
			{
				menuManager.setMenu(new gui::splitScreen(l));
			}
		};
		p->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(mkey == MENU_ENTER && activeChoice == SINGLE_PLAYER)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		shared_ptr<LevelFile> l(new LevelFile);
		if(l->loadZIP("n44_54w95_105.lvl") && l->checkValid())
		{
			menuManager.setMenu(new gui::campaign(l));
		}
	}
	else if(mkey == MENU_ENTER && activeChoice == MULTIPLAYER)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		shared_ptr<LevelFile> l(new LevelFile);
		if(l->loadZIP("n44_54w95_105.lvl") && l->checkValid())
		{
			menuManager.setMenu(new gui::splitScreen(l));
		}
	}
	else if(mkey == MENU_ENTER && activeChoice==MAP_EDITOR)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);
		messageBox("Level editor not currently available.");
	//	menuManager.setMenu(new gui::levelEditor);
	}
	else
	{
		return false;
	}
	return true;
}
bool chooseMode::keyDown(int vkey, char ascii)
{
	if(vkey==VK_ESCAPE)
	{
		game->done = true;//end the program
	}
	else if(vkey==VK_F1)
	{
		menuManager.setPopup(new gui::help);
	}
	else if(vkey==VK_F2)
	{
		menuManager.setPopup(new gui::credits);
	}
	else if(vkey==VK_F3)
	{
		menuManager.setPopup(new gui::options);
	}
#ifdef _DEBUG
	else if(vkey==VK_F12)
	{
		openFile* p = new openFile;
		p->callback = [](popup* p){
			input.up(VK_SPACE);
			input.up(VK_RETURN);

			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(((openFile*)p)->getFile()) && l->checkValid())
			{
				menuManager.setMenu(new gui::lightbox(l));
			}
		};
		p->init(".lvl");
		menuManager.setPopup(p);
	}
#endif
	else
	{
		return false;
	}
	return true;
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
bool chooseMap::keyDown(int vkey, char ascii)
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

	initialState.resolutionChoice = -1;

	unsigned int nSamples = graphics->getMultisampling();
	if(nSamples==0 || nSamples==1)	initialState.samplesChoice=0;
	else if(nSamples == 2)			initialState.samplesChoice=1;
	else if(nSamples == 4)			initialState.samplesChoice=2;
	else if(nSamples == 8)			initialState.samplesChoice=3;
	else if(nSamples == 16)			initialState.samplesChoice=4;

	initialState.vSync = graphics->getVSync();
	initialState.textureCompression = (settings.get<string>("graphics", "textureCompression")=="enabled");
	/*if(graphics->hasShaderModel5())		initialState.rendererVersion = 4;
	else*/if(graphics->hasShaderModel4())	initialState.rendererVersion = 3;
	else									initialState.rendererVersion = 2;


	string appData = fileManager.getAppDataDirectory();
	settingsFile = fileManager.loadFile<FileManager::iniFile>(appData + "settings.ini");
	
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
	initialState.refreshRate = currentDisplayMode.refreshRate;


	labels["multisampling"] = new label(0.34*sAspect-graphics->getTextSize("Multisampling: ").x,0.355, "Multisampling: ");
	listBoxes["multisampling"] = new listBox(0.34*sAspect,0.355,0.150,initialState.samplesChoice==0 ? "Disabled" : lexical_cast<string>(1<<initialState.samplesChoice)+"X", black);
	listBoxes["multisampling"]->addOption("Disabled");
	listBoxes["multisampling"]->addOption("2X");
	listBoxes["multisampling"]->addOption("4X");
	//listBoxes["multisampling"]->addOption("8X");			//even though these modes may work, they can also cause FighterPilot
	//listBoxes["multisampling"]->addOption("16X");			//to use up all the graphics memory and crash.

	labels["vSync"] = new label(0.34*sAspect-graphics->getTextSize("Vertical Sync: ").x,0.395, "Vertical Sync: ");
	checkBoxes["vSync"] = new checkBox(0.34*sAspect, 0.395, "",initialState.vSync);


	labels["rendererVersion"] = new label(0.64*sAspect-graphics->getTextSize("Render Version: ").x,0.275, "Render Version: ");
	listBoxes["rendererVersion"] = new listBox(0.64*sAspect,0.275,0.150,initialState.rendererVersion == 2 ? "OpenGL 2" : "OpenGL 3", black);
	listBoxes["rendererVersion"]->addOption("OpenGL 2");
	listBoxes["rendererVersion"]->addOption("OpenGL 3");

	labels["refreshRate"] = new label(0.64*sAspect-graphics->getTextSize("Refresh Rate: ").x,0.315, "Refresh Rate: ");
	listBoxes["refreshRate"] = new listBox(0.64*sAspect,0.315,0.150,lexical_cast<string>(currentDisplayMode.refreshRate) + " Hz", black);
	for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
	{
		if(i->resolution.x == currentDisplayMode.resolution.x && i->resolution.y == currentDisplayMode.resolution.y)
		{
			listBoxes["refreshRate"]->addOption(lexical_cast<string>(i->refreshRate) + " Hz");
			refreshRates.push_back(i->refreshRate);
		}
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
void options::updateFrame()
{
	graphics->setGamma(sliders["gamma"]->getValue());

	if(lastResolutionChoice != listBoxes["resolution"]->getOptionNumber())
	{
		refreshRates.clear();
		listBoxes["refreshRate"]->clearOptions();
		for(auto i=displayModes.begin(); i!=displayModes.end(); i++)
		{
			if(i->resolution.x == currentDisplayMode.resolution.x && i->resolution.y == currentDisplayMode.resolution.y)
			{
				listBoxes["refreshRate"]->addOption(lexical_cast<string>(i->refreshRate) + " Hz");
				refreshRates.push_back(i->refreshRate);
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
		unsigned int rate = refreshRates[listBoxes["refreshRate"]->getOptionNumber()];
		if(initialState.refreshRate != rate)
		{
			settingsFile->bindings["graphics"]["refreshRate"] = lexical_cast<string>(rate);
			graphics->setRefreshRate(rate);
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
			settingsFile->bindings["graphics"]["samples"] = lexical_cast<string>(listBoxes["multisampling"]->getOptionNumber()==0 ? 0 : (1<<listBoxes["multisampling"]->getOptionNumber()));
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
		if(initialState.rendererVersion != listBoxes["rendererVersion"]->getOptionNumber()+2)
		{
			settingsFile->bindings["graphics"]["rendererVersion"] = lexical_cast<string>(listBoxes["rendererVersion"]->getOptionNumber()+2);
			needRestart = true;
		}
		/////////////////////////////////////////////////////
		fileManager.writeFile(settingsFile);

		//menuManager.setMenu(new chooseMode);
		done = true;
		if(needRestart)
		{
			messageBox("Some changes will not take effect until you restart FighterPilot.");
		}
	}
	else if(buttons["cancel"]->checkChanged())
	{
		graphics->setGamma(initialState.gamma);
		//menuManager.setMenu(new chooseMode);
		done = true;
	}
}
void options::render()
{
	//graphics->drawOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),"menu background");
	graphics->drawPartialOverlay(Rect::CWH(sAspect/2,0.5,0.7*sAspect,0.5),Rect::XYWH(0,0,0.8,0.5),"dialog back");

	float f;
	f = std::pow(0.00f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.2375*sAspect,0.47,0.2625*sAspect,0.53),"white");
	f = std::pow(0.05f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.2625*sAspect,0.47,0.2875*sAspect,0.53),"white");
	f = std::pow(0.10f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.2875*sAspect,0.47,0.3125*sAspect,0.53),"white");
	f = std::pow(0.15f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.3125*sAspect,0.47,0.3375*sAspect,0.53),"white");
	f = std::pow(0.20f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.3375*sAspect,0.47,0.3625*sAspect,0.53),"white");
	f = std::pow(0.25f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.3625*sAspect,0.47,0.3875*sAspect,0.53),"white");
	f = std::pow(0.30f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.3875*sAspect,0.47,0.4125*sAspect,0.53),"white");
	f = std::pow(0.35f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.4125*sAspect,0.47,0.4375*sAspect,0.53),"white");
	f = std::pow(0.40f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.4375*sAspect,0.47,0.4625*sAspect,0.53),"white");
	f = std::pow(0.45f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.4625*sAspect,0.47,0.4875*sAspect,0.53),"white");
	f = std::pow(0.50f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.4875*sAspect,0.47,0.5125*sAspect,0.53),"white");
	f = std::pow(0.55f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.5125*sAspect,0.47,0.5375*sAspect,0.53),"white");
	f = std::pow(0.60f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.5375*sAspect,0.47,0.5625*sAspect,0.53),"white");
	f = std::pow(0.65f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.5625*sAspect,0.47,0.5875*sAspect,0.53),"white");
	f = std::pow(0.70f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.5875*sAspect,0.47,0.6125*sAspect,0.53),"white");
	f = std::pow(0.75f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.6125*sAspect,0.47,0.6375*sAspect,0.53),"white");
	f = std::pow(0.80f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.6375*sAspect,0.47,0.6625*sAspect,0.53),"white");
	f = std::pow(0.85f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.6625*sAspect,0.47,0.6875*sAspect,0.53),"white");
	f = std::pow(0.90f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.6875*sAspect,0.47,0.7125*sAspect,0.53),"white");
	f = std::pow(0.95f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.7125*sAspect,0.47,0.7375*sAspect,0.53),"white");
	f = std::pow(1.00f, sliders["gamma"]->getValue()); graphics->setColor(f,f,f);	graphics->drawOverlay(Rect::XYXY(0.7375*sAspect,0.47,0.7625*sAspect,0.53),"white");

	menuManager.drawCursor();
}
bool options::keyDown(int vkey, char ascii)
{
	if(vkey==VK_ESCAPE)
	{
		graphics->setGamma(initialState.gamma);
		done = true;
		//menuManager.setMenu(new chooseMode);
		return true;
	}
	return false;
}
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::help											            		|
// |____________________________________________________________________________________________________________________________|
//
bool help::init()
{
	buttons["OK"] = new button(0.9*sAspect-0.11, 0.81, 0.100, 0.030, "OK", lightGray);
	auto f = fileManager.loadFile<FileManager::textFile>("media/help.txt");
	helpText = f->contents;
	return true;
}
void help::render()
{
	graphics->drawPartialOverlay(Rect::CWH(sAspect/2,0.5,0.8*sAspect,0.7),Rect::XYWH(0,0,0.9,0.7),"dialog back");

	graphics->drawText(helpText,Rect::CWH(sAspect/2,0.5,0.8*sAspect-0.1,0.6));

	menuManager.drawCursor();
}
void help::updateFrame()
{
	if(buttons["OK"]->checkChanged())
	{
		menuManager.setMenu(new chooseMode);
	}
}
bool help::keyDown(int vkey, char ascii)
{
	if(vkey==VK_ESCAPE)
	{
		menuManager.setMenu(new chooseMode);
		return true;
	}
	return false;
}
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::credits										            		|
// |____________________________________________________________________________________________________________________________|
//
bool credits::init()
{
	buttons["Continue"] = new button(0.9*sAspect-0.230, 0.760, 0.200, 0.060, "Continue", lightGray);
	return true;
}
void credits::render()
{
	graphics->drawOverlay(Rect::CWH(sAspect/2,0.5,0.8*sAspect,0.7),"credits");

	menuManager.drawCursor();
}
void credits::updateFrame()
{
	if(buttons["Continue"]->checkChanged())
	{
		menuManager.setMenu(new chooseMode);
	}
}
bool credits::keyDown(int vkey, char ascii)
{
	if(vkey==VK_ESCAPE)
	{
		menuManager.setMenu(new chooseMode);
		return true;
	}
	return false;
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
void loading::update()
{
	static bool loadedObjectData = false;
	if(!loadedObjectData)
	{
		//assetLoader.loadAssetList();
		//assetLoader.loadAsset(); // does preload if this is the first time it is called
		objectInfo.loadObjectData();
		loadedObjectData = true;
	}
	else if(!loadedObjectData)
	{
		return;
	}
	
	static int totalAssets = -1;
	int assetsLeft;

	double startTime = GetTime();
	do{
		assetsLeft = assetLoader.loadAsset();
	} while(GetTime() - startTime < 10.0);

	if(totalAssets == -1)
	{
		totalAssets = assetsLeft;
	}
	progress = max(progress, 1.0-static_cast<float>(assetsLeft)/totalAssets);
	if(assetsLeft == 0)
	{
		objectInfo.linkObjectMeshes();
		assetLoader.saveAssetZip();
		shaders.writeErrorLog("media/shaderErrors.txt");

/*		if(!game->commandLineOptions.empty() && fileManager.fileExists(game->commandLineOptions[0]))
		{
			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(game->commandLineOptions[0]) && l->checkValid())
			{
				menuManager.setMenu(new gui::campaign(l));
			}
			else
			{
				menuManager.setMenu(new gui::chooseMode);
			}
		}
		else */
		{
			menuManager.setMenu(new gui::chooseMode); //otherwise just chose the chooseMode menu
		}
	}
}
void loading::render()
{
	//static int n = 0;	n++;
	//if(n <= 1) return;

	graphics->drawPartialOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),Rect::XYXY(0.5*(1.7778-sAspect),0.0,1.0-0.5*(1.7778-sAspect),1.0),"menu background");
	graphics->drawOverlay(Rect::CWH(0.5*sAspect,0.35,1.111,0.222), "menu game title");
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
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::lightbox										            		|
// |____________________________________________________________________________________________________________________________|
//
lightbox::lightbox(shared_ptr<const LevelFile> lvl): dogFight(lvl), eye(0,1500,-4000)
{
	view = graphics->genView();
	view->viewport(0, 0, sAspect, 1.0);
	view->perspective(50.0, (double)sw / ((double)sh), 1.0, 2000000.0);
	view->setRenderFunc(bind(&lightbox::render3D, this, std::placeholders::_1));
	view->setTransparentRenderFunc(bind(&lightbox::renderTransparency, this, std::placeholders::_1));
	graphics->setLightPosition(Vec3f(0.0, 100000.0, 100000.0).normalize());
}
bool lightbox::init()
{
	world = unique_ptr<WorldManager>(new WorldManager(level->clipMap));
	level->initializeWorld(1);

	if(level->info.night)
	{
		view->blurStage(true);
		view->postProcessShader(shaders("gamma night"));
	}
	else
	{
		view->blurStage(false);
		view->postProcessShader(shaders("gamma bloom"));
	}

	world->time.pause();

	return true;
}
bool lightbox::menuKey(int mkey)
{
	if(mkey == MENU_BACK)
	{
		menuManager.setMenu(new gui::chooseMode);
	}
	else if(mkey == MENU_UP)
	{
		eye.z += 500.0;
	}
	else if(mkey == MENU_DOWN)
	{
		eye.z -= 500.0;
	}
	else if(mkey == MENU_LEFT)
	{
		eye.x += 500.0;
	}
	else if(mkey == MENU_RIGHT)
	{
		eye.x -= 500.0;
	}
	else if(mkey == MENU_ENTER)
	{
		graphics->takeScreenshot(8);
	}
	else
	{
		return false;
	}
	return true;
}
void lightbox::updateFrame()
{
	//check whether to toggle first person view
	if(input.getKey(VK_F1))
	{
		players[0]->toggleFirstPerson();
		input.up(VK_F1);
	}

	Vec3f fwd(-sin(yaw), sin(pitch),cos(yaw));

	//set camera position
	shared_ptr<plane> p=players[0]->getObject();
	view->lookAt(eye, eye + fwd, Vec3f(0,1,0));
	if(level->info.night)
	{
		if(players[0]->firstPersonView && !p->controled && !p->dead)
		{
			view->blurStage(false);
			view->postProcessShader(shaders("gamma night vision"));
		}
		else
		{
			view->blurStage(true);
			view->postProcessShader(shaders("gamma night"));
		}
	}
	players[0]->setVibrate(p->cameraShake);
}
void lightbox::render()
{
	shared_ptr<plane> p = players[0]->getObject();

	if(!graphics->isHighResScreenshot())
	{
		auto planes = world->getAllOfType(PLANE);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			if(i->second->id != p->id && i->second->team == p->team && !i->second->dead)
				drawHudIndicator(view, p, i->second, Color(0,1,0,0.5), Color4(0.66*0.2989,0.34+0.66*0.2989,0.66*0.2989,0.5));
		}
	}
}

void lightbox::render3D(unsigned int v)
{
	drawScene(view, 0);
	sceneManager.renderScene(view);
	world->renderFoliage(view);
	sceneManager.renderSceneTransparency(view);
}
void lightbox::renderTransparency(unsigned int v)
{
	particleManager.render(view);
}
}
