
#include "game.h"
namespace gui{
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::objectProperties													|
// |____________________________________________________________________________________________________________________________|
//
bool objectProperties::init(LevelFile::Object* obj)
{
	if(obj == NULL)
		return false;

	object = obj;


	buttons["Ok"]		= new button(0.5*sAspect+0.244, 0.681, 0.098, 0.039, "Ok",lightGray);
	buttons["Cancel"]	= new button(0.5*sAspect+0.146, 0.681, 0.098, 0.039, "Cancel",lightGray);

	labels["x location"]	= new label(0.5*sAspect-0.205,0.5-0.015,"X location:");
	labels["z location"]	= new label(0.5*sAspect-0.205,0.5+0.024,"Y location:");
	labels["y location"]	= new label(0.5*sAspect-0.205,0.5+0.063,"Z location:");

	textBoxes["x location"]	= new numericTextBox(0.5*sAspect-0.098,0.5-0.015,0.098,floor(object->startloc.x+0.5),black);
	textBoxes["y location"]	= new numericTextBox(0.5*sAspect-0.098,0.5+0.024,0.098,floor(object->startloc.y+0.5),black);
	textBoxes["z location"]	= new numericTextBox(0.5*sAspect-0.098,0.5+0.063,0.098,floor(object->startloc.z+0.5),black);

	checkBoxes["control"]	= new checkBox(0.5*sAspect+0.098,0.5+0.049,"player controlled",(object->controlType & PLAYER_HUMAN)!=0,black);
	checkBoxes["respawn"]	= new checkBox(0.5*sAspect+0.098,0.5+0.078,"respawn when destroyed\n(comming soon)",false,black);

	listBox* l = new listBox(0.5*sAspect + 0.098,0.5-0.015,0.098,"team " + lexical_cast<string>(object->team+1),black);
	l->addOption("team 1");
	l->addOption("team 2");
	l->addOption("team 3");
	l->addOption("team 4");
	listBoxes["team"] = l;

	string ptype = objectTypeString(object->type);

	l = new listBox(0.5*sAspect+0.244,0.5-0.015,0.098,ptype,black);
	l->addOption("f16");			typeOptions.push_back(F16);
	l->addOption("f18");			typeOptions.push_back(F18);
	l->addOption("f22");			typeOptions.push_back(F22);
	l->addOption("UAV");			typeOptions.push_back(UAV);
	l->addOption("b2");				typeOptions.push_back(B2);
	l->addOption("mirage");			typeOptions.push_back(MIRAGE);
	l->addOption("PLAYER_PLANE");	typeOptions.push_back(PLAYER_PLANE);
	listBoxes["type"] = l;

	return true;
}
int objectProperties::update()
{
	if(buttons["Ok"]->checkChanged())
	{
		try{
			Vec3f v(lexical_cast<float>(textBoxes["x location"]->getText()),
					lexical_cast<float>(textBoxes["y location"]->getText()),
					lexical_cast<float>(textBoxes["z location"]->getText()));

			object->startloc = v;
			object->team = listBoxes["team"]->getOptionNumber();

			int ptype = listBoxes["type"]->getOptionNumber();
			object->type = typeOptions[listBoxes["type"]->getOptionNumber()];

			object->controlType = checkBoxes["control"]->getChecked() ? PLAYER_HUMAN : PLAYER_COMPUTER;
		}
		catch(...)
		{
			messageBox("object position invalid");
			return 0;
		}

		done = true;
	}
	else if(buttons["Cancel"]->checkChanged())
	{
		done = true;
	}
	return 0;
}
void objectProperties::render()
{
	graphics->drawPartialOverlay(Rect::CWH(sAspect/2,0.5,0.8,0.5),Rect::XYWH(0,0,0.8,0.5),"dialog back");
}
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
	if(vkey==VK_UP)		activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_DOWN)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<RESUME) activeChoice=QUIT;
	if(activeChoice>QUIT) activeChoice=RESUME;
	if(((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==RESUME) || vkey==VK_PAUSE || vkey==VK_ESCAPE)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);
		input.up(0x31);
		input.up(VK_ESCAPE);
		world.time.unpause();
		done = true;
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==OPTIONS)
	{
		//
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==QUIT)
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
bool chooseMode::keyDown(int vkey)
{
	if(vkey==VK_ESCAPE)	game->done = true;//end the program
	if(vkey==VK_LEFT)	activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_RIGHT)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<0) activeChoice=(choice)2;
	if(activeChoice>2) activeChoice=(choice)0;

	if((vkey==VK_SPACE || vkey==VK_RETURN) && input.getKey(VK_CONTROL) && (activeChoice==SINGLE_PLAYER || activeChoice==MULTIPLAYER))//if the control key is pressed
	{
		openFile* p = new openFile;
		p->callback = (functor<void,popup*>*)this;
		p->init(".lvl");
		menuManager.setPopup(p);
		choosingFile = true;
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==SINGLE_PLAYER)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		std::shared_ptr<LevelFile> l(new LevelFile);
		if(l->loadZIP("media/map file.lvl"))
		{
			menuManager.setMenu(new gui::campaign(l));
		}
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MULTIPLAYER)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);

		std::shared_ptr<LevelFile> l(new LevelFile);
		if(l->loadZIP("media/map file.lvl"))
		{
			menuManager.setMenu(new gui::splitScreen(l));
		}
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MAP_EDITOR)
	{
		input.up(VK_SPACE);
		input.up(VK_RETURN);
		menuManager.setMenu(new gui::levelEditor);
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


			std::shared_ptr<LevelFile> l(new LevelFile);
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

			std::shared_ptr<LevelFile> l(new LevelFile);
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
	auto resolutions = graphics->getSupportedResolutions();

	string appData = fileManager.getAppDataDirectory();
	settingsFile = fileManager.loadIniFile(appData + "settings.ini");
	
	initialGamma = graphics->getGamma();
	initialResolutionChoice = -1;

	listBoxes["resolution"] = new listBox(0.4*sAspect,0.3,0.150,lexical_cast<string>(sw) + "x" + lexical_cast<string>(sh),black);
	for(auto i=resolutions.begin(); i!=resolutions.end(); i++)
	{
		if(i->x >= 800 && i->y >= 600)
		{
			listBoxes["resolution"]->addOption(lexical_cast<string>(i->x) + "x" + lexical_cast<string>(i->y));
		
			resolutionChoices.push_back(*i);
			if(i->x == sw && i->y == sh)
			{
				initialResolutionChoice = resolutionChoices.size()-1;
			}
		}
	}

	labels["gamma"] = new label(0.3*sAspect, 0.7, "brightness:");
	sliders["gamma"] = new slider(0.4*sAspect, 0.7, 0.100, 0.03,3.0,1.0); 
	sliders["gamma"]->setValue(initialGamma);

	buttons["save"] = new button(0.9*sAspect-0.11, 0.71, 0.100, 0.030, "Save", lightGray);
	buttons["cancel"] = new button(0.9*sAspect-0.22, 0.71, 0.100, 0.030, "Cancel", lightGray);


	return true;
}
int options::update()
{
	graphics->setGamma(sliders["gamma"]->getValue());

	if(buttons["save"]->checkChanged())
	{
		bool needRestart = false;

		if(initialResolutionChoice != listBoxes["resolution"]->getOptionNumber())
		{
			settingsFile->bindings["graphics"]["resolutionX"] = lexical_cast<string>(resolutionChoices[listBoxes["resolution"]->getOptionNumber()].x);
			settingsFile->bindings["graphics"]["resolutionY"] = lexical_cast<string>(resolutionChoices[listBoxes["resolution"]->getOptionNumber()].y);
			needRestart = true;
		}

		settingsFile->bindings["graphics"]["gamma"] = lexical_cast<string>(sliders["gamma"]->getValue());
		fileManager.writeIniFile(settingsFile);

		menuManager.setMenu(new chooseMode);
		if(needRestart)
		{
			messageBox("Some changes will not take effect until you restart FighterPilot.");
		}
	}
	else if(buttons["cancel"]->checkChanged())
	{
		graphics->setGamma(initialGamma);
		menuManager.setMenu(new chooseMode);
	}

	

	return true;
}
void options::render()
{
	graphics->drawOverlay(Rect::XYXY(0.0,0.0,sAspect,1.0),"menu background");
	graphics->drawPartialOverlay(Rect::CWH(sAspect/2,0.5,0.8*sAspect,0.5),Rect::XYWH(0,0,0.8,0.5),"dialog back");

	graphics->setColor(0.00,0.00,0.00);	graphics->drawOverlay(Rect::XYXY(0.200*sAspect,0.47,0.225*sAspect,0.53),"white");
	graphics->setColor(0.05,0.05,0.05);	graphics->drawOverlay(Rect::XYXY(0.225*sAspect,0.47,0.250*sAspect,0.53),"white");
	graphics->setColor(0.10,0.10,0.10);	graphics->drawOverlay(Rect::XYXY(0.250*sAspect,0.47,0.275*sAspect,0.53),"white");
	graphics->setColor(0.15,0.15,0.15);	graphics->drawOverlay(Rect::XYXY(0.275*sAspect,0.47,0.300*sAspect,0.53),"white");
	graphics->setColor(0.20,0.20,0.20);	graphics->drawOverlay(Rect::XYXY(0.300*sAspect,0.47,0.325*sAspect,0.53),"white");
	graphics->setColor(0.25,0.25,0.25);	graphics->drawOverlay(Rect::XYXY(0.325*sAspect,0.47,0.350*sAspect,0.53),"white");
	graphics->setColor(0.30,0.30,0.30);	graphics->drawOverlay(Rect::XYXY(0.350*sAspect,0.47,0.375*sAspect,0.53),"white");
	graphics->setColor(0.35,0.35,0.35);	graphics->drawOverlay(Rect::XYXY(0.375*sAspect,0.47,0.400*sAspect,0.53),"white");
	graphics->setColor(0.40,0.40,0.40);	graphics->drawOverlay(Rect::XYXY(0.400*sAspect,0.47,0.425*sAspect,0.53),"white");
	graphics->setColor(0.45,0.45,0.45);	graphics->drawOverlay(Rect::XYXY(0.425*sAspect,0.47,0.450*sAspect,0.53),"white");
	graphics->setColor(0.50,0.50,0.50);	graphics->drawOverlay(Rect::XYXY(0.450*sAspect,0.47,0.475*sAspect,0.53),"white");
	graphics->setColor(0.55,0.55,0.55);	graphics->drawOverlay(Rect::XYXY(0.475*sAspect,0.47,0.500*sAspect,0.53),"white");
	graphics->setColor(0.60,0.60,0.60);	graphics->drawOverlay(Rect::XYXY(0.500*sAspect,0.47,0.525*sAspect,0.53),"white");
	graphics->setColor(0.65,0.65,0.65);	graphics->drawOverlay(Rect::XYXY(0.525*sAspect,0.47,0.550*sAspect,0.53),"white");
	graphics->setColor(0.70,0.70,0.70);	graphics->drawOverlay(Rect::XYXY(0.550*sAspect,0.47,0.575*sAspect,0.53),"white");
	graphics->setColor(0.75,0.75,0.75);	graphics->drawOverlay(Rect::XYXY(0.575*sAspect,0.47,0.600*sAspect,0.53),"white");
	graphics->setColor(0.80,0.80,0.80);	graphics->drawOverlay(Rect::XYXY(0.600*sAspect,0.47,0.625*sAspect,0.53),"white");
	graphics->setColor(0.85,0.85,0.85);	graphics->drawOverlay(Rect::XYXY(0.625*sAspect,0.47,0.650*sAspect,0.53),"white");
	graphics->setColor(0.90,0.90,0.90);	graphics->drawOverlay(Rect::XYXY(0.650*sAspect,0.47,0.675*sAspect,0.53),"white");
	graphics->setColor(0.95,0.95,0.95);	graphics->drawOverlay(Rect::XYXY(0.675*sAspect,0.47,0.700*sAspect,0.53),"white");
	graphics->setColor(1.00,1.00,1.00);	graphics->drawOverlay(Rect::XYXY(0.700*sAspect,0.47,0.725*sAspect,0.53),"white");

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
	static bool toLoad = true;
	if(toLoad)
	{
		assetLoader.loadAsset(); // does preload if this is the first time it is called

		settings.load("media/modelData.txt");

		//dataManager.registerAsset("menu background", "media/menu/menu background.png");
		//dataManager.registerAsset("progress back", "media/progress back.png");
		//dataManager.registerAsset("progress front", "media/progress front.png");
		//dataManager.registerShader("ortho", "media/ortho.vert", "media/ortho.frag");
		//dataManager.registerShader("gamma shader", "media/gamma.vert", "media/gamma.frag");
		toLoad=false;

		//messageBox("hello, this is a test."); //for testing fonts
	}
	
	static int totalAssets = -1;

	static vector<double> times;

	int assetsLeft = assetLoader.loadAsset();

	if(totalAssets == -1) totalAssets = assetsLeft+1;

	//graphics->minimizeWindow();
	//MessageBox(NULL,(wstring(L"loaded asset #") + lexical_cast<wstring>(totalAssets - assetsLeft)).c_str(),L"",0);
	progress = 1.0-(float)assetsLeft/totalAssets;
	if(assetsLeft==0)
	{
		dataManager.writeErrorLog("media/shaderErrors.txt");
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
