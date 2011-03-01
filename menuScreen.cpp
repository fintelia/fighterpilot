
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
	registerMenu("menuLoading", &CreateObject<menuLoading>);

	return true;
}
void MenuManager::render()
{
	mDrawCursor = false;
	if(menu != NULL)
	{
		menu->render();
		for(auto e = menu->labels.begin(); e != menu->labels.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = menu->buttons.begin(); e != menu->buttons.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = menu->toggles.begin(); e != menu->toggles.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
	}
	for(auto i = popups.begin(); i!=popups.end();i++)
	{
		(*i)->render();

		for(auto e = (*i)->labels.begin(); e != (*i)->labels.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = (*i)->buttons.begin(); e != (*i)->buttons.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = (*i)->toggles.begin(); e != (*i)->toggles.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
	}
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
	{
		menu->update();
	}
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
void MenuManager::inputCallback(Input::callBack* callback)
{
	if(callback->type == KEY_STROKE){
		Input::keyStroke* call = (Input::keyStroke*)callback;
		if(!popups.empty())
		{
			int pSize = popups.size();
			if(!call->up)
				popups.back()->keyDown(call->vkey);
			else
				popups.back()->keyUp(call->vkey);
			if(pSize==popups.size())
			{
				for(auto e = popups.back()->buttons.begin(); pSize==popups.size() && e != popups.back()->buttons.end(); e++)
				{
					if(e->second != NULL && call->up)		e->second->keyUp(call->vkey);
					else if(e->second != NULL && !call->up)	e->second->keyDown(call->vkey);
				}
			}
			if(pSize==popups.size())
			{
				for(auto e = popups.back()->toggles.begin(); pSize==popups.size() && e != popups.back()->toggles.end(); e++)
				{
					if(e->second != NULL && call->up)		e->second->keyUp(call->vkey);
					else if(e->second != NULL && !call->up)	e->second->keyDown(call->vkey);
				}
			}
		}
		else if(menu!=NULL)
		{
			if(!call->up)	menu->keyDown(call->vkey);
			else			menu->keyUp(call->vkey);

			if(menu!=NULL)//if menu is still not NULL (could have changed due to this keystroke)
			{
				for(auto e = menu->buttons.begin(); menu!=NULL && e != menu->buttons.end(); e++)
				{
					if(e->second != NULL && call->up)		e->second->keyUp(call->vkey);
					else if(e->second != NULL && !call->up)	e->second->keyDown(call->vkey);
				}
			}
			if(menu!=NULL)
			{
				for(auto e = menu->toggles.begin(); menu!=NULL && e != menu->toggles.end(); e++)
				{
					if(e->second != NULL && call->up)		e->second->keyUp(call->vkey);
					else if(e->second != NULL && !call->up)	e->second->keyDown(call->vkey);
				}
			}

		}
	}
	else if(callback->type == MOUSE_CLICK){
		Input::mouseClick* call = (Input::mouseClick*)callback;
		
		if(!popups.empty())
		{
			int pSize = popups.size();
			if(call->button == LEFT_BUTTON)
				popups.back()->mouseL(call->down,call->x,call->y);
			else if(call->button == RIGHT_BUTTON)
				popups.back()->mouseR(call->down,call->x,call->y);
			
			if(pSize==popups.size())
			{
				for(auto e = popups.back()->buttons.begin(); pSize==popups.size() && e != popups.back()->buttons.end(); e++)
				{
					if(e->second != NULL)
					{
						if(call->button == LEFT_BUTTON && call->down)		e->second->mouseDownL(call->x,call->y);
						else if(call->button == LEFT_BUTTON && !call->down)	e->second->mouseUpL(call->x,call->y);
						else if(call->button == RIGHT_BUTTON && call->down)	e->second->mouseDownR(call->x,call->y);
						else if(call->button == RIGHT_BUTTON && !call->down)e->second->mouseUpR(call->x,call->y);
					}
				}
			}
			if(pSize==popups.size())
			{
				for(auto e = popups.back()->toggles.begin(); pSize==popups.size() && e != popups.back()->toggles.end(); e++)
				{
					if(e->second != NULL)
					{
						if(call->button == LEFT_BUTTON && call->down)		e->second->mouseDownL(call->x,call->y);
						else if(call->button == LEFT_BUTTON && !call->down)	e->second->mouseUpL(call->x,call->y);
						else if(call->button == RIGHT_BUTTON && call->down)	e->second->mouseDownR(call->x,call->y);
						else if(call->button == RIGHT_BUTTON && !call->down)e->second->mouseUpR(call->x,call->y);
					}
				}
			}
		}
		else if(menu!=NULL)
		{
			if(call->button == LEFT_BUTTON)
				menu->mouseL(call->down,call->x,call->y);
			else if(call->button == MIDDLE_BUTTON)
				menu->mouseC(call->down,call->x,call->y);
			else if(call->button == RIGHT_BUTTON)
				menu->mouseR(call->down,call->x,call->y);

			if(menu!=NULL)
			{
				for(auto e = menu->buttons.begin(); menu!=NULL && e != menu->buttons.end(); e++)
				{
					if(e->second != NULL)
					{
						if(call->button == LEFT_BUTTON && call->down)	e->second->mouseDownL(call->x,call->y);
						if(call->button == LEFT_BUTTON && !call->down)	e->second->mouseUpL(call->x,call->y);
						if(call->button == RIGHT_BUTTON && call->down)	e->second->mouseDownR(call->x,call->y);
						if(call->button == RIGHT_BUTTON && !call->down)	e->second->mouseUpR(call->x,call->y);
					}
				}
			}
			if(menu!=NULL)
			{
				for(auto e = menu->toggles.begin(); menu!=NULL && e != menu->toggles.end(); e++)
				{
					if(e->second != NULL)
					{
						if(call->button == LEFT_BUTTON && call->down)	e->second->mouseDownL(call->x,call->y);
						if(call->button == LEFT_BUTTON && !call->down)	e->second->mouseUpL(call->x,call->y);
						if(call->button == RIGHT_BUTTON && call->down)	e->second->mouseDownR(call->x,call->y);
						if(call->button == RIGHT_BUTTON && !call->down)	e->second->mouseUpR(call->x,call->y);
					}
				}
			}
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
	buttons["desktop"]		= new menuButton(sw/2-405,sh/2-246,175,40,"Desktop",lightGreen,white);
	buttons["myDocuments"]	= new menuButton(sw/2-405,sh/2-196,175,40,"My Documents",lightGreen,white);
	buttons["myComputer"]	= new menuButton(sw/2-405,sh/2-146,175,40,"My Computer",lightGreen,white);
	buttons["myNetwork"]	= new menuButton(sw/2-405,sh/2-96,175,40,"My Network",lightGreen,white);

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
		folderButtons.push_back(new menuButton(12+140*column,5+40*row,135,35,(*i),Color(0.4,0.4,0.4)));
		if(++column==4){column=0;row++;}
	}
	for(vector<string>::iterator i=files.begin();i!=files.end();i++)
	{
		fileButtons.push_back(new menuButton(12+140*column,5+40*row,135,35,(*i),Color(0.6,0.6,0.6)));
		if(++column==4){column=0;row++;}
	}
}
int menuOpenFile::update()
{
	if(buttons["desktop"]->checkChanged())
	{
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	else if(buttons["myDocuments"]->checkChanged())
	{
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	else if(buttons["myComputer"]->checkChanged())
	{
		char aFolder[MAX_PATH]; 
		if(!SHGetFolderPathA(0, CSIDL_DRIVES, NULL, SHGFP_TYPE_CURRENT, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
		return 0;
	}
	else if(buttons["myNetwork"]->checkChanged())
	{
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
		if((*i)->checkChanged())
		{
			directory/=(*i)->getText();
			refreshView();
			return 0;
		}
	}
	for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)
	{
		if((*i)->checkChanged())
		{
			file=(*i)->getText();
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
	menuManager.drawCursor();
}
void menuOpenFile::keyDown(int vkey)
{
	if(vkey == VK_RETURN)
	{
		fileSelected();
	}
	else if(vkey == VK_ESCAPE)
	{
		file = "";
		fileSelected();
		input->up(VK_ESCAPE);//so the program will not quit
	}
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
		clicking = (x>sw/2+144) && (x<sw/2+184) && (y>sh/2+190) && (y<sh/2+230);
	}
	else
	{
		if(clicking) 
			fileSelected();
		for(vector<menuButton*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246));
		for(vector<menuButton*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246));
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

	labels["label"] = new menuLabel(x+(width-textManager->getTextWidth(t))/2, y+height*95/295-textManager->getTextHeight(t)/2, t, Color(1,1,1,0.8));

	float slotWidth = (685.0*width/715) / names.size();
	int slotNum = 0;
	for(vector<string>::iterator i = names.begin();i !=names.end(); i++, slotNum++)
	{
		options.push_back(new menuLabel(x + (15.0/715*width)+(0.5+slotNum)*slotWidth-textManager->getTextWidth(*i)/2,y + (235.0/295*height)-textManager->getTextHeight(*i)/2, *i, white));
	}
	value=-1;
	clicking=-1;
	return true;
}
void menuMessageBox::render()
{
	glColor4f(1,1,1,1);
	if(dataManager.getId("dialog box") != 0)
		dataManager.bind("dialog box");
	else
		glColor4f(0.3,0.3,0.3,1);

	glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(x,y);
		glTexCoord2f(0,1);	glVertex2f(x,y+height);
		glTexCoord2f(1,1);	glVertex2f(x+width,y+height);
		glTexCoord2f(1,0);	glVertex2f(x+width,y);
	glEnd();
	dataManager.bindTex(0);

	int startX = (15.0*width/715)+x;
	int startY = (193.0*height/295)+y;
	float slotWidth = (685.0*width/715) / options.size();
	float slotHeight = (84.0*height/295);
	int slotNum=0;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	for(vector<menuLabel*>::iterator i = options.begin(); i!=options.end();i++,slotNum++)
	{
		if(dataManager.getId("glow") != 0 && cursorPos.x > startX+slotWidth*slotNum && cursorPos.x < startX+slotWidth*(1+slotNum) && cursorPos.y > startY && cursorPos.y+slotHeight*slotNum < startY+slotHeight*(slotNum+1))
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
	menuManager.drawCursor();
}
void menuMessageBox::mouseL(bool down, int X, int Y)
{
	if(done) return;

	int startX = (15.0*width/715)+x;
	int startY = (193.0*height/295)+y;
	float slotWidth = (685.0*width/715) / options.size();
	float slotHeight = (84.0*height/295);
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

	//terrain

	buttons["newShader"]	= new menuButton(5,5,200,30,"new shader",lightGreen,white);
	buttons["dSquare"]		= new menuButton(sw-105,5,100,30,"d-square",lightGreen,white);
	buttons["faultLine"]	= new menuButton(sw-105,40,100,30,"fault line",lightGreen,white);
	buttons["fromFile"]		= new menuButton(sw-105,75,100,30,"from file",lightGreen,white);
	buttons["exportBMP"]	= new menuButton(sw-105,110,100,30,"export",lightGreen,white);

	buttons["load"]			= new menuButton(sw-320,sh-40,100,35,"Load",Color(0.8,0.8,0.8),white);
	buttons["save"]			= new menuButton(sw-215,sh-40,100,35,"Save",Color(0.8,0.8,0.8),white);
	buttons["exit"]			= new menuButton(sw-110,sh-40,100,35,"Exit",Color(0.8,0.8,0.8),white);

	toggles["shaders"]		= new menuToggle(vector<menuButton*>(),darkGreen,lightGreen,NULL,0);
	buttons["new shader"]	= new menuButton(5,5,200,30,"new shader",lightGreen,white);
	addShader("media/terrain.frag");
	addShader("media/snow.frag");
	addShader("media/grass.frag");
	//objects
	buttons["addPlane"]		= new menuButton(5,5,100,30,"new plane",lightGreen,white);

	//settings

	v.clear();
	v.push_back(new menuButton(120,5,100,30,"respawn",black,white));
	v.push_back(new menuButton(225,5,100,30,"restart",black,white));
	v.push_back(new menuButton(330,5,100,30,"die",black,white));
	toggles["onHit"]		= new menuToggle(v,darkBlue,lightBlue,new menuLabel(5,5,"player hit:"));

	v.clear();
	v.push_back(new menuButton(120,45,100,30,"respawn",black,white));
	v.push_back(new menuButton(225,45,100,30,"restart",black,white));
	v.push_back(new menuButton(330,45,100,30,"die",black,white));
	toggles["onAIHit"]		= new menuToggle(v,darkBlue,lightBlue,new menuLabel(5,45,"AI hit:"));

	v.clear();
	v.push_back(new menuButton(120,85,100,30,"ffa",black,white));
	v.push_back(new menuButton(225,85,100,30,"teams",black,white));
	v.push_back(new menuButton(330,85,100,30,"player vs",black,white));
	toggles["gameType"]	= new menuToggle(v,darkBlue,lightBlue,new menuLabel(5,85,"game type:"));

	v.clear();
	v.push_back(new menuButton(120,125,100,30,"water",black,white));
	v.push_back(new menuButton(225,125,100,30,"land",black,white));
	toggles["mapType"]	= new menuToggle(v,darkBlue,lightBlue,new menuLabel(5,125,"map type:"));

	v.clear();
	v.push_back(new menuButton(120,165,100,30,"rock",black,white));
	v.push_back(new menuButton(225,165,100,30,"sand",black,white));
	toggles["seaFloorType"]	= new menuToggle(v,darkBlue,lightBlue,new menuLabel(5,165,"sea floor:"));

	v.clear();
	v.push_back(new menuButton(5,sh-40,100,35,"Terrain",black,white));
	v.push_back(new menuButton(110,sh-40,100,35,"Objects",black,white));
	v.push_back(new menuButton(215,sh-40,100,35,"Settings",black,white));
	toggles["tabs"]	= new menuToggle(v,Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),NULL,0);

	return true;
}
void menuLevelEditor::operator() (menuPopup* p)
{
	if(awaitingShaderFile)
	{
		awaitingShaderFile=false;
		if(!((menuSaveFile*)p)->validFile()) return;
		string f=((menuOpenFile*)p)->getFile();
		addShader(f);
	}
	else if(awaitingMapFile)
	{
		awaitingMapFile=false;
		if(!((menuSaveFile*)p)->validFile()) return;
		string f=((menuOpenFile*)p)->getFile();
		((modeMapBuilder*)modeManager.getMode())->fromFile(f);
	}
	else if(awaitingMapSave)
	{
		awaitingMapSave=false;
		if(!((menuSaveFile*)p)->validFile()) return;
		string f=((menuSaveFile*)p)->getFile();
		((modeMapBuilder*)modeManager.getMode())->level->exportBMP(f);
	}
	else if(awaitingLevelFile)
	{
		awaitingLevelFile=false;
		if(!((menuSaveFile*)p)->validFile()) return;
		string f=((menuOpenFile*)p)->getFile();
		delete ((modeMapBuilder*)modeManager.getMode())->level;
		LevelFile l(f);
		((modeMapBuilder*)modeManager.getMode())->level = new editLevel(l);
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((menuSaveFile*)p)->validFile()) return;
		string f=((menuSaveFile*)p)->getFile();
		LevelFile l = ((modeMapBuilder*)modeManager.getMode())->level->getLevelFile();
		l.save(f);
	}
	else if(awaitingNewObject)
	{
		awaitingNewObject = false;
		newObjectType=((menuNewObject*)p)->getObjectType();
	}
	//else if(...)
	//   .
	//   .
	//   .
}
int menuLevelEditor::update()
{
	if(buttons["load"]->checkChanged())
	{
		awaitingLevelFile = true;
		menuPopup* p = new menuOpenFile;
		p->callback = (functor<void,menuPopup*>*)this;
		((menuOpenFile*)p)->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(buttons["save"]->checkChanged())
	{
		awaitingLevelSave = true;
		menuPopup* p = new menuSaveFile;
		p->callback = (functor<void,menuPopup*>*)this;
		((menuSaveFile*)p)->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(buttons["exit"]->checkChanged())
	{
		modeManager.setMode(NULL);
		menuManager.setMenu("menuChooseMode");
	}
	else if(getTab() == TERRAIN)
	{
		if(buttons["faultLine"]->checkChanged())
		{
			((modeMapBuilder*)modeManager.getMode())->faultLine();
		}
		else if(buttons["dSquare"]->checkChanged())
		{
			((modeMapBuilder*)modeManager.getMode())->diamondSquare(0.49);
		}
		else if(buttons["fromFile"]->checkChanged())
		{
			awaitingMapFile = true;
			menuPopup* p = new menuOpenFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuOpenFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		else if(buttons["exportBMP"]->checkChanged())
		{
			awaitingMapSave = true;
			menuPopup* p = new menuSaveFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuSaveFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		else if(buttons["newShader"]->checkChanged())
		{
			awaitingShaderFile=true;
			menuPopup* p = new menuOpenFile;
			p->callback = (functor<void,menuPopup*>*)this;
			((menuOpenFile*)p)->init(".frag");
			menuManager.setPopup(p);
		}
	}
	else if(getTab() == OBJECTS)
	{
		if(buttons["addPlane"]->checkChanged())
		{
			awaitingNewObject=true;
			menuPopup* p = new menuNewObject;
			p->callback = (functor<void,menuPopup*>*)this;
			menuManager.setPopup(p);
		}
	}
	else if(getTab() == SETTINGS)
	{

	}

	static Tab lastTab = (Tab)-1;
	Tab newTab = getTab();
	if(lastTab != newTab)
	{
		if(lastTab == TERRAIN || newTab==TERRAIN || lastTab == (Tab)-1)
		{
			buttons["newShader"]->setVisibility(newTab==TERRAIN);
			buttons["dSquare"]->setVisibility(newTab==TERRAIN);
			buttons["faultLine"]->setVisibility(newTab==TERRAIN);
			buttons["fromFile"]->setVisibility(newTab==TERRAIN);
			buttons["exportBMP"]->setVisibility(newTab==TERRAIN);
			toggles["shaders"]->setVisibility(newTab==TERRAIN);
			buttons["new shader"]->setVisibility(newTab==TERRAIN);
		}
		if(lastTab == OBJECTS || newTab==OBJECTS || lastTab == (Tab)-1)
		{
			buttons["addPlane"]->setVisibility(newTab==OBJECTS);
		}
		if(lastTab == SETTINGS || newTab==SETTINGS || lastTab == (Tab)-1)
		{
			toggles["onHit"]->setVisibility(newTab==SETTINGS);
			toggles["onAIHit"]->setVisibility(newTab==SETTINGS);
			toggles["gameType"]->setVisibility(newTab==SETTINGS);
			toggles["mapType"]->setVisibility(newTab==SETTINGS);
			toggles["seaFloorType"]->setVisibility(newTab==SETTINGS);
		}
	}
	lastTab = newTab;
	return 0;
}
void menuLevelEditor::render()
{
	menuManager.drawCursor();
}
void menuLevelEditor::mouseL(bool down, int x, int y)
{
	if(getTab() == OBJECTS)
	{
		if(newObjectType != 0)
		{
			static int teamNum=0;
			((modeMapBuilder*)modeManager.getMode())->addObject(newObjectType, teamNum, teamNum<=1 ? CONTROL_HUMAN : CONTROL_COMPUTER, x, y);
			newObjectType = 0;
			teamNum++;
		}
	}
}
void menuLevelEditor::scroll(float rotations)
{
	((modeMapBuilder*)modeManager.getMode())->zoom(rotations);
}
void menuLevelEditor::mouseC(bool down, int x, int y)
{
	if(!down)
		((modeMapBuilder*)modeManager.getMode())->trackBallUpdate(x,y);
}
void menuLevelEditor::addShader(string filename)
{
	int s = dataManager.loadTerrainShader(filename);//((mapBuilder*)mode)->loadShader("media/terrain.vert",(char*)(string("media/")+filename).c_str());
	((modeMapBuilder*)modeManager.getMode())->shaderButtons.push_back(s);

	toggles["shaders"]->addButton(new menuButton(5,toggles["shaders"]->getSize()*35+5,200,30,filename,black,white));
	buttons["newShader"]->setElementXY(5,toggles["shaders"]->getSize()*35+5);
}
int menuLevelEditor::getShader()
{
	return toggles["shaders"]->getValue();
}
menuLevelEditor::Tab menuLevelEditor::getTab()
{
	int t =  toggles["tabs"]->getValue();
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
	if(vkey==VK_ESCAPE)	done = true;//end the program
	if(vkey==VK_LEFT)	activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_RIGHT)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<0) activeChoice=(choice)2;
	if(activeChoice>2) activeChoice=(choice)0;
	if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==SINGLE_PLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu("");

		modeManager.setMode(new modeCampaign("media/map file.lvl"));
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MULTIPLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu("");

		modeManager.setMode(new modeSplitScreen("media/map file.lvl"));
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MAP_EDITOR)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu("");

		modeManager.setMode(new modeMapBuilder);
	}
}
bool menuInGame::init()
{
	activeChoice=RESUME;
	world.time.pause();
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
}
void menuInGame::keyDown(int vkey)
{
	if(vkey==VK_UP)		activeChoice = choice(int(activeChoice)-1);
	if(vkey==VK_DOWN)	activeChoice = choice(int(activeChoice)+1);
	if(activeChoice<RESUME) activeChoice=QUIT;
	if(activeChoice>QUIT) activeChoice=RESUME;
	if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==RESUME || vkey==0x31 || vkey==VK_ESCAPE)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		input->up(0x31);
		input->up(VK_ESCAPE);
		world.time.unpause();
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

		world.time.unpause();

		modeManager.setMode(NULL);
		menuManager.setMenu("menuChooseMode");
	}
}
bool menuLoading::init()
{
	progress = 0.0f;
	return true;
}
int menuLoading::update()
{
	static bool toLoad = true;
	if(toLoad)
	{
		dataManager.registerAsset("menu background", "media/menu/menu background.png");
		toLoad=false;
	}

	static int totalAssets = -1;
	int assetsLeft = dataManager.registerAssets();
	if(totalAssets == -1) totalAssets = assetsLeft+1;
	progress = 1.0-(float)assetsLeft/totalAssets;
	if(assetsLeft==0)	menuManager.setMenu("menuChooseMode");

	return 30;
}
void menuLoading::render()
{
	graphics->drawOverlay(Vec2f(0,0),Vec2f(sw,sh),"menu background");
	glColor3f(1,1,1);	graphics->drawOverlay(Vec2f(sw*0.4,sh*0.96),Vec2f(sw*0.2,sh*0.02));
	glColor3f(0,1,0);	graphics->drawOverlay(Vec2f(sw*0.4,sh*0.96),Vec2f(sw*0.2*progress,sh*0.02));
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
void menuCheckBox::render()
{
	glColor4f(1,1,1,1);
	dataManager.bind("check box");
	glBegin(GL_QUADS);
		glTexCoord2f(0,	0);	glVertex2f(x,y);		
		glTexCoord2f(1,	0);	glVertex2f(x+26,y);		
		glTexCoord2f(1,	1);	glVertex2f(x+26,y+26);	
		glTexCoord2f(0,	1);	glVertex2f(x,y+26);	
	glEnd();
	if(checked)
	{
		dataManager.bind("check");
		glBegin(GL_QUADS);
			glTexCoord2f(0,	0);	glVertex2f(x,y);		
			glTexCoord2f(1,	0);	glVertex2f(x+26,y);		
			glTexCoord2f(1,	1);	glVertex2f(x+26,y+26);	
			glTexCoord2f(0,	1);	glVertex2f(x,y+26);	
		glEnd();
	}
	dataManager.bindTex(0);

	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,x+30,y);
}
void menuCheckBox::mouseDownL(int X, int Y)
{
	if((x < X && y < Y && x+30 > X && y+25 > Y) || (x+30 < X && y < Y && x+width > X && y+height > Y))
		clicking = true;
}
void menuCheckBox::mouseUpL(int X, int Y)
{
	if(clicking && ((x < X && y < Y && x+30 > X && y+25 > Y) || (x+30 < X && y < Y && x+width > X && y+height > Y)) )
	{
		changed = true;
		checked = !checked;
	}
	clicking = false;
}

void menuLabel::render()
{
	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,x,y);
}
menuToggle::menuToggle(vector<menuButton*> b, Color clickedC, Color unclickedC, menuLabel* l, int startValue): menuElement(TOGGLE,0,0), value(startValue), label(l)
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
void closingMessage(string text,string title)
{
	if(!textManager->loadSuccessful())
	{
		graphics->destroyWindow();
		MessageBoxA(NULL,text.c_str(),title.c_str(),MB_ICONERROR);
		exit(0);
	}
	else
	{
		struct exitor: public functor<void,menuPopup*>
		{
			void operator() (menuPopup*){exit(0);}
		};
		menuMessageBox* m = new menuMessageBox;
		m->init(text);
		m->callback = (functor<void,menuPopup*>*)(new exitor);
		menuManager.setPopup(m);
	}
}