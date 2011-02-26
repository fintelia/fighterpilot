
class objectList
{
private:
	map<objId,object*>		mObjects;
	map<objId,nPlane*>		mPlanes;
	map<objId,missile*>		mMissiles;

public:
	
	objId newObject(LevelFile::Object obj);
	objId newMissile(missileType type, teamNum team,Vec3f pos, Vec3f vel, int disp, int owner);
	void deleteObject(objId id);
	void clear();
	void update(double time, double ms);

	object* operator[] (objId id) const;
	const map<objId,object*>& objects()const
	{
		return mObjects;
	}
	const map<objId,nPlane*>& planes()const
	{
		return mPlanes;
	}
	const map<objId,missile*>& missiles()const
	{
		return mMissiles;
	}
};