
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

	//return setMenu("menuLevelEditor");
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
		glColor3f(0,0,0);
		glBegin(GL_TRIANGLES);
			glVertex2f((float)cursorPos.x,(float)cursorPos.y);
			glVertex2f((float)cursorPos.x,(float)cursorPos.y+20);
			glVertex2f((float)cursorPos.x+10,(float)cursorPos.y+16);
		glEnd();
		glDisable(GL_BLEND);
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
bool menuChooseFile::init()
{
	return true;
}
bool menuChooseFile::init(string ExtFilter)
{
	extFilter=ExtFilter;
	directory=".";
	refreshView();
	return true;
}
void menuChooseFile::refreshView()
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
		folderButtons.back()->init(10+145*column,5+40*row,140,35,(*i),Color(0.4,0.4,0.4));
		if(++column==4){column=0;row++;}
	}
	for(vector<string>::iterator i=files.begin();i!=files.end();i++)
	{
		fileButtons.push_back(new menuButton());
		fileButtons.back()->init(10+145*column,5+40*row,140,35,(*i),Color(0.6,0.6,0.6));
		if(++column==4){column=0;row++;}
	}
}
int menuChooseFile::update()
{
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
			done=true;
			return 0;
		}
	}
	return 0;
}
void menuChooseFile::render()
{
	glColor3f(0,0,1);
	glBegin(GL_QUADS);
		glVertex2f(sw/2-300,sh/2+200);
		glVertex2f(sw/2+300,sh/2+200);
		glVertex2f(sw/2+300,sh/2-200);
		glVertex2f(sw/2-300,sh/2-200);
	glEnd();
	glEnable(GL_BLEND);
	glColor4f(0,0,0,1);
	textManager->renderText(file,(sw-textManager->getTextWidth(file))/2,sh/2+195-textManager->getTextHeight(file));
	glPushMatrix();
	glTranslatef((sw/2-300),(sh/2-200),0);
	for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->render();
	for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->render();
	glPopMatrix();
	glDisable(GL_BLEND);
	menuManager.drawCursor();
}
void menuChooseFile::keyDown(int vkey)
{
	if(vkey == VK_RETURN)
		done=true;
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
void menuChooseFile::mouseL(bool down, int x, int y)
{
	if(down)
	{
		for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseDownL(x-(sw/2-300),y-(sh/2-200));
		for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseDownL(x-(sw/2-300),y-(sh/2-200));
	}
	else
	{
		for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseUpL(x-(sw/2-300),y-(sh/2-200));
		for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseUpL(x-(sw/2-300),y-(sh/2-200));
	}
}

bool menuMessageBox::init(string t)
{
	return init(t,vector<string>(1,"OK"));
}
bool menuMessageBox::init(string t, vector<string> options)
{
	width = max(textManager->getTextWidth(t)+20,options.size()*200);

	height = textManager->getTextHeight(t)+110;
	x = (sw-width)/2;
	y = (sh-height)/2;
	if(x < 5) x = 5;

	label = new menuLabel;
	label->init((width-textManager->getTextWidth(t))/2,20,t);

	int xv = (width - options.size()*200)/2+25;
	for(vector<string>::iterator i = options.begin();i !=options.end(); i++)
	{
		menuButton* b = new menuButton;
		b->init(xv,textManager->getTextHeight(t)+40,150,50,*i,Color(0.3,0.3,0.3));
		buttons.push_back(b);
		xv += 200;
	}
	value=-1;
	return true;
}
void menuMessageBox::render()
{
	glColor3f(0.5,0.5,0.5);
	glBegin(GL_QUADS);
		glVertex2f(x,y);
		glVertex2f(x,y+height);
		glVertex2f(x+width,y+height);
		glVertex2f(x+width,y);
	glEnd();
	glEnable(GL_BLEND);
	glPushMatrix();
	glTranslatef(x,y,0);
	for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++)
	{
		(*i)->render();
	}
	if(label != NULL) label->render();
	glPopMatrix();
	glDisable(GL_BLEND);
	menuManager.drawCursor();
}
void menuMessageBox::mouseL(bool down, int X, int Y)
{
	if(down)
	{
		for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++)
		{
			(*i)->mouseDownL(X-x,Y-y);
		}
	}
	else
	{
		int n=0;
		for(vector<menuButton*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
		{
			(*i)->mouseUpL(X-x,Y-y);
			if((*i)->getChanged() && value != n)
			{
				value=n;
				done=true;
			}
		}
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
	bNewShader = new menuButton();		bNewShader->init(5,5,200,30,"new shader");
	bDiamondSquare = new menuButton();	bDiamondSquare->init(sw-105,5,100,30,"d-square");
	bFaultLine = new menuButton();		bFaultLine->init(sw-105,40,100,30,"fault line");
	bFromFile = new menuButton();		bFromFile->init(sw-105,75,100,30,"from file");

	bExit = new menuButton();			bExit->init(sw-110,sh-40,100,30,"Exit",Color(0.8,0.8,0.8));

	bShaders = new menuToggle();		bShaders->init(vector<menuButton*>(),Color(0,0.7,0),Color(0,1,0),0);
	addShader("media/terrain.frag");
	addShader("media/snow.frag");
	addShader("media/grass.frag");
	//zones

	//settings
	
	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,50,100,30,"respawn");
	v.push_back(new menuButton());		v[1]->init(225,50,100,30,"restart");
	v.push_back(new menuButton());		v[2]->init(330,50,100,30,"die");
	bOnHit = new menuToggle();			bOnHit->init(v,Color(0,0,1),Color(0,0,0.5));
	l->setElementText("player hit:");	l->setElementXY(5,50);
	bOnHit->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,90,100,30,"respawn");
	v.push_back(new menuButton());		v[1]->init(225,90,100,30,"restart");
	v.push_back(new menuButton());		v[2]->init(330,90,100,30,"die");
	bOnAIHit = new menuToggle();		bOnAIHit->init(v,Color(0,0,1),Color(0.3,0.3,0.5));
	l->setElementText("AI hit:");		l->setElementXY(5,90);
	bOnAIHit->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,130,100,30,"ffa");
	v.push_back(new menuButton());		v[1]->init(225,130,100,30,"teams");
	v.push_back(new menuButton());		v[2]->init(330,130,100,30,"player vs");
	bGameType = new menuToggle();		bGameType->init(v,Color(0,0,1),Color(0.3,0.3,0.5));
	l->setElementText("game type:");	l->setElementXY(5,130);
	bGameType->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,170,100,30,"water");
	v.push_back(new menuButton());		v[1]->init(225,170,100,30,"land");
	bMapType = new menuToggle();		bMapType->init(v,Color(0,0,1),Color(0.3,0.3,0.5));
	l->setElementText("map type:");		l->setElementXY(5,170);
	bMapType->setLabel(l);

	v.clear();
	l=new menuLabel;
	v.push_back(new menuButton());		v[0]->init(120,210,100,30,"rock");
	v.push_back(new menuButton());		v[1]->init(225,210,100,30,"sand");
	bSeaFloorType = new menuToggle();	bSeaFloorType->init(v,Color(0,0,1),Color(0.3,0.3,0.5));
	l->setElementText("sea floor:");	l->setElementXY(5,210);
	bSeaFloorType->setLabel(l);

	v.clear();
	v.push_back(new menuButton());		v[0]->init(5,sh-35,100,30,"Terrain");
	v.push_back(new menuButton());		v[1]->init(110,sh-35,100,30,"Zones");
	v.push_back(new menuButton());		v[2]->init(215,sh-35,100,30,"Settings");
	bTabs = new menuToggle();			bTabs->init(v,Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),0);
	return true;
}
void menuLevelEditor::operator() (menuPopup* p)
{
	if(awaitingShaderFile)
	{
		string f=((menuChooseFile*)p)->getFile();
		addShader(f);
		awaitingShaderFile=false;
	}
	else if(awaitingMapFile)
	{
		string f=((menuChooseFile*)p)->getFile();
		((mapBuilder*)mode)->fromFile(f);
		awaitingMapFile=false;
	}
	//else if(...)
	//   .
	//   .
	//   .
}
int menuLevelEditor::update()
{
	if(bExit->getChanged())
	{
		delete mode;
		mode=new blankMode;
		mode->init();
		Cmenu=new m_chooseMode(*((m_chooseMode*)Cmenu));
		menuManager.setMenu("");
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
			((mapBuilder*)mode)->diamondSquare(0.9);
			bDiamondSquare->resetChanged();
		}
		else if(bFromFile->getChanged())
		{
			awaitingMapFile = true;
			bFromFile->resetChanged();
			menuPopup* p = new menuChooseFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuChooseFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		else if(bNewShader->getChanged())
		{
			awaitingShaderFile=true;
			bNewShader->resetChanged();
			menuPopup* p = new menuChooseFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuChooseFile*)p)->init(".frag");
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
	else if(getTab() == ZONES)
	{

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
	glEnable(GL_BLEND);
	glColor3f(0,1,0);
	if(getTab() == TERRAIN)
	{
		bShaders->render();
		bNewShader->render();
		bDiamondSquare->render();
		bFaultLine->render();
		bFromFile->render();
	}
	else if(getTab() == ZONES)
	{

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

		}
		else if(getTab() == ZONES)
		{

		}
		else if(getTab() == SETTINGS)
		{
			bOnHit->mouseDownL(x,y);
			bOnAIHit->mouseDownL(x,y);
			bGameType->mouseDownL(x,y);
			bMapType->mouseDownL(x,y);
			bSeaFloorType->mouseDownL(x,y);
		}
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
			if(bShaders->getChanged())
			{
				bShaders->resetChanged();
				if(bShaders->getValue() == 0)		bMapType->setValue(0);
				else if(bShaders->getValue() == 1)	bMapType->setValue(1);
				else if(bShaders->getValue() == 2)	bMapType->setValue(0);
			}
		}
		else if(getTab() == ZONES)
		{

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
	b->init(5,bShaders->getSize()*35+5,200,30,filename);
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
	if(t == 1) return ZONES;
	if(t == 2) return SETTINGS;
	return NO_TAB;
}

void menuInGame::render()
{
	glEnable(GL_BLEND);
	if(activeChoice==RESUME)	glColor3f(1,1,0); else glColor3f(1,1,1);
	textManager->renderText("resume",sw/2-(textManager->getTextWidth("resume"))/2,sh/2-75);
	if(activeChoice==OPTIONS)	glColor3f(1,1,0); else glColor3f(1,1,1);
	textManager->renderText("options",sw/2-(textManager->getTextWidth("options"))/2,sh/2);
	if(activeChoice==QUIT)	glColor3f(1,1,0); else glColor3f(1,1,1);
	textManager->renderText("quit",sw/2-(textManager->getTextWidth("quit"))/2,sh/2+75);
	glDisable(GL_BLEND);
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
		Cmenu=new m_chooseMode(*((m_chooseMode*)Cmenu));
		menuManager.setMenu("");
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
void menuButton::setElementXYWH(int X, int Y, int Width, int Height)
{
	x=X;
	y=Y;
	width=Width;
	height=Height;
	setElementText(text);//update clampedText
}
void menuButton::init(int X, int Y, int Width, int Height, string t, Color c)
{
	setElementXYWH(X, Y, Width, Height);
	color=c;
	setElementText(t);
}
void menuButton::render()
{
	glColor3f(color.r,color.g,color.b);
	glBegin(GL_QUADS);
	glVertex2f(x,y);
	glVertex2f(x+width,y);
	glVertex2f(x+width,y+height);
	glVertex2f(x,y+height);
	glEnd();
	glEnable(GL_BLEND);
	glColor4f(0,0,0,1);
	textManager->renderText(clampedText,x+(width-textManager->getTextWidth(clampedText))/2,y+(height-textManager->getTextHeight(clampedText))/2);
	glDisable(GL_BLEND);
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
	glEnable(GL_BLEND);
	glColor4f(color.r,color.g,color.b,1);
	textManager->renderText(text,x,y);
	glDisable(GL_BLEND);
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