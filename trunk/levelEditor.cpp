
#include "game.h"
#include "GL/glee.h"
#include <GL/glu.h>

namespace gui{

bool levelEditor::init()
{
	view = graphics->genView();
	view->viewport(0,0, sAspect,1.0);
	view->perspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	view->setRenderFunc(bind(&levelEditor::render3D, this, placeholders::_1));

	//terrain
	buttons["dSquare"]		= new button(sAspect-0.1,0.005,0.1,0.030,"d-square",lightGreen,white);
	buttons["faultLine"]	= new button(sAspect-0.1,0.040,0.1,0.030,"fault line",lightGreen,white);
	buttons["fromFile"]		= new button(sAspect-0.1,0.075,0.1,0.030,"from file",lightGreen,white);
	buttons["exportBMP"]	= new button(sAspect-0.1,0.110,0.1,0.030,"export",lightGreen,white);
	sliders["sea level"]	= new slider(sAspect-0.1,0.145,0.1,0.030,1.0,0.0);
	sliders["height scale"] = new slider(sAspect-0.1,0.180,0.1,0.030,1.0,-1.0);	sliders["height scale"]->setValue(0.0);

	buttons["load"]			= new button(0.938,0.965,0.1,0.030,"Load",Color(0.8,0.8,0.8),white);
	buttons["save"]			= new button(1.040,0.965,0.1,0.030,"Save",Color(0.8,0.8,0.8),white);
	buttons["exit"]			= new button(1.143,0.965,0.1,0.030,"Exit",Color(0.8,0.8,0.8),white);

	toggles["shaders"]		= new toggle(vector<button*>(),darkGreen,lightGreen,NULL,0);

	toggles["shaders"]->addButton(new button(0.005,0.005,0.195,0.030,"grass",black,white));
	toggles["shaders"]->addButton(new button(0.005,0.040,0.195,0.030,"snow",black,white));
	toggles["shaders"]->addButton(new button(0.005,0.075,0.195,0.030,"ocean",black,white));

	//objects
	buttons["addPlane"]		= new button(0.005,0.005,0.195,0.030,"new plane",lightGreen,white);
	buttons["addAAgun"]		= new button(0.005,0.040,0.195,0.030,"new AA gun",lightGreen,white);
#ifdef _DEBUG
	buttons["addSAMbattery"]= new button(0.005,0.075,0.195,0.030,"new SAM Battery",lightGreen,white);
	buttons["addFlakCannon"]= new button(0.005,0.110,0.195,0.030,"new Flak Cannon",lightGreen,white);
#endif

	//settings
	//v.clear();
	//v.push_back(new button(120,5,100,30,"respawn",black,white));
	//v.push_back(new button(225,5,100,30,"restart",black,white));
	//v.push_back(new button(330,5,100,30,"die",black,white));
	//toggles["onHit"]		= new toggle(v,darkBlue,lightBlue,new label(5,5,"player hit:"));

	//v.clear();
	//v.push_back(new button(120,45,100,30,"respawn",black,white));
	//v.push_back(new button(225,45,100,30,"restart",black,white));
	//v.push_back(new button(330,45,100,30,"die",black,white));
	//toggles["onAIHit"]		= new toggle(v,darkBlue,lightBlue,new label(5,45,"AI hit:"));

	//v.clear();
	//v.push_back(new button(120,85,100,30,"ffa",black,white));
	//v.push_back(new button(225,85,100,30,"teams",black,white));
	//v.push_back(new button(330,85,100,30,"player vs",black,white));
	//toggles["gameType"]	= new toggle(v,darkBlue,lightBlue,new label(5,85,"game type:"));

	//v.clear();
	//v.push_back(new button(120,125,100,30,"water",black,white));
	//v.push_back(new button(225,125,100,30,"land",black,white));
	//toggles["mapType"]	= new toggle(v,darkBlue,lightBlue,new label(5,125,"map type:"));

	//v.clear();
	//v.push_back(new button(120,165,100,30,"rock",black,white));
	//v.push_back(new button(225,165,100,30,"sand",black,white));
	//toggles["seaFloorType"]	= new toggle(v,darkBlue,lightBlue,new label(5,165,"sea floor:"));

	toggles["tabs"]	= new toggle(vector<button*>(),Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),NULL,0);
	toggles["tabs"]->addButton(new button(0.005,0.965,0.1,0.030,"Terrain",black,white));
	toggles["tabs"]->addButton(new button(0.107,0.965,0.1,0.030,"Objects",black,white));
	toggles["tabs"]->addButton(new button(0.210,0.965,0.1,0.030,"Regions",black,white));

