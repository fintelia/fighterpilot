
#include "engine.h"
#ifdef WINDOWS
#include <Shlobj.h>
#endif

namespace gui
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
			if(e->second != NULL && e->second->getVisibility() && !e->second->hasFocus())
				e->second->render();
		}
		for(auto e = menu->listBoxes.begin(); e != menu->listBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility() && e->second->hasFocus())
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
			if(e->second != NULL && e->second->getVisibility() && !e->second->hasFocus())
				e->second->render();
		}
		for(auto e = (*i)->listBoxes.begin(); e != (*i)->listBoxes.end(); e++)
		{
			if(e->second != NULL && e->second->getVisibility() && e->second->hasFocus())
				e->second->render();
		}
	}
	if(mDrawCursor)
	{
		Vec2f cursorPos = input.getMousePos();
		if(dataManager.assetLoaded("cursor"))
			graphics->drawOverlay(Rect::XYWH(cursorPos.x,cursorPos.y,0.02,0.025),"cursor");
		else
		{
			dataManager.bind("white");
			graphics->drawTriangle(	Vec3f(cursorPos.x,cursorPos.y,0),
									Vec3f(cursorPos.x+0.015,cursorPos.y-0.023,0),
									Vec3f(cursorPos.x,cursorPos.y-0.028,0) );
		}
	}
}
// void manager::render3D(unsigned int view)
// {
// 	if(menu != NULL)
// 	{
// 		menu->render3D(view);
// 	}
// }
void manager::update()
{
	if(!popups.empty())
	{
		unsigned int numPopups = popups.size();
		auto p = popups.back();
		p->updateFrame();
		if(numPopups == popups.size() && p->isDone())
		{
			popups.erase(popups.end()-1); //remove the popup from the vector

			if(p->callback != nullptr)//see if we need to issue a callback (using the pointer we have)
				p->callback(p.get());
		}
	}
	else if(menu != nullptr)
	{
		menu->update();
	}
}
// void manager::updateSimulation()
// {
// 	if(popups.empty() && menu != nullptr)
// 	{
// 		menu->updateSimulation();
// 	}
// }
void manager::setMenu(screen* m)
{
	popups.clear();

	if(menu != nullptr)
		delete menu;

	menu = m;

	if(m != nullptr)
		m->init();
}
bool manager::setPopup(popup* p)
{
	if(p != NULL)
	{
		popups.push_back(shared_ptr<popup>(p));
		p->init();
		return true;
	}
	return false;
}
void manager::inputCallback(InputManager::callBack* callback)
{
	if(callback->type == KEY_STROKE)
	{
		InputManager::keyStroke* call = (InputManager::keyStroke*)callback;
		if(!popups.empty())
		{
#ifdef _DEBUG
			int pSize = popups.size();
#endif
			if(!call->up && popups.back()->keyDown(call->vkey, call->ascii)) return;
			if(call->up && popups.back()->keyUp(call->vkey, call->ascii)) return;

			debugAssert(popups.size() == pSize);

			popups.back()->inputCallback(call);
		}
		else if(menu!=NULL)
		{
			if(!call->up && menu->keyDown(call->vkey, call->ascii))return;
			if(call->up && menu->keyUp(call->vkey, call->ascii))return;

			debugAssert(menu != NULL);

			menu->inputCallback(call);
		}
	}
	else if(callback->type == MENU_KEY_STROKE)
	{
		InputManager::menuKeystroke* call = (InputManager::menuKeystroke*)callback;
		if(!popups.empty())
		{
#ifdef _DEBUG
			int pSize = popups.size();
#endif
			if(popups.back()->menuKey(call->mkey)) 
				return;

			debugAssert(popups.size() == pSize);

			popups.back()->inputCallback(call);
		}
		else if(menu!=NULL)
		{
			if(menu->menuKey(call->mkey))
				return;

			debugAssert(menu != NULL);

			menu->inputCallback(call);
		}
	}
	else if(callback->type == MOUSE_CLICK)
	{
		InputManager::mouseClick* call = (InputManager::mouseClick*)callback;

		if(!popups.empty())
		{
#ifdef _DEBUG
			int pSize = popups.size();
#endif

			if(call->button == LEFT_BUTTON && popups.back()->mouseL(call->down,call->pos.x,call->pos.y)) return;
			if(call->button == RIGHT_BUTTON && popups.back()->mouseR(call->down,call->pos.x,call->pos.y)) return;

			debugAssert(popups.size() == pSize);

			popups.back()->inputCallback(call);
		}
		else if(menu!=NULL)
		{
			if(menu->mouse(call->button,call->down))
				return;

			debugAssert(menu != NULL);

			menu->inputCallback(call);
		}

	}
	else if(callback->type == MOUSE_SCROLL)
	{
		InputManager::mouseScroll* call = (InputManager::mouseScroll*)callback;
		if(!popups.empty() && popups.back()->scroll(call->rotations)) return;
		if(menu!=NULL && menu->scroll(call->rotations)) return;
	}
}
bool elementContainer::issueInputCallback(InputManager::callBack* callback, element* e)
{
	if(e == nullptr)
		return false;

	if(callback->type == KEY_STROKE)
	{
		InputManager::keyStroke* call = (InputManager::keyStroke*)callback;
		if(call->up)	return e->keyUp(call->vkey, call->ascii);
		else			return e->keyDown(call->vkey, call->ascii);
	}
	else if(callback->type == MOUSE_CLICK)
	{
		InputManager::mouseClick* call = (InputManager::mouseClick*)callback;

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
void simulationScreen::update()
{
	unsigned int n = 0;
	if(world)
	{
		while(world->time.needsUpdate())
		{
			world->time.nextUpdate();
			updateSimulation();

			if(++n==20)
			{
				break;//we can no-longer keep up with the simulation
			}
		}
		world->time.nextFrame();	
		world->frameUpdate();
	}
	updateFrame();
}
void elementContainer::inputCallback(InputManager::callBack* callback)
{
	if(callback->type == KEY_STROKE && focus != NULL)
	{
	//	Input::keyStroke* call = (Input::keyStroke*)callback;
		issueInputCallback(callback,focus);
	}
	else if(callback->type == MOUSE_CLICK)
	{
		InputManager::mouseClick* call = (InputManager::mouseClick*)callback;


	//	call->pos.x *= sh;
	//	call->pos.y = (1.0 - call->pos.y) * sh;

		if(call->down)
		{
			if(focus != nullptr)
			{
				if(focus->getVisibility() && focus->getElementState() && focus->inElement(call->pos))
				{
					issueInputCallback(callback,focus);
					return;
				}
				else
				{
					focus->looseFocus();
					focus = nullptr;
				}
			}

			for(auto e = buttons.begin(); this!=NULL && e != buttons.end(); e++)		if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = toggles.begin(); this!=NULL && e != toggles.end(); e++)		if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = sliders.begin(); this!=NULL && e != sliders.end(); e++)		if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = checkBoxes.begin(); this!=NULL && e != checkBoxes.end(); e++)	if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = textBoxes.begin(); this!=NULL && e != textBoxes.end(); e++)	if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}
			for(auto e = listBoxes.begin(); this!=NULL && e != listBoxes.end(); e++)	if(e->second->inElement(call->pos) && e->second->getVisibility() && e->second->getElementState()){focus=e->second;	issueInputCallback(callback,focus); return;}

			debugAssert(focus == nullptr);//break if focus is not NULL
			focus = nullptr;//if anything responded to the mouse press, this will execute
		}
		else
		{
			if(focus != nullptr)	issueInputCallback(callback,focus);
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

	directory=".";
	extFilters.clear();
	extFilters.insert(ExtFilter);
	refreshView();
	return true;
}
bool openFile::init(set<string> ExtFilters)
{
	buttons["desktop"]		= new button(sw/2-405,sh/2-246,175,40,"Desktop",lightGreen,white);
	buttons["myDocuments"]	= new button(sw/2-405,sh/2-196,175,40,"My Documents",lightGreen,white);
	buttons["myComputer"]	= new button(sw/2-405,sh/2-146,175,40,"My Computer",lightGreen,white);
	buttons["myNetwork"]	= new button(sw/2-405,sh/2-96,175,40,"My Network",lightGreen,white);

	directory=".";
	extFilters=ExtFilters;
	refreshView();
	return true;
}
void openFile::refreshView()
{
//	for(auto i=folderButtons.begin();i!=folderButtons.end();i++)		if(*i!=NULL) delete (*i);
//	for(auto i=fileButtons.begin();i!=fileButtons.end();i++)			if(*i!=NULL) delete (*i);
	files.clear();
	folders.clear();
//	folderButtons.clear();
//	fileButtons.clear();

	auto Folders = fileManager.getAllDirectories(directory);
	auto Files = fileManager.getAllFiles(directory,extFilters);


	//filesystem::directory_iterator end_itr; // default construction yields past-the-end
	//for ( filesystem::directory_iterator itr( directory );	itr != end_itr;	 ++itr )
	//{
	//	if ( is_directory(itr->status()) && itr->path().generic_string().compare(".svn") != 0)
	//	{
	//		folders.push_back(itr->path().leaf().generic_string());
	//	}
	//	if(extFilters.find(extension(itr->path())) != extFilters.end())
	//	{
	//		files.push_back(itr->path().leaf().generic_string());
	//	}
	//}
	for(auto i=Folders.begin();i!=Folders.end();i++)
	{
		if(*i == "..")
			folders.push_back(thumbnail("..","<up dir>",thumbnail::FOLDER));
		else if(*i != ".svn" && *i != ".")
			folders.push_back(thumbnail(*i,*i,thumbnail::FOLDER));
	}
	for(auto i=Files.begin();i!=Files.end();i++)
	{
		files.push_back(thumbnail(*i,*i,thumbnail::UNKNOWN_FILE));
	}

	for(auto i=folders.begin(); i!=folders.end(); i++)
	{
		if(graphics->getTextSize(i->displayName,"font small").x > 0.0625)
		{
			string folder = i->displayName;
			while(graphics->getTextSize(folder + "...","font small").x > 0.06 && folder != "")
				folder = folder.substr(0,folder.size()-1);
			i->displayName = folder + "...";
		}
	}
	for(auto i=files.begin(); i!=files.end(); i++)
	{
		if(graphics->getTextSize(i->displayName,"font small").x > 0.0625)
		{
			string file = i->displayName;
			while(graphics->getTextSize(file + "...","font small").x > 0.06 && file != "")
				file = file.substr(0,file.size()-1);
			i->displayName = file + "...";
		}
	}

	scroll = 0.0;
}
void openFile::updateFrame()
{
#if defined WINDOWS
	if(buttons["desktop"]->checkChanged())
	{
		char aFolder[MAX_PATH];
		if(!SHGetFolderPathA(0, CSIDL_DESKTOPDIRECTORY, NULL, 0/*SHGFP_TYPE_CURRENT*/, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
	}
	else if(buttons["myDocuments"]->checkChanged())
	{
		char aFolder[MAX_PATH];
		if(!SHGetFolderPathA(0, CSIDL_PERSONAL, NULL, 0/*SHGFP_TYPE_CURRENT*/, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
	}
	else if(buttons["myComputer"]->checkChanged())
	{
		char aFolder[MAX_PATH];
		if(!SHGetFolderPathA(0, CSIDL_DRIVES, NULL, 0/*SHGFP_TYPE_CURRENT*/, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
	}
	else if(buttons["myNetwork"]->checkChanged())
	{
		char aFolder[MAX_PATH];
		if(!SHGetFolderPathA(0, CSIDL_NETHOOD, NULL, 0/*SHGFP_TYPE_CURRENT*/, aFolder))
		{
			directory = aFolder;
			refreshView();
		}
	}
#elif defined LINUX
	//TODO: add support for default directories on linux
#endif

	//for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)
	//{
	//	if((*i)->checkChanged())
	//	{
	//		directory = directory + "/" + (*i)->getText();
	//		refreshView();
	//		return 0;
	//	}
	//}
	//for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)
	//{
	//	if((*i)->checkChanged())
	//	{
	//		file=(*i)->getText();
	//		fileSelected();
	//		return 0;
	//	}
	//}
}
void openFile::render()
{
	graphics->drawOverlay(Rect::CWH(sAspect/2,0.5,0.8,0.505),"file viewer");
	graphics->drawOverlay(Rect::XYXY(0.5*sAspect-0.176,0.5+0.18,0.5*sAspect+0.186,0.5+0.224),"entry bar");
	graphics->drawText(file,Vec2f(0.5*sAspect-0.166,0.5+0.202-graphics->getTextSize(file).y/2));

	float row=scroll; int column=0;
	float size = 0.0625;
	for(auto i=folders.begin(); i != folders.end(); i++)
	{
		if(row >= 0)
		{
			graphics->drawPartialOverlay(Rect::CWH(sAspect/2-0.126 + size*1.19*column, 0.310 + size*1.15*row, size,size),Rect::XYWH(0,0,0.5,0.5),"thumbnails");
			graphics->drawText(i->displayName,Vec2f(floor((sAspect/2-0.126 + size*1.19*column-graphics->getTextSize(i->displayName,"font small").x/2)*sh)/sh, 0.328 + size*1.15*row),"font small");
		}
		if(++column == 7){column=0;row+=1.0;}
	}
	for(auto i=files.begin(); i != files.end(); i++)
	{
		if(row >= 0)
		{
			graphics->drawOverlay(Rect::CWH(sAspect/2-0.126 + size*1.19*column, 0.310 + size*1.15*row, size,size),"button");
			graphics->drawText(i->displayName,Vec2f(floor((sAspect/2-0.126 + size*1.19*column-graphics->getTextSize(i->displayName,"font small").x/2)*sh)/sh, 0.328 + size*1.15*row),"font small");
		}
		if(++column == 7){column=0;row+=1.0;}
	}
	menuManager.drawCursor();
}
void openFile::fileSelected()
{
	string e = fileManager.extension(file);
	if(extFilters.find(e) == extFilters.end())
	{
		file += *extFilters.begin();
	}
	if(!fileManager.fileExists(directory + "/" + file))
		file = "";
	
	done = true;
}
bool openFile::keyDown(int vkey, char ascii)
{
    if(vkey == VK_RETURN)
    {
		fileSelected();
    }
    else if(vkey == VK_ESCAPE)
    {
		file = "";
		fileSelected();
		input.up(VK_ESCAPE); //so the program will not quit?
    }
    else if(vkey == VK_BACK)
    {
		if(file.size() != 0)
			file=file.substr(0,file.size()-1);
    }
    else if(ascii != 0)
    {
		file += ascii;
    }
    return true;
}
bool openFile::getThumbnail(Vec2f v, vector<thumbnail>::iterator& itt)
{
	float row=scroll; int column=0;
	float size = 0.0625;
	for(auto i=folders.begin(); i != folders.end(); i++)
	{
		if(row >= 0 && Rect::CWH(sAspect/2-0.126 + size*1.19*column, 0.310 + size*1.15*row, size,size).inRect(v))
		{
			itt = i;
			return true;
		}
		if(++column == 7)
		{
			column=0;
			row+=1.0;
		}
	}
	for(auto i=files.begin(); i != files.end(); i++)
	{
		if(row >= 0 && Rect::CWH(sAspect/2-0.126 + size*1.19*column, 0.310 + size*1.15*row, size,size).inRect(v))
		{
			itt = i;
			return true;
		}
		if(++column == 7)
		{
			column=0;
			row+=1.0;
		}
	}
	return false;
}
bool openFile::mouseL(bool down, float x, float y)
{ 
	static bool clicking = false;
	if(down)
	{
//		for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		if((*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246))) return true;
//		for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			if((*i)->mouseDownL(x-(sw/2-190),y-(sh/2-246))) return true;

		clicking = getThumbnail(Vec2f(x,y), clickingThumbnail);
		return clicking;
	}
	else
	{
		
		if(clicking)
		{
			clicking = false;
			vector<thumbnail>::iterator i;
			if(getThumbnail(Vec2f(x,y), i))
			{
				if(i == clickingThumbnail && clickingThumbnail->type == thumbnail::FOLDER)
				{
					directory = directory + "/" + clickingThumbnail->name;
					refreshView();
				}
				else if(i == clickingThumbnail) // thumbnail must be some type of file
				{
					file = clickingThumbnail->name;
					fileSelected();
				}
			}
			return true;
		}

//		for(vector<button*>::iterator i=folderButtons.begin();i!=folderButtons.end();i++)		if((*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246))) return true;
//		for(vector<button*>::iterator i=fileButtons.begin();i!=fileButtons.end();i++)			if((*i)->mouseUpL(x-(sw/2-190),y-(sh/2-246))) return true;
		return false;
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
	if(fileManager.fileExists(directory + "/" + file))//if file exists
	{
		messageBox_c* m = new messageBox_c;
		vector<string> s;
		s.push_back("yes");
		s.push_back("no");
		m->initialize(file + " already exists. Do you want to replace it?",s);
		m->callback = [this](popup* p){
			if(((messageBox_c*)p)->getValue() == 0)
			{
				done = true;
			}
			else
			{
				replaceDialog = false;
			}
		};
		menuManager.setPopup(m);
	}
	else
	{
		done = true;
	}
}
bool messageBox_c::initialize(string t)
{
	return initialize(t, vector<string>(1,"OK"));
}
bool messageBox_c::initialize(string t, vector<string> names)
{
	if(names.empty()) return initialize(t);//watch out for infinite loop!!

	Vec2f tSize = graphics->getTextSize(t);
	width = clamp(tSize.x+0.039,0.700,sAspect-0.01);
	height = 0.288 + max(tSize.y-0.166,0.0);
	x = sAspect*0.5 - width/2;
	y = 0.5 - height/2;
	if(x < 0.005) x = 0.005;
	if(y < 0.005) y = 0.005;

	labels["label"] = new label(max(x+(width-tSize.x)/2,0.05), y+height*0.322-tSize.y/2, t, Color(1,1,1,0.8));

	float slotWidth = (0.958*width) / names.size();
	int slotNum = 0;
	for(vector<string>::iterator i = names.begin();i !=names.end(); i++, slotNum++)
	{
		options.push_back(new label(

									((float)(x + (0.021*width)+(0.5+slotNum)*slotWidth-graphics->getTextSize(*i).x/2)),
									((float)(y + (0.797*height)-graphics->getTextSize(*i).y/2)),

									*i, white));
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
		graphics->setColor(0.3,0.3,0.3);
		graphics->drawOverlay(Rect::XYWH(x,y,width,height));
		graphics->setColor(1,1,1);
	}

	float slotWidth = ((float)(685.0*width/715) / options.size());
	float slotHeight = ((float)(84.0*height/295));
	float startX = ((float)(15.0*width/715)+x);
	float startY = ((float)(193.0*height/295)+y);

	int slotNum=0;

	Vec2f cursorPos = input.getMousePos();
	for(auto i = options.begin(); i!=options.end();i++,slotNum++)
	{
		if(dataManager.assetLoaded("glow") && cursorPos.x > startX+slotWidth*slotNum && cursorPos.x < startX+slotWidth*(1+slotNum) && cursorPos.y > startY && cursorPos.y+slotHeight*slotNum < startY+slotHeight*(slotNum+1))
		{
			graphics->drawOverlay(Rect::XYXY(	startX+slotWidth*(0.5+slotNum)-graphics->getTextSize((*i)->getText()).x*0.75-0.03,		startY+slotHeight*0.5+0.034,
												startX+slotWidth*(0.5+slotNum)+graphics->getTextSize((*i)->getText()).x*0.75+0.03,		startY+slotHeight*0.5-0.034),
												"glow");
		}
		(*i)->render();
	}

	menuManager.drawCursor();
}
bool messageBox_c::mouseL(bool down, float X, float Y)
{
	if(done) return false;

	float slotWidth = ((float)(685.0*width/715) / options.size());
	float slotHeight = ((float)(84.0*height/295));
	float startX = ((float)(15.0*width/715)+x);
	float startY = ((float)(193.0*height/295)+y);
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

void button::setElementText(string t)
{
	text=t;
	clampedText=t;
	if(graphics->getTextSize(clampedText).x < shape.w-6)
	{
		return;
	}
	else if(graphics->getTextSize("...").x > shape.w-6)
	{
		clampedText="";
	}
	else
	{
		while(graphics->getTextSize(clampedText + "...").x > shape.w-6)
			clampedText.erase(clampedText.size()-1);
		int l=clampedText.find_last_not_of(" \t\f\v\n\r")+1;
		clampedText=clampedText.substr(0,l);
		clampedText += "...";
	}
	clampedText = text;//set clamped text to whole text so it is not trucnated (while otho shader is not in use)
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
	graphics->setColor(color.r,color.g,color.b,color.a);
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
				rSize.x=min(5.0f/sh,shape.w/2);
			}
			else if(ix==1)
			{
				tPos.x=10.0/140;
				tSize.x=1.0-20.0/140;
				rPos.x=shape.x+5.0/sh;
				rSize.x=max(shape.w-10.0/sh,0.0f);
			}
			else if(ix==2)
			{
				tPos.x=1.0-10.0/140;
				tSize.x=10.0/140;
				rPos.x=shape.x+shape.w-min(5.0f/sh,shape.w/2);
				rSize.x=min(5.0f/sh,shape.w/2);
			}
			if(iy==0)
			{
				tPos.y=0.0;
				tSize.y=10.0/45;
				rPos.y=shape.y;
				rSize.y=min(5.0f/sh,shape.h/2);
			}
			else if(iy==1)
			{
				tPos.y=10.0/45;
				tSize.y=1.0-20.0/45;
				rPos.y=shape.y+5.0/sh;
				rSize.y=max(shape.h-10.0/sh,0.0f);
			}
			else if(iy==2)
			{
				tPos.y=1.0-10.0/45;
				tSize.y=10.0/45;
				rPos.y=shape.y+shape.h-min(5.0f/sh,shape.h/2);
				rSize.y=min(5.0f/sh,shape.h/2);
			}
			graphics->drawPartialOverlay(Rect::XYWH(rPos,rSize),Rect::XYWH(tPos,tSize),"button");
		}
	}
	//graphics->drawOverlay(shape,"button");
	graphics->setColor(textColor.r,textColor.g,textColor.b,textColor.a);
	graphics->drawText(clampedText,Vec2f(shape.x+(shape.w-graphics->getTextSize(clampedText).x)/2,shape.y+(shape.h-graphics->getTextSize(clampedText).y)/2));
	graphics->setColor(1,1,1);
}
bool button::mouseDownL(float X, float Y)
{
	if(view && active && shape.inRect(Vec2f(X,Y)))
	{
		clicking = true;
		return true;
	}
	return false;
}
bool button::mouseUpL(float X, float Y)
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
	graphics->drawOverlay(Rect::XYWH(shape.x,shape.y,0.025,0.025),"check box");
	if(checked)
		graphics->drawOverlay(Rect::XYWH(shape.x,shape.y,0.025,0.025),"check");

	graphics->setColor(color.r,color.g,color.b,color.a);
	graphics->drawText(text,Vec2f(shape.x+0.029,shape.y));
	graphics->setColor(1,1,1);
}
bool checkBox::mouseDownL(float X, float Y)
{
	if(view && active && shape.inRect(Vec2f(X,Y)))
	{
		clicking = true;
		return true;
	}
	return false;
}
bool checkBox::mouseUpL(float X, float Y)
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
bool textBox::mouseDownL(float X, float Y)
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
bool textBox::mouseUpL(float X, float Y)
{
	if(clicking)
	{
	//	bool f = shape.inRect(Vec2f(X,Y));
		cursorPos = text.length();
		clicking = false;
		return true;
	}
	return false;
}
void textBox::render()
{
	if(focus)	graphics->setColor(0,0,0);
	else	graphics->setColor(0.3,0.3,0.3);
	graphics->drawOverlay(Rect::XYWH(shape.x-1.0/1280,shape.y-1.0/1024,shape.w+2.0/1024,shape.h+2.0/1024),"white");

	if(focus)	graphics->setColor(0.6,0.6,0.6);
	else	graphics->setColor(0.8,0.8,0.8);
	graphics->drawOverlay(shape,"white");

	graphics->setColor(color.r,color.g,color.b);
	if(focus)
	{
		Vec2f tSize = graphics->getTextSize(text.substr(0,cursorPos));

		graphics->drawText(text,Vec2f(shape.x+2.0/1280,shape.y));
		if(int(floor(world->time()/500)) % 2 == 1)
		{
			graphics->drawLine(Vec3f(shape.x+tSize.x,shape.y+2.0/1024,0),Vec3f(shape.x+tSize.x,shape.y+tSize.y,0));
		}
	}
	else
	{
		graphics->drawText(text,Vec2f(shape.x+2.0/1280,shape.y));
	}

	graphics->setColor(1,1,1);
}
void textBox::addChar(char c)
{
	text.insert(text.begin()+cursorPos,c);
	cursorPos++;
}
bool textBox::keyDown(int vkey, char ascii)
{
	if(focus)
	{	    
		if(vkey == 0x08 && cursorPos > 0)// backspace
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
		else if(ascii != 0)
		{
			addChar(ascii);
		}
		return true;
	}
	return false;
}
void numericTextBox::addChar(char c)
{
	if((c >= '0' && c <= '9') || c=='.')
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
	shape.h = 30.0/1024*(1 + options.size());
}
void listBox::loseFocus()
{
	shape.h = 30.0/1024;
	focus = false;
}
bool listBox::mouseDownL(float X, float Y)
{
	choosing = shape.inRect(Vec2f(X,Y)) && Y >= shape.y + 30.0/1024;
	return true;
}
bool listBox::mouseUpL(float X, float Y)
{
	if(choosing)
	{
		choosing = false;
		if(shape.inRect(Vec2f(X,Y)) && Y >= shape.y + 30.0/1024)
		{
			int n = (Y-(shape.y+30.0/1024)) / (30.0/1024);
			text = options[n];
			optionNum = n;
			loseFocus();
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
	if(optionNum >= 0 && optionNum < options.size())
		text = options[optionNum];

	graphics->setColor(0.3,0.3,0.3);
	graphics->drawOverlay(Rect::XYWH(shape.x-1.0/1280,shape.y-1.0/1024,shape.w+2.0/1280,0.0313),"white");

	if(focus)
	{
		graphics->setColor(0,0,0);
		graphics->drawOverlay(Rect::XYWH(shape.x-1.0/1280,shape.y+0.0313,shape.w+2.0/1280,shape.h-0.0313),"white");
	}

	if(focus)	graphics->setColor(0.8,0.8,0.8);
	else	graphics->setColor(0.9,0.9,0.9);
	graphics->drawOverlay(Rect::XYWH(shape.x,shape.y,shape.w,0.0293),"white");

	if(focus)
	graphics->drawOverlay(Rect::XYWH(shape.x,shape.y+0.032,shape.w,0.0293*options.size()),"white");


	if(focus)	graphics->setColor(0.3,0.3,0.3);
	else	graphics->setColor(0.5,0.5,0.5);
	graphics->drawOverlay(Rect::XYXY(shape.x+shape.w-0.0293,shape.y,shape.x+shape.w,shape.y+0.0293),"white");

	graphics->setColor(0,0,0);
//	graphics->drawTriangle(	Vec3f(shape.x+shape.w-0.0293 * 0.666,	shape.y + 0.0293 * 0.333,	0),
//							Vec3f(shape.x+shape.w-0.0293 * 0.333,	shape.y + 0.0293 * 0.333,	0),
//							Vec3f(shape.x+shape.w-0.0293 * 0.5,		shape.y + 0.0293 * 0.666,	0));

	graphics->setColor(color.r,color.g,color.b);

	graphics->drawText(text,Vec2f(shape.x+2.0/1280,shape.y));
	if(focus)
	{
		float h = shape.y + 0.0293 + 1.0/1024;
		for(auto i = options.begin(); i!= options.end(); i++)
		{
			graphics->drawText(*i,Vec2f(shape.x+2.0/1280,h));
			h += 0.0293;
		}
	}

	if(focus && optionNum != -1)
	{
		graphics->setColor(0.3,0.3,0.3,0.3);
		graphics->drawOverlay(Rect::XYWH(shape.x,shape.y+0.0293*(optionNum+1)+0.0029,shape.w,0.030),"white");
	}

	graphics->setColor(1,1,1);
}
void label::render()
{
	graphics->setColor(color.r,color.g,color.b,color.a);
	graphics->drawText(text, shape.origin());
	//textManager->renderText(text,shape.x,shape.y);
	graphics->setColor(1,1,1);
}
toggle::toggle(vector<button*> b, Color clickedC, Color unclickedC, label* l, int startValue): element(TOGGLE,0,0), Label(l), value(startValue)
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
bool toggle::mouseDownL(float X, float Y)
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
bool toggle::mouseUpL(float X, float Y)
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
		Vec2f cursorPos = input.getMousePos();
		value = clamp((maxValue - minValue) * (cursorPos.x - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
	}
	graphics->drawOverlay(Rect::XYXY(shape.x,0.5*shape.h+shape.y-0.010,shape.x+shape.w,0.5*shape.h+shape.y+0.010),"slider bar");

	float xv = (value-minValue)*shape.w/(maxValue-minValue)+shape.x;
	graphics->drawOverlay(Rect::XYXY(xv-0.021,shape.y,xv+0.021,shape.y+shape.h),"slider");
}
bool slider::mouseDownL(float X, float Y)
{
	if(view && active && abs(value*shape.w/(maxValue - minValue) + shape.x  - X) < 0.021 && Y > shape.y && Y < shape.y + shape.h)
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
bool slider::mouseUpL(float X, float Y)
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
		Vec2f cursorPos = input.getMousePos();
		value = clamp((maxValue - minValue) * (cursorPos.x - shape.x) / shape.w + minValue + mouseOffset, minValue, maxValue);
	}
	return value;
}
void slider::setMinValue(float m)
{
	if(clicking)
	{
		Vec2f cursorPos = input.getMousePos();
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
		Vec2f cursorPos = input.getMousePos();
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
	if(!dataManager.assetLoaded("default font"))
	{
#ifdef WINDOWS
		MessageBoxA(NULL,text.c_str(),"FighterPilot",MB_ICONERROR);
#else
		cout << text << endl;
		//TODO: add support for linux "message boxes"
#endif
	}
	else
	{
		gui::messageBox_c* m = new gui::messageBox_c;
		m->initialize(text);
		menuManager.setPopup(m);
	}
}
void closingMessage(string text,string title)
{
	if(!dataManager.assetLoaded("default font"))
	{
		graphics->destroyWindow();
#ifdef WINDOWS
		MessageBoxA(NULL,text.c_str(),title.c_str(),MB_ICONERROR);
#else
		cout << text << endl;
		//TODO: add support for linux "message boxes"
#endif
		exit(0);
	}
	else
	{
		gui::messageBox_c* m = new gui::messageBox_c;
		m->initialize(text);
		m->callback = [](gui::popup*){std::exit(0);};
		menuManager.setPopup(m);
	}
}
