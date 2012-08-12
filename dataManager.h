
class DataManager
{
private:
	struct asset{
		enum assetType{/*SHADER,*/ TEXTURE, MODEL, FONT}type;
		virtual ~asset(){}
	};
	struct textureAsset: public asset{
		shared_ptr<GraphicsManager::texture> texture;
		//unsigned int id;
		int width;
		int height;
		char bpp;				//bit per pixel
		unsigned char* data;	//currently just set to nullptr
	};
//	struct shaderAsset: public asset{
//		//unsigned int id;
//		shared_ptr<GraphicsManager::shader> shader;
//		bool use_sAspect;
//	//	map<string,int> uniforms;
//	};
	struct modelAsset: public asset
	{
		shared_ptr<SceneManager::mesh> mesh;
		//shared_ptr<GraphicsManager::vertexBuffer> VBO;
		//struct material{
		//	string tex;
		//	string specularMap;
		//	string normalMap;
		//	Color4 diffuse;
		//	Color3 specular;
		//	float hardness;
		//	shared_ptr<GraphicsManager::indexBuffer> indexBuffer;
		//};
		//vector<material> materials;
		//Sphere<float> boundingSphere;
	};
	struct fontAsset: public asset{
		shared_ptr<GraphicsManager::texture2D> texture;
		//unsigned int id;       //stores the texture id for the font
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

	map<string,shared_ptr<asset>>	assets;

	string				boundShader;
	//int					boundShaderId;


public:
	static DataManager& getInstance()
	{
		static DataManager* pInstance = new DataManager();
		return *pInstance;
	}
	void bind(string name, int textureUnit=0);

	shared_ptr<const fontAsset> getFont(string name);
	//shared_ptr<const modelAsset> getModel(string name);
	//shared_ptr<const modelAsset> getModel(objectType t){return getModel(objectTypeString(t));}

	shared_ptr<SceneManager::mesh> getModel(string name);
	shared_ptr<GraphicsManager::texture> getTexture(string name);

	bool assetLoaded(string name);

//	void setUniform1f(string name, float v0);
//	void setUniform2f(string name, float v0, float v1);
//	void setUniform3f(string name, float v0, float v1, float v2);
//	void setUniform4f(string name, float v0, float v1, float v2, float v3);
//	void setUniform2f(string name, Vec2f v){setUniform2f(name,v.x,v.y);}
//	void setUniform3f(string name, Vec3f v){setUniform3f(name,v.x,v.y,v.z);}
//	void setUniform3f(string name, Color3 c){setUniform3f(name,c.r,c.r,c.b);}
//	void setUniform4f(string name, Color4 c){setUniform4f(name,c.r,c.r,c.b,c.a);}
//	void setUniform1i(string name, int v0);
//	void setUniform2i(string name, int v0, int v1);
//	void setUniform3i(string name, int v0, int v1, int v2);
//	void setUniform4i(string name, int v0, int v1, int v2, int v3);

//	void setUniformMatrix(string name, const Mat3f& m);
//	void setUniformMatrix(string name, const Mat4f& m);


//	bool loadAssetList();
//	int loadAsset(); //also does preload if not already done


//	string getBoundShader() const{return boundShader;}
//	bool textureBound(int textureUnit = 0) {return boundTextureIds[textureUnit] != 0;}
	void shutdown();

	void addTexture(string name, shared_ptr<GraphicsManager::texture> tex);
//	void addShader(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect);
	void addModel(string name, string OBJfile);
	void addFont(string name, shared_ptr<FileManager::textFile> f);

//	void writeErrorLog(string filename);
private:

	//bool registerTGA(string name, string filename, bool tileable=false);
	//bool registerPNG(string name, string filename, bool tileable=false);
	//modelAsset* registerOBJ(string filename);
//	shaderAsset* registerShader(shared_ptr<FileManager::textFile> vert, shared_ptr<FileManager::textFile> frag, bool use_sAspect=false);
	//bool registerTerrainShader(string name, string frag);
	textureAsset* registerTexture(shared_ptr<FileManager::textureFile> f, bool tileable=false);
	fontAsset* registerFont(shared_ptr<FileManager::textFile> f);

	//int getId(string name);
	//int getId(objectType t);

	DataManager(){}
	~DataManager();
};
class ShaderManager
{
private:
	struct shaderAsset
	{
		shared_ptr<GraphicsManager::shader> shader;
		bool use_sAspect;
	};
	struct shaderFile
	{
		string name;
		string filename[2];		//extra for shaders
		set<string> options;
		vector<shared_ptr<FileManager::file>> files; //for textures
	};
	map<string,shared_ptr<shaderAsset>>	shaderAssets;

public:
	static ShaderManager& getInstance()
	{
		static ShaderManager* pInstance = new ShaderManager();
		return *pInstance;
	}
	shared_ptr<GraphicsManager::shader> bind(string name);
	shared_ptr<GraphicsManager::shader> operator() (string name);

	void shutdown();

	void add(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect);
	void writeErrorLog(string filename);
private:
	ShaderManager(){}
};


extern DataManager& dataManager;
extern ShaderManager& shaders;