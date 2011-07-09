
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
		for(auto e = menu->checkBoxes.begin(); e != menu->checkBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = menu->textBoxes.begin(); e != menu->textBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = menu->listBoxes.begin(); e != menu->listBoxes.end(); e++)
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
		for(auto e = (*i)->checkBoxes.begin(); e != (*i)->checkBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = (*i)->textBoxes.begin(); e != (*i)->textBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
		for(auto e = (*i)->listBoxes.begin(); e != (*i)->listBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility())
				e->second->render();
		}
	}
	if(mDrawCursor)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		if(dataManager.getId("cursor") != 0)
			graphics->drawOverlay(cursorPos.x,cursorPos.y,cursorPos.x+21,cursorPos.y+25,"cursor");
		else
		{
			glColor3f(0,0,0);
			glBegin(GL_TRIANGLES);
				glVertex2f(cursorPos.x,cursorPos.y);
				glVertex2f(cursorPos.x,cursorPos.y+25);
				glVertex2f(cursorPos.x+12,cursorPos.y+22);
			glEnd();
			glColor3f(1,1,1);
		}
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

	if(m != NULL)
		m->init();
}
void manager::issueInputCallback(Input::callBack* callback, element* e)
{
	if(e == NULL)
		return;

	if(callback->type == KEY_STROKE)
	{
		Input::keyStroke* call = (Input::keyStroke*)callback;
		if(call->up)	e->keyUp(call->vkey);
		else			e->keyDown(call->vkey);
	}
	else if(callback->type == MOUSE_CLICK)
	{
		Input::mouseClick* call = (Input::mouseClick*)callback;
		if(call->button == LEFT_BUTTON && call->down)		e->mouseDownL(call->x,call->y);
		else if(call->button == LEFT_BUTTON && !call->down)	e->mouseUpL(call->x,call->y);
		else if(call->button == RIGHT_BUTTON && call->down)	e->mouseDownR(call->x,call->y);
		else if(call->button == RIGHT_BUTTON && !call->down)e->mouseUpR(call->x,call->y);
	}
}
void manager::inputCallback(Input::callBack* callback)
{
	if(callback->type == KEY_STROKE)
	{
		Input::keyStroke* call = (Input::keyStroke*)callback;
		if(!popups.empty())
		{
			int pSize = popups.size();
			if(!call->up)	popups.back()->keyDown(call->vkey);
			else			popups.back()->keyUp(call->vkey);

			if(pSize==popups.size())	for(auto e = popups.back()->buttons.begin(); pSize==popups.size() && e != popups.back()->buttons.end(); e++)	issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->toggles.begin(); pSize==popups.size() && e != popups.back()->toggles.end(); e++)	issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->textBoxes.begin(); pSize==popups.size() && e != popups.back()->textBoxes.end(); e++)	issueInputCallback(callback,e->second);
		}
		else if(menu!=NULL)
		{
			if(!call->up)	menu->keyDown(call->vkey);
			else			menu->keyUp(call->vkey);

			if(menu)	for(auto e = menu->buttons.begin(); menu!=NULL && e != menu->buttons.end(); e++)		issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->toggles.begin(); menu!=NULL && e != menu->toggles.end(); e++)		issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->textBoxes.begin(); menu!=NULL && e != menu->textBoxes.end(); e++)	issueInputCallback(callback,e->second);
		}
	}
	else if(callback->type == MOUSE_CLICK)
	{
		Input::mouseClick* call = (Input::mouseClick*)callback;
		
		if(!popups.empty())
		{
			int pSize = popups.size();
			if(call->button == LEFT_BUTTON)			popups.back()->mouseL(call->down,call->x,call->y);
			else if(call->button == RIGHT_BUTTON)	popups.back()->mouseR(call->down,call->x,call->y);
			
			if(pSize==popups.size())	for(auto e = popups.back()->buttons.begin(); pSize==popups.size() && e != popups.back()->buttons.end(); e++)		issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->toggles.begin(); pSize==popups.size() && e != popups.back()->toggles.end(); e++)		issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->sliders.begin(); pSize==popups.size() && e != popups.back()->sliders.end(); e++)		issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->checkBoxes.begin(); pSize==popups.size() && e != popups.back()->checkBoxes.end(); e++)	issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->textBoxes.begin(); pSize==popups.size() && e != popups.back()->textBoxes.end(); e++)	issueInputCallback(callback,e->second);
			if(pSize==popups.size())	for(auto e = popups.back()->listBoxes.begin(); pSize==popups.size() && e != popups.back()->listBoxes.end(); e++)	issueInputCallback(callback,e->second);
		}
		else if(menu!=NULL)
		{
			if(call->button == LEFT_BUTTON)				menu->mouseL(call->down,call->x,call->y);
			else if(call->button == MIDDLE_BUTTON)		menu->mouseC(call->down,call->x,call->y);
			else if(call->button == RIGHT_BUTTON)		menu->mouseR(call->down,call->x,call->y);

			if(menu)	for(auto e = menu->buttons.begin(); menu!=NULL && e != menu->buttons.end(); e++)issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->toggles.begin(); menu!=NULL && e != menu->toggles.end(); e++)issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->sliders.begin(); menu!=NULL && e != menu->sliders.end(); e++)issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->checkBoxes.begin(); menu!=NULL && e != menu->checkBoxes.end(); e++)issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->textBoxes.begin(); menu!=NULL && e != menu->textBoxes.end(); e++)issueInputCallback(callback,e->second);
			if(menu)	for(auto e = menu->listBoxes.begin(); menu!=NULL && e != menu->listBoxes.end(); e++)issueInputCallback(callback,e->second);
		}

	}
	else if(callback->type == MOUSE_SCROLL)
	{
		Input::mouseScroll* call = (Input::mouseScroll*)callback;
		if(!popups.empty())		popups.back()->scroll(call->rotations);
		else if(menu!=NULL)		menu->scroll(call->rotations);
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
		if ( is_directory(itr->status()) && itr->path().generic_string().compare(".svn") != 0)
		{
			folders.push_back(itr->path().leaf().generic_string());
		}
		else if ( filesystem::extension(itr->path()).compare(extFilter) == 0 ) // see below
		{
			files.push_back(itr->path().leaf().generic_string());
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
	graphics->drawOverlay(sw/2-420,sh/2-261,sw/2+421,sh/2+262,"file viewer");
	graphics->drawOverlay(sw/2-180,sh/2+189,sw/2+190,sh/2+235,"entry bar");

	glColor3f(0,0,0);
	textManager->renderText(file,sw/2-170,sh/2+212-textManager->getTextHeight(file)/2);
	glPushMatrix();
	glTranslatef((sw/2-190),(sh/2-246),0);
	for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		(*i)->render();
	for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			(*i)->render();
	glPopMatrix();
	menuManager.drawCursor();
	glColor3f(1,1,1);
}
void openFile::fileSelected()
{
	string e = extension(file);
	if(e != extFilter)
	{
		file += extFilter;
	}
	if(!exists(directory/file))
		file = "";

	done = true;
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
		if(((messageBox_c*)p)->getValue() == 0)
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
	if(file == "")
	{
		done = true;
		return;
	}

	if(file.find(".") == file.npos)
		file += extFilter;
	if(exists(directory/file))//if file exists
	{
		messageBox_c* m = new messageBox_c;
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

bool messageBox_c::init(string t)
{
	return init(t, vector<string>(1,"OK"));
}
bool messageBox_c::init(string t, vector<string> names)
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
void messageBox_c::render()
{
	if(dataManager.getId("dialog box") != 0)
		graphics->drawOverlay(x,y,x+width,y+height,"dialog box");
	else
	{
		dataManager.bind("noTexture");
		glColor3f(0.3,0.3,0.3);
		graphics->drawOverlay(x,y,x+width,y+height);
		glColor3f(1,1,1);
	}

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
			graphics->drawOverlay(	startX+slotWidth*(0.5+slotNum)-textManager->getTextWidth((*i)->getText())*0.75-20,		startY+slotHeight*0.5-35,						
									startX+slotWidth*(0.5+slotNum)+textManager->getTextWidth((*i)->getText())*0.75+20,		startY+slotHeight*0.5+35,   "glow");
		}
		(*i)->render();
	}
	menuManager.drawCursor();
}
void messageBox_c::mouseL(bool down, int X, int Y)
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
	glColor4f(1,1,1,0.2);
	graphics->drawOverlay(sw/2-400,sh/2-250,sw/2+400,sh/2+250,"white");
	glColor3f(1,1,1);
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
	sliders["sea level"]	= new slider(sw-105,145,100,30,1.0,0.0);
	sliders["height scale"] = new slider(sw-105,165,100,30,1.0,-1.0);	sliders["height scale"]->setValue(0.0);

	buttons["load"]			= new button(sw-320,sh-40,100,35,"Load",Color(0.8,0.8,0.8),white);
	buttons["save"]			= new button(sw-215,sh-40,100,35,"Save",Color(0.8,0.8,0.8),white);
	buttons["exit"]			= new button(sw-110,sh-40,100,35,"Exit",Color(0.8,0.8,0.8),white);

	toggles["shaders"]		= new toggle(vector<button*>(),darkGreen,lightGreen,NULL,0);
	addShader("island");
	addShader("grass");
	addShader("snow");
	addShader("ocean");

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
		mode->level = new editLevel(l);
		mode->maxHeight=mode->level->ground()->getMaxHeight();
		mode->minHeight=mode->level->ground()->getMinHeight();
		sliders["sea level"]->setValue(-mode->minHeight/(mode->maxHeight - mode->minHeight));
		mode->resetView();
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		LevelFile l = ((modeMapBuilder*)modeManager.getMode())->level->getLevelFile(sliders["sea level"]->getValue() * (mode->maxHeight - mode->minHeight) + mode->minHeight);
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
		return 0;
	}
	else if(getTab() == TERRAIN)
	{
		if(buttons["faultLine"]->checkChanged())
		{
			((modeMapBuilder*)modeManager.getMode())->faultLine();
		}
		else if(buttons["dSquare"]->checkChanged())
		{
			((modeMapBuilder*)modeManager.getMode())->diamondSquare(0.17,0.5,4);
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
			sliders["height scale"]->setVisibility(newTab==TERRAIN);
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
	if(getTab() == OBJECTS && down)
	{
		if(newObjectType != 0)
		{
			static int teamNum=0;
			((modeMapBuilder*)modeManager.getMode())->addObject(newObjectType, teamNum, teamNum<=1 ? CONTROL_HUMAN : CONTROL_COMPUTER, x, y);
			newObjectType = 0;
			teamNum++;
		}
		else
		{
			mode->selectObject(x,y);
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
	//int s = dataManager.loadTerrainShader(filename);//((mapBuilder*)mode)->loadShader("media/terrain.vert",(char*)(string("media/")+filename).c_str());
	//((modeMapBuilder*)modeManager.getMode())->shaderButtons.push_back(s);

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

	graphics->drawOverlay(Vec2f(0,0),Vec2f(sw,sh),"menu background");
	float sx = (float)sw/800;
	float sy = (float)sh/600;
	graphics->drawOverlay(Vec2f((-327+420)*sx,304*sy),Vec2f(722*sx,419*sy),"menu pictures");
	for(int i=1;i<=5;i++)
	{
		if(i!=activeChoice+2)
		{
			graphics->drawOverlay(Vec2f((-327+i*210)*sx,300*sy),Vec2f((-118+i*210)*sx,450*sy),"menu slot");
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
#ifdef _DEBUG 
	if((vkey==VK_SPACE || vkey==VK_RETURN) && input->getKey(VK_CONTROL) && (activeChoice==SINGLE_PLAYER || activeChoice==MULTIPLAYER))//if the alt key is pressed
	{
		openFile* p = new openFile;
		p->callback = (functor<void,popup*>*)this;
		p->init(".lvl");
		menuManager.setPopup(p);
		choosingFile = true;
	}
	else
#endif
	
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
void chooseMode::operator() (popup* p)
{
	if(choosingFile && ((openFile*)p)->validFile())
	{
		if(activeChoice==SINGLE_PLAYER)
		{
			input->up(VK_SPACE);
			input->up(VK_RETURN);
			menuManager.setMenu(NULL);

			modeManager.setMode(new modeCampaign(((openFile*)p)->getFile()));
			return;
		}
		else if(activeChoice==MULTIPLAYER)
		{
			input->up(VK_SPACE);
			input->up(VK_RETURN);
			menuManager.setMenu(NULL);

			modeManager.setMode(new modeSplitScreen(((openFile*)p)->getFile()));
			return;
		}
		choosingFile = false;
	}
}
bool chooseMap::init()
{
	directory_iterator end_itr; // default construction yields past-the-end
	for(directory_iterator itr("./media/"); itr != end_itr; ++itr)
	{
		if (extension(itr->path()) == ".lvl")
		{
			mapChoices.push_back(itr->path().leaf().generic_string());
		}
	}
	currentChoice = 0;
	return true;
}
void chooseMap::render()
{
	graphics->drawOverlay(Vec2f(0,0),Vec2f(sw,sh),"menu background");
}
void chooseMap::keyDown(int vkey)
{
	if(mapChoices.empty())
		return;

	if(vkey==VK_ESCAPE)
	{
		input->up(VK_ESCAPE);
		menuManager.setMenu(new chooseMode);
	}
	if(vkey==VK_UP)		currentChoice--;
	if(vkey==VK_DOWN)	currentChoice++;

	if(currentChoice<0)	
		currentChoice=max(mapChoices.size()-1,0);
	if(currentChoice>=mapChoices.size())	
		currentChoice=0;

}
bool inGame::init()
{
	activeChoice=RESUME;
	world.time.pause();
	return true;
}
void inGame::render()
{
	graphics->drawOverlay(sw/2-77,sh/2-122,sw/2+76,sh/2+123,"menu in game");
	
	if(activeChoice==RESUME)	graphics->drawOverlay(sw/2-70,sh/2-122+16,sw/2+58,sh/2-122+66,"menu in game select");
	if(activeChoice==OPTIONS)	graphics->drawOverlay(sw/2-70,sh/2-122+92,sw/2+58,sh/2-122+142,"menu in game select");
	if(activeChoice==QUIT)		graphics->drawOverlay(sw/2-70,sh/2-122+169,sw/2+58,sh/2-122+219,"menu in game select");
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
		dataManager.registerShader("ortho", "media/ortho.vert", "media/ortho.frag");
		toLoad=false;
	}

	static int totalAssets = -1;
	int assetsLeft = dataManager.registerAssets();
	if(totalAssets == -1) totalAssets = assetsLeft+1;

	//graphics->minimizeWindow();
	//MessageBox(NULL,(wstring(L"loaded asset #") + lexical_cast<wstring>(totalAssets - assetsLeft)).c_str(),L"",0);
	progress = 1.0-(float)assetsLeft/totalAssets;
	if(assetsLeft==0)
	{
		menuManager.setMenu(new menu::chooseMode);
	}
	return 30;
}
void loading::render()
{
	//static int n = 0;	n++;
	//if(n <= 1) return;
	dataManager.bind("ortho");
	graphics->drawOverlay(-1.0,1.0,1.0,-1.0,"menu background");
	graphics->drawOverlay(-0.9,-0.92,0.9,-0.96,"progress back");

	if(dataManager.getId("progress front") != 0)
	{
		graphics->drawOverlay(-0.9,-0.92,1.8*progress-0.9,-0.96,"progress front");
	}
	else
	{
		glColor3f(0,1,0);
		graphics->drawOverlay(-0.9,-0.92,1.8*progress-0.9,-0.96);
		glColor3f(1,1,1);
	}
	dataManager.unbindShader();
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
	Vec2f tPos,tSize;
	Vec2f rPos,rSize;
	for(int ix=0;ix<3;ix++)
	{
		for(int iy=0;iy<3;iy++)
		{
			if(ix==0)
			{
				tPos.x=0.0;
				tSize.x=10.0/140;
				rPos.x=x;
				rSize.x=min(10,width/2);
			}
			else if(ix==1)
			{
				tPos.x=10.0/140;
				tSize.x=1.0-20.0/140;
				rPos.x=x+10;
				rSize.x=max(width-20,0);
			}
			else if(ix==2)
			{
				tPos.x=1.0-10.0/140;
				tSize.x=10.0/140;
				rPos.x=x+width-min(10,width/2);
				rSize.x=min(10,width/2);
			}
			if(iy==0)
			{
				tPos.y=0.0;
				tSize.y=10.0/45;
				rPos.y=y;
				rSize.y=min(10,height/2);
			}
			else if(iy==1)
			{
				tPos.y=10.0/45;
				tSize.y=1.0-20.0/45;
				rPos.y=y+10;
				rSize.y=max(height-20,0);
			}
			else if(iy==2)
			{
				tPos.y=1.0-10.0/45;
				tSize.y=10.0/45;
				rPos.y=y+height-min(10,height/2);
				rSize.y=min(10,height/2);
			}
			graphics->drawPartialOverlay(rPos,rSize,tPos,tSize,"button");
		}
	}
	dataManager.unbindTextures();
	glColor4f(textColor.r,textColor.g,textColor.b,textColor.a);
	textManager->renderText(clampedText,x+(width-textManager->getTextWidth(clampedText))/2,y+(height-textManager->getTextHeight(clampedText))/2);
	glColor3f(1,1,1);
}
void button::mouseDownL(int X, int Y)
{
	if(view && active && x < X && y < Y && x+width > X && y+height > Y)
		clicking = true;
}
void button::mouseUpL(int X, int Y)
{
	if(view && active && clicking && x < X && y < Y && x+width > X && y+height > Y)
		changed = true;
	clicking = false;
}
void checkBox::render()
{
	graphics->drawOverlay(x,y,x+26,y+26,"check box");
	if(checked)
		graphics->drawOverlay(x,y,x+26,y+26,"check");

	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,x+30,y);
	glColor3f(1,1,1);
}
void checkBox::mouseDownL(int X, int Y)
{
	if(view && active && ((x < X && y < Y && x+30 > X && y+25 > Y) || (x+30 < X && y < Y && x+width > X && y+height > Y)))
		clicking = true;
}
void checkBox::mouseUpL(int X, int Y)
{
	if(view && active && clicking && ((x < X && y < Y && x+30 > X && y+25 > Y) || (x+30 < X && y < Y && x+width > X && y+height > Y)) )
	{
		changed = true;
		checked = !checked;
	}
	clicking = false;
}
void textBox::mouseDownL(int X, int Y)
{
	clicking = (x <= X && x+width >= X && y <= Y && y+height >= Y);
	if(!clicking && focus)
	{
		loseFocus();
	}
}
void textBox::mouseUpL(int X, int Y)
{
	if(clicking)
	{
		bool f = (x <= X && x+width >= X && y <= Y && y+height >= Y);
		if(f && !focus) gainFocus();
		if(!f && focus) loseFocus();
		cursorPos = text.length();
		clicking = false;
	}
}
void textBox::render()
{
	if(focus)	glColor3f(0,0,0);
	else	glColor3f(0.3,0.3,0.3);
	graphics->drawOverlay(x-1,y-1,x+width+1,y+height+1,"white");

	if(focus)	glColor3f(0.6,0.6,0.6);
	else	glColor3f(0.8,0.8,0.8);
	graphics->drawOverlay(x,y,x+width,y+height,"white");

	glColor3f(color.r,color.g,color.b);
	if(focus)
	{
		
		float w = textManager->getTextWidth(text.substr(0,cursorPos));
		float h = textManager->getTextHeight(text);

		textManager->renderText(text,x+2,y);
		if(int(floor(world.time()/500)) % 2 == 1)
		{
			glBegin(GL_LINES);
			glVertex2f(x+w,y+2);
			glVertex2f(x+w,y+h);
			glEnd();
		}
	}
	else
	{
		textManager->renderText(text,x+2,y);
	}

	glColor3f(1,1,1);
}
void textBox::addChar(char c)
{
	text.insert(text.begin()+cursorPos,c);
	cursorPos++;
}
void textBox::keyDown(int vkey)
{
	if(focus)
	{
		if(0x60 <= vkey && 0x69 >= vkey)// numpad
		{
			addChar(vkey - 0x30);
		}
		else if(vkey == 0x20)// space
		{
			addChar(' ');
		}
		else if(vkey == 0x6e)
		{
			addChar('.');
		}
		else if(vkey == 0x08 && cursorPos > 0)// backspace
		{
			cursorPos--;
			text.erase(text.begin()+cursorPos);
		}
		else if(vkey == 0x2e && cursorPos < text.length())// delete
		{
			text.erase(text.begin()+cursorPos);
		}
		else if(vkey == 0x25 && cursorPos > 0)
		{
			cursorPos--;
		}
		else if(vkey == 0x27 && cursorPos < text.length())
		{
			cursorPos++;
		}


		else if(input->getKey(VK_SHIFT))
		{
			if(0x41 <= vkey && 0x5a >= vkey)	addChar(vkey);
			if(vkey == 0xba)					addChar(':');
			if(vkey == 0xbc)					addChar('<');
			if(vkey == 0xbe)					addChar('>');
			if(vkey == 0xbf)					addChar('?');
			if(vkey == 0xdb)					addChar('{');
			if(vkey == 0xdc)					addChar('|');
			if(vkey == 0xdd)					addChar('}');
			if(vkey == 0xde)					addChar('"');
		}
		else
		{
			if(0x41 <= vkey && 0x5a >= vkey)	addChar(vkey+0x20);
			if(0x30 <= vkey && 0x39 >= vkey)	addChar(vkey);
			if(vkey == 0xba)					addChar(';');
			if(vkey == 0xbc)					addChar(',');
			if(vkey == 0xbe)					addChar('.');
			if(vkey == 0xbf)					addChar('/');
			if(vkey == 0xdb)					addChar('[');
			if(vkey == 0xdc)					addChar('\\');
			if(vkey == 0xdd)					addChar(']');
			if(vkey == 0xde)					addChar('\'');
		}
	}
}
void numericTextBox::addChar(char c)
{
	if(c >= '0' && c <= '9' || c=='.')
	{
		text.insert(text.begin()+cursorPos,c);
		cursorPos++;
	}
}
void numericTextBox::loseFocus()
{
	try{
		float f = lexical_cast<float>(text);
		if(useMinMaxStep)
		{
			f = max(f,minVal);
			f = min(f,maxVal);
			f = floor(f / stepVal + 0.5) * stepVal;
		}
		if(!mAllowDecimal)
		{
			f = floor(f + 0.5f);
		}
		text = lexical_cast<string>(f);
	}catch(...){
		text = lexical_cast<string>(lastVal);
	}
}
void numericTextBox::gainFocus()
{
	try{
		lastVal = lexical_cast<float>(text);
	}catch(...){
		lastVal = 0.0f;
	}
}
void listBox::addOption(string option)
{
	if(option == text)
		optionNum = options.size();

	options.push_back(option);
}
void listBox::mouseDownL(int X, int Y)
{
	if(focus)
	{
		choosing = (x <= X && x+width >= X && y <= Y && y+height+30*options.size() >= Y);
		if(!choosing) focus = false;
	}
	else
	{
		clicking = (x <= X && x+width >= X && y <= Y && y+height >= Y);
	}
}
void listBox::mouseUpL(int X, int Y)
{
	if(clicking)
	{
		focus = (x <= X && x+width >= X && y <= Y && y+height >= Y);
		clicking = false;
	}
	else if(choosing)
	{
		if(x <= X && x+width >= X && y+30 <= Y && y+height+30*options.size() >= Y)
		{
			int n = (Y-(y+height)) / 30;
			text = options[n];
			optionNum = n;
		}
		else
		{
			focus = false;
		}
		choosing = false;
	}
}
void listBox::render()
{
	glColor3f(0.3,0.3,0.3);
	graphics->drawOverlay(x-1,y-1,x+width+1,y+height+1,"white");

	if(focus)
	{
		glColor3f(0,0,0);
		graphics->drawOverlay(x-1,y+height+2,x+width+1,y+height+2+30*options.size(),"white");
	}

	if(focus)	glColor3f(0.6,0.6,0.6);
	else	glColor3f(0.8,0.8,0.8);
	graphics->drawOverlay(x,y,x+width,y+height,"white");

	if(focus)
	graphics->drawOverlay(x,y+height+3,x+width,y+height+1+30*options.size(),"white");


	if(focus)	glColor3f(0.3,0.3,0.3);
	else	glColor3f(0.5,0.5,0.5);
	graphics->drawOverlay(x+width-height,y,x+width,y+height,"white");

	glColor3f(0,0,0);
	glBegin(GL_TRIANGLES);
		glVertex2f(x+width-height * 0.666,	y + height * 0.333);
		glVertex2f(x+width-height * 0.333,	y + height * 0.333);
		glVertex2f(x+width-height * 0.5,	y + height * 0.666);
	glEnd();

	glColor3f(color.r,color.g,color.b);
	//if(focus)
	//{
	//	
	//	float w = textManager->getTextWidth(text.substr(0,cursorPos));
	//	float h = textManager->getTextHeight(text);

	//	textManager->renderText(text,x,y);
	//	if(int(floor(world.time()/500)) % 2 == 1)
	//	{
	//		glBegin(GL_LINES);
	//		glVertex2f(x+w,y+2);
	//		glVertex2f(x+w,y+h);
	//		glEnd();
	//	}
	//}
	//else
	//{
	//	textManager->renderText(text,x,y);
	//}
	textManager->renderText(text,x+2,y);
	if(focus)
	{
		float h = y + height + 1;
		for(auto i = options.begin(); i!= options.end(); i++)
		{
			textManager->renderText(*i,x+2,h);
			h += 30;
		}
	}

	if(focus && optionNum != -1)
	{
		glColor4f(0.3,0.3,0.3,0.3);
		graphics->drawOverlay(x,y+height+3+30*optionNum,x+width,y+height+1+30*(optionNum+1),"white");
	}

	glColor3f(1,1,1);
}
void label::render()
{
	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,x,y);
	glColor3f(1,1,1);
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
	if(view && active)
	{
		for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++)
		{
			(*i)->mouseDownL(X,Y);
		}
	}
}
void toggle::mouseUpL(int X, int Y)
{
	if(view && active)
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
	graphics->drawOverlay(x,0.5*height+y-11,x+width,0.5*height+y+11,"slider bar");

	float xv = (value-minValue)*width/(maxValue-minValue)+x;
	graphics->drawOverlay(xv-22,y,xv+22,y+height,"slider");
}
void slider::mouseDownL(int X, int Y)
{
	if(view && active && abs(value*width/(maxValue - minValue) + x  - X) < 22 && Y > y && Y < y + height)	
	{
		clicking = true;
		mouseOffset = value - ((maxValue - minValue) * (X - x) / width + minValue);
	}
	else if(view && active && X > x && X < x + width && Y > y && Y < y + height)
	{
		clicking = true;
		mouseOffset = 0.0f;
	}
}
void slider::mouseUpL(int X, int Y)
{
	if(view && active && clicking)
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
	menu::messageBox_c* m = new menu::messageBox_c;
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
		menu::messageBox_c* m = new menu::messageBox_c;
		m->init(text);
		m->callback = (functor<void,menu::popup*>*)(new exitor);
		menuManager.setPopup(m);
	}
}