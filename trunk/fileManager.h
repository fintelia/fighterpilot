
class FileManager
{
public:
	static FileManager& getInstance()
	{
		static FileManager* pInstance = new FileManager();
		return *pInstance;
	}
	struct fileContents
	{
		unsigned char* contents;
		unsigned long size;
		fileContents():contents(nullptr), size(0){}
	};
	class file
	{
	private:
		bool mComplete;
		bool mValid;

		void completeLoad(bool s){mValid = s;mComplete=true;}
		friend class FileManager;

	public:
		string filename;
		enum Type{BINARY,TEXT,INI,ZIP,TEXTURE} type;
		enum Format{NO_FORMAT, BMP, PNG, TGA} format;

		bool valid(){return mComplete && mValid;}
		bool invalid(){return mComplete && !mValid;}
		bool complete(){return mComplete;}

		virtual ~file(){}

//		file(Type t):mComplete(false),mValid(false), type(t){}
//		file(Type t,string fName):mComplete(false),mValid(false), filename(fName),type(t){}
		file(string fName,Type t, Format f=NO_FORMAT):mComplete(false),mValid(false), filename(fName),type(t),format(f){}
	};
	struct binaryFile: public file
	{
		unsigned int size;
		unsigned char* contents;

//		binaryFile():file("",BINARY),size(0),contents(nullptr){}
		binaryFile(string fName):file(fName,BINARY),size(0),contents(nullptr){}
		~binaryFile(){delete[] contents;}
	};
	struct textFile: public file
	{
		string contents;

//		textFile():file("",TEXT){}
		textFile(string fName):file(fName,TEXT){}
	};
	struct iniFile: public file
	{
		map<string,map<string, string> > bindings;

//		iniFile(): file("",INI){}
		iniFile(string fName): file(fName,INI){}
	};	
	struct zipFile: public file
	{
		map<string,shared_ptr<file>> files;

		zipFile(string fName): file(fName,ZIP){}
	};
	struct textureFile: public file
	{
		char channels;
		unsigned int width;
		unsigned int height;
		unsigned char* contents;

		textureFile(string fName, Format f):file(fName, TEXTURE, BMP),channels(0),width(0),height(0),contents(nullptr){}
		~textureFile(){delete[] contents;}
	};
	struct bmpFile: public textureFile
	{
		bmpFile(string fName):textureFile(fName, BMP){}
	};

	shared_ptr<binaryFile> loadBinaryFile(string filename, bool asinc = false);
	shared_ptr<textFile> loadTextFile(string filename, bool asinc = false);
	shared_ptr<iniFile> loadIniFile(string filename, bool asinc = false);
	shared_ptr<zipFile> loadZipFile(string filename, bool asinc = false);
	shared_ptr<textureFile> loadBmpFile(string filename, bool asinc = false);

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
	template<class T> T readAs(void* c)
	{
		return *((T*)c);
	}

	void workerThread();
	static void startWorkerThread(void* pThis)
	{
		((FileManager*)pThis)->workerThread();
	}

	fileContents loadFileContents(string filename);

	shared_ptr<file> parseFile(string filename, fileContents data);
	void parseBinaryFile(shared_ptr<binaryFile> f, fileContents data);
	void parseTextFile(shared_ptr<textFile> f, fileContents data);
	void parseIniFile(shared_ptr<iniFile> f, fileContents data);
	void parseZipFile(shared_ptr<zipFile> f, fileContents data);
	void parseBmpFile(shared_ptr<textureFile> f, fileContents data);

	FileManager();
	~FileManager(){}

	queue<std::shared_ptr<file>> fileQueue;
	mutex fileQueueMutex;
};

extern FileManager& fileManager;