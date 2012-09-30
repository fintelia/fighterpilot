
class AssetLoader
{
private:
	struct assetFile{
		string name;
		//string filename[2];		//extra for shaders
		//set<string> options;
		enum assetType{SHADER, TEXTURE, TEXTURE_3D, TEXTURE_CUBE, MODEL, FONT};
		virtual assetType getType()=0;
		//vector<shared_ptr<FileManager::file>> files; //for textures
	};
	struct shaderAssetFile: public assetFile{
		assetType getType(){return SHADER;}

		shared_ptr<FileManager::textFile> vertFile;
		shared_ptr<FileManager::textFile> fragFile;
		bool use_sAspect;
	};
	struct textureAssetFile: public assetFile{
		assetType getType(){return TEXTURE;}

		shared_ptr<FileManager::textureFile> file;
		bool tileable;
	};
	struct texture3AssetFile: public assetFile{
		assetType getType(){return TEXTURE_3D;}

		shared_ptr<FileManager::textureFile> file;
		bool tileable;
		int depth;
	};
	struct textureCubeAssetFile: public assetFile{
		assetType getType(){return TEXTURE_CUBE;}

		shared_ptr<FileManager::textureFile> file;
	};
	struct modelAssetFile: public assetFile{
		assetType getType(){return MODEL;}

		string filename;
	};
	struct fontAssetFile: public assetFile{
		assetType getType(){return FONT;}

		shared_ptr<FileManager::textFile> file;
	};

	queue<shared_ptr<assetFile>> assetFiles;
	queue<shared_ptr<assetFile>> assetFilesPreload;
public:
	static AssetLoader& getInstance()
	{
		static AssetLoader* pInstance = new AssetLoader();
		return *pInstance;
	}

	bool loadAssetList();
	int loadAsset(); //also does preload if not already done
};
extern AssetLoader& assetLoader;