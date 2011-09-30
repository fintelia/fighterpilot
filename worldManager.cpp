
#include "engine.h"

void WorldManager::create(std::shared_ptr<Level> lvl)
{
	destroy();//just to be safe
	level = lvl;
	time.reset();

	//((Level::heightmapGL*)level->ground())->setShader(dataManager.getId("grass new terrain"));
	particleManager.init();
}
void WorldManager::destroy()
{
	clearObjects();
	particleManager.shutdown();
}

float WorldManager::elevation(Vec2f v) const
{
	if(level)
	{
		return max(0,level->ground()->height(v.x,v.y));
	}
	return 0;
}
float WorldManager::elevation(float x, float z) const
{
	if(level)
	{
		return max(0,level->ground()->height(x,z));
	}
	return 0;
}
float WorldManager::altitude(Vec3f v) const
{
	if(level)
	{
		float h = level->ground()->height(v.x,v.z);
		return v.y-max(0,h);
	}
	return 0;
}
float WorldManager::altitude(float x, float y, float z) const
{
	if(level)
	{
		float h = level->ground()->height(x,z);
		return y-max(0,h);
	}
	return 0;
}
bool WorldManager::isLand(Vec2f v) const
{
	return level->ground()->height(v.x,v.y) > 0;
}
bool WorldManager::isLand(float x, float z) const
{
	return level->ground()->height(x,z) > 0;
}
Level::heightmapGL* const WorldManager::ground() const
{
	if(level)
	{
		return (Level::heightmapGL*)level->ground();
	}
	return NULL;
}
void WorldManager::update()
{
	updateObjects(time(),time.length());
	particleManager.update();
}