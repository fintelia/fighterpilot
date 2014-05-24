
#include "game.h"

namespace gui{
campaign::campaign(shared_ptr<const LevelFile> lvl, planeType plane): dogFight(lvl), countdown(0.0), restart(false), levelup(false), victory(false), playerPlane(plane)
#ifdef _DEBUG
	,slow(false)
#endif
{
	view = graphics->genView();
	view->viewport(0, 0, sAspect, 1.0);
	view->perspective(50.0, (double)sw / ((double)sh), 1.0, 500000.0);
	view->setRenderFunc(std::bind(&campaign::render3D, this));
	view->setTransparentRenderFunc(std::bind(&campaign::renderTransparency, this));
#ifdef _DEBUG
	debugPane = graphics->genView();
	debugPane->viewport(sAspect * 0.65, 0.65, sAspect * 0.3, 0.3);
	debugPane->perspective(50.0, (double)sw / ((double)sh), 1.0, 500000.0);
	debugPane->setRenderFunc([this]{
		graphics->setDepthTest(false);
		//drawScene(debugPane, 0);
		//sceneManager.renderScene(debugPane);
		auto ortho = shaders.bind("ortho");
		ortho->setUniform4f("color", 1,1,1,0.5);
		graphics->drawOverlay(Rect::XYWH(0,0,sAspect,1),"white");

		dataManager.bind("white");
		auto model = shaders.bind("model");
		model->setUniform1i("tex", 0);
		
		model->setUniform3f("specular", 0,0,0);
		model->setUniformMatrix("modelTransform", Mat4f());
		model->setUniformMatrix("cameraProjection", debugPane->projectionMatrix() * debugPane->modelViewMatrix() );

		int index = 0;
		std::array<Color3,4> colorList = {green, red, blue, black};
		auto& planes = world(PLANE);
		for(auto o = planes.begin(); o != planes.end(); o++){
			model->setUniform4f("diffuse", colorList[(index++) % 4]);
			auto p = dynamic_pointer_cast<class plane>(o->second);
			for(unsigned int l = 0u; l+15u < p->locationHistory.size(); l += 15u){
				graphics->drawLine(p->locationHistory[l], p->locationHistory[l+15]);
			}
			if(p->locationHistory.size() > 15)
				graphics->drawLine(p->locationHistory[p->locationHistory.size() - 15u], p->locationHistory.back());
			else if(!p->locationHistory.empty())
				graphics->drawLine(p->locationHistory.front(), p->locationHistory.back());
		}
		graphics->setDepthTest(true);
	});
	debugPane->setTransparentRenderFunc([]{});
#endif
	//graphics->setLightPosition(Vec3f(0.0, 1600000.0, 0.0));
	graphics->setLightPosition(Vec3f(0.0, 100000.0, 100000.0).normalize());
	//ephemeris.setTime(0.0, 1, 1, 2000.0);
	//graphics->setLightPosition(ephemeris.getSunDirection() * 100000.0);
}
bool campaign::init()
{
	vector<planeType> planes(1, playerPlane);

	world.create();
	level->initializeWorld(planes);

	if(level->info.night)
	{
		view->blurStage(true);
		view->postProcessShader(shaders("gamma night"));
	}
	else
	{
		view->blurStage(false);
		view->postProcessShader(shaders("gamma bloom"));
	}
	return true;
}
bool campaign::menuKey(int mkey)
{
	if(mkey == MENU_BACK)
	{
		menuManager.setPopup(new gui::inGame);
		return true;
	}
	return false;
}
void campaign::updateFrame()
{
	//static double timeSinceSkyUpdate = 0;
	//ephemeris.setTime(world.time()/3000, 1, 1, 2000.0);
	//Vec3f sun = ephemeris.getSunDirection();
	//graphics->setLightPosition(sun* 100000.0);
	//if(timeSinceSkyUpdate > 200.0)
	//{
	//	world.generateSky(acos(sun.y), atan2(sun.z,sun.x), 1.8);
	//	timeSinceSkyUpdate -= 200.0;
	//}
	//timeSinceSkyUpdate +=world.time.length();


	//auto& controller = input.getXboxController(0);

	//check whether to toggle first person view
	if(input.getKey(VK_F1))
	{
		players[0]->toggleFirstPerson();
		input.up(VK_F1);
	}

//	graphics->setLightPosition(Vec3f(	0.0 , 10000.0* cos(world.time() *0.001 * 0.1), 
//										100000.0 * sin(world.time() *0.001 * 0.1)));
//	graphics->setLightPosition(Vec3f(0.0,10000.0,100000.0));

	//set camera position
	shared_ptr<plane> p=players[0]->getObject();
	auto camera = players[0]->getCamera(p->controled || p->dead);
	view->lookAt(camera.eye, camera.center, camera.up);
#ifdef _DEBUG
	debugPane->lookAt(camera.center + Vec3f(0, 3000, -760), camera.center, Vec3f(0,0,1));
#endif
	if(level->info.night)
	{
		if(players[0]->firstPersonView && !p->controled && !p->dead)
		{
			view->blurStage(false);
			view->postProcessShader(shaders("gamma night vision"));
		}
		else
		{
			view->blurStage(true);
			view->postProcessShader(shaders("gamma night"));
		}
	}
	players[0]->setVibrate(p->cameraShake);

#ifdef _DEBUG
	//slow down game speed to 10%
	if(input.getKey(0x54))
	{
		input.up(0x54);
		if(!slow)
		{
			world.time.changeSpeed(0.1, 300.0);
			slow = true;
		}
		else
		{
			world.time.changeSpeed(1.0, 300.0);
			slow = false;
		}
	}
	//test death animation
	if(input.getKey(0x4c))
	{
		players[0]->getObject()->loseHealth(world.time.length()/30.0);
	}
#endif

	if(levelup)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			string nLevel = level->info.nextLevel;
			//if(nLevel == "") nLevel="media/map file.lvl";

			shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(nLevel))
			{
				menuManager.setMenu(new gui::campaign(l, playerPlane));
			}
		}
	}
	else if(restart)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			menuManager.setMenu(new gui::campaign(level, playerPlane));
		}
	}
	else if(victory)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			menuManager.setMenu(new gui::chooseMode);
			menuManager.setPopup(new gui::credits);
		}
	}
	else
	{
		int enemies_left=0;
		auto planes = world(PLANE);
		auto AAA = world(ANTI_AIRCRAFT_ARTILLARY);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			if((*i).second->team != players[0]->getObject()->team && !(*i).second->dead)
				enemies_left++;
		}
		for(auto i = AAA.begin(); i != AAA.end();i++)
		{
			if((*i).second->team != players[0]->getObject()->team && !(*i).second->dead)
				enemies_left++;
		}

		if(enemies_left == 0 && level->info.nextLevel != "")
		{
			levelup=true;
			countdown=1000;
		}
		else if(enemies_left == 0)
		{
			victory=true;
			countdown=1000;
		}
		else if(players[0]->getObject()->dead)
		{
			restart=true;
			countdown=5000;
		}
	}
}
void campaign::render()
{
	shared_ptr<plane> p = players[0]->getObject();

	if(players[0]->firstPersonView && !p->controled && !p->dead)
	{
		graphics->drawOverlay(Rect::XYXY(0,0.0,sAspect,1.0),"cockpit square");

		targeter(0.5*sAspect, 0.5, 0.08, -p->roll);
		radar(0.2 * sAspect, 0.567, 0.125, 0.125, true, p);

		healthBar(0.175*sAspect, 0.35, 0.25*sAspect, 0.333, p->health/100.0);
	}
	else if(!p->dead && !p->controled)
	{
		//radar(sAspect-0.167, 0.833, 0.1333, 0.1333, false, p);
		if(!graphics->isHighResScreenshot())
		{
			auto planes = world(PLANE);
			for(auto i = planes.begin(); i != planes.end();i++)
			{
				if(i->second->id != p->id && i->second->team == p->team && !i->second->dead)
					drawHudIndicator(view, p, i->second, Color(0,1,0,0.5), Color4(0.66*0.2989,0.34+0.66*0.2989,0.66*0.2989,0.5));
			}
			
			if(p->target != 0 && !graphics->isHighResScreenshot())
			{
				auto targetPtr = world[p->target];
				drawHudIndicator(view, p, targetPtr, p->targetLocked ? Color4(1,0,0) : Color4(0,0,1), p->targetLocked ? Color4(0.34+0.66*0.2989,0.66*0.2989,0.66*0.2989) : Color4(0.66*0.1140,0.66*0.1140,0.34+0.66*0.1140,0.5));
			}
		}
	}

	if(levelup && !graphics->isHighResScreenshot())
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, v, 0.25*v), "next level");
	}
	if(victory && !graphics->isHighResScreenshot())
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, v, 0.25*v), "victory");
	}
}

void campaign::render3D()
{
	drawScene(view, 0);

	if(players[0]->firstPersonView && !(players[0]->getObject())->controled && !players[0]->getObject()->dead)
	{
		sceneManager.renderScene(view, players[0]->getObject()->meshInstance);
		world.renderFoliage(view);
		sceneManager.renderSceneTransparency(view, players[0]->getObject()->meshInstance);
	}
	else
	{
		sceneManager.renderScene(view);
		world.renderFoliage(view);
		sceneManager.renderSceneTransparency(view);
	}
}
void campaign::renderTransparency()
{
	particleManager.render(view);
}
}
