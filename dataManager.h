
class DataManager
{
private:
	struct asset{
		enum assetType{/*SHADER,*/ TEXTURE, MODEL, COLLISION_BOUNDS, FONT}type;
		virtual ~asset(){}
	};
	struct textureAsset: public asset{
		shared_ptr<GraphicsManager::texture> texture;
		int width;
		int height;
		char bpp;				//bit per pixel
		unsigned char* data;	//currently just set to nullptr
	};
	struct modelAsset: public asset
	{
		shared_ptr<SceneManager::mesh> mesh;
	};
	struct collisionBoundsAsset: public asset
	{
		shared_ptr<CollisionManager::collisionBounds> collisionBounds;
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

	map<string,shared_ptr<asset>>	assets;
	string							boundShader;

public:
	void bind(string name, int textureUnit=0);

	shared_ptr<const fontAsset>						getFont(string name);
	shared_ptr<SceneManager::mesh>					getModel(string name);
	shared_ptr<CollisionManager::collisionBounds>	getCollisionBounds(string name);
	shared_ptr<GraphicsManager::texture>			getTexture(string name);

	bool assetLoaded(string name);

	void addTexture(string name, shared_ptr<GraphicsManager::texture> tex);
	void addModel(string name, shared_ptr<FileManager::modelFile> model);
	void addCollisionBounds(string name, shared_ptr<FileManager::modelFile> collisionMesh);
	void addCollisionBounds(string name, Sphere<float> boundingSphere);
	void addFont(string name, shared_ptr<FileManager::textFile> f);

private:
	textureAsset* registerTexture(shared_ptr<FileManager::textureFile> f, bool tileable=false, bool compress=false);
	fontAsset* registerFont(shared_ptr<FileManager::textFile> f);
};
class ShaderManager
{
private:
	struct shaderAsset
	{
		shared_ptr<GraphicsManager::shader> shader;
		bool use_sAspect;
	};
	map<string,shared_ptr<shaderAsset>> shaderAssets;

public:
	GraphicsManager::shader* bind(string name);
	shared_ptr<GraphicsManager::shader> operator() (string name);
	bool shaderExists(string name);

	void add(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect);
	void writeErrorLog(string filename);
};


extern DataManager dataManager;
extern ShaderManager shaders;
