
#include "main.h"
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


	buttons["Ok"]		= new button(sw/2+275, sh/2+185, 100, 40, "Ok",lightGray);
	buttons["Cancel"]	= new button(sw/2+150, sh/2+185, 100, 40, "Cancel",lightGray);

	labels["x location"]	= new label(sw/2-210,sh/2-15,"X location:");
	labels["z location"]	= new label(sw/2-210,sh/2+25,"Y location:");
	labels["y location"]	= new label(sw/2-210,sh/2+65,"Z location:");

	textBoxes["x location"]	= new numericTextBox(sw/2-100,sh/2-15,100,floor(object->startloc.x+0.5),black);
	textBoxes["y location"]	= new numericTextBox(sw/2-100,sh/2+25,100,floor(object->startloc.y+0.5),black);
	textBoxes["z location"]	= new numericTextBox(sw/2-100,sh/2+65,100,floor(object->startloc.z+0.5),black);

	checkBoxes["control"]	= new checkBox(sw/2+100,sh/2+50,"player controlled",object->controlType & CONTROL_HUMAN,black);
	checkBoxes["respawn"]	= new checkBox(sw/2+100,sh/2+80,"respawn when destroyed\n(comming soon)",false,black);

	listBox* l = new listBox(sw/2+100,sh/2-15,100,"team " + lexical_cast<string>(object->team+1),black);
	l->addOption("team 1");
	l->addOption("team 2");
	l->addOption("team 3");
	l->addOption("team 4");
	listBoxes["team"] = l;

	string ptype;
	if(object->type == F16)	ptype = "F16";
	if(object->type == F18)	ptype = "F18";
	if(object->type == F22)	ptype = "F22";
	if(object->type == UAV)	ptype = "UAV";
	if(object->type == B2)	ptype = "B2";

	l = new listBox(sw/2+250,sh/2-15,100,ptype,black);
	l->addOption("F16");
	l->addOption("F18");
	l->addOption("F22");
	l->addOption("UAV");
	l->addOption("B2");
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
			if(ptype == 0)	object->type = F16;
			if(ptype == 1)	object->type = F18;
			if(ptype == 2)	object->type = F22;
			if(ptype == 3)	object->type = UAV;
			if(ptype == 4)	object->type = B2;

			object->controlType = checkBoxes["control"]->getChecked() ? CONTROL_HUMAN : CONTROL_COMPUTER;
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
	graphics->drawOverlay(Rect::CWH(sAspect/2,0.5,0.8,0.5),"white");
}
// ______________________________________________________________________________________________________________________________
// | 																															|
// | 													gui::inGame										            			|
// |____________________________________________________________________________________________________________________________|
//
void inGame::render()
{
	graphics->drawOverlay(Rect::XYXY(0.440*sAspect,0.620,0.559*sAspect,0.381),"menu in game");

	if(activeChoice==RESUME)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.600,0.545*sAspect,0.550),"menu in game select");
	if(activeChoice==OPTIONS)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.528,0.545*sAspect,0.478),"menu in game select");
	if(activeChoice==QUIT)		graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.454,0.545*sAspect,0.404),"menu in game select");
}
bool inGame::keyDown(int vkey)
{
	if(vkey==VK_UP)		activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_DOWN)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<RESUME) activeChoice=QUIT;
	if(activeChoice>QUIT) activeChoice=RESUME;
	if(((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==RESUME) || vkey==0x31 || vkey==VK_ESCAPE)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		input->up(0x31);
		input->up(VK_ESCAPE);
		world.time.unpause();
		menuManager.setMenu(NULL);
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==OPTIONS)
	{
		//
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==QUIT)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);

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
	graphics->drawOverlay(Rect::XYXY(0.0,1.0,sAspect,0.0),"menu background");
	graphics->drawOverlay(Rect::XYXY((-0.409+0.525)*sAspect,0.493,0.903*sAspect,0.302),"menu pictures");
	for(int i=1;i<=5;i++)
	{
		if(i!=activeChoice+2)
		{
			graphics->drawOverlay(Rect::XYXY((-0.409+i*0.263)*sAspect,0.5,(-0.148+i*0.263)*sAspect,0.25),"menu slot");
		}
	}
	for(int i=1;i<=3;i++)
	{
		graphics->drawPartialOverlay(Rect::XYWH((i*0.263-0.144)*sAspect,0.503,0.256*sAspect,-0.0417),Rect::XYWH(0,0.33*(i-1),1,0.33),"menu mode choices");
	}
}
bool chooseMode::keyDown(int vkey)
{
	if(vkey==VK_ESCAPE)	done = true;//end the program
	if(vkey==VK_LEFT)	activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_RIGHT)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<0) activeChoice=(choice)2;
	if(activeChoice>2) activeChoice=(choice)0;
