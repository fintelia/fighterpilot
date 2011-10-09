
class FileManager
{
private:

public:
	static FileManager& getInstance()
	{
		static FileManager* pInstance = new FileManager();
		return *pInstance;
	}

	struct file
	{
		bool loaded;
		bool loadFailed;
		string filename;
		enum Type{BINARY,TEXT,TEXTURE} type;

		std::shared_ptr<char> fileContents;
	};

	std::shared_ptr<file> loadTextFile(string filename, bool asinc = false);
	std::shared_ptr<file> loadBinaryFile(string filename, bool asinc = false);

	string filename(string filename);
	string extension(string filename);
	string directory(string filename);
	string changeExtension(string filename, string newExtension);

	vector<string> getAllFiles(string directory);
	vector<string> getAllFiles(string directory,string ext);
	vector<string> getAllFiles(string directory,set<string> ext);
	vector<string> getAllDirectories(string directory);
	vector<string> getAllFilesAndDirectories(string directory);

	bool fileExists(string file);
	bool directoryExists(string directory);
	bool createDirectory(string directory);
private:

	void lBinaryFile(std::shared_ptr<file> f);
	void lTextFile(std::shared_ptr<file> f);

	FileManager(){}
	~FileManager(){}
};

extern FileManager& fileManager;