
class AssetLoader
{
private:
	struct assetFile{
		string name;
		//string filename[2];		//extra for shaders
		//set<string> options;
		enum assetType{SHADER, TEXTURE, TEXTURE_3D, TEXTURE_CUBE, MODEL, COLLISION_MESH, FONT};
		virtual assetType getType()=0;
		//vector<shared_ptr<FileManager::file>> files; //for textures
	};
	struct shaderAssetFile: public assetFile{
		assetType getType(){return SHADER;}

		shared_ptr<FileManager::textFile> vertFile;
		shared_ptr<FileManager::textFile> fragFile;

		shared_ptr<FileManager::textFile> vert3File;
		shared_ptr<FileManager::textFile> geom3File;
		shared_ptr<FileManager::textFile> frag3File;
		bool use_sAspect;

		string feedbackTransformVaryingsStr;
		vector<const char*> feedbackTransformVaryings;
	};
	struct textureAssetFile: public assetFile{
		assetType getType(){return TEXTURE;}

		//shared_ptr<FileManager::textureFile> file;
		string filename;
		bool tileable;
		bool compress;
	};
	struct texture3AssetFile: public assetFile{
		assetType getType(){return TEXTURE_3D;}

		//shared_ptr<FileManager::textureFile> file;
		string filename;
		bool tileable;
		int depth;
	};
	struct textureCubeAssetFile: public assetFile{
		assetType getType(){return TEXTURE_CUBE;}

		string filename;
		//shared_ptr<FileManager::textureFile> file;
	};
	struct modelAssetFile: public assetFile{
		assetType getType(){return MODEL;}

		string filename;
	};
	struct collisionMeshAssetFile: public assetFile{
		assetType getType(){return COLLISION_MESH;}

		string filename;
		objectType objType;
	};
	struct fontAssetFile: public assetFile{
		assetType getType(){return FONT;}

		shared_ptr<FileManager::textFile> file;
	};

	deque<shared_ptr<assetFile>> assetFiles;
	deque<shared_ptr<assetFile>> assetFilesPreload;
	
	struct loadedModel{
		string name;
		shared_ptr<FileManager::modelFile> model;
	};
	vector<loadedModel> modelsToRegister;

	vector<shared_ptr<FileManager::modelFile>> modelsToAddToZip;
	vector<shared_ptr<FileManager::textureFile>> texturesToAddToZip;
	shared_ptr<FileManager::zipFile> assetsZipFile;

	shared_ptr<FileManager::modelFile> loadModel(string filename, bool loadTextures); //returns the file from assets.zip if it exists, or loads it from disk otherwise
	shared_ptr<FileManager::textureFile> loadTexture(string filename); //returns the file from assets.zip if it exists, or loads it from disk otherwise

	bool useTextureCompression;//whether compression will be used for textures loaded after this point

public:
	static AssetLoader& getInstance()
	{
		static AssetLoader* pInstance = new AssetLoader();
		return *pInstance;
	}

	void setTextureCompression(bool compression);

	bool loadAssetList();
	int loadAsset(); //also does preload if not already done

	void addModel(string filename,string name);
	void addModel(string filename){addModel(filename,filename);}

	void addCollisionMesh(string filename, objectType objType);

	void saveAssetZip();
};
extern AssetLoader& assetLoader;