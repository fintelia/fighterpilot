
#include "main.h"


//typedef map<string,MenuCreateFunc>::iterator Iterator;
//MenuManager* pInstance=NULL;
template<typename T>
menuScreen* CreateObject()
{
	return (menuScreen*)(new T);
}
void MenuManager::registerMenu(string menuName,menuCreateFunc menuFunc)
{
	menuList[menuName] = menuFunc;
}
menuScreen* MenuManager::createMenu(string menuName)
{
	map<string,menuCreateFunc>::iterator iter = menuList.find(menuName);
	if(iter==menuList.end())
		return NULL;
	return ((*iter).second)();
}
void MenuManager::destroyMenu(menuScreen* menu)
{
	if(menu != NULL)
	{
		delete menu;
		menu = NULL;
	}
}
bool MenuManager::init()
{
	menuScreen *menu = NULL;

	// register all the menus, each new menu
	// must call register before it can be used
	registerMenu("menuLevelEditor", &CreateObject<menuLevelEditor>);
	registerMenu("menuInGame", &CreateObject<menuInGame>);
	registerMenu("menuChooseMode", &CreateObject<menuChooseMode>);

	return true;
}
void MenuManager::render()
{
	mDrawCursor = false;
	if(menu != NULL)
		menu->render();

	for(vector<menuPopup*>::iterator i = popups.begin(); i!=popups.end();i++)
		(*i)->render();
	if(mDrawCursor)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		dataManager.bind("cursor");
		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);	glVertex2f((float)cursorPos.x,(float)cursorPos.y);
			glTexCoord2f(0,1);	glVertex2f((float)cursorPos.x,(float)cursorPos.y+25);
			glTexCoord2f(1,1);	glVertex2f((float)cursorPos.x+21,(float)cursorPos.y+25);
			glTexCoord2f(1,0);	glVertex2f((float)cursorPos.x+21,(float)cursorPos.y);
		glEnd();
	}
}
int MenuManager::update()
{
	if(!popups.empty())
	{
		popups.back()->update();
		if(popups.back()->isDone())
		{
			if(popups.back()->callback != NULL)
				(*popups.back()->callback)(popups.back());
			delete popups.back();
			popups.erase(popups.end()-1);
		}
	}
	else if(menu != NULL)
		menu->update();
	return 0;
}
void MenuManager::shutdown()
{
	for(vector<menuPopup*>::iterator i = popups.begin(); i!=popups.end();i++)
		delete (*i);
	if(menu != NULL)
		delete menu;
}
bool MenuManager::setMenu(string menuName)
{
	if(menuName.compare("")==0)
	{
		menu = NULL;
		return true;
	}
	// set the first menu to the mainMenu
	menu = createMenu(menuName);
	if (menu == NULL)
		return false;

	// initialize the menu
	if (!menu->init())
	{
		// shutdown the menu system
		shutdown();
		return false;
	}
	return true;
}
//void MenuManager::mouseUpdate(bool left,bool right, int x, int y)
//{
//	static bool lLeft=left;
//	static bool lRight=right;
//	static int lX=x;
//	static int lY=y;
//
//	if(menu == NULL) return;
//
//	if(left && !lLeft) menu->mouseL(true,x,y);
//	if(!left && lLeft) menu->mouseL(false,x,y);
//	if(right && !lRight) menu->mouseR(true,x,y);
//	if(!right && lRight) menu->mouseR(false,x,y);
//
//	lLeft=left;
//	lRight=right;
//	lX=x;
//	lY=y;
//}
//void MenuManager::keyUpdate(bool down, int vkey)
//{
//	if(down)
//	{
//		if(menu!=NULL)
//			menu->keyDown(vkey);
//	}
//	else
//	{
//		if(menu!=NULL)
//			menu->keyUp(vkey);
//	}
//}
//void MenuManager::scrollUpdate(float rotations)
//{
//	if(menu!=NULL)
//		menu->scroll(rotations);
//}
void MenuManager::inputCallback(Input::callBack* callback)
{
	if(callback->type == KEY_STROKE){
		Input::keyStroke* call = (Input::keyStroke*)callback;
		if(!popups.empty())
		{
			if(!call->up)
				popups.back()->keyDown(call->vkey);
			else
				popups.back()->keyUp(call->vkey);
		}
		else if(menu!=NULL)
		{
			if(!call->up)
				menu->keyDown(call->vkey);
			else
				menu->keyUp(call->vkey);
		}
	}
	else if(callback->type == MOUSE_CLICK){
		Input::mouseClick* call = (Input::mouseClick*)callback;
		if(!popups.empty())
		{
			if(call->button == LEFT_BUTTON)
				popups.back()->mouseL(call->down,call->x,call->y);
			else if(call->button == RIGHT_BUTTON)
				popups.back()->mouseR(call->down,call->x,call->y);
		}
		else if(menu!=NULL)
		{
			if(call->button == LEFT_BUTTON)
				menu->mouseL(call->down,call->x,call->y);
			else if(call->button == MIDDLE_BUTTON)
				menu->mouseC(call->down,call->x,call->y);
			else if(call->button == RIGHT_BUTTON)
				menu->mouseR(call->down,call->x,call->y);
		}

	}
	else if(callback->type == MOUSE_SCROLL){
		Input::mouseScroll* call = (Input::mouseScroll*)callback;
		if(!popups.empty())
			popups.back()->scroll(call->rotations);
		else if(menu!=NULL)
			menu->scroll(call->rotations);
	}
}
bool menuOpenFile::init()
{
	return true;
}
bool menuOpenFile::init(string ExtFilter)
{
	desktop		= new menuButton; desktop->init(sw/2-405,sh/2-246,175,40,"Desktop",lightGreen,white);
	myDocuments = new menuButton; myDocuments->init(sw/2-405,sh/2-196,175,40,"My Documents",lightGreen,white);
	myComputer	= new menuButton; myComputer->init(sw/2-405,sh/2-146,175,40,"My Computer",lightGreen,white);
	myNetwork	= new menuButton; myNetwork->init(sw/2-405,sh/2-96,175,40,"My Network",lightGreen,white);

	extFilter=ExtFilter;
	directory=".";
	refreshView();
	return true;
}
void menuOpenFile::refreshView()
{
	for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		if(*i!=NULL) delete (*i);
	for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			if(*i!=NULL) delete (*i);
	files.clear();
	folders.clear();
	folderButtons.clear();
	fileButtons.clear();

	folders.push_back("..");
	filesystem::directory_iterator end_itr; // default construction yields past-the-end
	for ( filesystem::directory_iterator itr( directory );	itr != end_itr;	 ++itr )
	{
		if ( is_directory(itr->status()) && itr->filename().compare(".svn") != 0)
		{
			folders.push_back(itr->filename());
		}
		else if ( filesystem::extension(itr->path()).compare(extFilter) == 0 ) // see below
		{
			files.push_back(itr->filename());
		}
	}
	int row=0, column=0;
	for(vector<string>::iterator i=folders.begin();i!=folders.end();i++)	
	{
		folderButtons.push_back(new menuButton());
		folderButtons.back()->init(12+140*column,5+40*row,135,35,(*i),Color(0.4,0.4,0.4));
		if(++column==4){column=0;row++;}
	}
	for(vector<string>::iterator i=files.begin();i!=files.end();i++)
	{
		fileButtons.push_back(new menuButton());
		fileButtons.back()->init(12+140*column,5+40*row,135,35,(*i),Color(0.6,0.6,0.6));
		if(++column==4){column=0;row++;}
	}
}
int menuOpenFile::update()
{
	if(desktop->getChanged())
	{
		desktop->resetChanged();
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	else if(myDocuments->getChanged())
	{
		myDocuments->resetChanged();
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	else if(myComputer->getChanged())
	{
		myComputer->resetChanged();
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_DRIVES, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	else if(myNetwork->getChanged())
	{
		myNetwork->resetChanged();
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_NETHOOD, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)
	{
		if((*i)->getChanged())
		{
			directory/=(*i)->getText();
			refreshView();
			return 0;
		}
	}
	for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)
	{
		if((*i)->getChanged())
		{
			file=(*i)->getText();
			(*i)->resetChanged();
			fileSelected();
			return 0;
		}
	}
	return 0;
}
void menuOpenFile::render()
{
	dataManager.bind("file viewer");
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2f(sw/2-420,sh/2+262);
		glTexCoord2f(1,1);	glVertex2f(sw/2+421,sh/2+262);
		glTexCoord2f(1,0);	glVertex2f(sw/2+421,sh/2-261);
		glTexCoord2f(0,0);	glVertex2f(sw/2-420,sh/2-261);
	glEnd();

	dataManager.bind("entry bar");
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2f(sw/2-180,sh/2+235);
		glTexCoord2f(1,1);	glVertex2f(sw/2+190,sh/2+235);
		glTexCoord2f(1,0);	glVertex2f(sw/2+190,sh/2+189);
		glTexCoord2f(0,0);	glVertex2f(sw/2-180,sh/2+189);
	glEnd();

	glColor4f(0,0,0,1);
	textManager->renderText(file,sw/2-170,sh/2+212-textManager->getTextHeight(file)/2);
	glPushMatrix();
	glTranslatef((sw/2-190),(sh/2-246),0);
	for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->render();
	for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->render();
	glPopMatrix();
	desktop->render();
	myDocuments->render();
	myComputer->render();
	myNetwork->render();
	menuManager.drawCursor();
}
void menuOpenFile::keyDown(int vkey)
{
	if(vkey == VK_RETURN)
		fileSelected();
	else if(vkey == VK_BACK)
	{
		if(file.size() != 0)
			file=file.substr(0,file.size()-1);
	}
	else
	{
		if(!input->getKey(VK_SHIFT))
			file  += tolower(MapVirtualKey(vkey,MAPVK_VK_TO_CHAR));
		else
			file  += MapVirtualKey(vkey,MAPVK_VK_TO_CHAR);
	}
}
void menuOpenFile::mouseL(bool down, int x, int y)
{
	static bool clicking = false;
	if(down)
	{
		for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246));
		for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246));
		desktop->mouseDownL(x,y);
		myDocuments->mouseDownL(x,y);
		myComputer->mouseDownL(x,y);
		myNetwork->mouseDownL(x,y);///y=451
		clicking = (x>sw/2+144) && (x<sw/2+184) && (y>sh/2+190) && (y<sh/2+230);
	}
	else
	{
		if(clicking) 
			fileSelected();
		for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246));
		for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246));
		desktop->mouseUpL(x,y);
		myDocuments->mouseUpL(x,y);
		myComputer->mouseUpL(x,y);
		myNetwork->mouseUpL(x,y);
		clicking = false;
	}
}
void menuSaveFile::operator() (menuPopup* p)
{
	if(replaceDialog)
	{
		if(((menuMessageBox*)p)->getValue() == 0)
		{
			done = true;
		}
		else 
		{
			replaceDialog = false;
		}
	}
}
void menuSaveFile::fileSelected()
{
	if(file.find(".") == file.npos)
		file += extFilter;
	if(exists(directory/file))//if file exists
	{
		menuMessageBox* m = new menuMessageBox;
		vector<string> s;
		s.push_back("yes");
		s.push_back("no");
		m->init(file + " already exists. Do you want to replace it?",s);
		m->callback = (functor<void,menuPopup*>*)this;
		menuManager.setPopup(m);
		replaceDialog = true;
	}
	else
	{
		done = true;
	}
}

