
class DataManager
{
private:
	struct asset{
		enum assetType{SHADER, TEXTURE, MODEL, FONT}type;
		int id;
	};
	struct shaderAsset: public asset{
		bool use_sAspect;
		map<string,int> uniforms;
	};
	struct fontAsset: public asset{
		//int id       --stores the texture id for the font 
		string fontName;
		float height;//in px
		struct character
		{
			float x, y, width, height;
			float xOffset, yOffset;
			float xAdvance;
		};
		map<unsigned char,character> characters;
	};

	map<string,asset*>	assets;

	int					activeTextureUnit;

	map<int,string>		boundTextures;
	map<int,int>		boundTextureIds;

	string				boundShader;
	int					boundShaderId;


public:
	static DataManager& getInstance()
	{
		static DataManager* pInstance = new DataManager();
		return *pInstance;
	}
	void bind(string name, int textureUnit=0);
	void bindTex(int id, int textureUnit=0);
	void bindShader(int id);

	void unbind(string name);
	void unbindTextures();
	void unbindShader();

	void draw(objectType p);
	void draw(string name);
	void drawCustomShader(string name);

	int getId(string name);	//should be phased out
	int getId(objectType t);

	//vector forms could be added if needed
	void setUniform1f(string name, float v0);
	void setUniform2f(string name, float v0, float v1);
	void setUniform3f(string name, float v0, float v1, float v2);
	void setUniform4f(string name, float v0, float v1, float v2, float v3);
	void setUniform2f(string name, Vec2f v){setUniform2f(name,v.x,v.y);}
	void setUniform3f(string name, Vec3f v){setUniform3f(name,v.x,v.y,v.z);}
	void setUniform1i(string name, int v0);
	void setUniform2i(string name, int v0, int v1);
	void setUniform3i(string name, int v0, int v1, int v2);
	void setUniform4i(string name, int v0, int v1, int v2, int v3);

	bool loadAssetList();
	void preloadAssets();
	int loadAsset();

	void shutdown();

	CollisionChecker::triangleList* getModel(objectType type);
private:

//	int loadTexture(string filename);//simply calls loadTGA/loadPNG based on file extension

//	int loadShader(string vert, string frag);
//	int loadTerrainShader(string frag);
	int loadTGA(string filename);
	int loadPNG(string filename);

//	int loadOBJ(string filename);

	//void registerAsset(string name, string filename);

	bool registerShader(string name, string vert, string frag, bool use_sAspect=false);
	bool registerTerrainShader(string name, string frag);
	bool registerOBJ(string name, string filename);
	bool registerTexture(string name, string filename);
	bool registerFont(string name, string filename);



	char *textFileRead(char *fn);//for shaders

	map<int, CollisionChecker::triangleList*> models;// <id,model>




	DataManager():activeTextureUnit(0),boundShaderId(0){}
	~DataManager();

	struct textureFile{
		string name;
		string filename;
	};
	queue<textureFile> textureFiles;
	queue<textureFile> textureFilesPreload;

	struct shaderFile{
		string name;
		string vertexShaderFile;
		string fragmentShaderFile;
		bool use_sAspect;
	};
	queue<shaderFile> shaderFiles;
	queue<shaderFile> shaderFilesPreload;

	struct modelFile{
		string name;
		string filename;
	};
	queue<modelFile> modelFiles;

	friend class CollisionChecker;
};

extern DataManager& dataManager;