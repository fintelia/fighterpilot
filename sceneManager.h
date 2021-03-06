
class SceneManager
{
public:
	class mesh
	{
	public:
		struct material{
			shared_ptr<GraphicsManager::texture> tex;
			shared_ptr<GraphicsManager::texture> specularMap;
			shared_ptr<GraphicsManager::texture> normalMap;
			Color4 diffuse;
			Color3 specular;
			float hardness;
			shared_ptr<GraphicsManager::indexBuffer> indexBuffer;
		};
		vector<material> materials;
		shared_ptr<GraphicsManager::vertexBuffer> VBO;
		Sphere<float> boundingSphere;
		unsigned int meshID;
	};
	class meshInstance
	{
		weak_ptr<mesh> meshPtr;
		Mat4f modelTransform;
		unsigned char flags;
		weak_ptr<meshInstance> parent; //just for association right now, does not impact transformation (yet?)

		friend class SceneManager;
		meshInstance(weak_ptr<mesh> mPtr, const Mat4f& transformation, weak_ptr<meshInstance> Parent=weak_ptr<meshInstance>()): meshPtr(mPtr), modelTransform(transformation), flags(0x01), parent(Parent) {}

	public:
		void update(const Mat4f& transformation,bool render=true);
		Sphere<float> getBoundingSphere();
		bool renderFlag();
		void setRenderFlag(bool b);
	};
	struct pointLight
	{
		Vec3f position;
		float strength;
		Color3 color;
	};
	friend class mesh;
	friend class meshInstance;
private:
	SceneManager(){}
	//map<string,vector<shared_ptr<meshInstance>>> meshInstances;
	vector<shared_ptr<mesh>> meshes;
	vector<weak_ptr<pointLight>> lights;
	map<unsigned int, vector<weak_ptr<meshInstance>>> nMeshInstances;

public:
	static SceneManager& getInstance()
	{
		static SceneManager* pInstance = new SceneManager();
		return *pInstance;
	}
	shared_ptr<mesh> createMesh(shared_ptr<FileManager::modelFile> modelFile);
	shared_ptr<meshInstance> newMeshInstance(shared_ptr<mesh> meshPtr, Mat4f transformation=Mat4f());
	shared_ptr<meshInstance> newChildMeshInstance(shared_ptr<mesh> meshPtr, weak_ptr<meshInstance> parent, Mat4f transformation=Mat4f());
	void drawMesh(shared_ptr<GraphicsManager::View> view, shared_ptr<mesh> meshPtr, Mat4f transformation);
	void drawMesh(shared_ptr<GraphicsManager::View> view, shared_ptr<mesh> meshPtr, Mat4f transformation, shared_ptr<GraphicsManager::shader> shader);

	void reset();
	void renderScene(shared_ptr<GraphicsManager::View> view, shared_ptr<meshInstance> firstPersonObject = nullptr);
	void renderSceneTransparency(shared_ptr<GraphicsManager::View> view, shared_ptr<meshInstance> firstPersonObject = nullptr);
	void endRender();

	void bindLights(shared_ptr<GraphicsManager::shader> shader);

	shared_ptr<pointLight> genPointLight();
};

typedef shared_ptr<SceneManager::meshInstance> meshInstancePtr;

extern SceneManager& sceneManager;
