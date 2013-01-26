
#include "game.h"

namespace gui{

bool levelEditor::init()
{
	view = graphics->genView();
	view->viewport(0,0, sAspect,1.0);
	view->perspective(80.0, (double)sw / ((double)sh),1.0, 50000.0);
	view->setRenderFunc(std::bind(&levelEditor::render3D, this, std::placeholders::_1));
	view->postProcessShader(shaders("gamma"));

	objectPreviewView = graphics->genView();
	objectPreviewView->viewport(0.0,0.0, 0.260,0.195);
	objectPreviewView->perspective(60.0, 1.0, 0.1, 10000.0);
	objectPreviewView->setRenderFunc(std::bind(&levelEditor::renderObjectPreview, this));
	objectPreviewView->postProcessShader(shaders("gamma"));


	//terrain
	buttons["dSquare"]			= new button(sAspect-0.16,0.005,0.15,0.030,"d-square",lightGreen,white);
	buttons["faultLine"]		= new button(sAspect-0.16,0.040,0.15,0.030,"fault line",lightGreen,white);
	buttons["fromFile"]			= new button(sAspect-0.16,0.075,0.15,0.030,"from file",lightGreen,white);

	buttons["beautify coast"]	= new button(sAspect-0.16,0.145,0.15,0.030,"beautify",lightGreen,white);
	buttons["smooth"]			= new button(sAspect-0.16,0.180,0.15,0.030,"smooth",lightGreen,white);
	buttons["roughen"]			= new button(sAspect-0.16,0.215,0.15,0.030,"roughen",lightGreen,white);
	sliders["sea level"]		= new slider(sAspect-0.16,0.250,0.15,0.030,1.0,0.0);
	sliders["height scale"]		= new slider(sAspect-0.16,0.285,0.15,0.030,1.0,-1.0);sliders["height scale"]->setValue(0.0);
	listBoxes["LOD"]			= new listBox(sAspect-0.16,0.320,0.15,"LOD", black);
	listBoxes["LOD"]->addOption("1");
	listBoxes["LOD"]->addOption("2");
	listBoxes["LOD"]->addOption("4");
	listBoxes["LOD"]->addOption("8");
	listBoxes["LOD"]->setOption(0);


	buttons["load"]			= new button(sAspect-0.462,0.965,0.15,0.030,"Load",Color(0.8,0.8,0.8),white);
	buttons["save"]			= new button(sAspect-0.310,0.965,0.15,0.030,"Save",Color(0.8,0.8,0.8),white);
	buttons["exit"]			= new button(sAspect-0.157,0.965,0.15,0.030,"Exit",Color(0.8,0.8,0.8),white);

	toggles["shaders"]		= new toggle(vector<button*>(),darkGreen,lightGreen,NULL,0);

	toggles["shaders"]->addButton(new button(0.005,0.005,0.15,0.030,"island",black,white));
	toggles["shaders"]->addButton(new button(0.005,0.040,0.15,0.030,"snow",black,white));
	toggles["shaders"]->addButton(new button(0.005,0.075,0.15,0.030,"desert",black,white));
	//toggles["shaders"]->addButton(new button(0.005,0.0.110,0.15,0.030,"mountains",black,white));

	buttons["createObject"]		= new button(0.005,0.895,0.25,0.060,"Create Object",darkGreen,white);
	buttons["deleteObject"]		= new button(0.005,0.895,0.25,0.060,"Delete Object",Color3(0.5,0,0),black);
	buttons["cancelObject"]		= new button(0.005,0.895,0.25,0.060,"Cancel",Color3(0.7,0.7,0.7),black);


	typeOptions = objectInfo.getPlaceableObjects();
	typeOptions.insert(typeOptions.begin(), PLAYER_PLANE);
	listBoxes["object type"] = new listBox(0.005,0.195,0.25,objectInfo.textName(PLAYER_PLANE),black);
	for(auto i=typeOptions.begin(); i!=typeOptions.end(); i++)
	{
		listBoxes["object type"]->addOption(objectInfo.textName(*i));
	}

	listBoxes["object team"] = new listBox(0.005,0.230,0.25,"Team 1",black);
	listBoxes["object team"]->addOption("Team 1");
	listBoxes["object team"]->addOption("Team 2");
	listBoxes["object team"]->addOption("Team 3");
	listBoxes["object team"]->addOption("Team 4");


	toggles["tabs"]	= new toggle(vector<button*>(),Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),NULL,0);
	toggles["tabs"]->addButton(new button(0.005,0.965,0.15,0.030,"Terrain",black,white));
	toggles["tabs"]->addButton(new button(0.157,0.965,0.15,0.030,"Objects",black,white));
	//toggles["tabs"]->addButton(new button(0.310,0.965,0.15,0.030,"Regions",black,white));

	LOD = 1;
	levelFile.heights = new float[((257-1)*LOD+1)*((257-1)*LOD+1)];
	levelFile.info.mapResolution.x = (257-1)*LOD+1;
	levelFile.info.mapResolution.y = (257-1)*LOD+1;
	diamondSquare(0.17,0.5,4);
	levelFile.info.mapSize.x = 25700;
	levelFile.info.mapSize.y = 25700;

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

		LOD = levelFile.info.LOD;
		listBoxes["LOD"]->setOption(0);
		if(LOD == 2)		listBoxes["LOD"]->setOption(1);
		else if(LOD == 4)	listBoxes["LOD"]->setOption(2);
		else if(LOD == 8)	listBoxes["LOD"]->setOption(3);

		sliders["sea level"]->setValue(clamp(-levelFile.info.minHeight/(levelFile.info.maxHeight - levelFile.info.minHeight),0.0,1.0));

		if(levelFile.info.shaderType == TERRAIN_ISLAND) toggles["shaders"]->setValue(0);
		else if(levelFile.info.shaderType == TERRAIN_SNOW) toggles["shaders"]->setValue(1);
		else if(levelFile.info.shaderType == TERRAIN_DESERT) toggles["shaders"]->setValue(2);
		else if(levelFile.info.shaderType == TERRAIN_MOUNTAINS) toggles["shaders"]->setValue(3);

