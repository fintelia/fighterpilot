
class AssetLoader
{
private:
	struct assetFile{
		string name;
		string filename[2];		//extra for shaders
		set<string> options;
		enum assetType{SHADER, TEXTURE, MODEL, FONT}type;
		vector<shared_ptr<FileManager::file>> files; //for textures
	};
	queue<assetFile> assetFiles;
	queue<assetFile> assetFilesPreload;

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