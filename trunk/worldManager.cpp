

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
void WorldManager::simulationUpdate()
{
	objectsSimulationUpdate(time(),time.length());
}
void WorldManager::frameUpdate()
{
	objectsFrameUpdate(time.interpolate());
}