		resetView();
		terrainValid=false;
	}
	else if(awaitingLevelSave)
	{
		awaitingLevelSave=false;
		if(!((saveFile*)p)->validFile()) return;
		string f=((saveFile*)p)->getFile();
		levelFile.info.LOD = 1 << listBoxes["LOD"]->getOptionNumber();
		levelFile.saveZIP(f,pow(10.0f,sliders["height scale"]->getValue()), sliders["sea level"]->getValue());
		//level->save(f, sliders["sea level"]->getValue() * (maxHeight - minHeight) + minHeight);
	}
	//else if(awaitingNewObject)
	//{
	//	awaitingNewObject = false;
	//	newObjectType=((newObject*)p)->getObjectType();
	//}
	//else if(...)
	//   .
	//   .
	//   .
}
void levelEditor::updateFrame()
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
		return;
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
			s.insert(".png");
			s.insert(".bil");

			awaitingMapFile = true;
			popup* p = new openFile;
			p->callback = (functor<void,popup*>*)this;
			((openFile*)p)->init(s);
			menuManager.setPopup(p);
		}
		else if(buttons["beautify coast"]->checkChanged())
		{
			beautifyCoastline();
		}
		else if(buttons["smooth"]->checkChanged())
		{
			smooth(3);
		}
		else if(buttons["roughen"]->checkChanged())
		{
			roughen(0.003 * (levelFile.info.maxHeight - levelFile.info.minHeight));
		}
		else if(1 << listBoxes["LOD"]->getOptionNumber() != LOD)
		{
			int oldLOD = LOD;
			LOD = 1 << listBoxes["LOD"]->getOptionNumber();
			terrainValid=false;
			levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
			center = Vec3f(center.x * oldLOD / LOD, center.y, center.z * oldLOD / LOD);
		}
		//else if(buttons["exportBMP"]->checkChanged())
		//{
		//	awaitingMapSave = true;
		//	popup* p = new saveFile;
		//	p->callback = (functor<void,popup*>*)this;
		//	((saveFile*)p)->init(".bmp");
		//	menuManager.setPopup(p);
		//}
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
		//if(buttons["addPlane"]->checkChanged())
		//{
		//	awaitingNewObject=true;
		//	popup* p = new newObject;
		//	p->callback = (functor<void,popup*>*)this;
		//	menuManager.setPopup(p);
		//}
		//else if(buttons["addAAgun"]->checkChanged())
		//{
		//	awaitingNewObject=true;
		//	popup* p = new newObject(AA_GUN);
		//	p->callback = (functor<void,popup*>*)this;
		//	menuManager.setPopup(p);
		//}
		//else if(buttons["addSAMbattery"]->checkChanged())
		//{
		//	awaitingNewObject=true;
		//	popup* p = new newObject(SAM_BATTERY);
		//	p->callback = (functor<void,popup*>*)this;
		//	menuManager.setPopup(p);
		//}
		//else if(buttons["addFlakCannon"]->checkChanged())
		//{
		//	awaitingNewObject=true;
		//	popup* p = new newObject(FLAK_CANNON);
		//	p->callback = (functor<void,popup*>*)this;
		//	menuManager.setPopup(p);
		//}
		//else if(buttons["addShip"]->checkChanged())
		//{
		//	awaitingNewObject=true;
		//	popup* p = new newObject(objectInfo.typeFromString("BATTLESHIP_1"));
		//	p->callback = (functor<void,popup*>*)this;
		//	menuManager.setPopup(p);
		//}
		if(buttons["createObject"]->checkChanged())
		{
			LevelFile::Object l;
			l.type = typeOptions[listBoxes["object type"]->getOptionNumber()];
			l.team = listBoxes["object team"]->getOptionNumber()+1;
			l.startloc = Vec3f(center.x,objPlacementAlt,center.z);
			selectedObject = levelFile.objects.size();
			levelFile.objects.push_back(l);
			placingNewObject = true;
			objPlacementAlt = (l.type & SHIP) ? 10.0 : levelFile.info.maxHeight+10.0;
			buttons["createObject"]->setVisibility(false);
			buttons["deleteObject"]->setVisibility(false);
			buttons["cancelObject"]->setVisibility(true);
		}
		else if(buttons["deleteObject"]->checkChanged() || buttons["cancelObject"]->checkChanged())
		{
			if(selectedObject < levelFile.objects.size())
			{
				levelFile.objects.erase(levelFile.objects.begin()+selectedObject);
			}
			placingNewObject = false;
			selectedObject = -1;
			buttons["createObject"]->setVisibility(true);
			buttons["cancelObject"]->setVisibility(false);
			buttons["deleteObject"]->setVisibility(false);
		}

		if(selectedObject >= 0 && selectedObject < levelFile.objects.size())
		{
			if(placingNewObject)
			{
				Vec2f cursorPos = input.getMousePos();
				if(cursorPos.x > 0.260 && cursorPos.y < 0.960)
				{
					Vec3f P0 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
					Vec3f P1 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
					Vec3f dir = P1-P0;
					if(abs(dir.y) >= 0.001)
					{
						if(levelFile.objects[selectedObject].type & ANTI_AIRCRAFT_ARTILLARY)
						{
							if(rayHeightmapIntersection(P0, dir.normalize(), levelFile.objects[selectedObject].startloc))
							{
								levelFile.objects[selectedObject].startRot = Quat4f(Vec3f(0,1,0),getTrueNormal(levelFile.objects[selectedObject].startloc.x,levelFile.objects[selectedObject].startloc.z));
							}
						}
						else
							levelFile.objects[selectedObject].startloc = P0 + dir*(objPlacementAlt-P0.y)/dir.y;
					}
				}
			}
			levelFile.objects[selectedObject].type = typeOptions[listBoxes["object type"]->getOptionNumber()];
			levelFile.objects[selectedObject].team = listBoxes["object team"]->getOptionNumber()+1;
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
			//buttons["exportBMP"]->setVisibility(newTab==TERRAIN);
			buttons["beautify coast"]->setVisibility(newTab==TERRAIN);
			buttons["smooth"]->setVisibility(newTab==TERRAIN);
			buttons["roughen"]->setVisibility(newTab==TERRAIN);
			toggles["shaders"]->setVisibility(newTab==TERRAIN);
			sliders["sea level"]->setVisibility(newTab==TERRAIN);
			sliders["height scale"]->setVisibility(newTab==TERRAIN);
			listBoxes["LOD"]->setVisibility(newTab==TERRAIN);

		}
		if(lastTab == OBJECTS || newTab==OBJECTS || lastTab == (Tab)-1)
		{
			//buttons["addPlane"]->setVisibility(newTab==OBJECTS);
			//buttons["addAAgun"]->setVisibility(newTab==OBJECTS);
			//buttons["addSAMbattery"]->setVisibility(newTab==OBJECTS);
			//buttons["addFlakCannon"]->setVisibility(newTab==OBJECTS);
			//buttons["addShip"]->setVisibility(newTab==OBJECTS);
			buttons["createObject"]->setVisibility(newTab==OBJECTS);
			buttons["deleteObject"]->setVisibility(false);
			buttons["cancelObject"]->setVisibility(false);
			listBoxes["object type"]->setVisibility(newTab==OBJECTS);
			listBoxes["object team"]->setVisibility(newTab==OBJECTS);
			if(lastTab == OBJECTS) view->viewport(0,0, sAspect, 1.0);
			if(newTab == OBJECTS) view->viewport(0.260,0, sAspect-0.260, 0.960);
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
		//if(p.x < 2.0/sh)			center -= rot * Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		//if(p.x > sAspect-2.0/sh)	center += rot * Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		//if(p.y < 2.0/sh)			center -= rot * Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		//if(p.y > 1.0-2.0/sh)		center += rot * Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000 * pow(1.1f,-scrollVal);
		if(p.x < 2.0/sh)			center += Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000 * fovy/80.0;
		if(p.x > sAspect-2.0/sh)	center -= Vec3f(0.25,0,0) * levelFile.info.mapSize.x * world.time.length() / 1000 * fovy/80.0;
		if(p.y < 2.0/sh)			center += Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000 * fovy/80.0;
		if(p.y > 1.0-2.0/sh)		center -= Vec3f(0,0,0.25) * levelFile.info.mapSize.y * world.time.length() / 1000 * fovy/80.0;
		center.x = clamp(center.x, 0.0, levelFile.info.mapSize.x);
		center.z = clamp(center.z, 0.0, levelFile.info.mapSize.y);
	}

	debugAssert(isPowerOfTwo(levelFile.info.mapResolution.x-1) && isPowerOfTwo(levelFile.info.mapResolution.y-1));

	bool orthoTerrain = (getTab() == REGIONS);
	if(orthoTerrain)
	{
		Rect viewRect = orthoView();
		view->ortho(viewRect.x,viewRect.x+viewRect.w,viewRect.y,viewRect.y+viewRect.h,-10000,10000);
	}
	else
	{
		view->perspective(fovy, (double)sw / ((double)sh),100.0, 500000.0);
	}
}
bool levelEditor::mouse(mouseButton button, bool down)
{
	Vec2f p = down ? input.getMouseState(button).downPos : input.getMouseState(button).upPos;

	if(button == LEFT_BUTTON)
	{
		if(getTab() == OBJECTS && down)
		{
			if(placingNewObject && p.x > 0.260 && p.y < 0.960)
			{
				Vec3f P0 = view->unProject(Vec3f(p.x,p.y,0.0));
				Vec3f P1 = view->unProject(Vec3f(p.x,p.y,1.0));
				Vec3f dir = P1-P0;
				if(abs(dir.y) >= 0.001)
				{
					if(levelFile.objects[selectedObject].type & ANTI_AIRCRAFT_ARTILLARY)
					{
						rayHeightmapIntersection(P0, dir.normalize(), levelFile.objects[selectedObject].startloc);
						levelFile.objects[selectedObject].startRot = Quat4f(Vec3f(0,1,0),getTrueNormal(levelFile.objects[selectedObject].startloc.x,levelFile.objects[selectedObject].startloc.z));
					}
					else
						levelFile.objects[selectedObject].startloc = P0 + dir*(objPlacementAlt-P0.y)/dir.y;
				//	levelFile.objects[selectedObject].startloc = P1 + dir*(objPlacementAlt-P1.y)/dir.y;
				}
				/*if(levelFile.objects[selectedObject].type & ANTI_AIRCRAFT_ARTILLARY)
				{
					levelFile.objects[selectedObject].startloc.y = getInterpolatedHeight(levelFile.objects[selectedObject].startloc.x,levelFile.objects[selectedObject].startloc.z);
					levelFile.objects[selectedObject].startRot = Quat4f(Vec3f(0,1,0),getInterpolatedNormal(levelFile.objects[selectedObject].startloc.x,levelFile.objects[selectedObject].startloc.z));
				}
				else */if(levelFile.objects[selectedObject].type & SHIP)
				{
					levelFile.objects[selectedObject].startloc.y = 10.0;
				}
				selectedObject = -1;
				placingNewObject = false;
				buttons["createObject"]->setVisibility(true);
				buttons["cancelObject"]->setVisibility(false);
				buttons["deleteObject"]->setVisibility(false);
			}
			else if(!objectCircles.empty() && p.x > 0.260 && p.y < 0.960)
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
					//auto* m = new objectProperties();
					//m->init(&levelFile.objects[closestCircle->first]);
					//menuManager.setPopup(m);
					selectedObject = closestCircle->first;
					int typeNum=-1;
					for(int i=0; i<typeOptions.size(); i++)
					{
						if(typeOptions[i]==levelFile.objects[selectedObject].type)
							typeNum = i;
					}
					if(typeNum == -1)
					{
						listBoxes["object type"]->addOption(objectInfo.typeString(levelFile.objects[selectedObject].type));
						typeOptions.push_back(levelFile.objects[selectedObject].type);
						typeNum = typeOptions.size()-1;
					}
					listBoxes["object type"]->setOption(typeNum);
					listBoxes["object team"]->setOption(levelFile.objects[selectedObject].team-1);
					buttons["createObject"]->setVisibility(false);
					buttons["cancelObject"]->setVisibility(false);
					buttons["deleteObject"]->setVisibility(true);
					return true;
				}
				else
				{
					selectedObject = -1;
					buttons["createObject"]->setVisibility(true);
					buttons["cancelObject"]->setVisibility(false);
					buttons["deleteObject"]->setVisibility(false);
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
	//else if(button == MIDDLE_BUTTON)
	//{
	//	if(!down)
	//	{
	//		Vec2f oldP = input.getMouseState(MIDDLE_BUTTON).downPos;
	//		if(oldP == p)
	//			return true;

	//		Vec3f xAxis = rot * Vec3f(-1,0,0);

	//		Vec3f axis = xAxis * (p.y-oldP.y) + Vec3f(0,-1,0) * (p.x-oldP.x);
	//		Angle ang = oldP.distance(p);
	//		rot = Quat4f(axis,ang) * rot;
	//		return true;
	//	}
	//}

	return false;
}
bool levelEditor::scroll(float rotations)
{
	if(getTab() == REGIONS)
		orthoScale = clamp(orthoScale + rotations,-8,8);
	else
		fovy = clamp(fovy - rotations*3.0,80.0,2.0);
		//scrollVal = clamp(scrollVal + rotations,-8,25);
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
	fovy = 80.0;
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
		Vec3f s;
		float r;

		auto obj = objectInfo[i->type];//dataManager.getModel(i->type);
		if(!obj || obj->mesh.expired())
		{
			r = 0.006;
			s = view->project3(i->startloc);
		}
		else
		{
			float scale = i->type & SHIP ? 1.0 : 10.0;

			Sphere<float> sphere = obj->mesh.lock()->boundingSphere;
			s = view->project3(i->startloc + i->startRot * sphere.center*scale);
			Vec2f t = view->project(i->startloc + sphere.center + view->camera().up*sphere.radius*scale);
			r = max(0.004f,sqrt((s.x-t.x)*(s.x-t.x)+(s.y-t.y)*(s.y-t.y)));
		}
		if(/*frustum.sphereInFrustum(i->startloc,r)!=FrustumG::OUTSIDE &&*/ s.x > -r && s.x < sAspect+r && s.y > -r && s.y < 1.0+r && s.z > 0.0)
		{
			objectCircles[n] = Circle<float>(Vec2f(s.x,s.y),r);
		}
	}
}
float levelEditor::getHeight(unsigned int x, unsigned int z) const
{
#ifdef _DEBUG
	if(x >= levelFile.info.mapResolution.x || z >= levelFile.info.mapResolution.y)
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
	if(x >= levelFile.info.mapResolution.x || z >= levelFile.info.mapResolution.y)
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
	if(x >= levelFile.info.mapResolution.x || z >= levelFile.info.mapResolution.y)
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
	if(x >= levelFile.info.mapResolution.x || z >= levelFile.info.mapResolution.y)
	{
		debugBreak();
		return Vec3f(0,1,0);
	}
#endif

	float Cy = (z < levelFile.info.mapResolution.y-1)	? (levelFile.heights[x+(z+1)*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;
	float Ay = (x < levelFile.info.mapResolution.x-1)	? (levelFile.heights[(x+1)+z*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;
	float Dy = (z > 0)									? (levelFile.heights[x+(z-1)*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;
	float By = (x > 0)									? (levelFile.heights[(x-1)+z*levelFile.info.mapResolution.x] - levelFile.heights[x+z*levelFile.info.mapResolution.x]) : 0.0f;

	return Vec3f((Cy - Ay) * (levelFile.info.mapResolution.x-1) / levelFile.info.mapSize.x, 2.0, (Dy - By) * (levelFile.info.mapResolution.y-1) / levelFile.info.mapSize.y).normalize(); //y value should be changed to calculate the true normal
}
float levelEditor::getInterpolatedHeight(float x, float y) const
{
	x = clamp(x * (levelFile.info.mapResolution.x-1) / levelFile.info.mapSize.x, 0, (float)(levelFile.info.mapResolution.x-1));
	y = clamp(y * (levelFile.info.mapResolution.y-1) / levelFile.info.mapSize.y, 0, (float)(levelFile.info.mapResolution.y-1));

	float A, B, C, D;
	//  A  _____  B
	//    |    /|
	//	  |  /  |
	//  D |/____| C

	float x_fract = x-floor(x);
	float y_fract = y-floor(y);

	if(x_fract + y_fract < 1.0)
	{
		A = getHeight(floor(x),floor(y));
		B = getHeight(floor(x),ceil(y));
		D = getHeight(ceil(x),floor(y));
		C = B + D - A;
	}
	else
	{
		B = getHeight(floor(x),ceil(y));
		C = getHeight(ceil(x),ceil(y));
		D = getHeight(ceil(x),floor(y));
		A = B + D - C;
	}
	return lerp(lerp(A,B,x_fract), lerp(C,D,x_fract), y_fract);
}
float levelEditor::getTrueHeight(float x, float y) const
{
	return (getInterpolatedHeight(x, y)-levelFile.info.minHeight - sliders.find("sea level")->second->getValue()*(levelFile.info.maxHeight-levelFile.info.minHeight)) * pow(10.0f,sliders.find("height scale")->second->getValue());
}
Vec3f levelEditor::getInterpolatedNormal(float x, float z) const
{
	x = clamp(x * (levelFile.info.mapResolution.x-1) / levelFile.info.mapSize.x, 0, (float)(levelFile.info.mapResolution.x-1));
	z = clamp(z * (levelFile.info.mapResolution.y-1) / levelFile.info.mapSize.y, 0, (float)(levelFile.info.mapResolution.y-1));

	Vec3f A, B, C, D;
	//  A  _____  B
	//    |    /|
	//	  |  /  |
	//  D |/____| C

	float x_fract = x-floor(x);
	float z_fract = z-floor(z);

	if(x_fract + z_fract < 1.0)
	{
		A = getNormal(floor(x),floor(z));
		B = getNormal(floor(x),ceil(z));
		D = getNormal(ceil(x),floor(z));
		C = B + D - A;
	}
	else
	{
		B = getNormal(floor(x),ceil(z));
		C = getNormal(ceil(x),ceil(z));
		D = getNormal(ceil(x),floor(z));
		A = B + D - C;
	}
	return lerp(lerp(A,B,x_fract), lerp(C,D,x_fract), z_fract).normalize();
}
Vec3f levelEditor::getTrueNormal(float x, float z) const
{
	Vec3f normal = getInterpolatedNormal(x,z);
	normal.y /= pow(10.0f, sliders.find("height scale")->second->getValue());
	return normal.normalize();
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
bool levelEditor::rayHeightmapIntersection(Vec3f rayStart, Vec3f rayDirection, Vec3f& collisionPoint) const//seems not to work when the view is not centered?
{
	Profiler.startElement("rayTest");
	cellFoundValid = false;
	checkedCells.clear();
	checkLine.clear();

	float invWidthScale = (levelFile.info.mapResolution.x-1) / levelFile.info.mapSize.x;
	float invHeightScale = (levelFile.info.mapResolution.y-1) / levelFile.info.mapSize.y;

	double yScale = pow(10.0f,sliders.find("height scale")->second->getValue());
	double yShift = levelFile.info.minHeight + sliders.find("sea level")->second->getValue()*(levelFile.info.maxHeight-levelFile.info.minHeight);



	rayStart.x *= invWidthScale;
	//rayStart.y = rayStart.y * yScale + seaLevel*(levelFile.info.maxHeight-levelFile.info.minHeight)*yScale;
	rayStart.z *= invHeightScale;

	rayDirection.x *= invWidthScale;
	//rayDirection.y *= yScale;
	rayDirection.z *= invHeightScale;
	rayDirection = rayDirection.normalize();

	rStart = rayStart;
	rDirection = rayDirection;

	BoundingBox<float> AABB;
	AABB.minXYZ = Vec3f(0,(levelFile.info.minHeight-yShift)*yScale,0);
	AABB.maxXYZ = Vec3f(levelFile.info.mapResolution.x-1,(levelFile.info.maxHeight-yShift)*yScale,levelFile.info.mapResolution.y-1);

	auto checkCell = [this,yScale,yShift,rayStart,invHeightScale,invWidthScale,rayDirection,&collisionPoint](int x, int y)->bool
	{
		if(x < 0 || y < 0 || x >= levelFile.info.mapResolution.x-1 || y >= levelFile.info.mapResolution.y-1)
			return false;

		checkedCells.push_back(Vec2i(x,y));
		//double h1 = (levelFile.heights[x   + (y) * levelFile.info.mapResolution.x]	- yShift) * yScale;
		//double h2 = (levelFile.heights[x+1 + (y) * levelFile.info.mapResolution.x]	- yShift) * yScale;
		//double h3 = (levelFile.heights[x   + (y+1) * levelFile.info.mapResolution.x]- yShift) * yScale;
		//double hT = getTrueHeight(1.0/invWidthScale*(0.3333+x),1.0/invHeightScale*(0.3333+y));
		//double diff = (h1 + h2 + h3) / 3.0 - hT;

		Vec3f t1 = Vec3f(x,		(levelFile.heights[x   + y * levelFile.info.mapResolution.x]		- yShift) * yScale,	y);
		Vec3f t2 = Vec3f(x+1,	(levelFile.heights[x+1 + y * levelFile.info.mapResolution.x]		- yShift) * yScale,	y);
		Vec3f t3 = Vec3f(x,		(levelFile.heights[x   + (y+1) * levelFile.info.mapResolution.x]	- yShift) * yScale,	y+1);

		Vec3f t4 = Vec3f(x+1,	(levelFile.heights[x+1   + (y+1) * levelFile.info.mapResolution.x]	- yShift) * yScale,	y+1);
		Vec3f t5 = Vec3f(x+1,	(levelFile.heights[x+1 + y * levelFile.info.mapResolution.x]		- yShift) * yScale,	y);
		Vec3f t6 = Vec3f(x,		(levelFile.heights[x   + (y+1) * levelFile.info.mapResolution.x]	- yShift) * yScale,	y+1);

		if(collisionManager.testRayTriangle(rayStart, rayDirection, t1,	t2, t3, collisionPoint))
		{
			cellFound = Vec2i(x,y);
			cellFoundValid = true;
			double error = collisionPoint.y - getTrueHeight(1.0/invWidthScale*(collisionPoint.x),1.0/invHeightScale*(collisionPoint.z));
			return true;
		}
		if(collisionManager.testRayTriangle(rayStart, rayDirection, t4,	t5, t6, collisionPoint))
		{
			cellFound = Vec2i(x,y);
			cellFoundValid = true;
			double error = collisionPoint.y - getTrueHeight(1.0/invWidthScale*(collisionPoint.x),1.0/invHeightScale*(collisionPoint.z));
			return true;
		}
		return false;

		return (collisionManager.testRayTriangle(rayStart, rayDirection, Vec3f(x,	(levelFile.heights[x   + y * levelFile.info.mapResolution.x]		- yShift) * yScale,	y),
																		Vec3f(x+1,	(levelFile.heights[x+1 + y * levelFile.info.mapResolution.x]		- yShift) * yScale,	y),
																		Vec3f(x,	(levelFile.heights[x   + (y+1) * levelFile.info.mapResolution.x]	- yShift) * yScale,	y+1),collisionPoint) ||
				collisionManager.testRayTriangle(rayStart, rayDirection,Vec3f(x+1,	(levelFile.heights[x+1 + (y+1) * levelFile.info.mapResolution.x]	- yShift) * yScale,	y+1),
																		Vec3f(x+1,	(levelFile.heights[x+1 + y * levelFile.info.mapResolution.x]		- yShift) * yScale,	y),
																		Vec3f(x,	(levelFile.heights[x   + (y+1) * levelFile.info.mapResolution.x]	- yShift) * yScale,	y+1),collisionPoint));
	};
	
	if(rayStart.x < 0.0 || rayStart.x > levelFile.info.mapResolution.x-1 || rayStart.y > (levelFile.info.minHeight-yShift) * yScale || rayStart.y < (levelFile.info.maxHeight-yShift) * yScale || rayStart.z < 0.0 || rayStart.z > levelFile.info.mapResolution.y-1)
	{
		Vec3f aabbIntersectionPoint;
		bool intersectsAABB = collisionManager.testRayAABB(rayStart, rayDirection, AABB, aabbIntersectionPoint);
		if(!intersectsAABB)
		{
			Profiler.endElement("rayTest");
			return false;	// returns false if the ray never intersects the AABB of terrain
		}
		else
		{
			rayStart = aabbIntersectionPoint;
		}
	}
	if(rayDirection.x > -0.00001 && rayDirection.x < 0.00001 && rayDirection.z > -0.00001 && rayDirection.z < 0.00001) // ray is almost vertical
	{																												   // (should check whether it ever passes through adjacent cells)
		if(checkCell(floor(rayStart.x), floor(rayStart.z)))
		{
			collisionPoint.x = clamp(collisionPoint.x/invWidthScale, 0, levelFile.info.mapSize.x);
			collisionPoint.z = clamp(collisionPoint.z/invHeightScale, 0, levelFile.info.mapSize.y);
			Profiler.endElement("rayTest");
			return true;
		}
		else
		{
			Profiler.endElement("rayTest");
			return false;
		}
	}

	if(abs(rayDirection.x) > abs(rayDirection.z))
	{
		int increment = (rayDirection.x > 0.0) ? 1 : -1;
		double step = rayDirection.z / rayDirection.x * increment;
		int xVal = rayStart.x;
		double yVal = rayStart.z + (floor(rayStart.x)+0.5-rayStart.x) * step;



		while(xVal >= 0 && xVal <= levelFile.info.mapResolution.x-2 && ceil(yVal-0.5) >= 0.0 && floor(yVal-0.5) <= levelFile.info.mapResolution.y-2)
		{
			checkLine.push_back(Vec2f(xVal, yVal));
			if(checkCell(xVal, floor(yVal-0.5)) || checkCell(xVal, ceil(yVal-0.5)))
			{
				collisionPoint.x = clamp(collisionPoint.x/invWidthScale, 0, levelFile.info.mapSize.x);
				collisionPoint.z = clamp(collisionPoint.z/invHeightScale, 0, levelFile.info.mapSize.y);

				double error = collisionPoint.y - getTrueHeight(collisionPoint.x, collisionPoint.z);
				Profiler.setOutput("raycast error", error);
				Profiler.endElement("rayTest");
				if(error > 3.0)
					return true;
				return true;
			}
			xVal += increment;
			yVal += step;
		}
		Profiler.endElement("rayTest");
		return false;
	}
	else
	{
		int increment = (rayDirection.z > 0.0) ? 1 : -1;
		double step = rayDirection.x / rayDirection.z * increment;

		int yVal = rayStart.z; // represents center of cell 
		double xVal = rayStart.x + (floor(rayStart.z)+0.5-rayStart.z) * step;


		while(ceil(xVal-0.5) >= 0.0 && floor(xVal-0.5) <= levelFile.info.mapResolution.x-2 && yVal >= 0 && yVal <= levelFile.info.mapResolution.y-2)
		{
			checkLine.push_back(Vec2f(xVal, yVal));
			if(checkCell(floor(xVal-0.5), yVal) || checkCell(ceil(xVal-0.5), yVal))
			{
				collisionPoint.x = clamp(collisionPoint.x/invWidthScale, 0, levelFile.info.mapSize.x);
				collisionPoint.z = clamp(collisionPoint.z/invHeightScale, 0, levelFile.info.mapSize.y);

				double error = collisionPoint.y - getTrueHeight(collisionPoint.x, collisionPoint.z);
				Profiler.setOutput("raycast error", error);
				Profiler.endElement("rayTest");
				if(error > 3.0)
					return true;
				return true;
			}
			yVal += increment;
			xVal += step;
		}
		Profiler.endElement("rayTest");
		return false;
	}
}
float levelEditor::randomDisplacement(float h1, float h2, float d)
{
	d *= 65.0;
	float r = random(-d/2,d/2);
	return (h1 + h2 + r) / 2.0;
 }
float levelEditor::randomDisplacement(float h1, float h2,float h3, float h4, float d)
{
	d *= 65.0;
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

	LOD = 1;
	listBoxes["LOD"]->setOption(0);
	setMinMaxHeights();
	levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
	sliders["sea level"]->setValue(0.5);
	resetView();
	terrainValid=false;

	graphics->setLightPosition(Vec3f(levelFile.info.mapSize.x*0.5, levelFile.info.maxHeight+(levelFile.info.maxHeight-levelFile.info.minHeight)*3.0, levelFile.info.mapSize.y*0.5));
}
void levelEditor::beautifyCoastline()
{
	int x, y;
	unsigned int length = levelFile.info.mapResolution.x;
	unsigned int width = levelFile.info.mapResolution.y;

	float heightScale = pow(10.0f,sliders["height scale"]->getValue());
	float seaLevel = sliders["sea level"]->getValue() * (levelFile.info.maxHeight-levelFile.info.minHeight) + levelFile.info.minHeight;

	float interpolateRange = 50.0;
	float heightChange = 10.0;
	float height;
	for(x=0;x<length;x++)
	{
		for(y=0;y<width;y++)
		{
			height = (getHeight(x,y) - seaLevel) * heightScale;
			if(height > interpolateRange)
				setHeight(x,y,(height + heightChange));
			else if(height > -interpolateRange)
				setHeight(x,y,height + heightChange*sin(height * (PI/2)/interpolateRange));
			else
				setHeight(x,y,height - heightChange);
		}
	}

	smooth(1);
	setMinMaxHeights();
	sliders["sea level"]->setValue(clamp(-levelFile.info.minHeight / (levelFile.info.maxHeight-levelFile.info.minHeight), 0.0, 1.0));
	sliders["height scale"]->setValue(0.0);
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

	LOD = 1;
	listBoxes["LOD"]->setOption(0);
	setMinMaxHeights();
	levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
	sliders["sea level"]->setValue(0.333);
	resetView();
	terrainValid=false;
	graphics->setLightPosition(Vec3f(levelFile.info.mapSize.x*0.5, levelFile.info.maxHeight+(levelFile.info.maxHeight-levelFile.info.minHeight)*3.0, levelFile.info.mapSize.y*0.5));
}
void levelEditor::fromFile(string filename)
{
	string ext = fileManager.extension(filename);
	if(ext == ".png")
	{
		auto image = fileManager.loadFile<FileManager::textureFile>(filename);
		if(!image->valid() ||  image->width == 0 || image->height == 0)
		{
			messageBox("ERROR: heightmap file failed to load properly.");
		}
		else if(image->width == 1 || image->height == 1)
		{
			messageBox("ERROR: heightmap file must have dimensions of at least 2x2!");
		}
		else if(!isPowerOfTwo(image->width-1) || !isPowerOfTwo(image->height-1))
		{
			delete[] levelFile.heights;
			if(image->width-1)
			{
				int w = uPowerOfTwo(image->width-1)+1;
				int h = uPowerOfTwo(image->height-1)+1;
				float wScale = static_cast<float>(image->width) / w;
				float hScale = static_cast<float>(image->height) / h;
				levelFile.info.mapResolution.x = w;
				levelFile.info.mapResolution.y = h;
				levelFile.heights = new float[w * h];

				float heightVals[4];
				float interp[2];
				for(int y = 0; y < h; y++)
				{
					for(int x = 0; x < w; x++)
					{
						heightVals[0] = image->contents[image->channels * (static_cast<int>(hScale*y)     * image->width + static_cast<int>(wScale*x))    ] * 5.0;
						heightVals[1] = image->contents[image->channels * (static_cast<int>(hScale*y)     * image->width + (static_cast<int>(wScale*x)+1))] * 5.0;
						heightVals[2] = image->contents[image->channels * ((static_cast<int>(hScale*y)+1) * image->width + static_cast<int>(wScale*x))    ] * 5.0;
						heightVals[3] = image->contents[image->channels * ((static_cast<int>(hScale*y)+1) * image->width + (static_cast<int>(wScale*x)+1))] * 5.0;
						interp[0] = lerp(heightVals[0], heightVals[1], wScale*x - floor(wScale*x));
						interp[1] = lerp(heightVals[2], heightVals[3], wScale*x - floor(wScale*x));
						levelFile.heights[y * w + x] = lerp(interp[0],interp[1], hScale*y - floor(hScale*y));
					}
				}
			}
			LOD = 1;
			listBoxes["LOD"]->setOption(0);
			terrainValid=false;
			setMinMaxHeights();
			levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
			sliders["sea level"]->setValue(0.333);
			resetView();
			graphics->setLightPosition(Vec3f(levelFile.info.mapSize.x*0.5, levelFile.info.maxHeight+(levelFile.info.maxHeight-levelFile.info.minHeight)*3.0, levelFile.info.mapSize.y*0.5));

			messageBox("WARNING: heightmap file must have dimensions that are 1 more than a power of 2:\n(129x129, 257x257, 513x513,... ), heightmap has been resized");
		}
		else if(image->valid() && image->width > 0 && image->height > 0)
		{
			delete[] levelFile.heights;
			if(image->width-1)
			{
				levelFile.heights = new float[image->width * image->height];
				levelFile.info.mapResolution.x = image->width;
				levelFile.info.mapResolution.y = image->height;

				for(int y = 0; y < image->height; y++)
				{
					for(int x = 0; x < image->width; x++)
					{
						levelFile.heights[y * image->width + x] = (unsigned char)image->contents[image->channels * (y * image->width + x)] * 5.0;
					}
				}
			}
			LOD = 1;
			listBoxes["LOD"]->setOption(0);
			terrainValid=false;
			setMinMaxHeights();
			levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
			sliders["sea level"]->setValue(0.333);
			resetView();
			graphics->setLightPosition(Vec3f(levelFile.info.mapSize.x*0.5, levelFile.info.maxHeight+(levelFile.info.maxHeight-levelFile.info.minHeight)*3.0, levelFile.info.mapSize.y*0.5));
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
		std::ifstream fin(file);
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

		fin.open(filename,std::ios::binary);
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

		LOD = 1;
		listBoxes["LOD"]->setOption(0);
		setMinMaxHeights();
		levelFile.info.mapSize = levelFile.info.mapResolution * 100.0 / LOD;
		sliders["sea level"]->setValue(0.333);
		resetView();
		terrainValid=false;
		graphics->setLightPosition(Vec3f(levelFile.info.mapSize.x*0.5, levelFile.info.maxHeight+(levelFile.info.maxHeight-levelFile.info.minHeight)*3.0, levelFile.info.mapSize.y*0.5));
	}
}
void levelEditor::smooth(unsigned int a)
{
	int radius = a;

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
			for(int i = max(x-radius,0); i <= min(x+radius,w-1); i++)
			{
				for(int j = max(y-radius,0); j <= min(y+radius,h-1); j++)
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
	setMinMaxHeights();
	terrainValid=false;
}
void levelEditor::roughen(float a)
{
	float s;
	int n;
	for(int x=0; x < levelFile.info.mapResolution.x; x++)
	{
		for(int y=0; y < levelFile.info.mapResolution.y; y++)
		{
			increaseHeight(x, y, random(-a, a));
		}
	}
	setMinMaxHeights();
	terrainValid=false;
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
		//view->ortho(viewRect.x,viewRect.x+viewRect.w,viewRect.y,viewRect.y+viewRect.h,-10000,10000); set in update frame
		view->lookAt(orthoCenter+Vec3f(0,10000,0),orthoCenter,Vec3f(0,0,1));

		if(toggles["shaders"]->getValue() == 0) levelFile.info.shaderType = TERRAIN_ISLAND;
		else if(toggles["shaders"]->getValue() == 1) levelFile.info.shaderType = TERRAIN_SNOW;
		else if(toggles["shaders"]->getValue() == 2) levelFile.info.shaderType = TERRAIN_DESERT;
		else if(toggles["shaders"]->getValue() == 3) levelFile.info.shaderType = TERRAIN_MOUNTAINS;
		else levelFile.info.shaderType = TERRAIN_ISLAND;

		bool w = getShader() != 1;

		renderTerrain(w,pow(10.0f,sliders["height scale"]->getValue()), sliders["sea level"]->getValue());
	}
	else
	{
		//view->perspective(fovy, (double)sw / ((double)sh),100.0, 500000.0); set in update frame

		Vec3f e,c,u;
		c = center;

		float mSize = max(levelFile.info.mapSize.x, levelFile.info.mapSize.y);
		float heightRange = levelFile.info.maxHeight-levelFile.info.minHeight;
		e = center + Vec3f(0,heightRange + 0.65*mSize, -0.45*mSize);
		u = Vec3f(0,1,0);
		view->lookAt(e,c,u);

		graphics->setLightPosition(Vec3f(30, 70, 40));

		if(toggles["shaders"]->getValue() == 0) levelFile.info.shaderType = TERRAIN_ISLAND;
		else if(toggles["shaders"]->getValue() == 1) levelFile.info.shaderType = TERRAIN_SNOW;
		else if(toggles["shaders"]->getValue() == 2) levelFile.info.shaderType = TERRAIN_DESERT;
		else if(toggles["shaders"]->getValue() == 3) levelFile.info.shaderType = TERRAIN_MOUNTAINS;
		else levelFile.info.shaderType = TERRAIN_ISLAND;

		bool w = getShader() != 1;

		renderTerrain(w, pow(10.0f,sliders["height scale"]->getValue()), sliders["sea level"]->getValue());
 		//level->renderPreview(w,pow(10.0f,sliders["height scale"]->getValue()),sl * (maxHeight - minHeight) + minHeight);
	}



	if(getTab() == OBJECTS)
	{

		for(auto i = levelFile.objects.begin(); i != levelFile.objects.end(); i++)
		{
			auto obj = i->type != PLAYER_PLANE ? objectInfo[i->type] : objectInfo[objectInfo.getDefaultPlane()];
			if(obj && !obj->mesh.expired())
			{
				float scale = i->type & SHIP ? 1.0 : 10.0;
				sceneManager.drawMesh(view, obj->mesh.lock(), Mat4f(i->startRot,i->startloc,scale));

				graphics->drawLine(i->startloc, Vec3f(i->startloc.x, getInterpolatedHeight(i->startloc.x,i->startloc.z), i->startloc.z));
			}
		}

		dataManager.bind("white");
		auto model = shaders.bind("model");
		model->setUniform1i("tex", 0);
		model->setUniform4f("diffuse", black);
		model->setUniform3f("specular", 0,0,0);
		model->setUniformMatrix("modelTransform", Mat4f());
		for(auto i = levelFile.objects.begin(); i != levelFile.objects.end(); i++)
		{
			auto obj = i->type != PLAYER_PLANE ? objectInfo[i->type] : objectInfo[objectInfo.getDefaultPlane()];
			if(obj && !obj->mesh.expired())
			{
				graphics->drawLine(i->startloc, Vec3f(i->startloc.x, getTrueHeight(i->startloc.x,i->startloc.z), i->startloc.z));
			}
		}
		//if(placingNewObject)
		//{
		//	////////////////////////////////draw object//////////////////////////////////

		//	Vec2f cursorPos = input.getMousePos();
		//	Vec3d P0 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,0.0));
		//	Vec3d P1 = view->unProject(Vec3f(cursorPos.x,cursorPos.y,1.0));
		//	Vec3d dir = P0-P1;

		//	if(abs(dir.y) < 0.001) return;
		//	Vec3d val = P1 + dir*(levelFile.info.maxHeight+objPlacementAlt-P1.y)/dir.y;
		//	
		//	auto obj = objectInfo[levelFile.oib];
		//	if(obj)
		//	{
		//		sceneManager.drawMesh(view, obj->mesh, Mat4f(Quat4f(),Vec3f(val),10.0));
		//	}
		//	////////////////////////////////draw grid////////////////////////////////// --- SHOULD BE REWRITTEN
		//	//glDepthMask(false);
		//	//glColor4f(0.1,0.3,1.0,0.3);

		//	//graphics->drawQuad(	Vec3f(0,						maxHeight+objPlacementAlt,	0),
		//	//					Vec3f(0,						maxHeight+objPlacementAlt,	level->ground()->sizeZ()),
		//	//					Vec3f(level->ground()->sizeX(),	maxHeight+objPlacementAlt,	0),
		//	//					Vec3f(level->ground()->sizeX(),	maxHeight+objPlacementAlt,	level->ground()->sizeZ()));

		//	//glColor4f(0.3,0.5,1.0,0.2);
		//	//for(float f=0.0; f<level->ground()->sizeX() + 0.001; f+=level->ground()->sizeX() / 32.0)
		//	//{
		//	//	graphics->drawLine(Vec3f(f,maxHeight+10,0), Vec3f(f,maxHeight+10,level->ground()->sizeZ()));
		//	//	//glVertex3f(f,maxHeight+10,0);
		//	//	//glVertex3f(f,maxHeight+10,level->ground()->sizeZ());
		//	//}
		//	//for(float f=0.0; f<level->ground()->sizeZ() + 0.001; f+=level->ground()->sizeZ() / 32.0)
		//	//{
		//	//	graphics->drawLine(Vec3f(0,maxHeight+10,f), Vec3f(level->ground()->sizeX(),maxHeight+10,f));
		//	//	//glVertex3f(0,maxHeight+10,f);
		//	//	//glVertex3f(level->ground()->sizeX(),maxHeight+10,f);
		//	//}
		//	//glColor3f(1,1,1);
		//	//glDepthMask(true);
		//	////////////////////////////////end grid///////////////////////////////////
		//}

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
		shaders.bind("circle shader");
		updateObjectCircles();
		for(auto i = objectCircles.begin(); i != objectCircles.end(); i++)
		{
			if(i->first == selectedObject)
				graphics->setColor(0.0,1.0,0.0);
			else
				graphics->setColor(1.0,1.0,1.0);
			graphics->drawOverlay(Rect::CWH(i->second.center.x, i->second.center.y, i->second.radius*2, i->second.radius*2));
		}
		shaders.bind("ortho");

		graphics->setColor(0.35,0.35,0.35);
		graphics->drawOverlay(Rect::XYXY(0.0, 0.195, 0.260, 1.0),"white");
		graphics->setColor(1.0,1.0,1.0);
	}

	if(getTab() == REGIONS)
	{
		Rect viewRect = orthoView();

		shaders.bind("circle shader");
		graphics->setColor(1.0,1.0,1.0);
		for(auto i = levelFile.regions.begin(); i != levelFile.regions.end(); i++)
		{
			Vec2f c((i->centerXYZ[0] - viewRect.x + orthoCenter.x)/viewRect.w*sAspect, (i->centerXYZ[2] - viewRect.y + orthoCenter.z)/viewRect.h);
			Vec2f s(i->radius/viewRect.w*sAspect*2.0, i->radius/viewRect.h*2.0);
			graphics->drawOverlay(Rect::CWH(c,s));
		}

		shaders.bind("ortho");
	}
	graphics->setColor(0.2,0.2,0.2);
	graphics->drawOverlay(Rect::XYXY(0.0,0.960, sAspect, 1.0),"white");
	graphics->setColor(1.0,1.0,1.0);
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

			terrainIndexBuffer->setData(indices, GraphicsManager::TRIANGLES, numTerrainIndices);
			//terrainVBO->setIndexData(sizeof(unsigned short)*numTerrainIndices, indices);


			//////////////////////////////////////texture///////////////////////////////////////////////////
			unsigned char* tex = new unsigned char[width*height*4];
			Vec3f n;
			for(int x = 0; x < width; x++)
			{
				for(int z = 0; z < height; z++)
				{
					n = getNormal(x, z);
					tex[4*(x+z*width) + 2] = static_cast<unsigned char>((n.x*0.5+0.5) * 255.0);
					tex[4*(x+z*width) + 1] = static_cast<unsigned char>(n.y * 255.0);
					tex[4*(x+z*width) + 0] = static_cast<unsigned char>((n.z*0.5+0.5) * 255.0);
					tex[4*(x+z*width) + 3] = static_cast<unsigned char>((getHeight(x, z)-levelFile.info.minHeight)/(levelFile.info.maxHeight-levelFile.info.minHeight)*255.0);
				}
			}
			groundTex->setData(width,height,GraphicsManager::texture::BGRA, false, false, tex);

			terrainValid=true;
		}
		//graphics->setWireFrame(true);
		if(levelFile.info.shaderType == TERRAIN_ISLAND)
		{
			/*if(levelFile.info.shaderType == TERRAIN_ISLAND)	dataManager.bind("island preview terrain");
			else											dataManager.bind("grass preview terrain");*/

			auto shader = shaders.bind("island preview terrain");
			//dataManager.bind("island preview terrain");
			shader->bind();

			dataManager.bind("sand",0);
			dataManager.bind("grass",1);
			dataManager.bind("rock",2);
			dataManager.bind("LCnoise",3);
			groundTex->bind(4);
			

			shader->setUniform1f("heightScale",	scale);
			shader->setUniform1f("minHeight", -seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			shader->setUniform1f("heightRange", (levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			shader->setUniform3f("invScale", 1.0/(levelFile.info.mapSize.x),1.0/((levelFile.info.maxHeight-levelFile.info.minHeight)*scale),1.0/(levelFile.info.mapSize.y));

			Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
			shader->setUniformMatrix("cameraProjection", cameraProjectionMat);
			shader->setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));

			shader->setUniform3f("invScale",	1.0/levelFile.info.mapSize.x, 1.0/(levelFile.info.maxHeight-levelFile.info.minHeight), 1.0/levelFile.info.mapSize.y);
			shader->setUniform3f("sunPosition", graphics->getLightPosition());
			shader->setUniform3f("eyePos",		view->camera().eye);
			shader->setUniform1f("time",		world.time());
			shader->setUniform1i("sand",		0);
			shader->setUniform1i("grass",		1);
			shader->setUniform1i("rock",		2);
			shader->setUniform1i("LCnoise",		3);
			shader->setUniform1i("groundTex",	4);
			terrainIndexBuffer->bindBuffer(terrainVertexBuffer);
			terrainIndexBuffer->drawBuffer();

			shaders.bind("model");
		}
		else if(levelFile.info.shaderType == TERRAIN_MOUNTAINS)
		{
			/*if(levelFile.info.shaderType == TERRAIN_ISLAND)	dataManager.bind("island preview terrain");
			else											dataManager.bind("grass preview terrain");*/

			auto shader = shaders.bind("mountain preview terrain");
			//dataManager.bind("island preview terrain");
			shader->bind();

			dataManager.bind("sand",0);
			dataManager.bind("grass",1);
			dataManager.bind("rock",2);
			dataManager.bind("LCnoise",3);
			groundTex->bind(4);
			

			shader->setUniform1f("heightScale",	scale);
			shader->setUniform1f("minHeight", -seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			shader->setUniform1f("heightRange", (levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			shader->setUniform3f("invScale", 1.0/(levelFile.info.mapSize.x),1.0/((levelFile.info.maxHeight-levelFile.info.minHeight)*scale),1.0/(levelFile.info.mapSize.y));

			Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
			shader->setUniformMatrix("cameraProjection", cameraProjectionMat);
			shader->setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));

			shader->setUniform3f("invScale",	1.0/levelFile.info.mapSize.x, 1.0/(levelFile.info.maxHeight-levelFile.info.minHeight), 1.0/levelFile.info.mapSize.y);
			shader->setUniform3f("sunPosition", graphics->getLightPosition());
			shader->setUniform3f("eyePos",		view->camera().eye);
			shader->setUniform1f("time",		world.time());
			shader->setUniform1i("sand",		0);
			shader->setUniform1i("grass",		1);
			shader->setUniform1i("rock",		2);
			shader->setUniform1i("LCnoise",		3);
			shader->setUniform1i("groundTex",	4);
			terrainIndexBuffer->bindBuffer(terrainVertexBuffer);
			terrainIndexBuffer->drawBuffer();

			shaders.bind("model");
		}
		else if(levelFile.info.shaderType == TERRAIN_DESERT)
		{
			auto shader = shaders.bind("desert preview terrain");
			//dataManager.bind("desert preview terrain");

			dataManager.bind("desertSand",0);
			dataManager.bind("sand", 1);
			dataManager.bind("LCnoise",2);
			groundTex->bind(3);



			shader->setUniform1f("heightScale",	scale);
			shader->setUniform1f("minHeight", -seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			shader->setUniform1f("heightRange", (levelFile.info.maxHeight-levelFile.info.minHeight)*scale);
			shader->setUniform3f("invScale", 1.0/(levelFile.info.mapSize.x),1.0/((levelFile.info.maxHeight-levelFile.info.minHeight)*scale),1.0/(levelFile.info.mapSize.y));

			Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
			shader->setUniformMatrix("cameraProjection", cameraProjectionMat);
			shader->setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));

			shader->setUniform3f("invScale",	1.0/levelFile.info.mapSize.x, 1.0/(levelFile.info.maxHeight-levelFile.info.minHeight), 1.0/levelFile.info.mapSize.y);
			shader->setUniform3f("sunPosition", graphics->getLightPosition());
			shader->setUniform3f("eyePos",		view->camera().eye);
			shader->setUniform1i("sand",		0);
			shader->setUniform1i("sand2",		1);
			shader->setUniform1i("LCnoise",		2);
			shader->setUniform1i("groundTex",	3);
			terrainIndexBuffer->bindBuffer(terrainVertexBuffer);
			terrainIndexBuffer->drawBuffer();

			shaders.bind("model");
		}
		else if(levelFile.info.shaderType == TERRAIN_SNOW)
		{
			auto shader = shaders.bind("snow preview terrain");
			//dataManager.bind("snow preview terrain");

			dataManager.bind("snow",0);
			dataManager.bind("LCnoise",1);
			groundTex->bind(2);

			shader->setUniform1f("heightScale",	scale);

			Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
			shader->setUniformMatrix("cameraProjection", cameraProjectionMat);
			shader->setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));

			shader->setUniform3f("invScale",	1.0/levelFile.info.mapSize.x, 1.0/(levelFile.info.maxHeight-levelFile.info.minHeight), 1.0/levelFile.info.mapSize.y);
			shader->setUniform3f("sunPosition", graphics->getLightPosition());
			shader->setUniform3f("eyePos",		view->camera().eye);
			shader->setUniform1f("time",		world.time());
			shader->setUniform1i("snow",		0);
			shader->setUniform1i("LCnoise",		1);
			shader->setUniform1i("groundTex",	2);

			terrainIndexBuffer->bindBuffer(terrainVertexBuffer);
			terrainIndexBuffer->drawBuffer();

			shaders.bind("model");
		}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(levelFile.info.shaderType == TERRAIN_ISLAND)
	{
		auto ocean = shaders.bind("ocean preview");
		//dataManager.bind("ocean preview");

		dataManager.bind("ocean normals",0);
	//	if(levelFile.info.shaderType == SHADER_OCEAN)	dataManager.bindTex(0,1);
	//	else											groundTex->bind(1);
	//	dataManager.bind("rock",2);
	//	dataManager.bind("sand",3);
		Mat4f cameraProjectionMat = view->projectionMatrix() * view->modelViewMatrix();
		ocean->setUniformMatrix("cameraProjection", cameraProjectionMat);
		ocean->setUniformMatrix("modelTransform", Mat4f());

		ocean->setUniform1i("bumpMap", 0);
	//	dataManager.setUniform1i("groundTex", 1);
	//	dataManager.setUniform1i("rock", 2);
	//	dataManager.setUniform1i("sand", 3);
		ocean->setUniform1f("time", world.time());
	//	dataManager.setUniform1f("seaLevel", (seaLevelOffset-levelFile.info.minHeight)/(levelFile.info.maxHeight-levelFile.info.minHeight));
	//	dataManager.setUniform1f("XZscale", levelFile.info.mapSize.x);

		//glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->mResolution.x)/uPowerOfTwo(mGround->mResolution.x),(float)(mGround->mResolution.y)/uPowerOfTwo(mGround->mResolution.y));
		graphics->drawQuad(	Vec3f(0,0,0),
							Vec3f(levelFile.info.mapSize.x,0,0),
							Vec3f(levelFile.info.mapSize.x,0,levelFile.info.mapSize.y),
							Vec3f(0,0,levelFile.info.mapSize.y));


		shaders.bind("model");
		//dataManager.bind("model");
	}


	/*  DEBUG DRAWING  */
	graphics->setDepthTest(false);
	float xMult = levelFile.info.mapSize.x / (levelFile.info.mapResolution.x - 1);
	float yMult = levelFile.info.mapSize.y / (levelFile.info.mapResolution.y - 1);
	auto color3D = shaders.bind("color3D");
	color3D->setUniform4f("color", 0.5, 0.5, 0.5, 1.0);
	color3D->setUniformMatrix("cameraProjection", view->projectionMatrix() * view->modelViewMatrix());
	color3D->setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(0,-seaLevelOffset*(levelFile.info.maxHeight-levelFile.info.minHeight),0)*scale, Vec3f(1,scale,1)));
	Vec3f A, B, C, D;
	for(auto i = checkedCells.begin(); i != checkedCells.end(); i++)
	{
		if(!cellFoundValid || *i != cellFound)
			color3D->setUniform4f("color", 1.0, 0.0, 0.0, 1.0);
		else
			color3D->setUniform4f("color", 0.0, 1.0, 0.0, 1.0);
		A = Vec3f((i->x)*xMult,		getHeight(i->x, i->y)-levelFile.info.minHeight+1,		(i->y)*yMult);	
		B = Vec3f((i->x+1)*xMult,	getHeight(i->x+1, i->y)-levelFile.info.minHeight+1,	(i->y)*yMult);
		C = Vec3f((i->x+1)*xMult,	getHeight(i->x+1, i->y+1)-levelFile.info.minHeight+1,	(i->y+1)*yMult);
		D = Vec3f((i->x)*xMult,		getHeight(i->x, i->y+1)-levelFile.info.minHeight+1,	(i->y+1)*yMult);
		graphics->drawLine(A,B);
		graphics->drawLine(B,C);
		graphics->drawLine(C,D);
		graphics->drawLine(D,A);
	}
	bool renderLine=false;
	color3D->setUniform4f("color", 0.0, 0.0, 0.0, 1.0);
	for(auto i = checkLine.begin(); i != checkLine.end() &&  i+1 != checkLine.end(); i++)
	{
		if(renderLine=!renderLine)
		{
			A = Vec3f((i->x)*xMult,		getHeight(i->x, i->y)-levelFile.info.minHeight+1,		(i->y)*yMult);	
			B = Vec3f(((i+1)->x)*xMult,	getHeight((i+1)->x, (i+1)->y)-levelFile.info.minHeight+1,	((i+1)->y)*yMult);
			graphics->drawLine(A,B);
		}
	}
	graphics->setDepthTest(true);
	color3D->setUniform4f("color", 0.0, 0.0, 0.0, 0.3);
	//graphics->drawQuad(	Vec3f(rStart.x*xMult,0,rStart.z*yMult),
	//					Vec3f(rStart.x*xMult,levelFile.info.maxHeight-levelFile.info.minHeight,rStart.z*yMult),
	//					Vec3f((rStart.x+200000.0*rDirection.x)*xMult,levelFile.info.maxHeight-levelFile.info.minHeight,(rStart.z+200000.0*rDirection.z)*yMult),
	//					Vec3f((rStart.x+200000.0*rDirection.x)*xMult,0,(rStart.z+200000.0*rDirection.z)*yMult));
	/*  END DEBUG DRAWING  */

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
	//graphics->setWireFrame(false);
}
void levelEditor::renderObjectPreview()
{
	if(getTab() == OBJECTS)
	{
		graphics->setDepthTest(false);
		auto ortho = shaders.bind("ortho");
		ortho->setUniform1f("sAspect", 1.333);
		//ortho->setUniform4f("viewConstraint", 0,0,1,1);
		graphics->setColor(0.35,0.35,0.35);
		graphics->drawOverlay(Rect::XYXY(0.0, 0.0, 1.333, 1.0),"white");
		graphics->setColor(1.0,1.0,1.0);
		graphics->drawOverlay(Rect::XYXY(0.04, 0.04, 1.333-0.04, 0.96),"white");
		ortho->setUniform1f("sAspect", sAspect);
//		ortho->setUniform4f("viewConstraint", 0,0,1,1);
		graphics->setDepthTest(true);
		objectPreviewView->lookAt(Vec3f(0,2,0), Vec3f(0,0,0), Vec3f(0,0,1));
		
		objectType objType = typeOptions[listBoxes["object type"]->getOptionNumber()];
		auto obj = objectInfo[objType != PLAYER_PLANE ? objType : objectInfo.getDefaultPlane()];
		if(obj && !obj->mesh.expired())
		{
			Sphere<float> sphere = obj->mesh.lock()->boundingSphere;
			sceneManager.drawMesh(objectPreviewView, obj->mesh.lock(), Mat4f(Quat4f(),-sphere.center/sphere.radius,1.0/sphere.radius));
		}
	}
}
}
