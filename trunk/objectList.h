
class objectList
{
protected:
	map<objId,std::shared_ptr<object>>		mObjects;

	map<objectType, map<objId,std::shared_ptr<object>>> mObjectTypes;

	void clearObjects();
	void objectsSimulationUpdate(double time, double ms);
	void objectsFrameUpdate(double interpolation);
public:
	objId newObject(object* newObj);
	void deleteObject(objId id);

	std::shared_ptr<object> operator[] (objId id) const;
	const map<objId,std::shared_ptr<object>>& operator() (objectType t);
};