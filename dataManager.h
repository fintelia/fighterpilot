
class DataManager
{
private:
	struct asset{
		enum assetType{SHADER, TEXTURE, MODEL, FONT}type;
		int id;
	};
	struct textureAsset: public asset{
		int width;
		int height;
		char bpp;				//bit per pixel
		unsigned char* data;	//currently just set to NULL
	};
	struct shaderAsset: public asset{
		bool use_sAspect;
		map<string,int> uniforms;
	};
	struct modelAsset: public asset
	{
		//int numVertices;
		//std::shared_ptr<texturedLitVertex3D> vertices;
		GLuint VBO_id;

		int numMaterials;
		struct material{
			string tex;
			Color color;
			//string shader;
			int numIndices;		//for glDrawArrays 
			int indicesOffset;	//for glDrawArrays 
		};
		vector<material> materials;

		std::shared_ptr<CollisionChecker::triangleList> trl;
	};
	struct fontAsset: public asset{
		//int id       --stores the texture id for the font 
		string texName;
		float height;//in px
		struct character
		{
			Rect UV;					// UV coords in texture
			float xOffset, yOffset;		// pixels that char is offset by
			float width, height;		// width and height in pixels
			float xAdvance;				// pixels that the insertion point is advanced
		};
		map<unsigned char,character> characters;
	};

	struct assetFile{
		string name;
		string filename[2];		//extra for shaders
		set<string> options;
		asset::assetType type; 
	};
	queue<assetFile> assetFiles;
	queue<assetFile> assetFilesPreload;

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

	const fontAsset* getFont(string name);
	const modelAsset* getModel(string name);
	const modelAsset* getModel(objectType t){return getModel(objectTypeString(t));}

	bool assetLoaded(string name);

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
private:

	void loadAssetFile(assetFile &file);

	bool registerTGA(string name, string filename);
	bool registerPNG(string name, string filename);
	bool registerOBJ(string name, string filename);
	bool registerShader(string name, string vert, string frag, bool use_sAspect=false);
	bool registerTerrainShader(string name, string frag);
	bool registerTexture(string name, string filename);
	bool registerFont(string name, string filename);

	int getId(string name);
	int getId(objectType t);

	char *textFileRead(char *fn);//for shaders

	DataManager():activeTextureUnit(0),boundShaderId(0){}
	~DataManager();
};

extern DataManager& dataManager;