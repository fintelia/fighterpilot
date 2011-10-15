

#include "engine.h"

void WorldManager::create()
{
	destroy();//just to be safe
	time.reset();

	particleManager.init();
}
void WorldManager::destroy()
{
	clearObjects();
	particleManager.shutdown();
}
void WorldManager::update()
{
	updateObjects(time(),time.length());
	particleManager.update();
}
