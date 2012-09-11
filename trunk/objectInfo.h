
//////////////////////////////////////////////////////////////////
const int PLANE						= 0x0100;
const int PLAYER_PLANE				= 0x01ff;
const int ANTI_AIRCRAFT_ARTILLARY	= 0x0200;
const int AA_GUN					= 0x0201;
const int SAM_BATTERY				= 0x0202;
const int FLAK_CANNON				= 0x0203;
const int SHIP						= 0x0400;
const int MISSILE					= 0x0800;
const int BOMB						= 0x1000;
const int BULLET_CLOUD				= 0x2000;
//////////////////////////////////////////////////////////////////
typedef int planeType;
typedef int missileType;
typedef int bombType;

//////////////////////////////////////////////////////////////////
class ObjectInfo
{
public:
	struct objectData
	{
	private:
		shared_ptr<FileManager::modelFile>			meshFile;
		shared_ptr<FileManager::modelFile>			collisionMeshFile;
	public:
		string name;
		string textName;
		objectType type;

		//shared_ptr<PhysicsManager::collisionBounds> collisionMesh;
		shared_ptr<SceneManager::mesh>				mesh;

		friend class ObjectInfo;
	};
	struct planeObjectData: public objectData
	{
		struct hardpoint
		{
			objectType mType;
			Vec3f offset;
		};
		struct engine
		{
			Vec3f offset;
			float radius;
		};
		vector<hardpoint> hardpoints;//offsets
		vector<Vec3f> machineGuns;
		vector<engine> engines;
		float cameraDistance;
	};
private:
	planeType defaultPlane;

	vector<objectType> placeableObjects;
	map<objectType,shared_ptr<objectData>> objectMap;
	ObjectInfo(){}
public:
	static ObjectInfo& getInstance()
	{
		static ObjectInfo* pInstance = new ObjectInfo();
		return *pInstance;
	}
	bool loadObjectData(string filename="media/objectData.xml");
	int loadObjectMeshes();
	objectType typeFromString(string s);
	string typeString(objectType t);
	string textName(objectType t);
	shared_ptr<objectData> operator[] (objectType t);
	const planeObjectData& planeStats(objectType t);
	planeType getDefaultPlane(){return defaultPlane;}
	const vector<objectType>& getPlaceableObjects(){return placeableObjects;}
};

extern ObjectInfo& objectInfo;