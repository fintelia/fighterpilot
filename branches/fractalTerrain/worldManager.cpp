

#include "engine.h"

WorldManager::WorldManager(shared_ptr<ClipMap> clipMap):
	Terrain(clipMap)
{
	particleManager.init();
}
WorldManager::~WorldManager()
{
	clearObjects();
	sceneManager.reset();
	particleManager.shutdown();
}
void WorldManager::simulationUpdate()
{
	objectsSimulationUpdate(time(),time.length());
	particleManager.update();
}
void WorldManager::frameUpdate()
{
	terrainFrameUpdate(time());
	objectsFrameUpdate(time.interpolate());
}
