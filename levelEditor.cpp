
#include "main.h"

namespace menu{

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
//	addShader("island");
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


	if(level != NULL) delete level;
	level = new editLevel;

	scrollVal=0.0;
	level->newGround(65,65);
	diamondSquare(0.17,0.5,1);
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
		delete level;
		LevelFile l(f);
		level = new editLevel(l);
		maxHeight=level->ground()->getMaxHeight();
		minHeight=level->ground()->getMinHeight();
		sliders["sea level"]->setValue(-minHeight/(maxHeight - minHeight));
		resetView();
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		LevelFile l = level->getLevelFile(/*sliders["sea level"]->getValue() * (mode->maxHeight - mode->minHeight) + mode->minHeight*/);
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
		}
		if(lastTab == REGIONS || newTab==REGIONS || lastTab == (Tab)-1)
		{
			toggles["onHit"]->setVisibility(newTab==REGIONS);
			toggles["onAIHit"]->setVisibility(newTab==REGIONS);
			toggles["gameType"]->setVisibility(newTab==REGIONS);
			toggles["mapType"]->setVisibility(newTab==REGIONS);
			toggles["seaFloorType"]->setVisibility(newTab==REGIONS);
		}
	}
	lastTab = newTab;


	//if(input->getKey(0x52) && !menuManager.getMenu()->popupActive())//r key
	//	rot+=value/1000;
	POINT p;
	GetCursorPos(&p);
	if(!input->getMouseState(MIDDLE_BUTTON).down && (p.x < 2 || p.x > sw-2 || p.y < 2 || p.y > sh-2))
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
	Vec2f p = down ? input->getMouseState(button).downPos : input->getMouseState(button).upPos;

	if(button == LEFT_BUTTON)
	{
		if(getTab() == OBJECTS && down)
		{
			if(newObjectType != 0)
			{
				static int teamNum=0;
				addObject(newObjectType, teamNum, teamNum<=1 ? CONTROL_HUMAN : CONTROL_COMPUTER, p.x * sh, p.y * sh);
				newObjectType = 0;
				teamNum++;
				return true;
			}
			else
			{
				return selectObject(p.x * sh, p.y * sh);
			}
		}
	}
	else if(button == MIDDLE_BUTTON)
	{
		if(!down)
		{
			Vec2f oldP = input->getMouseState(MIDDLE_BUTTON).downPos;
			if(oldP == p) 
				return true;

			Vec3f xAxis = rot * Vec3f(1,0,0);

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
	scrollVal = clamp(scrollVal + rotations,-8,25);
	return true;
}
void levelEditor::addShader(string filename)
{
	//int s = dataManager.loadTerrainShader(filename);//((mapBuilder*)mode)->loadShader("media/terrain.vert",(char*)(string("media/")+filename).c_str());
	//shaderButtons.push_back(s);

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
	if(t == 2) return REGIONS;
	return NO_TAB;
}
void levelEditor::resetView()
{
	rot = Quat4f(Vec3f(1,0,0),1.0);
	center = Vec3f(level->ground()->sizeX()/2,minHeight,level->ground()->sizeZ()/2);
}
bool levelEditor::selectObject(int x, int y)
{
	if(objectCircles.empty())
		return false;

	Vec2f mouse((float)x/sw,(float)y/sw);


	float minDist;
	map<int,circle<float>>::iterator min = objectCircles.end();
	for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
	{
		float d = i->second.center.distanceSquared(mouse);
		if(d < i->second.radius*i->second.radius && (min == objectCircles.end() || d < minDist))
		{
			minDist = d;
			min = i;
		}
	}

	if(min != objectCircles.end())
	{
		//object found
		auto* m = new menu::objectProperties();
		m->init(((editLevel*)level)->getObject(min->first));
		menuManager.setPopup(m);
		return true;
	}
	return false;
}
void levelEditor::updateObjectCircles()
{
	objectCircles.clear();
	int n=0;
	for(auto i = level->objects().begin(); i!= level->objects().end(); i++, n++)
	{
		Vec2f s = graphics->project(i->startloc);
		float r;

		auto p = dataManager.getModel(i->type);
		if(p==NULL)
		{
			r = 0.006;
		}
		else
		{
			Vec2f t = graphics->project(i->startloc + p->getCenter() + graphics->getView().camera.up*p->getRadius()*10);
			r = max(0.004,s.distance(t));
		}
		if(/*frustum.sphereInFrustum(i->startloc,r)!=FrustumG::OUTSIDE &&*/ s.x > -r && s.x < 1.0+r && s.y > -r && s.y < 1.0+r)
		{
			objectCircles[n] = circle<float>(Vec2f(s.x,s.y*sh/sw),r);
		}
	}
}
float levelEditor::randomDisplacement(float h1, float h2, float d)
{
	d *= 30;
	float r = random(-d/2,d/2);
	return (h1 + h2 + r) / 2.0;
 }
float levelEditor::randomDisplacement(float h1, float h2,float h3, float h4, float d)
{
	d *= 30;
	float r = random(-d/2,d/2);
	return (h1 + h2 + h3 + h4 + r) / 4.0;
}
void levelEditor::diamondSquareFill(int x1, int y1, int x2, int y2)
{
	if(x1 == x2 && y1 == y2) 
		return;

	auto& g = *level->ground();

	int midx = (x1 + x2) / 2;
	int midy = (y1 + y2) / 2;

	if(x2-x1 > 1 && y1==0)	g(midx, y1, randomDisplacement(g(x1,y1), g(x2,y1), x2-x1) );
	if(y2-y1 > 1 && x1==0)	g(x1, midy, randomDisplacement(g(x1,y1), g(x1,y2), y2-y1) );

	if(x2-x1 > 1)	g(midx, y2, randomDisplacement(g(x1,y2), g(x2,y2), x2-x1) );
	if(y2-y1 > 1)	g(x2, midy, randomDisplacement(g(x2,y1), g(x2,y2), y2-y1) );

	if(x2-x1 > 1 && y2-y1 > 1)	
		g(midx, midy, randomDisplacement(g(x1,y1), g(x1,y2), g(x2,y1), g(x2,y2), min(x2-x1,y2-y1)) );


	if(x2-x1 > 1 || y2-y1 > 1)
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

	int x, z, numSquares, squareSize;

	float c[4];

	//h=pow(2.0f,-h);
	float rVal=h, y;
		

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
	for(int x=0;x<subdivide;x++)
	{
		for(int y=0;y<subdivide;y++)
		{
			diamondSquareFill(n*x, n*y, n*(x+1), n*(y+1));
		}
	}
//	smooth(1);

	//for(int itt=3; (0x1 << itt) < (level->ground()->resolutionX()-1);itt++, rVal*=m)
	//{
	//	numSquares = 0x1 << itt;
	//	squareSize = (sLen-1)/numSquares;
	//	//diamond
	//	for(x = 0; x < numSquares; x++)
	//	{
	//		for(z = 0; z < numSquares; z++)
	//		{
	//			y = (level->ground()->rasterHeight(x*squareSize,z*squareSize)+
	//				level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize)+
	//				level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize+squareSize)+
	//				level->ground()->rasterHeight(x*squareSize,z*squareSize+squareSize))/4;
	//			y += random(1000.0*rVal);

	//			level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize+squareSize/2);
	//		}
	//	}
	//	//square
	//	for(x = 0; x < numSquares; x++)
	//	{
	//		for(z = 0; z < numSquares; z++)
	//		{
	//			//left
	//			c[0]=level->ground()->rasterHeight(x*squareSize,z*squareSize);
	//			c[1]=level->ground()->rasterHeight(x*squareSize,z*squareSize+squareSize);
	//			c[2]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
	//			c[3]=level->ground()->rasterHeight(x*squareSize-squareSize/2,z*squareSize+squareSize/2);
	//			if(x==0)			y=(c[0]+c[1]+c[2])/3	  +	random(1000.0*rVal);
	//			else				y=(c[0]+c[1]+c[2]+c[3])/4 + random(1000.0*rVal);
	//			level->ground()->setHeight(x*squareSize,y,z*squareSize+squareSize/2);


	//			//top
	//			c[0]=level->ground()->rasterHeight(x*squareSize,z*squareSize);
	//			c[1]=level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize);
	//			c[2]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
	//			c[3]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize-squareSize/2);
	//			if(z==0)			y=(c[0]+c[1]+c[2])/3	  +	random(1000.0*rVal);
	//			else				y=(c[0]+c[1]+c[2]+c[3])/4 + random(1000.0*rVal);
	//			level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize);

	//			if(x == numSquares-1)//right
	//			{
	//				c[0]=level->ground()->rasterHeight((x+1)*squareSize,z*squareSize);
	//				c[1]=level->ground()->rasterHeight((x+1)*squareSize,z*squareSize+squareSize);
	//				//c[2]=getHeight((x+1)*squareSize+squareSize/2,z*squareSize+squareSize/2);
	//				c[3]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
	//				y=(c[0]+c[1]+c[3])/3	  +	random(1000.0*rVal);
	//				//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
	//				level->ground()->setHeight((x+1)*squareSize,y,z*squareSize+squareSize/2);
	//			}
	//			if(z == numSquares-1)//bottom
	//			{
	//				c[0]=level->ground()->rasterHeight(x*squareSize,z*squareSize+squareSize);
	//				c[1]=level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize+squareSize);
	//				//c[2]=getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
	//				c[3]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
	//				y=(c[0]+c[1]+c[3])/3	  +	random(1000.0*rVal);
	//				//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
	//				level->ground()->setHeight(x*squareSize+squareSize/2,y,(z+1)*squareSize);
	//			}
	//		}
	//	}
	//	//rVal*=h;
	//}
	////for(x = 0; x < level->ground()->getSize(); x++)
	////	level->ground()->setHeight(x,getHeight(x,0),level->ground()->getSize()-2);
	////for(z = 0; z < level->ground()->getSize(); z++)
	////	level->ground()->setHeight(level->ground()->getSize()-2,getHeight(x,z),z);
	////maxHeight=-9999;
	////minHeight=9999;
	////float h;
	////for(x=0;x<level->ground()->resolutionX();x++)
	////{
	////	for(y=0;y<level->ground()->resolutionZ();y++)
	////	{
	////		h = level->ground()->rasterHeight(x,y);
	////		if(h>maxHeight) maxHeight=h;
	////		if(h<minHeight) minHeight=h;
	////	}
	////}
	
	
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

	level->ground()->setSize(Vec2f(level->ground()->resolutionX()*40,level->ground()->resolutionZ()*40));



	level->ground()->setMinMaxHeights();
	maxHeight=level->ground()->getMaxHeight();
	minHeight=level->ground()->getMinHeight();
	((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.333);
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
	((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.333);
	resetView();
}
void levelEditor::fromFile(string filename)
{
	string ext = extension(filename);
	if(ext == ".bmp")
	{
		Image* image = loadBMP(filename.c_str());
		float* t = new float[image->width * image->height];
		for(int y = 0; y < image->height; y++) {
			for(int x = 0; x < image->width; x++) {
				t[y * image->width + x] = (unsigned char)image->pixels[3 * (y * image->width + x)] * 10.0;
			}
		}
		//assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL"); 
		level->newGround(image->height,image->width,t);
		level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));
		delete[] t;
		delete image;
		
		
		level->ground()->setMinMaxHeights();
		maxHeight=level->ground()->getMaxHeight();
		minHeight=level->ground()->getMinHeight();
		((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.333);
		resetView();
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
		 
		string file = change_extension(filename, ".hdr").generic_string();
		ifstream fin(file);
		if(!fin.is_open())
			return;

		while(fin.good())
		{
			getline(fin,line);
			split(tokens,line,is_any_of(" "));

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
		((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.0);
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
void levelEditor::addObject(int type, int team, int controlType, int x, int y)//in screen coordinates
{
	Vec2f cursorPos = input->getMousePos();
	Vec3d P0 = graphics->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
	Vec3d P1 = graphics->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
	Vec3d dir = P0-P1;
				
	if(abs(dir.y) < 0.001) return;
	Vec3d val = P1 + dir*(maxHeight+objPlacementAlt-P1.y)/dir.y;
	((editLevel*)level)->addObject(type,team,controlType,Vec3f(val.x,val.y,val.z));
}
void levelEditor::render3D() 
{	
	graphics->perspective(80.0, (double)sw / ((double)sh),10.0, 500000.0);
	//static FrustumG frustum;
	//frustum.setCamInternals(80.0, (double)sw / ((double)sh),10.0, 500000.0);
	

	Vec3f e,c,u;
	c = center;
	if(input->getMouseState(MIDDLE_BUTTON).down)
	{
		POINT p;
		GetCursorPos(&p);

		Vec2f oldP = input->getMouseState(MIDDLE_BUTTON).downPos;
		Vec2f newP((float)p.x/sh,1.0 - (float)p.y/sh);
		
			
		Vec3f xAxis = rot * Vec3f(1,0,0);

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
	graphics->lookAt(e,c,u);

	GLfloat lightPos0[] = {-0.3f, 0.7f, -0.4f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	if(getShader() != -1)
		((Level::heightmapGL*)level->ground())->setShader(toggles["shaders"]->getValue() + 2);
	bool w = getShader() != 1;
	float sl = sliders["sea level"]->getValue();

	glPushMatrix();
	glScalef(1,pow(10.0f,sliders["height scale"]->getValue()),1);
 	level->renderPreview(w,sl * (maxHeight - minHeight) + minHeight);
	glPopMatrix();

	if(getTab() == menu::levelEditor::OBJECTS)
	{
		level->renderObjectsPreview();
		if(placingObject() != 0)
		{
			////////////////////////////////draw object//////////////////////////////////

			Vec2f cursorPos = input->getMousePos();
			Vec3d P0 = graphics->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
			Vec3d P1 = graphics->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
			Vec3d dir = P0-P1;
				
			if(abs(dir.y) < 0.001) return;
			Vec3d val = P1 + dir*(maxHeight+objPlacementAlt-P1.y)/dir.y;
			glPushMatrix();
			glTranslatef(val.x,val.y,val.z);
			glScalef(10,10,10);
			dataManager.draw((planeType)((menu::levelEditor*)menuManager.getMenu())->placingObject());
			glPopMatrix();
			////////////////////////////////draw grid////////////////////////////////// --- SHOULD BE REWRITTEN
			glDepthMask(false);
			glColor4f(0.1,0.3,1.0,0.3);
				
			graphics->drawQuad(	Vec3f(0,						maxHeight+objPlacementAlt,	0),
								
								Vec3f(0,						maxHeight+objPlacementAlt,	level->ground()->sizeZ()),
								Vec3f(level->ground()->sizeX(),	maxHeight+objPlacementAlt,	0),
								Vec3f(level->ground()->sizeX(),	maxHeight+objPlacementAlt,	level->ground()->sizeZ()));

			glColor4f(0.3,0.5,1.0,0.2);
			for(float f=0.0; f<level->ground()->sizeX() + 0.001; f+=level->ground()->sizeX() / 32.0)
			{
				graphics->drawLine(Vec3f(f,maxHeight+10,0), Vec3f(f,maxHeight+10,level->ground()->sizeZ()));
				//glVertex3f(f,maxHeight+10,0);
				//glVertex3f(f,maxHeight+10,level->ground()->sizeZ());
			}
			for(float f=0.0; f<level->ground()->sizeZ() + 0.001; f+=level->ground()->sizeZ() / 32.0)
			{
				graphics->drawLine(Vec3f(0,maxHeight+10,f), Vec3f(level->ground()->sizeX(),maxHeight+10,f));
				//glVertex3f(0,maxHeight+10,f);
				//glVertex3f(level->ground()->sizeX(),maxHeight+10,f);
			}
			glColor3f(1,1,1);
			glDepthMask(true);
			////////////////////////////////end grid///////////////////////////////////
		}

		glDisable(GL_DEPTH_TEST);
	
		glBindTexture(GL_TEXTURE_2D,0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 1, (float)sh/sw , 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(0,1,0);

		updateObjectCircles();
		for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
		{
			graphics->drawOverlay(Rect::CWH(i->second.center.x, i->second.center.y,	i->second.radius*2, i->second.radius*2),"target ring");
		}

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
	
		glMatrixMode( GL_PROJECTION );			// Select Projection
		glPopMatrix();							// Pop The Matrix
		glMatrixMode( GL_MODELVIEW );			// Select Modelview
		glEnable(GL_DEPTH_TEST);
	}
}
}