bool menuMessageBox::init(string t)
{
	return init(t, vector<string>(1,"OK"));
}
bool menuMessageBox::init(string t, vector<string> names)
{
	if(names.empty()) return init(t);//watch out for infinite loop!!

	width = max(textManager->getTextWidth(t)+40,715);
	height = max(textManager->getTextHeight(t)-170,0) + 295;
	x = (sw-width)/2;
	y = (sh-height)/2;
	if(x < 5) x = 5;
	if(y < 5) y = 5;

	label = new menuLabel;
	label->init(x+(width-textManager->getTextWidth(t))/2, y+height*95/295-textManager->getTextHeight(t)/2, t, Color(1,1,1,0.8));

	float slotWidth = (685.0*715/width) / names.size();
	int slotNum = 0;
	for(vector<string>::iterator i = names.begin();i !=names.end(); i++, slotNum++)
	{
		menuLabel* l = new menuLabel;
		l->init(x + (15.0/715*width)+(0.5+slotNum)*slotWidth-textManager->getTextWidth(*i)/2,y + (235.0/295*height)-textManager->getTextHeight(*i)/2, *i, white);
		options.push_back(l);
	}
	value=-1;
	clicking=-1;
	return true;
}
void menuMessageBox::render()
{
	glColor4f(1,1,1,1);
	dataManager.bind("dialog box");
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(x,y);
		glTexCoord2f(0,1);	glVertex2f(x,y+height);
		glTexCoord2f(1,1);	glVertex2f(x+width,y+height);
		glTexCoord2f(1,0);	glVertex2f(x+width,y);
	glEnd();
	dataManager.bindTex(0);

	int startX = (15.0/715*width)+x;
	int startY = (193.0/295*height)+y;
	float slotWidth = (685.0*715/width) / options.size();
	float slotHeight = (84.0/295*height);
	int slotNum=0;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	for(vector<menuLabel*>::iterator i = options.begin(); i!=options.end();i++,slotNum++)
	{
		if(cursorPos.x > startX+slotWidth*slotNum && cursorPos.x < startX+slotWidth*(1+slotNum) && cursorPos.y > startY && cursorPos.y+slotHeight*slotNum < startY+slotHeight*(slotNum+1))
		{
			dataManager.bind("glow");
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);	glVertex2f(startX+slotWidth*(0.5+slotNum)-textManager->getTextWidth((*i)->getText())*0.75-20,startY+slotHeight*0.5-35);
				glTexCoord2f(0,1);	glVertex2f(startX+slotWidth*(0.5+slotNum)-textManager->getTextWidth((*i)->getText())*0.75-20,startY+slotHeight*0.5+35);
				glTexCoord2f(1,1);	glVertex2f(startX+slotWidth*(0.5+slotNum)+textManager->getTextWidth((*i)->getText())*0.75+20,startY+slotHeight*0.5+35);
				glTexCoord2f(1,0);	glVertex2f(startX+slotWidth*(0.5+slotNum)+textManager->getTextWidth((*i)->getText())*0.75+20,startY+slotHeight*0.5-35);
			glEnd();
		}
		(*i)->render();
	}
	if(label != NULL) label->render();
	menuManager.drawCursor();

}
void menuMessageBox::mouseL(bool down, int X, int Y)
{
	if(done) return;

	int startX = (15.0/715*width)+x;
	int startY = (193.0/295*height)+y;
	float slotWidth = (685.0*715/width) / options.size();
	float slotHeight = (84.0/295*height)+y;
	if(down)
	{
		clicking=-1;
		for(int slotNum = 0; slotNum < options.size();slotNum++)
		{
			if(X > startX+slotNum*slotWidth && X < startX+(slotNum+1)*slotWidth && Y > startY && Y < startY+slotHeight)
				clicking = slotNum;
		}

	}
	else
	{
		int slotNum = 0;
		for(int slotNum = 0; slotNum < options.size();slotNum++)
		{
			if(X > startX+slotNum*slotWidth && X < startX+(slotNum+1)*slotWidth && Y > startY && Y < startY+slotHeight && clicking == slotNum)
			{
				value=clicking;
				done=true;
				return;
			}
		}
		clicking = -1;
	}
}


