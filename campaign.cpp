
#include "game.h"

namespace gui{
campaign::campaign(std::shared_ptr<LevelFile> lvl): dogFight(lvl), countdown(0.0), restart(false), levelup(false)
#ifdef _DEBUG
	,slow(false)
#endif
{
	view = graphics->genView();
	view->viewport(0, 0, sAspect, 1.0);
	view->perspective(50.0, (double)sw / ((double)sh), 1.0, 500000.0);
	view->setRenderFunc(bind(&campaign::render3D, this, placeholders::_1));
	//graphics->setLightPosition(Vec3f(0.0, 1600000.0, 0.0));
	graphics->setLightPosition(Vec3f(0.0, 160000.0, 100000.0));
	//ephemeris.setTime(0.0, 1, 1, 2000.0);
	//graphics->setLightPosition(ephemeris.getSunDirection() * 100000.0);
}
bool campaign::init()
{
	world.create();
	level->initializeWorld(1);
	return true;
}
int campaign::update()
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

	//set camera position
	nPlane* p=(nPlane*)players[0]->getObject();
	auto camera = players[0]->getCamera(p->controled || p->dead);
	view->lookAt(camera.eye, camera.center, camera.up);

	players[0]->setVibrate(p->cameraShake);

	//check whether to toggle first person view
	if(input.getKey(VK_F1))
	{
		players[0]->toggleFirstPerson();
		input.up(VK_F1);
	}
	//check whether to bring up the in-game menu
	if(input.getKey(VK_ESCAPE) || input.getXboxController(0).getButton(XINPUT_START))
	{
		menuManager.setPopup(new gui::inGame);
		input.up(VK_ESCAPE);
	}
	//check for collisions between objects
	checkCollisions();

#ifdef _DEBUG
	//slow down game speed to 10%
	if(input.getKey(0x54))
	{
		input.up(0x54);
		if(!slow)
		{
			world.time.changeSpeed(0.1, 5.0);
			slow = true;
		}
		else
		{
			world.time.changeSpeed(1.0, 5.0);
			slow = false;
		}
	}
	//test death animation
	if(input.getKey(0x4c))
	{
		((nPlane*)players[0]->getObject())->loseHealth(world.time.length()/10.0);
	}
#endif

	if(levelup)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			string nLevel = level->info.nextLevel;
			if(nLevel == "") nLevel="media/map file.lvl";

			std::shared_ptr<LevelFile> l(new LevelFile);
			if(l->loadZIP(nLevel))
			{
				menuManager.setMenu(new gui::campaign(l));
			}
		}
	}
	else if(restart)
	{
		countdown-=world.time.length();
		if(countdown<=0)
		{
			menuManager.setMenu(new gui::campaign(level));
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

		if(enemies_left == 0)
		{
			levelup=true;
			countdown=1000;
		}
		else if(players[0]->getObject()->dead)
		{
			restart=true;
			countdown=3000;
		}
	}



	//((plane*)world.objectList[players[0].objectNum()])->setControlState(players[0].getControlState());



	//if(settings.ON_HIT==RESTART && world.objectList[players[0].objectNum()]->dead)
	//{
	//	//need to add code to restart the level
	//	return 30;
	//}
	//if((100-enemies_left*100/settings.ENEMY_PLANES>=settings.KILL_PERCENT_NEEDED || input.getKey(0x4E)) && (levelNum<TOTAL_LEVELS && !levelup))
	//{
	//	levelup=true;//newLevel(level+1);
	//	countdown=1000;
	//}
	return 30;
}
void campaign::render()
{
	nPlane* p = ((nPlane*)players[0]->getObject());

	if(players[0]->firstPersonView && !p->controled && !p->dead)
	{
	//	planeIdBoxes(p,0,0,sw,sh);
	//	dataManager.bind("ortho");
		graphics->drawOverlay(Rect::XYXY(0,0.0,sAspect,1.0),"cockpit square");
	//	dataManager.unbindShader();

		targeter(0.5*sAspect, 0.5, 0.08, -p->roll);
		radar(0.2 * sAspect, 0.567, 0.125, 0.125, true, p);

		healthBar(0.175*sAspect, 0.35, 0.25*sAspect, 0.333, p->health/p->maxHealth,true);

		//speedMeter(280,533,344,597,p.accel.magnitude()*30.5+212);
		//altitudeMeter(456,533,520,597,p.altitude);
	}
	else if(!p->dead  && !p->controled)
	{
//		radar(sAspect-0.167, 0.833, 0.1333, 0.1333, false, p);
//		healthBar(0.768*sAspect, 0.042, 0.188*sAspect, 0.042, p->health/p->maxHealth,false);

		if(p->target != 0 && world[p->target] != nullptr)
		{
			Vec3f proj = (view->projectionMatrix() * view->modelViewMatrix()) * world[p->target]->position;
			if(proj.z < 1.0 && (proj.x > -1.02 && proj.x < 1.02) && (proj.y > -1.02 && proj.y < 1.02))
			{
				dataManager.bind("circle shader");
				if(p->targetLocked)		graphics->setColor(1,0,0);
				else					graphics->setColor(0,0,1);
				graphics->drawOverlay(Rect::CWH(view->project(world[p->target]->position), Vec2f(0.02,0.02)));
				graphics->setColor(1,1,1);
				dataManager.bind("ortho");
			}
			else
			{
				Vec3f fwd = view->camera().fwd;
				Vec3f up = view->camera().up;
				Vec3f right = view->camera().right;
				Vec3f direction = (world[p->target]->position - p->position).normalize();
				Vec3f projectedDirection = direction - fwd * (fwd.dot(direction));
				Vec2f screenDirection(projectedDirection.dot(right), projectedDirection.dot(up));
				screenDirection = screenDirection.normalize();
				Angle ang = atan2A(screenDirection.y,screenDirection.x);
				screenDirection.x = clamp( (screenDirection.x+1.0)*sAspect/2.0, 0.05, sAspect-0.05);
				screenDirection.y = clamp( (-screenDirection.y+1.0)/2.0, 0.05, 0.95);
				if(p->targetLocked)		graphics->setColor(1,0,0);
				else					graphics->setColor(0,0,1);
				graphics->drawRotatedOverlay(Rect::CWH(screenDirection, Vec2f(0.08,0.08)),ang, "arrow");
				graphics->setColor(1,1,1);
			}
		}
	}


	if(levelup)
	{
		float v = (countdown > 250) ? ((750-(countdown-250))/750) : (countdown/250);
		graphics->drawOverlay(Rect::CWH(sAspect/2, 0.5, sAspect*v, v), "next level");
	}
}

void campaign::render3D(unsigned int v)
{
	//if(firstFrame)
	//{
	//	firstFrame = false;
	//	return;
	//}

	drawScene(view, 0);
	if(players[v]->firstPersonView && !((nPlane*)players[v]->getObject())->controled && !players[v]->getObject()->dead)
		sceneManager.renderScene(view, players[v]->getObject()->meshInstance);
	else
		sceneManager.renderScene(view);

	//graphics->setDepthTest(false);
	//graphics->setColor(1,0,0,1);
	//dataManager.bind("model");
	//dataManager.bind("white");
	//dataManager.setUniformMatrix("modelTransform", Mat4f());
	//graphics->drawTriangle(Vec3f(0,0,0), Vec3f(100000,0,100000), Vec3f(50000,1000,50000));
	//graphics->setDepthTest(true);
}
}
