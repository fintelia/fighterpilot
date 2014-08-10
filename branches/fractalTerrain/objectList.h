
class objectList
{
protected:
	map<objId,shared_ptr<object>>			mObjects;
	map<objectType, map<objId,shared_ptr<object>>> mObjectTypes;

	void clearObjects();
	void objectsSimulationUpdate(double time, double ms);
	void objectsFrameUpdate(double interpolation);
public:
	objId newObject(object* newObj);

	shared_ptr<object> getObjectById(objId id) const;
	const map<objId,shared_ptr<object>>& getAllOfType(objectType t);
};