int menuScreen::backgroundImage=0;

bool menuScreen::loadBackground()
{
	backgroundImage=dataManager.loadTexture("media/menu/menu background2.tga");
	return backgroundImage != 0;
}

bool menuLevelEditor::init()
{
	vector<menuButton*> v;//for toggles
	menuLabel* l;

	//terrain
	bNewShader = new menuButton();		bNewShader->init(5,5,200,30,"new shader",lightGreen,white);
	bDiamondSquare = new menuButton();	bDiamondSquare->init(sw-105,5,100,30,"d-square",lightGreen,white);
	bFaultLine = new menuButton();		bFaultLine->init(sw-105,40,100,30,"fault line",lightGreen,white);
	bFromFile = new menuButton();		bFromFile->init(sw-105,75,100,30,"from file",lightGreen,white);
	bExportBMP = new menuButton();		bExportBMP->init(sw-105,110,100,30,"export",lightGreen,white);

	bLoad = new menuButton();			bLoad->init(sw-320,sh-40,100,35,"Load",Color(0.8,0.8,0.8),white);
	bSave = new menuButton();			bSave->init(sw-215,sh-40,100,35,"Save",Color(0.8,0.8,0.8),white);
	bExit = new menuButton();			bExit->init(sw-110,sh-40,100,35,"Exit",Color(0.8,0.8,0.8),white);

	bShaders = new menuToggle();		bShaders->init(vector<menuButton*>(),darkGreen,lightGreen,0);
	addShader("media/terrain.frag");
	addShader("media/snow.frag");
	addShader("media/grass.frag");
	//objects
	bAddPlane = new menuButton();		bAddPlane->init(5,5,100,30,"new plane",lightGreen,white);

	//settings

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,5,100,30,"respawn",black,white);
	v.push_back(new menuButton());		v[1]->init(225,5,100,30,"restart",black,white);
	v.push_back(new menuButton());		v[2]->init(330,5,100,30,"die",black,white);
	bOnHit = new menuToggle();			bOnHit->init(v,darkBlue,lightBlue);
	l->setElementText("player hit:");	l->setElementXY(5,5);
	bOnHit->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,45,100,30,"respawn",black,white);
	v.push_back(new menuButton());		v[1]->init(225,45,100,30,"restart",black,white);
	v.push_back(new menuButton());		v[2]->init(330,45,100,30,"die",black,white);
	bOnAIHit = new menuToggle();		bOnAIHit->init(v,darkBlue,lightBlue);
	l->setElementText("AI hit:");		l->setElementXY(5,45);
	bOnAIHit->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,85,100,30,"ffa",black,white);
	v.push_back(new menuButton());		v[1]->init(225,85,100,30,"teams",black,white);
	v.push_back(new menuButton());		v[2]->init(330,85,100,30,"player vs",black,white);
	bGameType = new menuToggle();		bGameType->init(v,darkBlue,lightBlue);
	l->setElementText("game type:");	l->setElementXY(5,85);
	bGameType->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,125,100,30,"water",black,white);
	v.push_back(new menuButton());		v[1]->init(225,125,100,30,"land",black,white);
	bMapType = new menuToggle();		bMapType->init(v,darkBlue,lightBlue);
	l->setElementText("map type:");		l->setElementXY(5,125);
	bMapType->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,165,100,30,"rock",black,white);
	v.push_back(new menuButton());		v[1]->init(225,165,100,30,"sand",black,white);
	bSeaFloorType = new menuToggle();	bSeaFloorType->init(v,darkBlue,lightBlue);
	l->setElementText("sea floor:");	l->setElementXY(5,165);
	bSeaFloorType->setLabel(l);

	v.clear();
	v.push_back(new menuButton());		v[0]->init(5,sh-40,100,35,"Terrain",black,white);
	v.push_back(new menuButton());		v[1]->init(110,sh-40,100,35,"Objects",black,white);
	v.push_back(new menuButton());		v[2]->init(215,sh-40,100,35,"Settings",black,white);
	bTabs = new menuToggle();			bTabs->init(v,Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),0);
	return true;
}
void menuLevelEditor::operator() (menuPopup* p)
{
	if(awaitingShaderFile)
	{
		string f=((menuOpenFile*)p)->getFile();
		addShader(f);
		awaitingShaderFile=false;
	}
	else if(awaitingMapFile)
	{
		string f=((menuOpenFile*)p)->getFile();
		((mapBuilder*)mode)->fromFile(f);
		awaitingMapFile=false;
	}
	else if(awaitingMapSave)
	{
		string f=((menuSaveFile*)p)->getFile();
		((mapBuilder*)mode)->level->exportBMP(f);
		awaitingMapSave=false;
	}
	else if(awaitingLevelFile)
	{
		string f=((menuOpenFile*)p)->getFile();
		delete ((mapBuilder*)mode)->level;
		LevelFile l(f);
		((mapBuilder*)mode)->level = new editLevel(l);
		awaitingLevelFile=false;
	}
	else if(awaitingLevelSave)
	{
		string f=((menuSaveFile*)p)->getFile();
		LevelFile l = ((mapBuilder*)mode)->level->getLevelFile();
		l.save(f);
		awaitingLevelSave=false;
	}
	else if(awaitingNewObject)
	{
		newObjectType=((menuNewObject*)p)->getObjectType();
		awaitingNewObject = false;
	}
	//else if(...)
	//   .
	//   .
	//   .
}
int menuLevelEditor::update()
{
	if(bLoad->getChanged())
	{
		awaitingLevelFile = true;
		bLoad->resetChanged();
		menuPopup* p = new menuOpenFile;
		p->callback = (functor<void,menuPopup*>*)this;
		((menuOpenFile*)p)->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(bSave->getChanged())
	{
		awaitingLevelSave = true;
		bSave->resetChanged();
		menuPopup* p = new menuSaveFile;
		p->callback = (functor<void,menuPopup*>*)this;
		((menuSaveFile*)p)->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(bExit->getChanged())
	{
		delete mode;
		mode=new blankMode;
		mode->init();
		//Cmenu=new m_chooseMode(*((m_chooseMode*)Cmenu));
		menuManager.setMenu("menuChooseMode");
	}
	else if(getTab() == TERRAIN)
	{
		if(bFaultLine->getChanged())
		{
			((mapBuilder*)mode)->faultLine();
			bFaultLine->resetChanged();
		}
		else if(bDiamondSquare->getChanged())
		{
			((mapBuilder*)mode)->diamondSquare(0.49);
			bDiamondSquare->resetChanged();
		}
		else if(bFromFile->getChanged())
		{
			awaitingMapFile = true;
			bFromFile->resetChanged();
			menuPopup* p = new menuOpenFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuOpenFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		else if(bExportBMP->getChanged())
		{
			awaitingMapSave = true;
			bExportBMP->resetChanged();
			menuPopup* p = new menuSaveFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuSaveFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		else if(bNewShader->getChanged())
		{
			awaitingShaderFile=true;
			bNewShader->resetChanged();
			menuPopup* p = new menuOpenFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuOpenFile*)p)->init(".frag");
			menuManager.setPopup(p);
		}
		else
		{
			//int n=0;
			//for(vector<menuButton*>::iterator i=bShaders.begin(); i != shaders.end();i++,n++)
			//{
			//	if((*i)->getPressed())
			//	{
			//		shaders[((mapBuilder*)mode)->currentShader]->setElementColor(Color(0,1,0));
			//		((mapBuilder*)mode)->currentShader=n;
			//		(*i)->setElementColor(Color(0,0.7,0));
			//		(*i)->reset();
			//	}
			//}
		}
	}
	else if(getTab() == OBJECTS)
	{
		if(bAddPlane->getChanged())
		{
			awaitingNewObject=true;
			bAddPlane->resetChanged();
			menuPopup* p = new menuNewObject;
			p->callback = (functor<void,menuPopup*>*)this;
			menuManager.setPopup(p);
		}
	}
	else if(getTab() == SETTINGS)
	{

	}
	//if(bTerrain->getPressed())
	//{
	//	currentTab=TERRAIN;
	//	bTerrain->reset();
	//}
	//if(bZones->getPressed())
	//{
	//	currentTab=ZONES;
	//	bZones->reset();
	//}
	//if(bSettings->getPressed())
	//{
	//	currentTab=SETTINGS;
	//	bSettings->reset();
	//}
	return 0;
}
void menuLevelEditor::render()
{
	glColor3f(0,1,0);
	if(getTab() == TERRAIN)
	{
		bShaders->render();
		bNewShader->render();
		bDiamondSquare->render();
		bFaultLine->render();
		bFromFile->render();
		bExportBMP->render();
	}
	else if(getTab() == OBJECTS)
	{
		bAddPlane->render();
	}
	else if(getTab() == SETTINGS)
	{
		bOnHit->render();
		bOnAIHit->render();
		bGameType->render();
		bMapType->render();
		bSeaFloorType->render();
	}
	bTabs->render();
	bLoad->render();
	bSave->render();
	bExit->render();
	menuManager.drawCursor();
}
void menuLevelEditor::mouseL(bool down, int x, int y)
{
	if(down)
	{
		if(getTab() == TERRAIN)
		{
			bNewShader->mouseDownL(x,y);
			bDiamondSquare->mouseDownL(x,y);
			bFaultLine->mouseDownL(x,y);
			bShaders->mouseDownL(x,y);
			bFromFile->mouseDownL(x,y);
			bExportBMP->mouseDownL(x,y);
		}
		else if(getTab() == OBJECTS)
		{
			if(!input->getMouseState(MIDDLE_BUTTON).down)
				bAddPlane->mouseDownL(x,y);
			if(newObjectType != 0)
			{
				((mapBuilder*)mode)->addObject(newObjectType, 0, x, y);
				newObjectType = 0;
			}
		}
		else if(getTab() == SETTINGS)
		{
			bOnHit->mouseDownL(x,y);
			bOnAIHit->mouseDownL(x,y);
			bGameType->mouseDownL(x,y);
			bMapType->mouseDownL(x,y);
			bSeaFloorType->mouseDownL(x,y);
		}
		bLoad->mouseDownL(x,y);
		bSave->mouseDownL(x,y);
		bExit->mouseDownL(x,y);
		bTabs->mouseDownL(x,y);
	}
	else
	{
		if(getTab() == TERRAIN)
		{
			bNewShader->mouseUpL(x,y);
			bDiamondSquare->mouseUpL(x,y);
			bFaultLine->mouseUpL(x,y);
			bShaders->mouseUpL(x,y);
			bTabs->mouseUpL(x,y);
			bFromFile->mouseUpL(x,y);
			bExportBMP->mouseUpL(x,y);
			if(bShaders->getChanged())
			{
				bShaders->resetChanged();
				if(bShaders->getValue() == 0)		bMapType->setValue(0);
				else if(bShaders->getValue() == 1)	bMapType->setValue(1);
				else if(bShaders->getValue() == 2)	bMapType->setValue(0);
			}
		}
		else if(getTab() == OBJECTS)
		{
			bAddPlane->mouseUpL(x,y);
		}
		else if(getTab() == SETTINGS)
		{
			bOnHit->mouseUpL(x,y);
			bOnAIHit->mouseUpL(x,y);
			bGameType->mouseUpL(x,y);
			bMapType->mouseUpL(x,y);
			bSeaFloorType->mouseUpL(x,y);
		}
		bTabs->mouseUpL(x,y);
		bLoad->mouseUpL(x,y);
		bSave->mouseUpL(x,y);
		bExit->mouseUpL(x,y);
	}

}
void menuLevelEditor::scroll(float rotations)
{
	((mapBuilder*)mode)->zoom(rotations);
}
void menuLevelEditor::mouseC(bool down, int x, int y)
{
	if(!down)
		((mapBuilder*)mode)->trackBallUpdate(x,y);
}
void menuLevelEditor::addShader(string filename)
{
	int s = dataManager.loadTerrainShader(filename);//((mapBuilder*)mode)->loadShader("media/terrain.vert",(char*)(string("media/")+filename).c_str());
	((mapBuilder*)mode)->shaderButtons.push_back(s);

	menuButton* b = new menuButton();
	b->init(5,bShaders->getSize()*35+5,200,30,filename,black,white);
	bShaders->addButton(b);
	//shaders.push_back(b);
	bNewShader->setElementXY(5,bShaders->getSize()*35+5);
}
int menuLevelEditor::getShader()
{
	return bShaders->getValue();
}
menuLevelEditor::Tab menuLevelEditor::getTab()
{
	int t =  bTabs->getValue();
	if(t == 0) return TERRAIN;
	if(t == 1) return OBJECTS;
	if(t == 2) return SETTINGS;
	return NO_TAB;
}
void menuChooseMode::render()
{
	graphics->drawOverlay(Vec2f(0,0),Vec2f(sw,sh),"menu background");
	float sx = (float)sw/800;
	float sy = (float)sh/600;
	graphics->drawOverlay(Vec2f((-327+420)*sx,304*sy),Vec2f(629*sx,115*sy),"menu pictures");
	for(int i=1;i<=5;i++)
	{
		if(i!=activeChoice+2)
		{
			graphics->drawOverlay(Vec2f((-327+i*210)*sx,300*sy),Vec2f(209*sx,150*sy),"menu slot");
		}
	}
	for(int i=1;i<=3;i++)
	{
		graphics->drawPartialOverlay(Vec2f((i*210-115)*sx,298*sy),Vec2f(205*sx,25*sy),Vec2f(0,0.33*(i-1)),Vec2f(1,0.33),"menu mode choices");
	}
}
void menuChooseMode::keyDown(int vkey)
{
	if(vkey==VK_LEFT)	activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_RIGHT)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<0) activeChoice=(choice)2;
	if(activeChoice>2) activeChoice=(choice)0;
	if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==SINGLE_PLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu("");

		delete mode;
		mode = new onePlayer;
		mode->init();
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MULTIPLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu("");

		delete mode;
		mode = new twoPlayerVs;
		mode->init();
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MAP_EDITOR)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu("");

		delete mode;
		mode = new mapBuilder;
		mode->init();
	}
}
bool menuInGame::init()
{
	activeChoice=RESUME;
	gameTime.pause();
	return true;
}
void menuInGame::render()
{
	if(activeChoice==RESUME)	glColor3f(1,1,0); else glColor3f(1,1,1);
	textManager->renderText("resume",sw/2-(textManager->getTextWidth("resume"))/2,sh/2-75);
	if(activeChoice==OPTIONS)	glColor3f(1,1,0); else glColor3f(1,1,1);
	textManager->renderText("options",sw/2-(textManager->getTextWidth("options"))/2,sh/2);
	if(activeChoice==QUIT)	glColor3f(1,1,0); else glColor3f(1,1,1);
	textManager->renderText("quit",sw/2-(textManager->getTextWidth("quit"))/2,sh/2+75);
	Redisplay=true;
}
void menuInGame::keyDown(int vkey)
{
	if(vkey==VK_UP)		activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_DOWN)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<RESUME) activeChoice=QUIT;
	if(activeChoice>QUIT) activeChoice=RESUME;
	if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==RESUME || vkey==0x31)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		input->up(0x31);
		gameTime.unpause();
		menuManager.setMenu("");
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==OPTIONS)
	{
		//
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==QUIT)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);

		gameTime.unpause();

		delete mode;
		mode=new blankMode;
		mode->init();

		//Cmenu=new m_chooseMode(*((m_chooseMode*)Cmenu));
		menuManager.setMenu("menuChooseMode");
	}
}