	if(level != NULL) delete level;
	level = new editLevel;

	scrollVal=0.0;
	level->newGround(129,129);
	diamondSquare(0.17,0.5,2);
	smooth(1);

	resetView();
	return true;
}
void levelEditor::operator() (popup* p)
{
	if(awaitingMapFile)
	{
		awaitingMapFile=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((openFile*)p)->getFile();
		fromFile(f);
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
		level->exportBMP(f);
	}
	else if(awaitingLevelFile)
	{
		awaitingLevelFile=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((openFile*)p)->getFile();

		Level* l = new editLevel();
		if(l->init(f))
		{
			delete level;
			level = (editLevel*)l;

			maxHeight=level->ground()->getMaxHeight();
			minHeight=level->ground()->getMinHeight();
			sliders["sea level"]->setValue(-minHeight/(maxHeight - minHeight));
			resetView();
		}
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		level->save(f, sliders["sea level"]->getValue() * (maxHeight - minHeight) + minHeight);
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
		menuManager.setMenu(new gui::chooseMode);
		return 0;
	}
	else if(getTab() == TERRAIN)
	{
		if(buttons["faultLine"]->checkChanged())
		{
			faultLine();
		}
		else if(buttons["dSquare"]->checkChanged())
		{
			diamondSquare(0.17,0.5,1);
		}
		else if(buttons["fromFile"]->checkChanged())
		{
			set<string> s;
			s.insert(".bmp");
			s.insert(".bil");

			awaitingMapFile = true;
			popup* p = new openFile;
			p->callback = (functor<void,popup*>*)this;
			((openFile*)p)->init(s);
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
#ifdef _DEBUG
		else if(buttons["addSAMbattery"]->checkChanged())
		{
			awaitingNewObject=true;
			popup* p = new newObject(SAM_BATTERY);
			p->callback = (functor<void,popup*>*)this;
			menuManager.setPopup(p);
		}
		else if(buttons["addFlakCannon"]->checkChanged())
		{
			awaitingNewObject=true;
			popup* p = new newObject(FLAK_CANNON);
			p->callback = (functor<void,popup*>*)this;
			menuManager.setPopup(p);
		}
#endif
	}
	else if(getTab() == REGIONS)
	{

	}

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
#ifdef _DEBUG
			buttons["addSAMbattery"]->setVisibility(newTab==OBJECTS);
			buttons["addFlakCannon"]->setVisibility(newTab==OBJECTS);
#endif
		}
		if(lastTab == REGIONS || newTab==REGIONS || lastTab == (Tab)-1)
		{
		//	toggles["onHit"]->setVisibility(newTab==REGIONS);
		//	toggles["onAIHit"]->setVisibility(newTab==REGIONS);
		//	toggles["gameType"]->setVisibility(newTab==REGIONS);
		//	toggles["mapType"]->setVisibility(newTab==REGIONS);
		//	toggles["seaFloorType"]->setVisibility(newTab==REGIONS);
		}
	}
	lastTab = newTab;


	//if(input.getKey(0x52) && !popupActive())//r key
	//	rot+=value/1000;
	POINT p;
	GetCursorPos(&p);
	if(getTab() == REGIONS && (p.x < 2 || p.x > sw-2 || p.y < 2 || p.y > sh-2))
	{
		if(p.x < 2)		orthoCenter += Vec3f(0.25,0,0) * level->ground()->sizeX() * world.time.length() / 1000;
		if(p.x > sw-2)	orthoCenter -= Vec3f(0.25,0,0) * level->ground()->sizeX() * world.time.length() / 1000;
		if(p.y < 2)		orthoCenter += Vec3f(0,0,0.25) * level->ground()->sizeZ() * world.time.length() / 1000;
		if(p.y > sh-2)	orthoCenter -= Vec3f(0,0,0.25) * level->ground()->sizeZ() * world.time.length() / 1000;
	}
	else if(!input.getMouseState(MIDDLE_BUTTON).down && (p.x < 2 || p.x > sw-2 || p.y < 2 || p.y > sh-2))
	{
		if(p.x < 2)		center -= rot * Vec3f(0.25,0,0) * level->ground()->sizeX() * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.x > sw-2)	center += rot * Vec3f(0.25,0,0) * level->ground()->sizeX() * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.y < 2)		center -= rot * Vec3f(0,0,0.25) * level->ground()->sizeZ() * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.y > sh-2)	center += rot * Vec3f(0,0,0.25) * level->ground()->sizeZ() * world.time.length() / 1000 * pow(1.1f,-scrollVal);
	}

	return 7;
}
void levelEditor::render()
{
	menuManager.drawCursor();
}
bool levelEditor::mouse(mouseButton button, bool down)
{
	Vec2f p = down ? input.getMouseState(button).downPos : input.getMouseState(button).upPos;

	if(button == LEFT_BUTTON)
	{
		if(getTab() == OBJECTS && down)
		{
			if(newObjectType != 0)
			{
				static int teamNum=0;
				addObject(newObjectType, teamNum, teamNum<=1 ? PLAYER_HUMAN : PLAYER_COMPUTER, p.x, p.y);
				newObjectType = 0;
				teamNum++;
				return true;
			}
			else if(!objectCircles.empty())
			{
				float minDist;
				map<int,Circle<float>>::iterator min = objectCircles.end();
				for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
				{
					float d = i->second.center.distanceSquared(p);
					if(d < i->second.radius*i->second.radius && (min == objectCircles.end() || d < minDist))
					{
						minDist = d;
						min = i;
					}
				}

				if(min != objectCircles.end())
				{
					//object found
					auto* m = new objectProperties();
					m->init(((editLevel*)level)->getObject(min->first));
					menuManager.setPopup(m);
					return true;
				}
				return false;
			}
		}
		else if(getTab() == REGIONS)
		{

			Rect view = orthoView();

			p.x = (p.x*view.w/sAspect + view.x) - orthoCenter.x;
			p.y = (p.y*view.h + view.y) - orthoCenter.z;
			if(down)
			{
				newRegionRadius = true;
				newRegionCenter = p;
			}
			else if(newRegionRadius)
			{
				level->addRegionCircle(newRegionCenter, newRegionCenter.distance(p));
			}
		}
	}
	else if(button == MIDDLE_BUTTON)
	{
		if(!down)
		{
			Vec2f oldP = input.getMouseState(MIDDLE_BUTTON).downPos;
			if(oldP == p)
				return true;

			Vec3f xAxis = rot * Vec3f(-1,0,0);

			Vec3f axis = xAxis * (p.y-oldP.y) + Vec3f(0,-1,0) * (p.x-oldP.x);
			Angle ang = oldP.distance(p);
			rot = Quat4f(axis,ang) * rot;
			return true;
		}
	}

	return false;
}
bool levelEditor::scroll(float rotations)
{
	if(getTab() == REGIONS)
		orthoScale = clamp(orthoScale + rotations,-8,8);
	else
		scrollVal = clamp(scrollVal + rotations,-8,25);
	return true;
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
	if(t == 2) return REGIONS;
	return NO_TAB;
}
void levelEditor::resetView()
{
	rot = Quat4f(Vec3f(1,0,0),1.0);
	center = Vec3f(level->ground()->sizeX()/2,minHeight,level->ground()->sizeZ()/2);

	orthoCenter = Vec3f(level->ground()->sizeX()/2,minHeight,level->ground()->sizeZ()/2);
	orthoScale = 0.0;
}
void levelEditor::updateObjectCircles()
{
	objectCircles.clear();
	int n=0;
	for(auto i = level->objects().begin(); i!= level->objects().end(); i++, n++)
	{
		Vec2f s;
		float r;

		auto model = dataManager.getModel(i->type);
		if(model==NULL)
		{
			r = 0.006;
			s = view->project(i->startloc);
		}
		else
		{
			Sphere<float> sphere = model->boundingSphere;
			s = view->project(i->startloc + i->startRot * sphere.center*10);
			Vec2f t = view->project(i->startloc + sphere.center + view->camera().up*sphere.radius*10);
			r = max(0.004f,s.distance(t));

			
		}
		if(/*frustum.sphereInFrustum(i->startloc,r)!=FrustumG::OUTSIDE &&*/ s.x > -r && s.x < sAspect+r && s.y > -r && s.y < 1.0+r)
		{
			objectCircles[n] = Circle<float>(Vec2f(s.x,s.y),r);
		}
	}
}
float levelEditor::randomDisplacement(float h1, float h2, float d)
{
	d *= 65;
	float r = random(-d/2,d/2);
	return (h1 + h2 + r) / 2.0;
 }
