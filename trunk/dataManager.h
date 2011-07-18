
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

	void draw(objectType p);
	void draw(string name);
	void drawCustomShader(string name);

	int getId(string name);
	int getId(objectType t);

	//vector forms could be added if needed
	void setUniform1f(string name, float v0);
	void setUniform2f(string name, float v0, float v1);
	void setUniform3f(string name, float v0, float v1, float v2);
	void setUniform4f(string name, float v0, float v1, float v2, float v3);
	void setUniform1i(string name, int v0);
	void setUniform2i(string name, int v0, int v1);
	void setUniform3i(string name, int v0, int v1, int v2);
	void setUniform4i(string name, int v0, int v1, int v2, int v3);

	int registerAssets();
	void registerAsset(string name, string filename);
	void registerShader(string name, string vert, string frag);

	int getCurrentShaderId(){return boundShaderId;}
	bool texturesBound(){return !boundTextureIds.empty();}

	void shutdown();

	CollisionChecker::triangleList* getModel(objectType type);
private:
	int loadTGA(string filename);
	int loadMMP(string filename);
	int loadShader(string filename);
	int loadOBJ(string filename);
	int loadPNG(string filename);

	void registerTexture(string name, int id);
	void registerShader(string name, int id);
	
	void registerModel(string name, int id);

	char *textFileRead(char *fn);//for shaders

	map<int, CollisionChecker::triangleList*> models;// <id,model>
	struct asset
	{
		enum assetType{SHADER, TEXTURE, MODEL}type;
		int id;
	};
	struct shaderAsset: public asset
	{
		map<string,int> uniforms;
	};

	map<string,asset*>	assets;

	int					activeTextureUnit;

	map<int,string>		boundTextures;
	map<int,int>		boundTextureIds;

	string				boundShader;
	int					boundShaderId;

	DataManager():activeTextureUnit(0),boundShaderId(0){}
	~DataManager();

	friend class CollisionChecker;
};

extern DataManager& dataManager;