void menuButton::setElementText(string t)
{
	text=t;
	clampedText=t;
	if(textManager->getTextWidth(clampedText) < width-6)
	{
		return;
	}
	else if(textManager->getTextWidth("...") > width-6)
	{
		clampedText="";
	}
	else
	{
		while(textManager->getTextWidth(clampedText + "...") > width-6)
			clampedText.erase(clampedText.size()-1);
		int l=clampedText.find_last_not_of(" \t\f\v\n\r")+1;
		clampedText=clampedText.substr(0,l);
		clampedText += "...";
	}

}
void menuButton::setElementTextColor(Color c)
{
	textColor = c;
}
void menuButton::setElementXYWH(int X, int Y, int Width, int Height)
{
	x=X;
	y=Y;
	width=Width;
	height=Height;
	setElementText(text);//update clampedText
}
void menuButton::init(int X, int Y, int Width, int Height, string t, Color c, Color textC)
{
	setElementXYWH(X, Y, Width, Height);
	color = c;
	textColor = textC;
	setElementText(t);
}
void menuButton::render()
{
	glColor4f(color.r,color.g,color.b,color.a);
	dataManager.bind("button");
	glBegin(GL_QUADS);
	float tx,ty,twidth,theight;
	float rx,ry,rwidth,rheight;
	for(int ix=0;ix<3;ix++)
	{
		for(int iy=0;iy<3;iy++)
		{
			if(ix==0)
			{
				tx=0.0;
				twidth=10.0/140;
				rx=x;
				rwidth=min(10,width/2);
			}
			else if(ix==1)
			{
				tx=10.0/140;
				twidth=1.0-20.0/140;
				rx=x+10;
				rwidth=max(width-20,0);
			}
			else if(ix==2)
			{
				tx=1.0-10.0/140;
				twidth=10.0/140;
				rx=x+width-min(10,width/2);
				rwidth=min(10,width/2);
			}
			if(iy==0)
			{
				ty=0.0;
				theight=10.0/45;
				ry=y;
				rheight=min(10,height/2);
			}
			else if(iy==1)
			{
				ty=10.0/45;
				theight=1.0-20.0/45;
				ry=y+10;
				rheight=max(height-20,0);
			}
			else if(iy==2)
			{
				ty=1.0-10.0/45;
				theight=10.0/45;
				ry=y+height-min(10,height/2);
				rheight=min(10,height/2);
			}
			glTexCoord2f(tx,		ty);			glVertex2f(rx,ry);
			glTexCoord2f(tx+twidth,	ty);			glVertex2f(rx+rwidth,ry);
			glTexCoord2f(tx+twidth,	ty+theight);	glVertex2f(rx+rwidth,ry+rheight);
			glTexCoord2f(tx,		ty+theight);	glVertex2f(rx,ry+rheight);	
		}
	}
	glEnd();
	dataManager.bindTex(0);
	glColor4f(textColor.r,textColor.g,textColor.b,textColor.a);
	textManager->renderText(clampedText,x+(width-textManager->getTextWidth(clampedText))/2,y+(height-textManager->getTextHeight(clampedText))/2);
}
void menuButton::mouseDownL(int X, int Y)
{
	if(x < X && y < Y && x+width > X && y+height > Y)
		clicking = true;
}
void menuButton::mouseUpL(int X, int Y)
{
	if(clicking && x < X && y < Y && x+width > X && y+height > Y)
		changed = true;
	clicking = false;
}
void menuLabel::render()
{
	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,x,y);
}
void menuToggle::init(vector<menuButton*> b, Color clickedC, Color unclickedC, int startValue)
{
	buttons=b;
	if(startValue >= 0 && startValue < buttons.size())
		value=startValue;
	else 
		value=-1;
	clicked=clickedC;
	unclicked=unclickedC;
	
	int n=0;
	for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
	{
		if(value==n)
			(*i)->setElementColor(clicked);
		else
			(*i)->setElementColor(unclicked);
	}
}
int menuToggle::addButton(menuButton* button)
{
	button->setElementColor(unclicked);
	buttons.push_back(button);
	if(buttons.size()==1)
	{
		value=0;
		buttons[0]->setElementColor(clicked);
	}
	return buttons.size()-1;
}
void menuToggle::render()
{
	for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++)
	{
		(*i)->render();
	}
	if(label != NULL) label->render();
}
void menuToggle::mouseDownL(int X, int Y)
{
	for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++)
	{
		(*i)->mouseDownL(X,Y);
	}
}
void menuToggle::mouseUpL(int X, int Y)
{
	int n=0;
	for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
	{
		(*i)->resetChanged();
		(*i)->mouseUpL(X,Y);
		if((*i)->getChanged() && value != n)
		{
			value=n;
			changed=true;
		}
	}
	updateColors();
}
void menuToggle::updateColors()
{
	int n=0;
	for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
	{
		if(value==n)	(*i)->setElementColor(clicked);
		else			(*i)->setElementColor(unclicked);
	}
}
void messageBox(string text)
{
	menuMessageBox* m = new menuMessageBox;
	m->init(text);
	menuManager.setPopup(m);
}