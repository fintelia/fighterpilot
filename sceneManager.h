
class SceneManager
{
public:
	class meshInstance
	{
		string modelName;

		Quat4f rotation;
		Vec3f position;

		unsigned char flags;

		friend class SceneManager;
	public:
		meshInstance(string model, Vec3f pos, Quat4f rot): modelName(model), rotation(rot), position(pos), flags(0x01) {}

		void update(const Vec3f& pos,bool render=true);
		void update(const Vec3f& pos, const Quat4f& rot,bool render=true);

		bool renderFlag()	{return (flags & 0x01) != 0;}
		bool deleteFlag()	{return (flags & 0x02) != 0;}
		bool temperaryFlag()	{return (flags & 0x04) != 0;}

		void setRenderFlag(bool b);
		void setDeleteFlag(bool b);
		void setTemperaryFlag(bool b);
	};
private:
	SceneManager(){}
	map<string,vector<std::shared_ptr<meshInstance>>> meshInstances;
public:
	static SceneManager& getInstance()
	{
		static SceneManager* pInstance = new SceneManager();
		return *pInstance;
	}

	std::shared_ptr<meshInstance> newMeshInstance(string model, Vec3f position=Vec3f(), Quat4f rotation=Quat4f());
	void newTemperaryMesh(string model, Vec3f position=Vec3f(), Quat4f rotation=Quat4f());
	void resetMeshInstances();
	void renderScene(shared_ptr<meshInstance> firstPersonObject = nullptr);
};

typedef shared_ptr<SceneManager::meshInstance> meshInstancePtr;

extern SceneManager& sceneManager;
