
#include "game.h"

namespace gui{

	const int LOD=1;

bool levelEditor::init()
{
	view = graphics->genView();
	view->viewport(0,0, sAspect,1.0);
	view->perspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	view->setRenderFunc(bind(&levelEditor::render3D, this, placeholders::_1));

	//terrain
	buttons["dSquare"]		= new button(sAspect-0.16,0.005,0.15,0.030,"d-square",lightGreen,white);
	buttons["faultLine"]	= new button(sAspect-0.16,0.040,0.15,0.030,"fault line",lightGreen,white);
	buttons["fromFile"]		= new button(sAspect-0.16,0.075,0.15,0.030,"from file",lightGreen,white);
	buttons["exportBMP"]	= new button(sAspect-0.16,0.110,0.15,0.030,"export",lightGreen,white);
	sliders["sea level"]	= new slider(sAspect-0.16,0.145,0.15,0.030,1.0,0.0);
	sliders["height scale"] = new slider(sAspect-0.16,0.180,0.15,0.030,1.0,-1.0);sliders["height scale"]->setValue(0.0);

	buttons["load"]			= new button(sAspect-0.462,0.965,0.15,0.030,"Load",Color(0.8,0.8,0.8),white);
	buttons["save"]			= new button(sAspect-0.310,0.965,0.15,0.030,"Save",Color(0.8,0.8,0.8),white);
	buttons["exit"]			= new button(sAspect-0.157,0.965,0.15,0.030,"Exit",Color(0.8,0.8,0.8),white);

	toggles["shaders"]		= new toggle(vector<button*>(),darkGreen,lightGreen,NULL,0);

	toggles["shaders"]->addButton(new button(0.005,0.005,0.15,0.030,"island",black,white));
	toggles["shaders"]->addButton(new button(0.005,0.040,0.15,0.030,"snow",black,white));
	//toggles["shaders"]->addButton(new button(0.005,0.075,0.15,0.030,"desert",black,white));

	//objects
	buttons["addPlane"]		= new button(0.005,0.005,0.25,0.030,"new plane",lightGreen,white);
	buttons["addAAgun"]		= new button(0.005,0.040,0.25,0.030,"new AA gun",lightGreen,white);
	buttons["addSAMbattery"]= new button(0.005,0.075,0.25,0.030,"new SAM Battery",lightGreen,white);
	buttons["addFlakCannon"]= new button(0.005,0.110,0.25,0.030,"new Flak Cannon",lightGreen,white);

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
	toggles["tabs"]->addButton(new button(0.005,0.965,0.15,0.030,"Terrain",black,white));
	toggles["tabs"]->addButton(new button(0.157,0.965,0.15,0.030,"Objects",black,white));
	toggles["tabs"]->addButton(new button(0.310,0.965,0.15,0.030,"Regions",black,white));

	//if(level != NULL) delete level;
	//level = new editLevel;

	scrollVal=0.0;
	//level->newGround(129,129);
	levelFile.heights = new float[((257-1)*LOD+1)*((257-1)*LOD+1)];
	levelFile.info.mapResolution.x = (257-1)*LOD+1;
	levelFile.info.mapResolution.y = (257-1)*LOD+1;
	diamondSquare(0.17,0.5,4);
	levelFile.info.mapSize.x = 25700;
	levelFile.info.mapSize.y = 25700;


	//smooth(1);

	resetView();


	groundTex = graphics->genTexture2D();
	terrainVertexBuffer = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::DYNAMIC);
	terrainIndexBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::DYNAMIC);

	terrainSkirtVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::DYNAMIC);

	graphics->setLightPosition(Vec3f(-100000,1000000,0));

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
		//level->exportBMP(f);
		messageBox("feature currently unavailable");
	}
	else if(awaitingLevelFile)
	{
		awaitingLevelFile=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((openFile*)p)->getFile();

		if(!levelFile.loadZIP(f))
			debugBreak();

		sliders["sea level"]->setValue(-levelFile.info.minHeight/(levelFile.info.maxHeight - levelFile.info.minHeight));
		resetView();
		terrainValid=false;
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		levelFile.saveZIP(f); // we do not apply the sea level adjustment into account!!!
		//level->save(f, sliders["sea level"]->getValue() * (maxHeight - minHeight) + minHeight);
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
			diamondSquare(0.17,0.5,4);
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
			buttons["addSAMbattery"]->setVisibility(newTab==OBJECTS);
			buttons["addFlakCannon"]->setVisibility(newTab==OBJECTS);
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
	Vec2f p = input.getMousePos();
	if(getTab() == REGIONS && (p.x < 2.0/sh || p.x > sAspect-2.0/sh || p.y < 2.0/sh || p.y > 1.0-2.0/sh))
	{
		if(p.x < 2.0/sh)			orthoCenter += Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000;
		if(p.x > sAspect-2.0/sh)	orthoCenter -= Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000;
		if(p.y < 2.0/sh)			orthoCenter += Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000;
		if(p.y > 1.0-2.0/sh)		orthoCenter -= Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000;
	}
	else if(!input.getMouseState(MIDDLE_BUTTON).down && (p.x < 2.0/sh || p.x > sAspect-2.0/sh || p.y < 2.0/sh || p.y > 1.0-2.0/sh))
	{
		if(p.x < 2.0/sh)			center -= rot * Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.x > sAspect-2.0/sh)	center += rot * Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.y < 2.0/sh)			center -= rot * Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.y > 1.0-2.0/sh)		center += rot * Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000 * pow(1.1f,-scrollVal);
	}

	return 7;
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
				addObject(newObjectType, teamNum, p.x, p.y);
				newObjectType = 0;
				teamNum++;
				return true;
			}
			else if(!objectCircles.empty())
			{
				float minDist;
				map<int,Circle<float>>::iterator closestCircle = objectCircles.end();
				for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
				{
					float d = i->second.center.distanceSquared(p);
					if(d < i->second.radius*i->second.radius && (closestCircle == objectCircles.end() || d < minDist))
					{
						minDist = d;
						closestCircle = i;
					}
				}

				if(closestCircle != objectCircles.end())
				{
					//object found
					auto* m = new objectProperties();
					m->init(&levelFile.objects[closestCircle->first]);
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
				LevelFile::Region r;
				r.shape = 0;							//make region a circle
				r.type = 0;								//use region for map bounds
				r.centerXYZ[0] = newRegionCenter.x;		//set x coordinate for center
				r.centerXYZ[2] = newRegionCenter.y;		//set y coordinate for center
				r.radius = newRegionCenter.distance(p); //set the radius
				r.colorR = 0.0;							//set red component
				r.colorG = 1.0;							//set green component
				r.colorB = 0.0;							//set blue component
				levelFile.regions.push_back(r);			//insert the region into the array
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
	center = Vec3f(levelFile.info.mapSize.x/2,levelFile.info.minHeight,levelFile.info.mapSize.y/2);

	orthoCenter = Vec3f(levelFile.info.mapSize.x/2,levelFile.info.minHeight,levelFile.info.mapSize.y/2);
	orthoScale = 0.0;
}
void levelEditor::updateObjectCircles()
{
	objectCircles.clear();
	int n=0;
	for(auto i = levelFile.objects.begin(); i!= levelFile.objects.end(); i++, n++)
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
float levelEditor::getHeight(unsigned int x, unsigned int z) const
{
#ifdef _DEBUG
	if(x > levelFile.info.mapResolution.x || z > levelFile.info.mapResolution.y)
	{
		debugBreak();
		return 0.0f;
	}
#endif

	return levelFile.heights[x + z * levelFile.info.mapResolution.x];
}
void levelEditor::setHeight(unsigned int x, unsigned int z, float height) const
{
#ifdef _DEBUG
	if(x > levelFile.info.mapResolution.x || z > levelFile.info.mapResolution.y)
	{
		debugBreak();
		return;
	}
#endif

	levelFile.heights[x + z * levelFile.info.mapResolution.x] = height;
}
void levelEditor::increaseHeight(unsigned x, unsigned int z, float increase) const
{
#ifdef _DEBUG
	if(x > levelFile.info.mapResolution.x || z > levelFile.info.mapResolution.y)
	{
		debugBreak();
		return;
	}
#endif

	levelFile.heights[x + z * levelFile.info.mapResolution.x] += increase;
}
Vec3f levelEditor::getNormal(unsigned int x, unsigned int z) const
{
#ifdef _DEBUG
	if(x > levelFile.info.mapResolution.x || z > levelFile.info.mapResolution.y)
	{
		debugBreak();
		return Vec3f(0,1,0);
	}
#endif

	float Cy = (z < levelFile.info.mapResolution.x-1)	? (levelFile.heights[x+(z+1)*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;
	float Ay = (x < levelFile.info.mapResolution.y-1)	? (levelFile.heights[(x+1)+z*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;
	float Dy = (z > 0)									? (levelFile.heights[x+(z-1)*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;
	float By = (x > 0)									? (levelFile.heights[(x-1)+z*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;

	return Vec3f(Cy - Ay, 200.0, Dy - By).normalize(); //y value should be changed to calculate the true normal6
}

void levelEditor::setMinMaxHeights()
{
	levelFile.info.maxHeight = getHeight(0,0) + 0.001;
	levelFile.info.minHeight = getHeight(0,0);

	for(int x=0;x<levelFile.info.mapResolution.x;x++)
	{
		for(int y=0;y<levelFile.info.mapResolution.y;y++)
		{
			levelFile.info.maxHeight = max(levelFile.info.maxHeight, getHeight(x,y));
			levelFile.info.minHeight = min(levelFile.info.minHeight, getHeight(x,y));
		}
	}
}
float levelEditor::randomDisplacement(float h1, float h2, float d)
{
	d *= 65.0/LOD;
	float r = random(-d/2,d/2);
	return (h1 + h2 + r) / 2.0;
 }
float levelEditor::randomDisplacement(float h1, float h2,float h3, float h4, float d)
{
	d *= 65.0/LOD;
	float r = random(-d/2,d/2);
	return (h1 + h2 + h3 + h4 + r) / 4.0;
}
void levelEditor::diamondSquareFill(int x1, int y1, int x2, int y2)
{
	if(x1 == x2 && y1 == y2)
		return;

//	float h[5] = {g(x1,y1), g(x1,y2), g(x2,y1), g(x2,y2), (float)min(x2-x1,y2-y1)};

	int midx = (x1 + x2) / 2;
	int midy = (y1 + y2) / 2;

	if(x2-x1 > 1 && y1==0)	setHeight(midx, y1, randomDisplacement(getHeight(x1,y1), getHeight(x2,y1), x2-x1) );
	if(y2-y1 > 1 && x1==0)	setHeight(x1, midy, randomDisplacement(getHeight(x1,y1), getHeight(x1,y2), y2-y1) );

	if(x2-x1 > 1)	setHeight(midx, y2, randomDisplacement(getHeight(x1,y2), getHeight(x2,y2), x2-x1) );
	if(y2-y1 > 1)	setHeight(x2, midy, randomDisplacement(getHeight(x2,y1), getHeight(x2,y2), y2-y1) );

	if(x2-x1 > 1 && y2-y1 > 1)
		setHeight(midx, midy, randomDisplacement(getHeight(x1,y1), getHeight(x1,y2), getHeight(x2,y1), getHeight(x2,y2), min(x2-x1,y2-y1)) );


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
	int x, y;
	int sLen = max(levelFile.info.mapResolution.x, levelFile.info.mapResolution.y);
	if( !(!((sLen-1) & (sLen-2) ) && sLen > 1) || levelFile.info.mapResolution.x != levelFile.info.mapResolution.y) //if v is not one more than a power of 2
	{
		sLen = uPowerOfTwo(sLen-1) + 1; //makes sLen one more than a power of 2

		delete[] levelFile.heights;
		levelFile.heights = new float[sLen*sLen](); //initializes all floats to 0.0f
		levelFile.info.mapResolution.x = sLen;
		levelFile.info.mapResolution.y = sLen;
	}
	else
	{
		for(x=0;x<sLen;x++)
		{
			for(y=0;y<sLen;y++)
			{
				setHeight(x, y, 0.0);
			}
		}
	}
	//set corners



	//h=pow(2.0f,-h);


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
	setMinMaxHeights();

	//float d, d_x, d_y;
	//for(x=0;x<levelFile.info.mapResolution.x;x++)					//island
	//{
	//	for(y=0;y<levelFile.info.mapResolution.y;y++)
	//	{
	//		d_x = 2.0*x/levelFile.info.mapResolution.x - 1.0;
	//		d_y = 2.0*y/levelFile.info.mapResolution.y - 1.0;
	//		d = sqrt(d_x*d_x + d_y*d_y);
	//
	//		setHeight(x, y, (getHeight(x, y)-levelFile.info.minHeight) * clamp(2.0 - 2.0*d, 0.0, 1.0) + levelFile.info.minHeight);
	//	}
	//}


	setMinMaxHeights();
	levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
	sliders["sea level"]->setValue(0.5);
	resetView();
	terrainValid=false;
}
void levelEditor::faultLine()
{
	unsigned int length = levelFile.info.mapResolution.x;
	unsigned int width = levelFile.info.mapResolution.y;
	float a,b,c;
	float x,y;
	float v;

	for(x=0;x<length;x++)
	{
		for(y=0;y<width;y++)
		{
			setHeight(x,y, 0.0);
		}
	}

	int n=0;
	for(int i=0;i<1000;i++)
	{
		v = random<float>(PI*100.0);
		a = sin(v);
		b = cos(v);
		c = random<float>(-0.5,0.5) * sqrt((float)length * length + width * width);

		if(c>0)
		n += 1;

		for(x=0;x<length;x++)
		{
			for(y=0;y<width;y++)
			{
				//pd = ((x-level->ground()->resolutionX()/2) * a + (y-level->ground()->resolutionZ()/2) * b + c)/(128.0/30)/2;//  ***/2500
				//if (pd > 1.57) pd = 1.57;
				//else if (pd < 0.05) pd = 0.05;
				//level->ground()->increaseHeight(x,-disp/2 + sin(pd)*disp,y);
				if(a*(x-length/2) + b*(y-width/2) - c > 0)
					increaseHeight(x, y, 10.0);
				else
					increaseHeight(x, y, -10.0);
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

 	smooth(1);

	setMinMaxHeights();
	levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
	sliders["sea level"]->setValue(0.333);
	resetView();
	terrainValid=false;
}
void levelEditor::fromFile(string filename)
{
	string ext = fileManager.extension(filename);
	if(ext == ".bmp")
	{
		auto image = fileManager.loadBmpFile(filename);
		if(image->valid() && image->width > 0 && image->height > 0)
		{
			levelFile.heights = new float[image->width * image->height];
			levelFile.info.mapResolution.x = image->width;
			levelFile.info.mapResolution.y = image->height;

			for(int y = 0; y < image->height; y++) {
				for(int x = 0; x < image->width; x++) {
					levelFile.heights[y * image->width + x] = (unsigned char)image->contents[image->channels * (y * image->width + x)] * 10.0;
				}
			}

			setMinMaxHeights();
			levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
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

		unsigned char* tmpHeights = new unsigned char[nRows*nColumns*bpp];
		fin.read((char*)tmpHeights,nRows*nColumns*bpp);

		levelFile.heights = new float[nRows*nColumns];
		levelFile.info.mapResolution.x = nRows;
		levelFile.info.mapResolution.y = nColumns;

		if(bpp == 1)
		{
			for(int x = 0; x < nRows; x++)
			{
				for(int y = 0; y < nColumns; y++)
				{
					levelFile.heights[x + y*nRows] = tmpHeights[x + y*nRows];
				}
			}
		}
		if(bpp == 2)
		{
			for(int x = 0; x < nRows; x++)
			{
				for(int y = 0; y < nColumns; y++)
				{
					levelFile.heights[x + y*nRows] = ((__int16*)tmpHeights)[x + y*nRows];
				}
			}
		}
		if(bpp == 4)
		{
			for(int x = 0; x < nRows; x++)
			{
				for(int y = 0; y < nColumns; y++)
				{
					levelFile.heights[x + y*nRows] = ((__int32*)tmpHeights)[x + y*nRows];
				}
			}
		}
		fin.close();


		delete[] tmpHeights;

		levelFile.info.mapSize.x = nRows * 30.87 * xRes;
		levelFile.info.mapSize.y = nColumns * 30.87 * zRes;


		setMinMaxHeights();
		levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
		sliders["sea level"]->setValue(0.333);
		resetView();
		terrainValid=false;
	}
}
void levelEditor::smooth(int a)
{
	int w = levelFile.info.mapResolution.x;
	int h = levelFile.info.mapResolution.y;
	float* smoothed = new float[w*h];
	memcpy(smoothed, levelFile.heights, w * h * sizeof(float));

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
					s += getHeight(i,j);
					n++;
				}
			}
			smoothed[x+y*w] = s/n;
		}
	}

	memcpy(levelFile.heights, smoothed, w * h * sizeof(float));
	delete[] smoothed;
	terrainValid=false;
}
void levelEditor::addObject(int type, int team, float x, float y)//in screen coordinates
{
	Vec2f cursorPos = input.getMousePos();
	Vec3f P0 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
	Vec3f P1 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
	Vec3f dir = P0-P1;

	if(abs(dir.y) < 0.001) return;
	Vec3f val = P1 + dir*(levelFile.info.maxHeight+objPlacementAlt-P1.y)/dir.y;

	LevelFile::Object o;
	o.type=type;
	o.team=team;
	o.startloc=val;
	o.startRot=rot;
	levelFile.objects.push_back(o);
}
Rect levelEditor::orthoView()
{
	Vec2f gSize = levelFile.info.mapSize * 1.2 * pow(1.3f,-orthoScale);
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

		if(toggles["shaders"]->getValue() == 0) levelFile.info.shaderType = TERRAIN_ISLAND;
		else if(toggles["shaders"]->getValue() == 1) levelFile.info.shaderType = TERRAIN_SNOW;
		else if(toggles["shaders"]->getValue() == 2) levelFile.info.shaderType = TERRAIN_DESERT;
		else levelFile.info.shaderType = TERRAIN_ISLAND;

		bool w = getShader() != 1;

		renderTerrain(w,pow(10.0f,sliders["height scale"]->getValue()), sliders["sea level"]->getValue());
		//level->renderPreview(w,pow(10.0f,sliders["height scale"]->getValue()),sl * (maxHeight - minHeight) + minHeight);




		//if(getTab() == REGIONS)
		//{
		//	dataManager.bind("circle shader");
		//	glDisable(GL_DEPTH_TEST);
		//	graphics->setDepthMask(false);
		//
		//	auto v = level->regions();
		//	for(auto i = v.begin(); i != v.end(); i++)
		//	{
		//		Vec2f c((i->centerXYZ[0] - viewRect.x + orthoCenter.x)/viewRect.w*sAspect, (i->centerXYZ[2] - viewRect.y + orthoCenter.z)/viewRect.h);
		//		Vec2f s(i->radius/viewRect.w*sAspect*2.0, i->radius/viewRect.h*2.0);
		//		graphics->drawOverlay(Rect::CWH(c,s));
		//	}
		//
		//	dataManager.unbindShader();
		//	glEnable(GL_DEPTH_TEST);
		//}
	}
	else
	{
		view->perspective(80.0, (double)sw / ((double)sh),10.0, 500000.0);

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

			e = c + tmpRot * Vec3f(0,0.75,0) * max(levelFile.info.mapSize.x,levelFile.info.mapSize.y) * pow(1.1f,-scrollVal);
			u = tmpRot * Vec3f(0,0,-1);
		}
		else
		{
			e = c + rot * Vec3f(0,0.75,0) * max(levelFile.info.mapSize.x,levelFile.info.mapSize.y) * pow(1.1f,-scrollVal);
			u = rot * Vec3f(0,0,-1);
		}
		view->lookAt(e,c,u);

		graphics->setLightPosition(Vec3f(30, 70, 40));

		if(toggles["shaders"]->getValue() == 0) levelFile.info.shaderType = TERRAIN_ISLAND;
		else if(toggles["shaders"]->getValue() == 1) levelFile.info.shaderType = TERRAIN_SNOW;
		else if(toggles["shaders"]->getValue() == 2) levelFile.info.shaderType = TERRAIN_DESERT;
		else levelFile.info.shaderType = TERRAIN_ISLAND;

		bool w = getShader() != 1;

		renderTerrain(w, pow(10.0f,sliders["height scale"]->getValue()), sliders["sea level"]->getValue());
 		//level->renderPreview(w,pow(10.0f,sliders["height scale"]->getValue()),sl * (maxHeight - minHeight) + minHeight);
	}



	if(getTab() == OBJECTS)
	{
		for(auto i = levelFile.objects.begin(); i != levelFile.objects.end(); i++)
		{
			graphics->drawModel(i->type,i->startloc,i->startRot,10.0);
		}

		if(newObjectType != 0)
		{
			////////////////////////////////draw object//////////////////////////////////

			Vec2f cursorPos = input.getMousePos();
			Vec3d P0 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
			Vec3d P1 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
			Vec3d dir = P0-P1;

			if(abs(dir.y) < 0.001) return;
			Vec3d val = P1 + dir*(levelFile.info.maxHeight+objPlacementAlt-P1.y)/dir.y;
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

		//glDisable(GL_DEPTH_TEST);




		//dataManager.bind("ortho");
		//dataManager.bind("circle shader");

	//	glColor3f(0,1,0);

		//updateObjectCircles();
		//for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
		//{
		//	graphics->drawOverlay(Rect::CWH(i->second.center.x, i->second.center.y,	i->second.radius*2, i->second.radius*2));
		//}
		//dataManager.unbindShader();

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
		//		graphics->drawOverlay(s.x - r, s.y*sh/sw - r, s.x + r, s.y*sh/sw + r);
		//	}
		//}
	}
}
void levelEditor::render()
{
	menuManager.drawCursor();

	if(getTab() == OBJECTS)
	{
		dataManager.bind("circle shader");
		updateObjectCircles();
		for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
		{
			graphics->drawOverlay(Rect::CWH(i->second.center.x, i->second.center.y,	i->second.radius*2, i->second.radius*2));
		}
		dataManager.bind("ortho");
	}

	if(getTab() == REGIONS)
	{
		Rect viewRect = orthoView();

		dataManager.bind("circle shader");

		for(auto i = levelFile.regions.begin(); i != levelFile.regions.end(); i++)
		{
			Vec2f c((i->centerXYZ[0] - viewRect.x + orthoCenter.x)/viewRect.w*sAspect, (i->centerXYZ[2] - viewRect.y + orthoCenter.z)/viewRect.h);
			Vec2f s(i->radius/viewRect.w*sAspect*2.0, i->radius/viewRect.h*2.0);
			graphics->drawOverlay(Rect::CWH(c,s));
		}

		dataManager.bind("ortho");
	}
}
void levelEditor::renderTerrain(bool drawWater, float scale, float seaLevelOffset)
{
	if(levelFile.info.mapResolution.x < 2 || levelFile.info.mapResolution.y < 2)
	{
		debugBreak();
		return;
	}
	//glPushMatrix();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(!terrainValid)
		{
			unsigned int width = levelFile.info.mapResolution.x;
			unsigned int height = levelFile.info.mapResolution.y;
			unsigned int nWidth = (width-1)/LOD+1;
			unsigned int nHeight = (height-1)/LOD+1;

			//if(nWidth * nHeight > 65535) //we are now using 4 byte indices so this is not a problem
			//{
			//	debugBreak(); // terrain size too large to use 2 byte indices
			//	return;
			//}



			float* vertices = new float[nWidth * nHeight * 3];
			for(int x = 0; x < width; x += LOD)
			{
				for(int z = 0; z < height; z += LOD)
				{
					vertices[(x/LOD + nWidth * z/LOD)*3 + 0] = levelFile.info.mapSize.x * static_cast<float>(x) / (width-1);
					vertices[(x/LOD + nWidth * z/LOD)*3 + 1] = getHeight(x, z)-levelFile.info.minHeight;
					vertices[(x/LOD + nWidth * z/LOD)*3 + 2] = levelFile.info.mapSize.y * static_cast<float>(z) / (height-1);
				}
			}

			numTerrainIndices = 6 * (width-1)/LOD * (height-1)/LOD;
			unsigned int* indices = new unsigned int[numTerrainIndices];
			int i=0;
			for(int x = 0; x < nWidth-1; x++)
			{
				for(int z = 0; z < nHeight-1; z++)
				{
					indices[i++] = x + z * nWidth;
					indices[i++] = x + (z+1) * nWidth;
					indices[i++] = (x+1) + z * nWidth;

					indices[i++] = (x+1) + z * nWidth;
					indices[i++] = (x+1) + (z+1) * nWidth;
					indices[i++] = x + (z+1) * nWidth;
				}
			}
			//terrainVertexBuffer->addPositionData(3,	0);
			terrainVertexBuffer->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3, 0);
			terrainVertexBuffer->setTotalVertexSize(sizeof(float)*3);
			terrainVertexBuffer->setVertexData(sizeof(float)*3*nWidth*nHeight, vertices);

			terrainIndexBuffer->setData(indices, numTerrainIndices);
			//terrainVBO->setIndexData(sizeof(unsigned short)*numTerrainIndices, indices);


			//////////////////////////////////////texture///////////////////////////////////////////////////
			unsigned char* tex = new unsigned char[width*height*4];
			Vec3f n;
			for(int x = 0; x < width; x++)
			{
				for(int z = 0; z < height; z++)
				{
					n = getNormal(x, z);
					tex[4*(x+z*width) + 0] = static_cast<unsigned char>((n.x*0.5+0.5) * 255.0);
					tex[4*(x+z*width) + 1] = static_cast<unsigned char>(n.y * 255.0);
					tex[4*(x+z*width) + 2] = static_cast<unsigned char>((n.z*0.5+0.5) * 255.0);
					tex[4*(x+z*width) + 3] = static_cast<unsigned char>((getHeight(x, z)-levelFile.info.minHeight)/(levelFile.info.maxHeight-levelFile.info.minHeight)*255.0);
				}
			}
			groundTex->setData(width,height,GraphicsManager::texture::RGBA, tex);

			terrainValid=true;
		}
		if(levelFile.info.shaderType == TERRAIN_ISLAND || levelFile.info.shaderType == TERRAIN_DESERT)
		{
			/*if(levelFile.info.shaderType == TERRAIN_ISLAND)	dataManager.bind("island preview terrain");
			else											dataManager.bind("grass preview terrain");*/

			dataManager.bind("island preview terrain");

			dataManager.bind("sand",0);
			dataManager.bind("grass",1);
			dataManager.bind("rock",2);
			dataManager.bind("LCnoise",3);
			groundTex->bind(4);

			dataManager.setUniform1f("heightScale",	scale);
			dataManager.setUniform1f("minHeight", -seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			dataManager.setUniform1f("heightRange", (levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			dataManager.setUniform3f("invScale", 1.0/(levelFile.info.mapSize.x),1.0/((levelFile.info.maxHeight-levelFile.info.minHeight)*scale),1.0/(levelFile.info.mapSize.y));

			Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
			dataManager.setUniformMatrix("cameraProjection", cameraProjectionMat);
			dataManager.setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));

			dataManager.setUniform3f("invScale",	1.0/levelFile.info.mapSize.x, 1.0/(levelFile.info.maxHeight-levelFile.info.minHeight), 1.0/levelFile.info.mapSize.y);
			dataManager.setUniform3f("lightPosition", graphics->getLightPosition());
			dataManager.setUniform3f("eyePos",		view->camera().eye);
			dataManager.setUniform1f("time",		world.time());
			dataManager.setUniform1i("sand",		0);
			dataManager.setUniform1i("grass",		1);
			dataManager.setUniform1i("rock",		2);
			dataManager.setUniform1i("LCnoise",		3);
			dataManager.setUniform1i("groundTex",	4);
			terrainIndexBuffer->drawBuffer(GraphicsManager::TRIANGLES, terrainVertexBuffer);
			//terrainVBO->drawBuffer(GraphicsManager::TRIANGLES, 0, numTerrainIndices);

			dataManager.bind("model");
		}
		else if(levelFile.info.shaderType == TERRAIN_SNOW)
		{
			dataManager.bind("snow preview terrain");

			dataManager.bind("snow",0);
			dataManager.bind("LCnoise",1);
			groundTex->bind(2);

			dataManager.setUniform1f("heightScale",	scale);

			Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
			dataManager.setUniformMatrix("cameraProjection", cameraProjectionMat);
			dataManager.setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));

			dataManager.setUniform3f("invScale",	1.0/levelFile.info.mapSize.x, 1.0/(levelFile.info.maxHeight-levelFile.info.minHeight), 1.0/levelFile.info.mapSize.y);
			dataManager.setUniform3f("lightPosition", graphics->getLightPosition());
			dataManager.setUniform3f("eyePos",		view->camera().eye);
			dataManager.setUniform1f("time",		world.time());
			dataManager.setUniform1i("snow",		0);
			dataManager.setUniform1i("LCnoise",		1);
			dataManager.setUniform1i("groundTex",	2);

			terrainIndexBuffer->drawBuffer(GraphicsManager::TRIANGLES, terrainVertexBuffer);
			//terrainVBO->drawBuffer(GraphicsManager::TRIANGLES, 0, numTerrainIndices);

			dataManager.bind("model");
		}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(levelFile.info.shaderType == TERRAIN_ISLAND)
	{
		dataManager.bind("ocean preview");

		dataManager.bind("ocean normals",0);
	//	if(levelFile.info.shaderType == SHADER_OCEAN)	dataManager.bindTex(0,1);
	//	else											groundTex->bind(1);
	//	dataManager.bind("rock",2);
	//	dataManager.bind("sand",3);
		Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
		dataManager.setUniformMatrix("cameraProjection", cameraProjectionMat);
		dataManager.setUniformMatrix("modelTransform", Mat4f());

		dataManager.setUniform1i("bumpMap", 0);
	//	dataManager.setUniform1i("groundTex", 1);
	//	dataManager.setUniform1i("rock", 2);
	//	dataManager.setUniform1i("sand", 3);
		dataManager.setUniform1f("time", world.time());
	//	dataManager.setUniform1f("seaLevel", (seaLevelOffset-levelFile.info.minHeight)/(levelFile.info.maxHeight-levelFile.info.minHeight));
	//	dataManager.setUniform1f("XZscale", levelFile.info.mapSize.x);

		//glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->mResolution.x)/uPowerOfTwo(mGround->mResolution.x),(float)(mGround->mResolution.y)/uPowerOfTwo(mGround->mResolution.y));
		graphics->drawQuad(	Vec3f(0,0,0),
							Vec3f(levelFile.info.mapSize.x,0,0),
							Vec3f(levelFile.info.mapSize.x,0,levelFile.info.mapSize.y),
							Vec3f(0,0,levelFile.info.mapSize.y));



		dataManager.bind("model");
	}

	//if(levelFile.info.shaderType == SHADER_GRASS || levelFile.info.shaderType == SHADER_SNOW)
	//{
	//	dataManager.bind("model");
	//	dataManager.setUniformMatrix("modelTransform", Mat4f(Quat4f(), Vec3f(), Vec3f(levelFile.info.mapSize.x/(levelFile.info.mapResolution.x-1),1,levelFile.info.mapSize.y/(levelFile.info.mapResolution.y-1))));
	//	dataManager.setUniform1i("tex", 0);
	//
	//	float h = levelFile.info.minHeight*scale-20.0;
	////	dataManager.bind("layers");
	////	float t=0.0;
	//
	////	glBegin(GL_TRIANGLE_STRIP);
	////	glNormal3f(0,1,0);
	////	for(int i = 0; i < mGround->resolutionX()-1; i++)
	////	{
	////		glTexCoord2f(t,(mGround->rasterHeight(i,0)-mGround->minHeight+20)/256);		glVertex3f(i,h,0);
	////		glTexCoord2f(t,0);	glVertex3f(i,max(mGround->rasterHeight(i,0)*scale,h) ,0);
	////		t+=0.2;
	////	}
	////	for(int i = 0; i < mGround->resolutionZ()-1; i++)
	////	{
	////		glTexCoord2f(t,(mGround->rasterHeight(mGround->resolutionX()-1,i)-mGround->minHeight+20)/256);	glVertex3f(mGround->resolutionX()-1,h,i);
	////		glTexCoord2f(t,0);	glVertex3f(mGround->resolutionX()-1,max(mGround->rasterHeight(mGround->resolutionX()-1,i)*scale,h),i);
	////		t+=0.2;
	////	}
	////	t+=0.2;
	////	for(int i = mGround->resolutionX()-1; i > 0; i--)
	////	{
	////		t-=0.2;
	////		glTexCoord2f(t,(mGround->rasterHeight(i,mGround->resolutionZ()-1)-mGround->minHeight+20)/256);	glVertex3f(i,h,mGround->resolutionZ()-1);
	////		glTexCoord2f(t,0);	glVertex3f(i,max(mGround->rasterHeight(i,mGround->resolutionZ()-1)*scale,h),mGround->resolutionZ()-1);
	////	}
	////	for(int i = mGround->resolutionZ()-1; i >= 0; i--)
	////	{
	////		t-=0.2;
	////		glTexCoord2f(t,(mGround->rasterHeight(0,i)-mGround->minHeight+20)/256);		glVertex3f(0,h,i);
	////		glTexCoord2f(t,0);	glVertex3f(0,max(mGround->rasterHeight(0,i)*scale,h),i);
	////	}
	////	glEnd();
	//
	//	//dataManager.bind("white");
	//	//graphics->setColor(0.73,0.6,0.47);
	//	//graphics->drawQuad(	Vec3f(0,h,0),
	//	//					Vec3f(0,h,levelFile.info.mapResolution.y-1),
	//	//					Vec3f(levelFile.info.mapResolution.x-1,h,0),
	//	//					Vec3f(levelFile.info.mapResolution.x-1,h,levelFile.info.mapResolution.y-1));
	//	//graphics->setColor(1,1,1);
	//
	//	dataManager.setUniformMatrix("modelTransform", Mat4f());
	//}
	//glPopMatrix();
}
}
