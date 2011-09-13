
#include "main.h"

void WorldManager::create(std::shared_ptr<Level> lvl)
{
	destroy();//just to be safe
	level = std::shared_ptr<Level>(lvl);
	time.reset();

	//((Level::heightmapGL*)level->ground())->setShader(dataManager.getId("grass new terrain"));

	int playerNum=0;
	for(auto i = level->objects().begin(); i != level->objects().end(); i++)
	{
		world.newObject(*i);
		//if(i->controlType & CONTROL_HUMAN)
		//{
		//	if(playerNum <= 1)
		//	{
		//		int planeNum = world.objectList.newPlane(*);
		//		players[playerNum].active(true);
		//		players[playerNum].objectNum(planeNum);
		//		playerNum++;
		//	}
		//}
		//else
		//{
		//	world.objectList.newPlane(defaultPlane,i->team,true);
		//}
	}
	particleManager.init();
}
void WorldManager::destroy()
{
	clearObjects();
	bullets.clear();
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
	double ms = time.length();
	//Vec3f sp;
	//Vec3f v;
	//Vec3f p;			MUST CHECK FOR BULLET/MISSILE HITS
	//int l;

	std::shared_ptr<CollisionChecker::triangleList> trl1, trl2;

	updateObjects(time(),ms);

	for(auto i = mObjectTypes[PLANE].begin(); i != mObjectTypes[PLANE].end();i++)
	{
		if(!i->second->dead)
		{
			for(int l=0;l<(signed int)bullets.size();l++)
			{
				if(bullets[l].owner != i->second->id && bullets[l].startTime < time.lastTime() && bullets[l].startTime + bullets[l].life > time())
				{
					if(collisionCheck(i->second->type,bullets[l].startPos+bullets[l].velocity*(time()-bullets[l].startTime)/1000-i->second->position, bullets[l].startPos+bullets[l].velocity*(time.lastTime()-bullets[l].startTime)/1000-i->second->position))
					{
						((nPlane*)(*i).second.get())->loseHealth(25);
						if((*i).second->dead)
						{
							if(bullets[l].owner==players[0].objectNum() && players[0].active()) players[0].addKill();
							if(bullets[l].owner==players[1].objectNum() && players[1].active()) players[1].addKill();
						}
						bullets.erase(bullets.begin()+l);
						l--;
					}
				}
			}
			for(auto l=mObjectTypes[MISSILE].begin();l!=mObjectTypes[MISSILE].end();l++)
			{
				trl1 = dataManager.getModel(objectTypeString(i->second->type))->trl;
				trl2 = dataManager.getModel(objectTypeString(l->second->type))->trl;
				objId owner = ((missile*)l->second.get())->owner;
				if(owner != i->second->id &&  owner != (*i).first && 
					(i->second->position + i->second->rotation*(trl1!=NULL?trl1->getCenter():Vec3f(0,0,0))).distance(l->second->position + l->second->rotation*(trl2!=NULL?trl2->getCenter():Vec3f(0,0,0))) < (trl1!=NULL?trl1->getRadius():0)+(trl2!=NULL?trl2->getRadius():0) )
					//collisionCheck(i->second,l->second))
				{
					((nPlane*)(*i).second.get())->loseHealth(105);
					if((*i).second->dead) 
					{
						if(owner==players[0].objectNum() && players[0].active()) players[0].addKill();
						if(owner==players[1].objectNum() && players[1].active()) players[1].addKill();
					}
					l->second->awaitingDelete = true;
				}
			}
		}
	}
	double t = time();

	bullets.erase(remove_if(bullets.begin(), bullets.end(), [&t] (bullet& b) -> bool {return b.startTime + b.life < t;}), bullets.end());

	particleManager.update();
}