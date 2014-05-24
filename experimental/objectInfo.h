
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
		string meshFilename;
		string collisionMeshFilename;

	public:
		string name;
		string textName;
		objectType type;
		string previewTexture;

		weak_ptr<SceneManager::mesh>				mesh;
		weak_ptr<CollisionManager::collisionBounds>	collisionMesh;

		shared_ptr<SceneManager::meshInstance> newMeshInstance(Mat4f transformation) const;
		shared_ptr<CollisionManager::collisionInstance> newCollisionInstance(Mat4f transformation) const;

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
	struct aaaObjectData: public objectData
	{
		struct turret
		{
			string meshFilename;
			Vec3f rotationCenter;
			weak_ptr<SceneManager::mesh> mesh;
			shared_ptr<SceneManager::meshInstance> newMeshInstance(weak_ptr<SceneManager::meshInstance> parentMeshInstance) const;
		};
		struct cannon
		{
			string meshFilename;
			Vec3f rotationCenter;
			weak_ptr<SceneManager::mesh> mesh;
			shared_ptr<SceneManager::meshInstance> newMeshInstance(weak_ptr<SceneManager::meshInstance> parentMeshInstance) const;
		};
		vector<turret> turrets;
		vector<cannon> cannons;
	};
private:
	planeType defaultPlane;

	vector<objectType> placeableObjects;
	vector<planeType> playablePlanes;
	map<objectType,shared_ptr<objectData>> objectMap;
	ObjectInfo(){}
public:
	static ObjectInfo& getInstance()
	{
		static ObjectInfo* pInstance = new ObjectInfo();
		return *pInstance;
	}
	bool loadObjectData(string filename="media/objectData.xml");
	void linkObjectMeshes();
	objectType typeFromString(string s);
	string typeString(objectType t);
	string textName(objectType t);
	shared_ptr<objectData> operator[] (objectType t);
	shared_ptr<const ObjectInfo::planeObjectData> planeData(objectType t);
	shared_ptr<const ObjectInfo::aaaObjectData> aaaData(objectType t);
	planeType getDefaultPlane(){return defaultPlane;}
	const vector<objectType>& getPlaceableObjects(){return placeableObjects;}
	const vector<planeType>& getPlayablePlanes(){return playablePlanes;}
};

extern ObjectInfo& objectInfo;