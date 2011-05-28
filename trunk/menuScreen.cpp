
#include "main.h"


//typedef map<string,MenuCreateFunc>::iterator Iterator;
//manager* pInstance=NULL;
namespace menu
{
void manager::render()
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
		for(auto e = menu->sliders.begin(); e != menu->sliders.end(); e++)
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
		for(auto e = (*i)->sliders.begin(); e != (*i)->sliders.end(); e++)
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
int manager::update()
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
void manager::shutdown()
{
	for(vector<popup*>::iterator i = popups.begin(); i!=popups.end();i++)
		delete (*i);
	if(menu != NULL)
		delete menu;
}
void manager::setMenu(screen* m)
{
	if(menu != NULL)
		delete menu;

	menu = m;
}
void manager::inputCallback(Input::callBack* callback)
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
			if(pSize==popups.size())
			{
				for(auto e = popups.back()->sliders.begin(); pSize==popups.size() && e != popups.back()->sliders.end(); e++)
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
			if(menu!=NULL)
			{
				for(auto e = menu->sliders.begin(); menu!=NULL && e != menu->sliders.end(); e++)
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
bool openFile::init()
{
	return true;
}
bool openFile::init(string ExtFilter)
{
	buttons["desktop"]		= new button(sw/2-405,sh/2-246,175,40,"Desktop",lightGreen,white);
	buttons["myDocuments"]	= new button(sw/2-405,sh/2-196,175,40,"My Documents",lightGreen,white);
	buttons["myComputer"]	= new button(sw/2-405,sh/2-146,175,40,"My Computer",lightGreen,white);
	buttons["myNetwork"]	= new button(sw/2-405,sh/2-96,175,40,"My Network",lightGreen,white);

	extFilter=ExtFilter;
	directory=".";
	refreshView();
	return true;
}
void openFile::refreshView()
{
	for(auto i=folderButtons.begin();i!=folderButtons.end();i++)		if(*i!=NULL) delete (*i);
	for(auto i=fileButtons.begin();i!=fileButtons.end();i++)			if(*i!=NULL) delete (*i);
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
	for(auto i=folders.begin();i!=folders.end();i++)	
	{
		folderButtons.push_back(new button(12+140*column,5+40*row,135,35,(*i),Color(0.4,0.4,0.4)));
		if(++column==4){column=0;row++;}
	}
	for(auto i=files.begin();i!=files.end();i++)
	{
		fileButtons.push_back(new button(12+140*column,5+40*row,135,35,(*i),Color(0.6,0.6,0.6)));
		if(++column==4){column=0;row++;}
	}
}
int openFile::update()
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
	for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)
	{
		if((*i)->checkChanged())
		{
			directory/=(*i)->getText();
			refreshView();
			return 0;
		}
	}
	for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)
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
void openFile::render()
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
	for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->render();
	for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->render();
	glPopMatrix();
	menuManager.drawCursor();
}
void openFile::keyDown(int vkey)
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
void openFile::mouseL(bool down, int x, int y)
{
	static bool clicking = false;
	if(down)
	{
		for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246));
		for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246));
		clicking = (x>sw/2+144) && (x<sw/2+184) && (y>sh/2+190) && (y<sh/2+230);
	}
	else
	{
		if(clicking) 
			fileSelected();
		for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246));
		for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246));
		clicking = false;
	}
}
void saveFile::operator() (popup* p)
{
	if(replaceDialog)
	{
		if(((messageBox*)p)->getValue() == 0)
		{
			done = true;
		}
		else 
		{
			replaceDialog = false;
		}
	}
}
void saveFile::fileSelected()
{
	if(file.find(".") == file.npos)
		file += extFilter;
	if(exists(directory/file))//if file exists
	{
		messageBox* m = new messageBox;
		vector<string> s;
		s.push_back("yes");
		s.push_back("no");
		m->init(file + " already exists. Do you want to replace it?",s);
		m->callback = (functor<void,popup*>*)this;
		menuManager.setPopup(m);
		replaceDialog = true;
	}
	else
	{
		done = true;
	}
}

