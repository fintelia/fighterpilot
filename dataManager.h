
class DataManager
{
private:
	struct asset{
		enum assetType{SHADER, TEXTURE, MODEL, FONT}type;
	};
	struct textureAsset: public asset{
		shared_ptr<GraphicsManager::texture2D> texture;
		//unsigned int id;
		int width;
		int height;
		char bpp;				//bit per pixel
		unsigned char* data;	//currently just set to nullptr
	};
	struct shaderAsset: public asset{
		//unsigned int id;
		shared_ptr<GraphicsManager::shader> shader;
		bool use_sAspect;
	//	map<string,int> uniforms;
	};
	struct modelAsset: public asset
	{
		//int numVertices;
		//std::shared_ptr<texturedLitVertex3D> vertices;
//		unsigned int VBO_id;
		shared_ptr<GraphicsManager::vertexBuffer> VBO;
//		int numMaterials;
		struct material{
			string tex;
			Color color;
			//string shader;
			int numIndices;		//for glDrawArrays
			int indicesOffset;	//for glDrawArrays
		};
		vector<material> materials;

		Sphere<float> boundingSphere;
		std::shared_ptr<CollisionChecker::triangleList> trl;
	};
	struct fontAsset: public asset{
		shared_ptr<GraphicsManager::texture2D> texture;
		//unsigned int id;       //stores the texture id for the font
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
		vector<shared_ptr<FileManager::file>> files; //for textures
	};
	queue<assetFile> assetFiles;
	queue<assetFile> assetFilesPreload;

	map<string,shared_ptr<asset>>	assets;
	map<int, shared_ptr<asset>>	unnamedAssets;
	int					currentAssetIndex;

	map<int,string>		boundTextures;

	string				boundShader;
	//int					boundShaderId;


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

	shared_ptr<const fontAsset> getFont(string name);
	shared_ptr<const modelAsset> getModel(string name);
	shared_ptr<const modelAsset> getModel(objectType t){return getModel(objectTypeString(t));}

	bool assetLoaded(string name);

	void setUniform1f(string name, float v0);
	void setUniform2f(string name, float v0, float v1);
	void setUniform3f(string name, float v0, float v1, float v2);
	void setUniform4f(string name, float v0, float v1, float v2, float v3);
	void setUniform2f(string name, Vec2f v){setUniform2f(name,v.x,v.y);}
	void setUniform3f(string name, Vec3f v){setUniform3f(name,v.x,v.y,v.z);}
	void setUniform3f(string name, Color c){setUniform3f(name,c.r,c.r,c.b);}
	void setUniform4f(string name, Color c, float a = 1.0){setUniform4f(name,c.r,c.r,c.b,a);}
	void setUniform1i(string name, int v0);
	void setUniform2i(string name, int v0, int v1);
	void setUniform3i(string name, int v0, int v1, int v2);
	void setUniform4i(string name, int v0, int v1, int v2, int v3);

	void setUniformMatrix(string name, const Mat3f& m);
	void setUniformMatrix(string name, const Mat4f& m);


//	bool loadAssetList();
//	int loadAsset(); //also does preload if not already done


	string getBoundShader() const{return boundShader;}
//	bool textureBound(int textureUnit = 0) {return boundTextureIds[textureUnit] != 0;}
	void shutdown();

	void addTexture(string name, shared_ptr<GraphicsManager::texture2D> tex);
	void addShader(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect);
	void addModel(string name, string OBJfile);
	void addFont(string name, shared_ptr<FileManager::textFile> f);

	void writeErrorLog(string filename);
private:

	//bool registerTGA(string name, string filename, bool tileable=false);
	//bool registerPNG(string name, string filename, bool tileable=false);
	asset* registerOBJ(string filename);
	asset* registerShader(shared_ptr<FileManager::textFile> vert, shared_ptr<FileManager::textFile> frag, bool use_sAspect=false);
	//bool registerTerrainShader(string name, string frag);
	asset* registerTexture(shared_ptr<FileManager::textureFile> f, bool tileable=false);
	asset* registerFont(shared_ptr<FileManager::textFile> f);

	//int getId(string name);
	//int getId(objectType t);

	DataManager():currentAssetIndex(0){}
	~DataManager();
};

extern DataManager& dataManager;
