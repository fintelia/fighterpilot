
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
		if(dataManager.assetLoaded("cursor"))
			graphics->drawOverlay(Rect::XYWH(cursorPos.x,cursorPos.y,21,25),"cursor");
		else
		{
			glColor3f(0,0,0);
			graphics->drawTriangle(Vec3f(cursorPos.x,cursorPos.y,0),Vec3f(cursorPos.x,cursorPos.y+25,0),Vec3f(cursorPos.x+12,cursorPos.y+22,0));
			glColor3f(1,1,1);
		}
	}
}
void manager::render3D()
{
	if(menu != NULL)
	{
		menu->render3D();
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

void manager::inputCallback(Input::callBack* callback)
{
	if(callback->type == KEY_STROKE)
	{
		Input::keyStroke* call = (Input::keyStroke*)callback;
		if(!popups.empty())
		{
			int pSize = popups.size();
			if(!call->up && popups.back()->keyDown(call->vkey)) return;
			if(call->up && popups.back()->keyUp(call->vkey)) return;

			debugAssert(popups.size() == pSize);

			popups.back()->inputCallback(call);
		}
		else if(menu!=NULL)
		{
			if(!call->up && menu->keyDown(call->vkey))return;
			if(call->up && menu->keyUp(call->vkey))return;

			debugAssert(menu != NULL);

			menu->inputCallback(call);
		}
	}
	else if(callback->type == MOUSE_CLICK)
	{
		Input::mouseClick* call = (Input::mouseClick*)callback;
		
		if(!popups.empty())
		{
			call->pos.x *= sh;
			call->pos.y = (1.0 - call->pos.y) * sh;

			int pSize = popups.size();
			if(call->button == LEFT_BUTTON && popups.back()->mouseL(call->down,call->pos.x,call->pos.y)) return;
			if(call->button == RIGHT_BUTTON && popups.back()->mouseR(call->down,call->pos.x,call->pos.y)) return;
			
			debugAssert(popups.size() == pSize);

			popups.back()->inputCallback(call);
		}
		else if(menu!=NULL)
		{
			if(menu->mouse(call->button,call->down))
				return;

			call->pos.x *= sh;
			call->pos.y = (1.0 - call->pos.y) * sh;

			debugAssert(menu != NULL);

			menu->inputCallback(call);
		}

	}
	else if(callback->type == MOUSE_SCROLL)
	{
		Input::mouseScroll* call = (Input::mouseScroll*)callback;
		if(!popups.empty() && popups.back()->scroll(call->rotations)) return;
		if(menu!=NULL && menu->scroll(call->rotations)) return;
	}
}
bool elementContainer::issueInputCallback(Input::callBack* callback, element* e)
{
	if(e == NULL)
		return false;

	if(callback->type == KEY_STROKE)
	{
		Input::keyStroke* call = (Input::keyStroke*)callback;
		if(call->up)	return e->keyUp(call->vkey);
		else			return e->keyDown(call->vkey);
	}
	else if(callback->type == MOUSE_CLICK)
	{
		Input::mouseClick* call = (Input::mouseClick*)callback;

		if(call->button == LEFT_BUTTON && call->down)
		{
			if(!e->hasFocus())	e->gainFocus();
			return e->mouseDownL(call->pos.x,call->pos.y);
		}
		else if(call->button == LEFT_BUTTON && !call->down)	return e->mouseUpL(call->pos.x,call->pos.y);
		else if(call->button == RIGHT_BUTTON && call->down)	return e->mouseDownR(call->pos.x,call->pos.y);
		else if(call->button == RIGHT_BUTTON && !call->down)return e->mouseUpR(call->pos.x,call->pos.y);
	}
	return false;
}
void elementContainer::inputCallback(Input::callBack* callback)
{
	if(callback->type == KEY_STROKE && focus != NULL)
	{
		Input::keyStroke* call = (Input::keyStroke*)callback;
		issueInputCallback(callback,focus);
	}
	else if(callback->type == MOUSE_CLICK)
	{
		Input::mouseClick* call = (Input::mouseClick*)callback;
	
		
	//	call->pos.x *= sh;
	//	call->pos.y = (1.0 - call->pos.y) * sh;
		
		if(call->down)
		{
			if(focus != NULL)
			{
				if(focus->inElement(call->pos))
				{
					issueInputCallback(callback,focus);
					return;
				}
				else
				{
					focus->looseFocus();
					focus = NULL;
				}
			}

			for(auto e = buttons.begin(); this!=NULL && e != buttons.end(); e++)		if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = toggles.begin(); this!=NULL && e != toggles.end(); e++)		if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = sliders.begin(); this!=NULL && e != sliders.end(); e++)		if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = checkBoxes.begin(); this!=NULL && e != checkBoxes.end(); e++)	if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = textBoxes.begin(); this!=NULL && e != textBoxes.end(); e++)	if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = listBoxes.begin(); this!=NULL && e != listBoxes.end(); e++)	if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
		
			debugAssert(focus == NULL);//break if focus is not NULL
			focus = NULL;//if anything responded to the mouse press, this will execute
		}
		else
		{
			if(focus != NULL)	issueInputCallback(callback,focus);
		}
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

	extFilters.clear();
	extFilters.insert(ExtFilter);
	directory=".";
	refreshView();
	return true;
}
bool openFile::init(set<string> ExtFilters)
{
	buttons["desktop"]		= new button(sw/2-405,sh/2-246,175,40,"Desktop",lightGreen,white);
	buttons["myDocuments"]	= new button(sw/2-405,sh/2-196,175,40,"My Documents",lightGreen,white);
	buttons["myComputer"]	= new button(sw/2-405,sh/2-146,175,40,"My Computer",lightGreen,white);
	buttons["myNetwork"]	= new button(sw/2-405,sh/2-96,175,40,"My Network",lightGreen,white);

	extFilters=ExtFilters;
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
		if(extFilters.find(extension(itr->path())) != extFilters.end())
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
	graphics->drawOverlay(Rect::CWH(sw/2,sh/2,841,523),"file viewer");
	graphics->drawOverlay(Rect::XYXY(sw/2-180,sh/2+189,sw/2+190,sh/2+235),"entry bar");

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
	if(extFilters.find(e) == extFilters.end())
	{
		file += *extFilters.begin();
	}
	if(!exists(directory/file))
		file = "";

	done = true;
}
bool openFile::keyDown(int vkey)
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
		char ascii = MapVirtualKey(vkey,MAPVK_VK_TO_CHAR);

		if(ascii != 0 && !input->getKey(VK_SHIFT))
			file  += tolower(ascii);
		else if(ascii != 0)
			file  += ascii;
	}
	return true;
}
bool openFile::mouseL(bool down, int x, int y)
{
	static bool clicking = false;
	if(down)
	{
		for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		if((*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246))) return true;
		for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			if((*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246))) return true;
		clicking = (x>sw/2+144) && (x<sw/2+184) && (y>sh/2+190) && (y<sh/2+230);
		return clicking;
	}
	else
	{
		if(clicking) 
		{
			clicking = false;
			fileSelected();
			return true;
		}

		for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		if((*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246))) return true;
		for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			if((*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246))) return true;
		return false;
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
		file += *extFilters.begin();
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
	if(dataManager.assetLoaded("dialog box"))
		graphics->drawOverlay(Rect::XYWH(x,y,width,height),"dialog box");
	else
	{
		dataManager.bind("noTexture");
		glColor3f(0.3,0.3,0.3);
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
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
		if(dataManager.assetLoaded("glow") && cursorPos.x > startX+slotWidth*slotNum && cursorPos.x < startX+slotWidth*(1+slotNum) && cursorPos.y > startY && cursorPos.y+slotHeight*slotNum < startY+slotHeight*(slotNum+1))
		{
			graphics->drawOverlay(Rect::XYXY(	startX+slotWidth*(0.5+slotNum)-textManager->getTextWidth((*i)->getText())*0.75-20,		startY+slotHeight*0.5-35,						
												startX+slotWidth*(0.5+slotNum)+textManager->getTextWidth((*i)->getText())*0.75+20,		startY+slotHeight*0.5+35),
												"glow");
		}
		(*i)->render();
	}
	menuManager.drawCursor();
}
bool messageBox_c::mouseL(bool down, int X, int Y)
{
	if(done) return false;

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
		return (clicking != -1);
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
				return true;
			}
		}
		clicking = -1;
		return true;
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
	graphics->drawOverlay(Rect::CWH(sw/2,sh/2,800,500),"white");
	glColor3f(1,1,1);
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
	dataManager.bind("ortho");
	graphics->drawOverlay(Rect::XYXY(0.0,1.0,sAspect,0.0),"menu background");
	float sx = (float)sw/800;
	float sy = (float)sh/600;
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
	dataManager.unbindShader();
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

		Level* l = new Level;
		if(l->init("media/map file.lvl"))
		{
			menuManager.setMenu(NULL);
			modeManager.setMode(new modeCampaign(l));
		}
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MULTIPLAYER)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);

		Level* l = new Level;
		if(l->init("media/map file.lvl"))
		{
			menuManager.setMenu(NULL);
			modeManager.setMode(new modeSplitScreen(l));
		}
	}
	else if((vkey==VK_SPACE || vkey==VK_RETURN) && activeChoice==MAP_EDITOR)
	{
		input->up(VK_SPACE);
		input->up(VK_RETURN);
		menuManager.setMenu(new menu::levelEditor);

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
			
			
			Level* l = new Level;
			if(l->init(((openFile*)p)->getFile()))
			{
				menuManager.setMenu(NULL);
				modeManager.setMode(new modeCampaign(l));
			}
			return;
		}
		else if(activeChoice==MULTIPLAYER)
		{
			input->up(VK_SPACE);
			input->up(VK_RETURN);

			Level* l = new Level;
			if(l->init(((openFile*)p)->getFile()))
			{
				menuManager.setMenu(NULL);
				modeManager.setMode(new modeSplitScreen(l));
			}
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
	dataManager.bind("ortho");
	graphics->drawOverlay(Rect::XYXY(0.0,1.0,sAspect,0.0),"menu background");
	dataManager.unbindShader();
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
		currentChoice=max(mapChoices.size()-1,0);
	if(currentChoice>=mapChoices.size())	
		currentChoice=0;


	return (vkey==VK_UP || vkey==VK_DOWN || vkey==VK_ESCAPE);
}
bool inGame::init()
{
	activeChoice=RESUME;
	world.time.pause();
	return true;
}
void inGame::render()
{
	dataManager.bind("ortho");
	graphics->drawOverlay(Rect::XYXY(0.440*sAspect,0.620,0.559*sAspect,0.381),"menu in game");

	if(activeChoice==RESUME)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.600,0.545*sAspect,0.550),"menu in game select");
	if(activeChoice==OPTIONS)	graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.528,0.545*sAspect,0.478),"menu in game select");
	if(activeChoice==QUIT)		graphics->drawOverlay(Rect::XYXY(0.445*sAspect,0.454,0.545*sAspect,0.404),"menu in game select");

	dataManager.unbindShader();
}
bool inGame::keyDown(int vkey)
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
	else
	{
		return false;
	}
	return true;
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
		dataManager.preloadAssets();

		textManager->init("media/ascii");
		settings.load("media/modelData.txt");

		//dataManager.registerAsset("menu background", "media/menu/menu background.png");
		//dataManager.registerAsset("progress back", "media/progress back.png");
		//dataManager.registerAsset("progress front", "media/progress front.png");
		//dataManager.registerShader("ortho", "media/ortho.vert", "media/ortho.frag");
		//dataManager.registerShader("gamma shader", "media/gamma.vert", "media/gamma.frag");
		toLoad=false;
	}

	static int totalAssets = -1;
	int assetsLeft = dataManager.loadAsset();
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
	dataManager.unbindShader();
}
void button::setElementText(string t)
{
	text=t;
	clampedText=t;
	if(textManager->getTextWidth(clampedText) < shape.w-6)
	{
		return;
	}
	else if(textManager->getTextWidth("...") > shape.w-6)
	{
		clampedText="";
	}
	else
	{
		while(textManager->getTextWidth(clampedText + "...") > shape.w-6)
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
void button::setElementSize(Vec2f size)
{
	shape.w = size.x;
	shape.h = size.y;
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
				rPos.x=shape.x;
				rSize.x=min(10,shape.w/2);
			}
			else if(ix==1)
			{
				tPos.x=10.0/140;
				tSize.x=1.0-20.0/140;
				rPos.x=shape.x+10;
				rSize.x=max(shape.w-20,0);
			}
			else if(ix==2)
			{
				tPos.x=1.0-10.0/140;
				tSize.x=10.0/140;
				rPos.x=shape.x+shape.w-min(10,shape.w/2);
				rSize.x=min(10,shape.w/2);
			}
			if(iy==0)
			{
				tPos.y=0.0;
				tSize.y=10.0/45;
				rPos.y=shape.y;
				rSize.y=min(10,shape.h/2);
			}
			else if(iy==1)
			{
				tPos.y=10.0/45;
				tSize.y=1.0-20.0/45;
				rPos.y=shape.y+10;
				rSize.y=max(shape.h-20,0);
			}
			else if(iy==2)
			{
				tPos.y=1.0-10.0/45;
				tSize.y=10.0/45;
				rPos.y=shape.y+shape.h-min(10,shape.h/2);
				rSize.y=min(10,shape.h/2);
			}
			graphics->drawPartialOverlay(Rect::XYWH(rPos,rSize),Rect::XYWH(tPos,tSize),"button");
		}
	}
	dataManager.unbindTextures();
	glColor4f(textColor.r,textColor.g,textColor.b,textColor.a);
	textManager->renderText(clampedText,shape.x+(shape.w-textManager->getTextWidth(clampedText))/2,shape.y+(shape.h-textManager->getTextHeight(clampedText))/2);
	glColor3f(1,1,1);
}
bool button::mouseDownL(int X, int Y)
{
	if(view && active && shape.inRect(Vec2f(X,Y)))
	{
		clicking = true;
		return true;
	}
	return false;
}
bool button::mouseUpL(int X, int Y)
{
	if(view && active && clicking && shape.inRect(Vec2f(X,Y)))
	{
		changed = true;
		clicking = false;
		return true;
	}
	clicking = false;
	return false;
}
void checkBox::render()
{
	graphics->drawOverlay(Rect::XYWH(shape.x,shape.y,26,26),"check box");
	if(checked)
		graphics->drawOverlay(Rect::XYWH(shape.x,shape.y,26,26),"check");

	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,shape.x+30,shape.y);
	glColor3f(1,1,1);
}
bool checkBox::mouseDownL(int X, int Y)
{
	if(view && active && shape.inRect(Vec2f(X,Y)))
	{
		clicking = true;
		return true;
	}
	return false;
}
bool checkBox::mouseUpL(int X, int Y)
{
	if(view && active && clicking && shape.inRect(Vec2f(X,Y)) )
	{
		changed = true;
		checked = !checked;
		return true;
	}
	if(clicking)
	{
		clicking = false;
		return true;
	}
	return false;
}
bool textBox::mouseDownL(int X, int Y)
{
	if(!clicking)
	{
		clicking = shape.inRect(Vec2f(X,Y));
		return clicking;
	}
	else
	{
		clicking = shape.inRect(Vec2f(X,Y));
		return !clicking;
	}
}
bool textBox::mouseUpL(int X, int Y)
{
	if(clicking)
	{
		bool f = shape.inRect(Vec2f(X,Y));
		cursorPos = text.length();
		clicking = false;
		return true;
	}
	return false;
}
void textBox::render()
{
	if(focus)	glColor3f(0,0,0);
	else	glColor3f(0.3,0.3,0.3);
	graphics->drawOverlay(Rect::XYWH(shape.x-1,shape.y-1,shape.w+2,shape.h+2),"white");

	if(focus)	glColor3f(0.6,0.6,0.6);
	else	glColor3f(0.8,0.8,0.8);
	graphics->drawOverlay(shape,"white");

	glColor3f(color.r,color.g,color.b);
	if(focus)
	{
		
		float w = textManager->getTextWidth(text.substr(0,cursorPos));
		float h = textManager->getTextHeight(text);

		textManager->renderText(text,shape.x+2,shape.y);
		if(int(floor(world.time()/500)) % 2 == 1)
		{
			graphics->drawLine(Vec3f(shape.x+w,shape.y+2,0),Vec3f(shape.x+w,shape.y+h,0));
		}
	}
	else
	{
		textManager->renderText(text,shape.x+2,shape.y);
	}

	glColor3f(1,1,1);
}
void textBox::addChar(char c)
{
	text.insert(text.begin()+cursorPos,c);
	cursorPos++;
}
bool textBox::keyDown(int vkey)
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
		return true;
	}
	return false;
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
		focus = false;
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
		focus = true;
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
void listBox::gainFocus()
{
	focus = true;
	shape.h = 30*(1 + options.size());
}
void listBox::looseFocus()
{
	shape.h = 30;
	focus = false;
}
bool listBox::mouseDownL(int X, int Y)
{
	choosing = shape.inRect(Vec2f(X,Y)) && Y >= shape.y + 30;
	return true;
}
bool listBox::mouseUpL(int X, int Y)
{
	if(choosing)
	{
		choosing = false;
		if(shape.inRect(Vec2f(X,Y)) && Y >= shape.y + 30)
		{
			int n = (Y-(shape.y+30)) / 30;
			text = options[n];
			optionNum = n;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
void listBox::render()
{
	glColor3f(0.3,0.3,0.3);
	graphics->drawOverlay(Rect::XYWH(shape.x-1,shape.y-1,shape.w+2,30+2),"white");

	if(focus)
	{
		glColor3f(0,0,0);
		graphics->drawOverlay(Rect::XYWH(shape.x-1,shape.y+30+2,shape.w+2,shape.h-30+2),"white");
	}

	if(focus)	glColor3f(0.8,0.8,0.8);
	else	glColor3f(0.9,0.9,0.9);
	graphics->drawOverlay(Rect::XYWH(shape.x,shape.y,shape.w,30),"white");

	if(focus)
	graphics->drawOverlay(Rect::XYWH(shape.x,shape.y+30+3,shape.w,30*options.size()),"white");


	if(focus)	glColor3f(0.3,0.3,0.3);
	else	glColor3f(0.5,0.5,0.5);
	graphics->drawOverlay(Rect::XYXY(shape.x+shape.w-30,shape.y,shape.x+shape.w,shape.y+30),"white");

	glColor3f(0,0,0);
	graphics->drawTriangle(	Vec3f(shape.x+shape.w-30.0 * 0.666,	shape.y + 30.0 * 0.333,	0),
							Vec3f(shape.x+shape.w-30.0 * 0.333,	shape.y + 30.0 * 0.333,	0),
							Vec3f(shape.x+shape.w-30.0 * 0.5,	shape.y + 30.0 * 0.666,	0));
	glColor3f(color.r,color.g,color.b);

	textManager->renderText(text,shape.x+2,shape.y);
	if(focus)
	{
		float h = shape.y + 30 + 1;
		for(auto i = options.begin(); i!= options.end(); i++)
		{
			textManager->renderText(*i,shape.x+2,h);
			h += 30;
		}
	}

	if(focus && optionNum != -1)
	{
		glColor4f(0.3,0.3,0.3,0.3);
		graphics->drawOverlay(Rect::XYWH(shape.x,shape.y+30*(optionNum+1)+3,shape.w,30+1),"white");
	}

	glColor3f(1,1,1);
}
void label::render()
{
	glColor4f(color.r,color.g,color.b,color.a);
	textManager->renderText(text,shape.x,shape.y);
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
	for(auto i = buttons.begin(); i!=buttons.end();i++,n++)
	{
		if(value==n)
			(*i)->setElementColor(clicked);
		else
			(*i)->setElementColor(unclicked);
	}

	if(!buttons.empty())
	{
		Vec2f vMin = buttons.front()->getShape().bl();
		Vec2f vMax = buttons.front()->getShape().tr();
		Vec2f tMin, tMax;
		for(auto i = buttons.begin() + 1; i!=buttons.end();i++)
		{
			tMin = (*i)->getShape().bl();
			tMax = (*i)->getShape().tr();
			vMin.x = min(vMin.x, tMin.x);
			vMin.y = min(vMin.y, tMin.y);
			vMax.x = max(vMax.x, tMax.x);
			vMax.y = max(vMax.y, tMax.y);
		}
		shape = Rect::XYXY(vMin,vMax);
	}
}
int toggle::addButton(button* b)
{
	if(b == NULL)
		return -1;

	b->setElementColor(unclicked);
	buttons.push_back(b);
	if(buttons.size()==1)
	{
		value=0;
		buttons[0]->setElementColor(clicked);
		shape = b->getShape();
	}
	else
	{
		Vec2f vMin = shape.bl();
		Vec2f vMax = shape.tr();
		Vec2f tMin = b->getShape().bl();
		Vec2f tMax = b->getShape().tr();
		vMin.x = min(vMin.x, tMin.x);
		vMin.y = min(vMin.y, tMin.y);
		vMax.x = max(vMax.x, tMax.x);
		vMax.y = max(vMax.y, tMax.y);
		shape = Rect::XYXY(vMin,vMax);
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
bool toggle::mouseDownL(int X, int Y)
{
	if(view && active)
	{
		for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++)
		{
			if((*i)->mouseDownL(X,Y))
				return true;
		}
	}
	return false;
}
bool toggle::mouseUpL(int X, int Y)
{
	bool retVal=false;
	if(view && active)
	{
		int n=0;
		for(vector<button*>::iterator i = buttons.begin(); i!=buttons.end();i++,n++)
		{
			(*i)->resetChanged();
			retVal = retVal || (*i)->mouseUpL(X,Y);
			if((*i)->getChanged() && value != n)
			{
				value=n;
				changed=true;
			}
		}
		updateColors();
	}
	return retVal;
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
		value = clamp((maxValue - minValue) * (cursorPos.x - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
	}
	graphics->drawOverlay(Rect::XYXY(shape.x,0.5*shape.h+shape.y-11,shape.x+shape.w,0.5*shape.h+shape.y+11),"slider bar");

	float xv = (value-minValue)*shape.w/(maxValue-minValue)+shape.x;
	graphics->drawOverlay(Rect::XYXY(xv-22,shape.y,xv+22,shape.y+shape.h),"slider");
}
bool slider::mouseDownL(int X, int Y)
{
	if(view && active && abs(value*shape.w/(maxValue - minValue) + shape.x  - X) < 22 && Y > shape.y && Y < shape.y + shape.h)	
	{
		clicking = true;
		mouseOffset = value - ((maxValue - minValue) * (X - shape.x) / shape.w + minValue);
		return true;
	}
	else if(view && active && shape.inRect(Vec2f(X,Y)))
	{
		clicking = true;
		mouseOffset = 0.0f;
		return true;
	}
	return false;
}
bool slider::mouseUpL(int X, int Y)
{
	if(view && active && clicking)
	{
		value = clamp((maxValue - minValue) * (X - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
		clicking = false;
		return true;
	}
	return false;
}
float slider::getValue()
{
	if(clicking)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		value = clamp((maxValue - minValue) * (cursorPos.x - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
	}
	return value;
}
void slider::setMinValue(float m)
{
	if(clicking)
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		value = clamp((maxValue - minValue) * (cursorPos.x - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
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
		value = clamp((maxValue - minValue) * (cursorPos.x - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
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