bool messageBox::init(string t)
{
	return init(t, vector<string>(1,"OK"));
}
bool messageBox::init(string t, vector<string> names)
{
	if(names.empty()) return init(t);//watch out for infinite loop!!

	width = max(textManager->getTextWidth(t)+40,715);
	height = max(textManager->getTextHeight(t)-170,0) + 295;
	x = (sw-width)/2;
	y = (sh-height)/2;
	if(x < 5) x = 5;
	if(y < 5) y = 5;

	labels["label"] = new label(x+(width-textManager->getTextWidth(t))/2, y+height*95/295-textManager->getTextHeight(t)/2, t, Color(1,1,1,0.8));

	float slotWidth = (685.0*width/715) / names.size();
	int slotNum = 0;
	for(vector<string>::iterator i = names.begin();i !=names.end(); i++, slotNum++)
	{
		options.push_back(new label(x + (15.0/715*width)+(0.5+slotNum)*slotWidth-textManager->getTextWidth(*i)/2,y + (235.0/295*height)-textManager->getTextHeight(*i)/2, *i, white));
	}
	value=-1;
	clicking=-1;
	return true;
}
void messageBox::render()
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
	for(vector<label*>::iterator i = options.begin(); i!=options.end();i++,slotNum++)
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
void messageBox::mouseL(bool down, int X, int Y)
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


int screen::backgroundImage=0;

bool screen::loadBackground()
{
	backgroundImage=dataManager.loadTexture("media/menu/menu background2.tga");
	return backgroundImage != 0;
}

