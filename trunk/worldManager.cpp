
#include "main.h"

void WorldManager::create(Level* lvl)
{
	destroy();//just to be safe
	level = lvl;
	time.reset();

	((Level::heightmapGL*)level->ground())->setShader(dataManager.getId("grass new terrain"));

	int playerNum=0;
	for(auto i = level->objects().begin(); i != level->objects().end(); i++)
	{
		world.objectList.newObject(*i);
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
}
//void WorldManager::create()
//{
//	settings.ENEMY_PLANES=7;
//	settings.GAME_TYPE=FFA;
//	settings.HEIGHT_RANGE=1000;
//	settings.KILL_PERCENT_NEEDED=100;
//	settings.LEVEL_NAME="unnamed";
//	settings.MAP_FILE="media/heightmap5.bmp";
//	settings.MAP_TYPE=WATER;
//	settings.MAX_X=level->ground()->sizeX();
//	settings.MAX_Y=level->ground()->sizeZ();
//	settings.MIN_X=0;
//	settings.MIN_Y=0;
//	settings.ON_AI_HIT=RESPAWN;
//	settings.ON_HIT=RESPAWN;
//	settings.SEA_FLOOR_TYPE=ROCK;
//	settings.SEA_LEVEL=-150;
//
//	create(new Level("media/heightmap5.bmp",-150,1000));
//}
void WorldManager::destroy()
{
	if(level != NULL)
		delete level;
	objectList.clear();
	bullets.clear();
	smoke.clear();
	exaust.clear();
}

float WorldManager::elevation(Vec2f v) const
{
	if(level!=NULL)
	{
		return max(0,level->ground()->height(v.x,v.y));
	}
	return 0;
}
float WorldManager::elevation(float x, float z) const
{
	if(level!=NULL)
	{
		return max(0,level->ground()->height(x,z));
	}
	return 0;
}
float WorldManager::altitude(Vec3f v) const
{
	if(level!=NULL)
	{
		float h = level->ground()->height(v.x,v.z);
		return v.y-max(0,h);
	}
	return 0;
}
float WorldManager::altitude(float x, float y, float z) const
{
	if(level!=NULL)
	{
		float h = level->ground()->height(x,z);
		return y-max(0,h);
	}
	return 0;
}

Level::heightmapGL* const WorldManager::ground() const
{
	if(level!=NULL)
	{
		return (Level::heightmapGL*)level->ground();
	}
	return NULL;
}

const map<objId,object*>& WorldManager::objects()const
{
	return objectList.objects();
}
const map<objId,nPlane*>& WorldManager::planes()const
{
	return objectList.planes();
}
const map<objId,missile*>& WorldManager::missiles()const
{
	return objectList.missiles();
}

void WorldManager::update()
{
	double ms = time.getLength();
	//Vec3f sp;
	//Vec3f v;
	//Vec3f p;			MUST CHECK FOR BULLET/MISSILE HITS
	//int l;

	for(auto i = objectList.planes().begin(); i != objectList.planes().end();i++)
	{
		if(!i->second->dead)
		{
			for(int l=0;l<(signed int)bullets.size();l++)
			{
				if(bullets[l].startTime < time.getLastTime() && bullets[l].startTime + bullets[l].life > time())
				{
					if(collisionCheck(i->second->type,bullets[l].startPos+bullets[l].velocity*(time()-bullets[l].startTime)-i->second->position, bullets[l].startPos+bullets[l].velocity*(time.getLastTime()-bullets[l].startTime)-i->second->position))
					{
						(*i).second->loseHealth(250.0);
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
	//		for(l=0;l<(signed int)missiles.size();l++)
	//		{
	//			if(p.distance(missiles[l].pos)<32 && missiles[l].owner != (*i).first)
	//			{
	//				(*i).second->loseHealth(55.0);
	//				if((*i).second->dead) 
	//				{
	//					if(missiles[l].owner==players[0].planeNum() && players[0].active()) players[0].addKill();
	//					if(missiles[l].owner==players[1].planeNum() && players[1].active()) players[1].addKill();
	//				}
	//				missiles.erase(missiles.begin()+l);
	//				l--;
	//			}
		}
	}

	objectList.update(time(),ms);
	//for(int i=0;i<(signed int)bullets.size();i++)
	//{
	//	bullets.erase(bullets.begin()+i);
	//}
	smoke.update(ms);
	exaust.update(ms);
}