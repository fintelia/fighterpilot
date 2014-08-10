
#include "engine.h"
objId objectList::newObject(object* newObj)
{
	if(newObj != nullptr)
	{
		shared_ptr<object> ptr(newObj);

		mObjects[ptr->id] = ptr;
		mObjectTypes[ptr->type][ptr->id] = ptr;
		mObjectTypes[ptr->type & MAJOR_OBJECT_TYPE][ptr->id] = ptr;

		ptr->init();

		return ptr->id;
	}
	return 0;
}
void objectList::clearObjects()
{
	mObjects.clear();
	mObjectTypes.clear();
	object::currentId = 0;
}
void objectList::objectsSimulationUpdate(double time, double ms)
{
	for(auto i = mObjects.begin(); i != mObjects.end();i++)
	{
		i->second->updateSimulation(time,ms);
	}
	for(auto i = mObjects.begin(); i != mObjects.end();)
	{
		if(i->second->awaitingDelete)
		{
			mObjects.erase(i++);
		}
		else
			i++;
	}
}
void objectList::objectsFrameUpdate(double interpolation)
{
	for(auto i = mObjects.begin(); i != mObjects.end();i++)
	{
		i->second->updateFrame(interpolation);
	}
}
shared_ptr<object> objectList::getObjectById(objId id) const
{
	auto o = mObjects.find(id);
	if(o!=mObjects.end())
		return o->second;
	return shared_ptr<object>();
}
const map<objId,shared_ptr<object>>& objectList::getAllOfType(objectType t)
{
	return mObjectTypes[t];
}
