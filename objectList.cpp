
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
		return newObject(p);
	}
	else if(obj.type & AA_GUN)// can't be player controlled
	{
		return newObject(new aaGun(obj.startloc,obj.startRot,obj.type));
	}
	return 0;
}
objId objectList::newObject(object* newObj)
{
	if(newObj != nullptr)
	{
		std::shared_ptr<object> ptr(newObj);

		mObjects[ptr->id] = ptr;
		mObjectTypes[ptr->type][ptr->id] = ptr;
		mObjectTypes[ptr->type & MAJOR_OBJECT_TYPE][ptr->id] = ptr;

		ptr->init();
		return ptr->id;
	}
	return 0;
}
void objectList::deleteObject(objId id)
{
	if(mObjects.count(id)==0) return;

	objectType t = mObjects[id]->type;

	mObjects.erase(id);
	mObjectTypes[t].erase(id);
	mObjectTypes[t & MAJOR_OBJECT_TYPE].erase(id);
}
void objectList::clearObjects()
{
	mObjects.clear();
	mObjectTypes.clear();
	object::currentId = 0;
}
void objectList::updateObjects(double time, double ms)
{
	for(auto i = mObjects.begin(); i != mObjects.end();i++)
	{
		i->second->update(time,ms);
	}
	for(auto i = mObjects.begin(); i != mObjects.end();)
	{
		if(i->second->awaitingDelete)
			mObjects.erase(i++);
		else
			i++;
	}
}
std::shared_ptr<object> objectList::operator[] (objId id) const
{
	auto o = mObjects.find(id);
	if(o!=mObjects.end())
		return o->second;
	return std::shared_ptr<object>();
}
const map<objId,std::shared_ptr<object>>& objectList::operator() (objectType t)
{
	return mObjectTypes[t];
}