float levelEditor::randomDisplacement(float h1, float h2,float h3, float h4, float d)
{
	d *= 65;
	float r = random(-d/2,d/2);
	return (h1 + h2 + h3 + h4 + r) / 4.0;
}
void levelEditor::diamondSquareFill(int x1, int y1, int x2, int y2)
{
	if(x1 == x2 && y1 == y2)
		return;

	auto& g = *level->ground();

//	float h[5] = {g(x1,y1), g(x1,y2), g(x2,y1), g(x2,y2), (float)min(x2-x1,y2-y1)};

	int midx = (x1 + x2) / 2;
	int midy = (y1 + y2) / 2;

	if(x2-x1 > 1 && y1==0)	g(midx, y1, randomDisplacement(g(x1,y1), g(x2,y1), x2-x1) );
	if(y2-y1 > 1 && x1==0)	g(x1, midy, randomDisplacement(g(x1,y1), g(x1,y2), y2-y1) );

	if(x2-x1 > 1)	g(midx, y2, randomDisplacement(g(x1,y2), g(x2,y2), x2-x1) );
	if(y2-y1 > 1)	g(x2, midy, randomDisplacement(g(x2,y1), g(x2,y2), y2-y1) );

	if(x2-x1 > 1 && y2-y1 > 1)
		g(midx, midy, randomDisplacement(g(x1,y1), g(x1,y2), g(x2,y1), g(x2,y2), min(x2-x1,y2-y1)) );


	if(x2-x1 > 1 || y2-y1 > 1 )
	{
		diamondSquareFill(x1,	y1,		midx,	midy);
		diamondSquareFill(midx, y1,		x2,		midy);
		diamondSquareFill(x1,	midy,	midx,	y2);
		diamondSquareFill(midx,	midy,	x2,		y2);
	}
}
void levelEditor::diamondSquare(float h, float m, int subdivide)//mapsize must be (2^x+1, 2^x+1)!!!
{
	int sLen=max(level->ground()->resolutionX(),level->ground()->resolutionZ());
	if( !(!((sLen-1) & (sLen-2) ) && sLen > 1) || level->ground()->resolutionX() != level->ground()->resolutionZ())//if v is not one more than a power of 2
	{
		sLen--;
		sLen |= sLen >> 1;
		sLen |= sLen >> 2;
		sLen |= sLen >> 4;
		sLen |= sLen >> 8;
		sLen |= sLen >> 16;
		sLen+=2;//changes mapsize to one more than a power of 2
		level->newGround(sLen,sLen);
	}

	//set corners

	int x, y;

	//h=pow(2.0f,-h);

	for(x=0;x<sLen;x++)
	{
		for(y=0;y<sLen;y++)
		{
			level->ground()->setHeight(x,0,y);
		}
	}
	//level->ground()->setHeight(0,0,0);
	//level->ground()->setHeight(0,0,sLen-1);
	//level->ground()->setHeight(sLen-1,0,sLen-1);
	//level->ground()->setHeight(sLen-1,0,0);

	int n = (sLen-1)/subdivide;
	for(x=0;x<subdivide;x++)
	{
		for(y=0;y<subdivide;y++)
		{
			diamondSquareFill(n*x, n*y, n*(x+1), n*(y+1));
		}
	}
	smooth(1);

	level->ground()->setMinMaxHeights();
	minHeight=level->ground()->getMinHeight();

	//for(x=0;x<sLen;x++)					//island
	//{
	//	for(y=0;y<sLen;y++)
	//	{
	//		level->ground()->setHeight(x,(level->ground()->height(x,y)-minHeight)*clamp(2.0-2.0*sqrt((x-sLen/2)*(x-sLen/2)+(y-sLen/2)*(y-sLen/2))/(sLen/2),1.0,0.0),y);
	//	}
	//}
	//

	level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));



	level->ground()->setMinMaxHeights();
	maxHeight=level->ground()->getMaxHeight();
	minHeight=level->ground()->getMinHeight();
	sliders["sea level"]->setValue(0.333);
	resetView();
}
void levelEditor::faultLine()
{
	float pd;
	float a,b,c;
	float disp=0.5  * 100.0;
	float x,y;
	for(x=0;x<level->ground()->resolutionX();x++)
	{
		for(y=0;y<level->ground()->resolutionZ();y++)
		{
			level->ground()->setHeight(x,0,y);
		}
	}
	for(int i=0;i<100;i++)
	{
		a = random(-1.0f,1.0f);
		b = random(-1.0f,1.0f);
		c = random(-0.5f,0.5f) * level->ground()->resolutionX();
		for(x=0;x<level->ground()->resolutionX();x++)
		{
			for(y=0;y<level->ground()->resolutionZ();y++)
			{
				pd = ((x-level->ground()->resolutionX()/2) * a + (y-level->ground()->resolutionZ()/2) * b + c)/(128.0/30)/2;//  ***/2500
				if (pd > 1.57) pd = 1.57;
				else if (pd < 0.05) pd = 0.05;
				level->ground()->increaseHeight(x,-disp/2 + sin(pd)*disp,y);
			}
		}
	}

	//float h;
	//for(x=0;x<level->ground()->resolutionX();x++)
	//{
	//	for(y=0;y<level->ground()->resolutionZ();y++)
	//	{
	//		h = level->ground()->rasterHeight(x,y);
	//		if(h>maxHeight) maxHeight=h;
	//		if(h<minHeight) minHeight=h;
	//	}
	//}

	level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));

	level->ground()->setMinMaxHeights();
	maxHeight=level->ground()->getMaxHeight();
	minHeight=level->ground()->getMinHeight();
	sliders["sea level"]->setValue(0.333);
	resetView();
}
void levelEditor::fromFile(string filename)
{
	string ext = fileManager.extension(filename);
	if(ext == ".bmp")
	{
		auto image = fileManager.loadBmpFile(filename);
		if(image->valid() && image->width > 0 && image->height > 0)
		{
			float* t = new float[image->width * image->height];
			for(int y = 0; y < image->height; y++) {
				for(int x = 0; x < image->width; x++) {
					t[y * image->width + x] = (unsigned char)image->contents[image->channels * (y * image->width + x)] * 10.0;
				}
			}
			//assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL");
			level->newGround(image->height,image->width,t);
			level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));
			delete[] t;


			level->ground()->setMinMaxHeights();
			maxHeight=level->ground()->getMaxHeight();
			minHeight=level->ground()->getMinHeight();
			sliders["sea level"]->setValue(0.333);
			resetView();
		}
	}
	else if(ext == ".bil")
	{
		int nRows;
		int nColumns;
		int bpp;

		float xRes;
		float zRes;


		string line;
		vector<string> tokens;

		string file = fileManager.changeExtension(filename, ".hdr");
		ifstream fin(file);
		if(!fin.is_open())
			return;

		while(fin.good())
		{
			getline(fin,line);
			boost::split(tokens,line,boost::is_any_of(" "));

			if(tokens.size() < 2)
				continue;

			try{
				if(tokens[0] == "NROWS")			nRows =		lexical_cast<int>(tokens[1]);
				else if(tokens[0] == "NCOLS")		nColumns =	lexical_cast<int>(tokens[1]);
				else if(tokens[0] == "NBITS")		bpp =		lexical_cast<int>(tokens[1]) / 8;
				else if(tokens[0] == "XDIM")		xRes =		lexical_cast<float>(tokens[1]);
				else if(tokens[0] == "YDIM")		zRes =		lexical_cast<float>(tokens[1]);
			}catch(...){}
		}
		fin.close();

		fin.open(filename,ios::binary);
		if(!fin.is_open())
			return;

		unsigned char* heights = new unsigned char[nRows*nColumns*bpp];
		fin.read((char*)heights,nRows*nColumns*bpp);

		float* fHeights = new float[nRows*nColumns];

		if(bpp == 1)
		{
			for(int x = 0; x < nRows; x++)
			{
				for(int y = 0; y < nColumns; y++)
				{
					fHeights[x + y*nRows] = heights[x + y*nRows];
				}
			}
		}
		if(bpp == 2)
		{
			for(int x = 0; x < nRows; x++)
			{
				for(int y = 0; y < nColumns; y++)
				{
					fHeights[x + y*nRows] = ((__int16*)heights)[x + y*nRows];
				}
			}
		}
		if(bpp == 4)
		{
			for(int x = 0; x < nRows; x++)
			{
				for(int y = 0; y < nColumns; y++)
				{
					fHeights[x + y*nRows] = ((__int32*)heights)[x + y*nRows];
				}
			}
		}
		fin.close();


		level->newGround(nRows,nColumns,fHeights);
		delete[] heights;
		delete[] fHeights;

		level->ground()->setMinMaxHeights();
		maxHeight=level->ground()->getMaxHeight();
		minHeight=level->ground()->getMinHeight();
		sliders["sea level"]->setValue(0.0);
		level->ground()->setSizeX(nRows * 30.87 * xRes);
		level->ground()->setSizeZ(nColumns * 30.87 * zRes);
		resetView();
	}
}
void levelEditor::smooth(int a)
{
	auto& g = *level->ground();
	int w = g.resolutionX();
	int h = g.resolutionZ();
	float* smoothed = new float[w*h];
	memcpy(smoothed, g.heights, w * h * sizeof(float));

	float s;
	int n;
	for(int x=0; x < w; x++)
	{
		for(int y=0; y < h; y++)
		{
			s=0;
			n=0;
			for(int i = max(x-a,0); i <= min(x+a,w-1); i++)
			{
				for(int j = max(y-a,0); j <= min(y+a,h-1); j++)
				{
					s += g(i,j);
					n++;
				}
			}
			smoothed[x+y*w] = s/n;
		}
	}

	memcpy(g.heights, smoothed, w * h * sizeof(float));
	delete[] smoothed;
}
void levelEditor::addObject(int type, int team, int controlType, float x, float y)//in screen coordinates
{
	Vec2f cursorPos = input.getMousePos();
	Vec3d P0 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
	Vec3d P1 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
	Vec3d dir = P0-P1;

	if(abs(dir.y) < 0.001) return;
	Vec3d val = P1 + dir*(maxHeight+objPlacementAlt-P1.y)/dir.y;
	((editLevel*)level)->addObject(type,team,controlType,Vec3f(val.x,val.y,val.z));
}
Rect levelEditor::orthoView()
{
	Vec2f gSize = level->ground()->size() * 1.2 * pow(1.3f,-orthoScale);
	gSize.x *= sAspect;

	return Rect::CWH(Vec2f(0,0),gSize);
}
void levelEditor::render3D(unsigned int v)
{
	bool orthoTerrain = (getTab() == REGIONS);

	if(orthoTerrain)
	{
		Rect viewRect = orthoView();
		view->ortho(viewRect.x,viewRect.x+viewRect.w,viewRect.y,viewRect.y+viewRect.h,-10000,10000);
		view->lookAt(orthoCenter+Vec3f(0,10000,0),orthoCenter,Vec3f(0,0,1));

		if(getShader() != -1)
			((Level::heightmapGL*)level->ground())->setShader(toggles["shaders"]->getValue() + 2);
		bool w = getShader() != 1;
		float sl = sliders["sea level"]->getValue();

		glPushMatrix();
	//	glScalef(0.01*pow(10.0f,orthoScale),1,0.01*pow(10.0f,orthoScale));
		//glScalef(1,pow(10.0f,sliders["height scale"]->getValue()),1);
 		level->renderPreview(w,pow(10.0f,sliders["height scale"]->getValue()),sl * (maxHeight - minHeight) + minHeight);
		glPopMatrix();

		if(getTab() == REGIONS)
		{
			dataManager.bind("circle shader");
			glDisable(GL_DEPTH_TEST);

			auto v = level->regions();
			for(auto i = v.begin(); i != v.end(); i++)
			{
				Vec2f c((i->centerXYZ[0] - viewRect.x + orthoCenter.x)/viewRect.w*sAspect, (i->centerXYZ[2] - viewRect.y + orthoCenter.z)/viewRect.h);
				Vec2f s(i->radius/viewRect.w*sAspect*2.0, i->radius/viewRect.h*2.0);
				graphics->drawOverlay(Rect::CWH(c,s));
			}

			dataManager.unbindShader();
			glEnable(GL_DEPTH_TEST);
		}
	}
	else
	{
		view->perspective(80.0, (double)sw / ((double)sh),10.0, 500000.0);
		//static FrustumG frustum;
		//frustum.setCamInternals(80.0, (double)sw / ((double)sh),10.0, 500000.0);


		Vec3f e,c,u;
		c = center;
		if(input.getMouseState(MIDDLE_BUTTON).down)
		{
			Vec2f oldP = input.getMouseState(MIDDLE_BUTTON).downPos;
			Vec2f newP = input.getMousePos();


			Vec3f xAxis = rot * Vec3f(-1,0,0);

			Vec3f axis = (xAxis * (newP.y-oldP.y) + Vec3f(0,-1,0) * (newP.x-oldP.x)).normalize();
			Angle ang = oldP.distance(newP);

			Quat4f tmpRot;
			if(ang > 0.01)	tmpRot = Quat4f(axis,ang) * rot;
			else			tmpRot = rot;

			e = c + tmpRot * Vec3f(0,0.75,0) * max(level->ground()->sizeX(),level->ground()->sizeZ()) * pow(1.1f,-scrollVal);
			u = tmpRot * Vec3f(0,0,-1);

		}
		else
		{
			e = c + rot * Vec3f(0,0.75,0) * max(level->ground()->sizeX(),level->ground()->sizeZ()) * pow(1.1f,-scrollVal);
			u = rot * Vec3f(0,0,-1);
		}
		view->lookAt(e,c,u);

		graphics->setLightPosition(Vec3f(30, 70, 40));

		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		if(getShader() != -1)
			((Level::heightmapGL*)level->ground())->setShader(toggles["shaders"]->getValue() + 2);
		bool w = getShader() != 1;
		float sl = sliders["sea level"]->getValue();

		glPushMatrix();
		//glScalef(1,pow(10.0f,sliders["height scale"]->getValue()),1);
 		level->renderPreview(w,pow(10.0f,sliders["height scale"]->getValue()),sl * (maxHeight - minHeight) + minHeight);
		glPopMatrix();
	}

	

	if(getTab() == OBJECTS)
	{
		level->renderObjectsPreview();
		if(newObjectType != 0)
		{
			////////////////////////////////draw object//////////////////////////////////

			Vec2f cursorPos = input.getMousePos();
			Vec3d P0 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
			Vec3d P1 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
			Vec3d dir = P0-P1;

			if(abs(dir.y) < 0.001) return;
			Vec3d val = P1 + dir*(maxHeight+objPlacementAlt-P1.y)/dir.y;
			graphics->drawModel(newObjectType, Vec3f(val), Quat4f(),10.0);
			////////////////////////////////draw grid////////////////////////////////// --- SHOULD BE REWRITTEN
			//glDepthMask(false);
			//glColor4f(0.1,0.3,1.0,0.3);

			//graphics->drawQuad(	Vec3f(0,						maxHeight+objPlacementAlt,	0),
			//					Vec3f(0,						maxHeight+objPlacementAlt,	level->ground()->sizeZ()),
			//					Vec3f(level->ground()->sizeX(),	maxHeight+objPlacementAlt,	0),
			//					Vec3f(level->ground()->sizeX(),	maxHeight+objPlacementAlt,	level->ground()->sizeZ()));

			//glColor4f(0.3,0.5,1.0,0.2);
			//for(float f=0.0; f<level->ground()->sizeX() + 0.001; f+=level->ground()->sizeX() / 32.0)
			//{
			//	graphics->drawLine(Vec3f(f,maxHeight+10,0), Vec3f(f,maxHeight+10,level->ground()->sizeZ()));
			//	//glVertex3f(f,maxHeight+10,0);
			//	//glVertex3f(f,maxHeight+10,level->ground()->sizeZ());
			//}
			//for(float f=0.0; f<level->ground()->sizeZ() + 0.001; f+=level->ground()->sizeZ() / 32.0)
			//{
			//	graphics->drawLine(Vec3f(0,maxHeight+10,f), Vec3f(level->ground()->sizeX(),maxHeight+10,f));
			//	//glVertex3f(0,maxHeight+10,f);
			//	//glVertex3f(level->ground()->sizeX(),maxHeight+10,f);
			//}
			//glColor3f(1,1,1);
			//glDepthMask(true);
			////////////////////////////////end grid///////////////////////////////////
		}

		glDisable(GL_DEPTH_TEST);

		//glBindTexture(GL_TEXTURE_2D,0);
		//glMatrixMode(GL_PROJECTION);
		//glPushMatrix();
		//glLoadIdentity();
		//glOrtho(0, 1, (float)sh/sw , 0, -1, 1);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();


		
		//dataManager.bind("ortho");
		dataManager.bind("circle shader");

	//	glColor3f(0,1,0);

		updateObjectCircles();
		for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
		{
			graphics->drawOverlay(Rect::CWH(i->second.center.x, i->second.center.y,	i->second.radius*2, i->second.radius*2)/*,"target ring"*/);
		}
		dataManager.unbindShader();

		//for(auto i = level->objects().begin(); i!= level->objects().end(); i++)
		//{
		//	Vec2f s = frustum.project(i->startloc);
		//	float r;

		//	auto p = dataManager.getModel(i->type);
		//	if(p==NULL)
		//	{
		//		r = 0.006;
		//	}
		//	else
		//	{
		//		Vec2f t = frustum.project(i->startloc + p->getCenter() + u*p->getRadius()*10);
		//		r = max(0.004,s.distance(t));
		//	}
		//	if(frustum.sphereInFrustum(i->startloc,r)!=FrustumG::OUTSIDE && s.x > -r && s.x < 1.0+r && s.y > -r && s.y < 1.0+r)
		//	{
		//		graphics->drawOverlay(s.x - r, s.y*sh/sw - r, s.x + r, s.y*sh/sw + r,"target ring");
		//	}
		//}

	//	glMatrixMode( GL_PROJECTION );			// Select Projection
	//	glPopMatrix();							// Pop The Matrix
	//	glMatrixMode( GL_MODELVIEW );			// Select Modelview
		glEnable(GL_DEPTH_TEST);
	}
}
}