//#ifdef _DEBUG
	if((vkey==VK_SPACE || vkey==VK_RETURN) && input->getKey(VK_CONTROL) && (activeChoice==SINGLE_PLAYER || activeChoice==MULTIPLAYER))//if the control key is pressed
	{
		openFile* p = new openFile;
		p->callback = (functor<void,popup*>*)this;
		p->init(".lvl");
		menuManager.setPopup(p);
		choosingFile = true;
	}
	else
//#endif

	if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==SINGLE_PLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);

		std::shared_ptr<Level> l(new Level);
		if(l->init("media/map file.lvl"))
		{
			menuManager.setMenu(new gui::campaign(l));
	//		modeManager.setMode(new modeCampaign(l));
		}
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MULTIPLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);

		std::shared_ptr<Level> l(new Level);
		if(l->init("media/map file.lvl"))
		{
			menuManager.setMenu(new gui::splitScreen(l));
		//	modeManager.setMode(new modeSplitScreen(l));
		}
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MAP_EDITOR)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu(new gui::levelEditor);

		//modeManager.setMode(new modeMapBuilder);
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
			input->up(VK_SPACE);
			input->up(VK_RETURN);


			std::shared_ptr<Level> l(new Level);
			if(l->init(((openFile*)p)->getFile()))
			{
				menuManager.setMenu(new gui::campaign(l));
			}
			return;
		}
		else if(activeChoice==MULTIPLAYER)
		{
			input->up(VK_SPACE);
			input->up(VK_RETURN);

			std::shared_ptr<Level> l(new Level);
			if(l->init(((openFile*)p)->getFile()))
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
	graphics->drawOverlay(Rect::XYXY(0.0,1.0,sAspect,0.0),"menu background");
}
bool chooseMap::keyDown(int vkey)
{
	if(mapChoices.empty())
		return false;

	if(vkey==VK_ESCAPE)
	{
		input->up(VK_ESCAPE);
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
// | 													gui::loading										            		|
// |____________________________________________________________________________________________________________________________|
//
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
		dataManager.preloadAssets();

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

	int assetsLeft = dataManager.loadAsset();

	if(totalAssets == -1) totalAssets = assetsLeft+1;

	//graphics->minimizeWindow();
	//MessageBox(NULL,(wstring(L"loaded asset #") + lexical_cast<wstring>(totalAssets - assetsLeft)).c_str(),L"",0);
	progress = 1.0-(float)assetsLeft/totalAssets;
	if(assetsLeft==0)
	{
		menuManager.setMenu(new gui::chooseMode);
	}
	return 30;
}
void loading::render()
{
	//static int n = 0;	n++;
	//if(n <= 1) return;
	graphics->drawOverlay(Rect::XYXY(0.0,1.0,sAspect,0.0),"menu background");
	graphics->drawOverlay(Rect::XYXY(0.05*sAspect,0.04,0.95*sAspect,0.02),"progress back");

	if(dataManager.assetLoaded("progress front"))
	{
		graphics->drawOverlay(Rect::XYXY(0.05*sAspect,0.04,(0.05+0.9*progress)*sAspect,0.02),"progress front");
	}
	else
	{
		glColor3f(0,1,0);
		graphics->drawOverlay(Rect::XYXY(0.05*sAspect,0.04,(0.05+0.9*progress)*sAspect,0.02));
		glColor3f(1,1,1);
	}
}
}
