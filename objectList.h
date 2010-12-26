
typedef unsigned int objId;

class objectList
{
private:
	objId lastId;

	map<objId,entity*>		mObjects;
	map<objId,planeBase*>	mPlanes;
	map<objId,missile*>		mMissiles;

public:
	objectList():lastId(0){}
	
	objId newPlane(planeType type, teamNum team, bool computer)
	{
		if(!(type & PLANE)) return NULL;
		planeBase* objPtr;
		if(computer)
			objPtr = new AIplane(++lastId,type,team);
		else
			objPtr = new plane(++lastId,type,team);
		mObjects.insert(pair<objId,entity*>(lastId,objPtr));
		mPlanes.insert(pair<objId,planeBase*>(lastId,objPtr));
		return lastId;
	}
	objId newMissile(missileType type, teamNum team,Vec3f pos, Vec3f vel, int disp)
	{
		if(!(type & MISSILE)) return NULL;

		missile* objPtr = new missile(++lastId,type,team,pos,vel,disp);
		mObjects.insert(pair<objId,entity*>(lastId,objPtr));
		mMissiles.insert(pair<objId,missile*>(lastId,objPtr));
		return lastId;
	}
	void deleteObject(objId id)
	{
		if(mObjects.find(id)==mObjects.end()) return;

		entity* objPtr = mObjects[id];
		mObjects.erase(id);
		if(objPtr->type & PLANE)	mPlanes.erase(id);
		if(objPtr->type & MISSILE)	mMissiles.erase(id);
		delete objPtr;
	}
	void clear()
	{
		for(map<objId,entity*>::iterator i = mObjects.begin(); i!= mObjects.end(); i++)
			delete i->second;

		mObjects.clear();
		mPlanes.clear();
		mMissiles.clear();
	}
	void update(float ms)
	{
		vector<int> toDelete;
		for(map<objId,entity*>::iterator i = mObjects.begin(); i != mObjects.end();i++)
		{
			if(!(*i).second->Update(ms))
				toDelete.push_back((*i).first);
		}
		for(vector<int>::iterator i=toDelete.begin();i!=toDelete.end();i++)
		{
			deleteObject(*i);
		}
	}


	entity* operator[] (objId id) const
	{
		map<objId,entity*>::const_iterator obj = mObjects.find(id);
		return obj!=mObjects.end() ? obj->second : NULL;
	}
	const map<objId,entity*>& objects()const
	{
		return mObjects;
	}
	const map<objId,planeBase*>& planes()const
	{
		return mPlanes;
	}
	const map<objId,missile*>& missiles()const
	{
		return mMissiles;
	}
};