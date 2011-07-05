
#include "main.h"

objId objectList::newObject(LevelFile::Object obj)
{
	if(obj.type & PLANE)
	{
		nPlane* p = NULL;
		//if(obj.controlType == CONTROL_HUMAN)
		{
			for(int i=0;i<NumPlayers;i++)
			{
				if(players[i].objectNum()==0)
				{
					p = new nPlane(obj.startloc,obj.startRot,obj.type,&players[i]);
					break;
				}
				else if(i == NumPlayers-1)
					p = new nPlane(obj.startloc,obj.startRot,obj.type);
			}
		}
		//else
		//{
		//	p = new nPlane(obj.startloc,obj.startRot,obj.type);
		//}
		p->team = obj.team;
		mObjects[p->id] = p;
		mPlanes[p->id] = p;
		p->init();
		return p->id;
	}
	else if(obj.type & AA_GUN)// can't be player controlled
	{
		aaGun* p = new aaGun(obj.startloc,obj.startRot,obj.type);
		p->team = obj.team;
		mObjects[p->id] = p;
		mAAguns[p->id] = p;
		p->init();
		return p->id;
	}
	return 0;
}
objId objectList::newMissile(missileType type, teamNum team,Vec3f pos, Quat4f rot, float speed, int owner,int target)
{
	if(!(type & MISSILE)) return NULL;

	missile* m = new missile(type,team,pos,rot,speed,owner,target);
	mObjects[m->id] = m;
	mMissiles[m->id] = m;

	m->init();
	return m->id;
}
void objectList::deleteObject(objId id)
{
	if(mObjects.find(id)==mObjects.end()) return;

	object* objPtr = mObjects[id];
	mObjects.erase(id);
	if(objPtr->type & PLANE)	mPlanes.erase(id);
	if(objPtr->type & MISSILE)	mMissiles.erase(id);
	delete objPtr;
}
void objectList::clear()
{
	for(auto i = mObjects.begin(); i!= mObjects.end(); i++)
		delete i->second;

	mObjects.clear();
	mPlanes.clear();
	mMissiles.clear();
	object::currentId = 0;
}
void objectList::update(double time, double ms)
{
	vector<int> toDelete;
	int m=0;
	for(auto i = mObjects.begin(); i != mObjects.end();i++)
	{
		i->second->update(time,ms);
		if(i->second->awaitingDelete)
			toDelete.push_back((*i).first);
		m++;
	}
	for(vector<int>::iterator i=toDelete.begin();i!=toDelete.end();i++)
	{
		deleteObject(*i);
	}
}
object* objectList::operator[] (objId id) const
{
	map<objId,object*>::const_iterator o = mObjects.find(id);
	if(o!=mObjects.end())
		return o->second;
	return NULL;
}