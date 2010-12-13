
class DataManager
{
public:
	static DataManager& getInstance()
	{
		static DataManager* pInstance = new DataManager();
		return *pInstance;
	}
	int loadTexture(string filename);
	int loadShader(string vert, string frag);
	int loadTerrainShader(string frag);
	int loadModel(string filename);

	void bind(string name, int textureUnit=0);
	void bindTex(int id, int textureUnit=0);
	void bindShader(int id);

	void unbind(string name);
	void unbindTextures();
	void unbindShader();

	void draw(planeType p);
	void draw(string name);

	int getId(string name);

	bool registerAssets();
	void registerAsset(string name, string filename);


private:
	int loadTGA(string filename);
	int loadMMP(string filename);
	int loadShader(string filename);
	int loadOBJ(string filename);
	int loadPNG(string filename);

	void registerTexture(string name, int id);
	void registerShader(string name, int id);
	void registerShader(string name, string vert, string frag);
	void registerModel(string name, int id);

	char *textFileRead(char *fn);//for shaders

	map<int, CollisionChecker::triangleList> models;// <id,model>
	struct asset
	{
		enum assetType{SHADER, TEXTURE, MODEL}type;
		int id;
	};
	map<string,asset> assets;

	map<int,string> boundTextures;
	string			boundShader;


	DataManager(){}
	~DataManager();
};

extern DataManager& dataManager;