bool levelEditor::init()
{
	vector<button*> v;//for toggles

	//terrain

	//buttons["newShader"]	= new button(5,5,200,30,"new shader",lightGreen,white);
	buttons["dSquare"]		= new button(sw-105,5,100,30,"d-square",lightGreen,white);
	buttons["faultLine"]	= new button(sw-105,40,100,30,"fault line",lightGreen,white);
	buttons["fromFile"]		= new button(sw-105,75,100,30,"from file",lightGreen,white);
	buttons["exportBMP"]	= new button(sw-105,110,100,30,"export",lightGreen,white);
	sliders["sea level"]	= new slider(sw-105,145,100,30,5000.0,0.0);

	buttons["load"]			= new button(sw-320,sh-40,100,35,"Load",Color(0.8,0.8,0.8),white);
	buttons["save"]			= new button(sw-215,sh-40,100,35,"Save",Color(0.8,0.8,0.8),white);
	buttons["exit"]			= new button(sw-110,sh-40,100,35,"Exit",Color(0.8,0.8,0.8),white);

	toggles["shaders"]		= new toggle(vector<button*>(),darkGreen,lightGreen,NULL,0);

	addShader("media/grass.frag");
	addShader("media/snow.frag");

	//objects
	buttons["addPlane"]		= new button(5,5,200,30,"new plane",lightGreen,white);
	buttons["addAAgun"]		= new button(5,40,200,30,"new AA gun",lightGreen,white);
	//settings

	v.clear();
	v.push_back(new button(120,5,100,30,"respawn",black,white));
	v.push_back(new button(225,5,100,30,"restart",black,white));
	v.push_back(new button(330,5,100,30,"die",black,white));
	toggles["onHit"]		= new toggle(v,darkBlue,lightBlue,new label(5,5,"player hit:"));

	v.clear();
	v.push_back(new button(120,45,100,30,"respawn",black,white));
	v.push_back(new button(225,45,100,30,"restart",black,white));
	v.push_back(new button(330,45,100,30,"die",black,white));
	toggles["onAIHit"]		= new toggle(v,darkBlue,lightBlue,new label(5,45,"AI hit:"));

	v.clear();
	v.push_back(new button(120,85,100,30,"ffa",black,white));
	v.push_back(new button(225,85,100,30,"teams",black,white));
	v.push_back(new button(330,85,100,30,"player vs",black,white));
	toggles["gameType"]	= new toggle(v,darkBlue,lightBlue,new label(5,85,"game type:"));

	v.clear();
	v.push_back(new button(120,125,100,30,"water",black,white));
	v.push_back(new button(225,125,100,30,"land",black,white));
	toggles["mapType"]	= new toggle(v,darkBlue,lightBlue,new label(5,125,"map type:"));

	v.clear();
	v.push_back(new button(120,165,100,30,"rock",black,white));
	v.push_back(new button(225,165,100,30,"sand",black,white));
	toggles["seaFloorType"]	= new toggle(v,darkBlue,lightBlue,new label(5,165,"sea floor:"));

	v.clear();
	v.push_back(new button(5,sh-40,100,35,"Terrain",black,white));
	v.push_back(new button(110,sh-40,100,35,"Objects",black,white));
	v.push_back(new button(215,sh-40,100,35,"Settings",black,white));
	toggles["tabs"]	= new toggle(v,Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),NULL,0);

	return true;
}
void levelEditor::operator() (popup* p)
{
	if(awaitingMapFile)
	{
		awaitingMapFile=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((openFile*)p)->getFile();
		((modeMapBuilder*)modeManager.getMode())->fromFile(f);
	}
	//else if(awaitingShaderFile)
	//{
	//	awaitingShaderFile=false;
	//	if(!((saveFile*)p)->validFile()) return;
	//	string f=((openFile*)p)->getFile();
	//	addShader(f);
	//}
	else if(awaitingMapSave)
	{
		awaitingMapSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		((modeMapBuilder*)modeManager.getMode())->level->exportBMP(f);
	}
	else if(awaitingLevelFile)
	{
		awaitingLevelFile=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((openFile*)p)->getFile();
		delete ((modeMapBuilder*)modeManager.getMode())->level;
		LevelFile l(f);
		((modeMapBuilder*)modeManager.getMode())->level = new editLevel(l);
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		LevelFile l = ((modeMapBuilder*)modeManager.getMode())->level->getLevelFile(sliders["sea level"]->getValue());
		l.save(f);
	}
	else if(awaitingNewObject)
	{
		awaitingNewObject = false;
		newObjectType=((newObject*)p)->getObjectType();
	}
	//else if(...)
	//   .
	//   .
	//   .
}
int levelEditor::update()
{
	if(buttons["load"]->checkChanged())
	{
		awaitingLevelFile = true;
		popup* p = new openFile;
		p->callback = (functor<void,popup*>*)this;
		((openFile*)p)->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(buttons["save"]->checkChanged())
	{
		awaitingLevelSave = true;
		popup* p = new saveFile;
		p->callback = (functor<void,popup*>*)this;
		((saveFile*)p)->init(".lvl");
		menuManager.setPopup(p);
	}
	else if(buttons["exit"]->checkChanged())
	{
		modeManager.setMode(NULL);
		menuManager.setMenu(new menu::chooseMode);
	}
	else if(getTab() == TERRAIN)
	{
		if(buttons["faultLine"]->checkChanged())
		{
			((modeMapBuilder*)modeManager.getMode())->faultLine();
		}
		else if(buttons["dSquare"]->checkChanged())
		{
			((modeMapBuilder*)modeManager.getMode())->diamondSquare(0.17,0.5);
		}
		else if(buttons["fromFile"]->checkChanged())
		{
			awaitingMapFile = true;
			popup* p = new openFile;
			p->callback = (functor<void,popup*>*)this;
			((openFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		else if(buttons["exportBMP"]->checkChanged())
		{
			awaitingMapSave = true;
			popup* p = new saveFile;
			p->callback = (functor<void,popup*>*)this;
			((saveFile*)p)->init(".bmp");
			menuManager.setPopup(p);
		}
		//else if(buttons["newShader"]->checkChanged())
		//{
		//	awaitingShaderFile=true;
		//	popup* p = new openFile;
		//	p->callback = (functor<void,popup*>*)this;
		//	((openFile*)p)->init(".frag");
		//	manager.setPopup(p);
		//}
	}
	else if(getTab() == OBJECTS)
	{
		if(buttons["addPlane"]->checkChanged())
		{
			awaitingNewObject=true;
			popup* p = new newObject;
			p->callback = (functor<void,popup*>*)this;
			menuManager.setPopup(p);
		}
		else if(buttons["addAAgun"]->checkChanged())
		{
			awaitingNewObject=true;
			popup* p = new newObject(AA_GUN);
			p->callback = (functor<void,popup*>*)this;
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
			//buttons["newShader"]->setVisibility(newTab==TERRAIN);
			buttons["dSquare"]->setVisibility(newTab==TERRAIN);
			buttons["faultLine"]->setVisibility(newTab==TERRAIN);
			buttons["fromFile"]->setVisibility(newTab==TERRAIN);
			buttons["exportBMP"]->setVisibility(newTab==TERRAIN);
			toggles["shaders"]->setVisibility(newTab==TERRAIN);
			sliders["sea level"]->setVisibility(newTab==TERRAIN);
		}
		if(lastTab == OBJECTS || newTab==OBJECTS || lastTab == (Tab)-1)
		{
			buttons["addPlane"]->setVisibility(newTab==OBJECTS);
			buttons["addAAgun"]->setVisibility(newTab==OBJECTS);
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
void levelEditor::render()
{
	menuManager.drawCursor();
}
void levelEditor::mouseL(bool down, int x, int y)
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
void levelEditor::scroll(float rotations)
{
	((modeMapBuilder*)modeManager.getMode())->zoom(rotations);
}
void levelEditor::mouseC(bool down, int x, int y)
{
	if(!down)
		((modeMapBuilder*)modeManager.getMode())->trackBallUpdate(x,y);
}
void levelEditor::addShader(string filename)
{
	int s = dataManager.loadTerrainShader(filename);//((mapBuilder*)mode)->loadShader("media/terrain.vert",(char*)(string("media/")+filename).c_str());
	((modeMapBuilder*)modeManager.getMode())->shaderButtons.push_back(s);

	toggles["shaders"]->addButton(new button(5,toggles["shaders"]->getSize()*35+5,200,30,filename,black,white));
	//buttons["newShader"]->setElementXY(5,toggles["shaders"]->getSize()*35+5);
}
int levelEditor::getShader()
{
	return toggles["shaders"]->getValue();
}
levelEditor::Tab levelEditor::getTab()
{
	int t =  toggles["tabs"]->getValue();
	if(t == 0) return TERRAIN;
	if(t == 1) return OBJECTS;
	if(t == 2) return SETTINGS;
	return NO_TAB;
}
void chooseMode::render()
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
void chooseMode::keyDown(int vkey)
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
		menuManager.setMenu(NULL);

		modeManager.setMode(new modeCampaign("media/map file.lvl"));
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MULTIPLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu(NULL);

		modeManager.setMode(new modeSplitScreen("media/map file.lvl"));
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MAP_EDITOR)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu(NULL);

		modeManager.setMode(new modeMapBuilder);
	}
}
bool inGame::init()
{
	activeChoice=RESUME;
	world.time.pause();
	return true;
}
void inGame::render()
{
	dataManager.bind("menu in game");
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(sw/2-77,sh/2-122);
		glTexCoord2f(1,0);	glVertex2f(sw/2+76,sh/2-122);
		glTexCoord2f(1,1);	glVertex2f(sw/2+76,sh/2+123);
		glTexCoord2f(0,1);	glVertex2f(sw/2-77,sh/2+123);
	glEnd();
	
	float y;
	if(activeChoice==RESUME)	y = sh/2-123 + 17;
	if(activeChoice==OPTIONS)	y = sh/2-122 + 92;
	if(activeChoice==QUIT)		y = sh/2-122 + 169;

	dataManager.bind("menu in game select");
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);	glVertex2f(sw/2-70,y+50);
		glTexCoord2f(0,1);	glVertex2f(sw/2+58,y+50);
		glTexCoord2f(0,0);	glVertex2f(sw/2+58,y);
		glTexCoord2f(1,0);	glVertex2f(sw/2-70,y);
	glEnd();

	dataManager.bindTex(0);
}
void inGame::keyDown(int vkey)
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

		modeManager.setMode(NULL);
		menuManager.setMenu(new menu::chooseMode);
	}
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
		dataManager.registerAsset("menu background", "media/menu/menu background.png");
		dataManager.registerAsset("progress back", "media/progress back.png");
		dataManager.registerAsset("progress front", "media/progress front.png");
		toLoad=false;
	}

	static int totalAssets = -1;
	int assetsLeft = dataManager.registerAssets();
	if(totalAssets == -1) totalAssets = assetsLeft+1;
	progress = 1.0-(float)assetsLeft/totalAssets;
	if(assetsLeft==0)	menuManager.setMenu(new menu::chooseMode);

	return 30;
}
void loading::render()
{
	graphics->drawOverlay(Vec2f(0,0),Vec2f(sw,sh),"menu background");
	graphics->drawOverlay(Vec2f(sw*0.05,sh*0.96),Vec2f(sw*0.9,sh*0.02),"progress back");
	if(dataManager.getId("progress front") != 0)
		graphics->drawOverlay(Vec2f(sw*0.05,sh*0.96),Vec2f(sw*0.9*progress,sh*0.02),"progress front");
	else
	{
		glColor3f(0,1,0);
		graphics->drawOverlay(Vec2f(sw*0.05,sh*0.96),Vec2f(sw*0.9*progress,sh*0.02));
	}
}
void button::setElementText(string t)
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
void button::setElementTextColor(Color c)
{
	textColor = c;
}
void button::setElementXYWH(int X, int Y, int Width, int Height)
{
	x=X;
	y=Y;
	width=Width;
	height=Height;
	setElementText(text);//update clampedText
}
void button::render()
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
void button::mouseDownL(int X, int Y)
{
	if(x < X && y < Y && x+width > X && y+height > Y)
		clicking = true;
}
void button::mouseUpL(int X, int Y)
{
	if(clicking && x < X && y < Y && x+width > X && y+height > Y)
		changed = true;
	clicking = false;
}
void checkBox::render()
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
void checkBox::mouseDownL(int X, int Y)
{
	if((x < X && y < Y && x+30 > X && y+25 > Y) || (x+30 < X && y < Y && x+width > X && y+height > Y))
		clicking = true;
}
void checkBox::mouseUpL(int X, int Y)
{
	if(clicking && ((x < X && y < Y && x+30 > X && y+25 > Y) || (x+30 < X && y < Y && x+width > X && y+height > Y)) )
	{
		changed = true;
		checked = !checked;
	}
	clicking = false;
}

void label::render()
{
	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,x,y);
}
toggle::toggle(vector<button*> b, Color clickedC, Color unclickedC, label* l, int startValue): element(TOGGLE,0,0), value(startValue), Label(l)
{
	buttons=b;
	if(startValue >= 0 && startValue < buttons.size())
		value=startValue;
	else 
		value=-1;
	clicked=clickedC;
	unclicked=unclickedC;
	
	int n=0;
	for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
	{
		if(value==n)
			(*i)->setElementColor(clicked);
		else
			(*i)->setElementColor(unclicked);
	}
}
int toggle::addButton(button* button)
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
void toggle::render()
{
	for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++)
	{
		(*i)->render();
	}
	if(Label != NULL) Label->render();
}
void toggle::mouseDownL(int X, int Y)
{
	for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++)
	{
		(*i)->mouseDownL(X,Y);
	}
}
void toggle::mouseUpL(int X, int Y)
{
	int n=0;
	for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
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
void toggle::updateColors()
{
	int n=0;
	for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
	{
		if(value==n)	(*i)->setElementColor(clicked);
		else			(*i)->setElementColor(unclicked);
	}
}
void slider::render()
{
	if(clicking)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		value = clamp((maxValue - minValue) * (cursorPos.x - x) / width + minValue + mouseOffset, minValue, maxValue);
	}
	glColor4f(1,1,1,1);
	dataManager.bind("slider bar");
	glBegin(GL_QUADS);
		glTexCoord2f(0,	0);	glVertex2f(x,		0.5*height+y-11);		
		glTexCoord2f(1,	0);	glVertex2f(x+width,	0.5*height+y-11);		
		glTexCoord2f(1,	1);	glVertex2f(x+width,	0.5*height+y+11);	
		glTexCoord2f(0,	1);	glVertex2f(x,		0.5*height+y+11);	
	glEnd();
	dataManager.bind("slider");
	glBegin(GL_QUADS);
		glTexCoord2f(0,	0);	glVertex2f((value-minValue)*width/(maxValue - minValue) + x - 22	, y);
		glTexCoord2f(1,	0);	glVertex2f((value-minValue)*width/(maxValue - minValue) + x + 22	, y);
		glTexCoord2f(1,	1);	glVertex2f((value-minValue)*width/(maxValue - minValue) + x + 22	, y + height);	
		glTexCoord2f(0,	1);	glVertex2f((value-minValue)*width/(maxValue - minValue) + x - 22	, y + height);
	glEnd();
	dataManager.bindTex(0);
}
void slider::mouseDownL(int X, int Y)
{
	if(abs(value*width/(maxValue - minValue) + x  - X) < 22 && Y > y && Y < y + height)	
	{
		clicking = true;
		mouseOffset = value - ((maxValue - minValue) * (X - x) / width + minValue);
	}
	else if(X > x && X < x + width && Y > y && Y < y + height)
	{
		clicking = true;
		mouseOffset = 0.0f;
	}
}
void slider::mouseUpL(int X, int Y)
{
	if(clicking)
	{
		value = clamp((maxValue - minValue) * (X - x) / width + minValue + mouseOffset, minValue, maxValue);
		clicking = false;
	}
}
float slider::getValue()
{
	if(clicking)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		value = clamp((maxValue - minValue) * (cursorPos.x - x) / width + minValue + mouseOffset, minValue, maxValue);
	}
	return value;
}
void slider::setMinValue(float m)
{
	if(clicking)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		value = clamp((maxValue - minValue) * (cursorPos.x - x) / width + minValue + mouseOffset, minValue, maxValue);
		clicking = false;
	}
	minValue = m;
	if(value < minValue)
		value = minValue;
}
void slider::setMaxValue(float m)
{
	if(clicking)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		value = clamp((maxValue - minValue) * (cursorPos.x - x) / width + minValue + mouseOffset, minValue, maxValue);
		clicking = false;
	}
	maxValue = m;
	if(value > maxValue)
		value = maxValue;
}
}
void messageBox(string text)
{
	menu::messageBox* m = new menu::messageBox;
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
		struct exitor: public functor<void,menu::popup*>
		{
			void operator() (menu::popup*){exit(0);}
		};
		menu::messageBox* m = new menu::messageBox;
		m->init(text);
		m->callback = (functor<void,menu::popup*>*)(new exitor);
		menuManager.setPopup(m);
	}
}