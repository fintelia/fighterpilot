
class FileManager
{
public:	
	enum FileType{NO_FILE_TYPE, BINARY_FILE,TEXT_FILE,INI_FILE,ZIP_FILE,TEXTURE_FILE,MODEL_FILE};
	enum FileFormat{NO_FILE_FORMAT, BIN, TXT, INI, ZIP, BMP, PNG, TGA, OBJ};
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
	struct writeFile
	{
		string filename;
		fileContents contents;
	};

	class file
	{
	private:
		bool mComplete;
		bool mValid;
		bool writeFile;
		void completeLoad(bool s){mValid = s;mComplete=true;}
		friend class FileManager;

	public:
		string filename;
		FileType type;
		FileFormat format;

		bool valid(){return mComplete && mValid;}
		bool invalid(){return mComplete && !mValid;}
		bool complete(){return mComplete;}

		virtual ~file(){}

//		file(Type t):mComplete(false),mValid(false), type(t){}
//		file(Type t,string fName):mComplete(false),mValid(false), filename(fName),type(t){}
		file(string fName,FileType t, FileFormat f=NO_FILE_FORMAT):mComplete(false),mValid(false),writeFile(false), filename(fName),type(t),format(f){}
	};
	struct binaryFile: public file
	{
		unsigned int size;
		unsigned char* contents;

//		binaryFile():file("",BINARY),size(0),contents(nullptr){}
		binaryFile(string fName):file(fName,BINARY_FILE),size(0),contents(nullptr){}
		~binaryFile(){delete[] contents;}
	};
	struct textFile: public file
	{
		string contents;

//		textFile():file("",TEXT){}
		textFile(string fName):file(fName,TEXT_FILE){}
	};
	struct iniFile: public file
	{
		map<string,map<string, string>> bindings;

//		iniFile(): file("",INI){}
		iniFile(string fName): file(fName,INI_FILE){}
		template<class T> void readValue(string section, string name, T& val, T defaultVal = T())
		{
			auto sec = bindings.find(section);			if(sec == bindings.end()) {val = defaultVal; return;}
			auto itt = sec->second.find(name);			if(itt == sec->second.end()) {val = defaultVal; return;}
			val = lexical_cast<T>(itt->second);
		}
		template<class T> T getValue(string section, string name, T defaultVal = T())
		{
			auto sec = bindings.find(section);			if(sec == bindings.end()) return defaultVal;
			auto itt = sec->second.find(name);			if(itt == sec->second.end()) return defaultVal;
			return lexical_cast<T>(itt->second);
		}
	};	
	struct zipFile: public file
	{
		map<string,shared_ptr<file>> files;

		zipFile(string fName): file(fName,ZIP_FILE){}
	};
	struct textureFile: public file
	{
		char channels;
		unsigned int width;
		unsigned int height;
		unsigned char* contents;

		textureFile(string fName, FileFormat fmt):file(fName, TEXTURE_FILE, fmt),channels(0),width(0),height(0),contents(nullptr){}
		~textureFile(){delete[] contents;}
	};
	struct bmpFile: public textureFile
	{
		bmpFile(string fName):textureFile(fName, BMP){}
	};
	struct pngFile: public textureFile
	{
		pngFile(string fName):textureFile(fName, PNG){}
	};
	struct tgaFile: public textureFile
	{
		tgaFile(string fName):textureFile(fName, TGA){}
	};
	struct modelFile: public file
	{
		struct material
		{
			shared_ptr<textureFile> tex;
			shared_ptr<textureFile> specularMap;
			shared_ptr<textureFile> normalMap;
			Color4 diffuse;
			Color3 specular;
			float hardness;
			vector<unsigned int> indices; //indices are offsets in the vertices vector, every 3 indices represent a triangle
		};

		vector<normalMappedVertex3D> vertices;
		vector<material> materials;
		Sphere<float> boundingSphere;
		modelFile(string fName, FileFormat fmt): file(fName, MODEL_FILE, fmt){}
	};
	struct objFile: public modelFile
	{
		objFile(string fName):modelFile(fName, OBJ){}
	};
	shared_ptr<binaryFile> loadBinaryFile(string filename, bool asinc = false);
	shared_ptr<textFile> loadTextFile(string filename, bool asinc = false);
	shared_ptr<iniFile> loadIniFile(string filename, bool asinc = false);
	shared_ptr<zipFile> loadZipFile(string filename, bool asinc = false);
	shared_ptr<textureFile> loadTextureFile(string filename, bool asinc = false);
	shared_ptr<textureFile> loadBmpFile(string filename, bool asinc = false);
	shared_ptr<textureFile> loadTgaFile(string filename, bool asinc = false);
	shared_ptr<textureFile> loadPngFile(string filename, bool asinc = false);
	shared_ptr<modelFile> loadObjFile(string filename, bool asinc = false);

	bool writeBinaryFile(shared_ptr<binaryFile> f, bool asinc = false);
	bool writeTextFile(shared_ptr<textFile> f, bool asinc = false);
	bool writeIniFile(shared_ptr<iniFile> f, bool asinc = false);
	bool writeZipFile(shared_ptr<zipFile> f, bool asinc = false);
	bool writeBmpFile(shared_ptr<textureFile> f, bool asinc = false);
	bool writeTgaFile(shared_ptr<textureFile> f, bool asinc = false);
	bool writePngFile(shared_ptr<textureFile> f, bool asinc = false);

	string filename(string filename);
	string extension(string filename);
	string directory(string filename);
	string changeExtension(string filename, string newExtension);
	string getAppDataDirectory();

	vector<string> getAllFiles(string directory);
	vector<string> getAllFiles(string directory,string ext);
	vector<string> getAllFiles(string directory,set<string> ext);
	vector<string> getAllDirectories(string directory);
	vector<string> getAllFilesAndDirectories(string directory);

	bool fileExists(string file);
	bool directoryExists(string directory);
	bool createDirectory(string directory);

	void shutdown();
private:
	template<class T> T readAs(void* c)
	{
		return *((T*)c);
	}
	template<class T>void writeAs(void* c, const T& value)
	{
		memcpy(c, &value, sizeof(T));
	}
	template<class T>void writeAs(vector<unsigned char>& vec, const T& value)
	{
		vec.reserve(vec.size() + sizeof(T));
		vec.insert(vec.end(), (unsigned char*)&value, (unsigned char*)&value + sizeof(T));
	}
	void workerThread();
#if defined(WINDOWS)
	static void startWorkerThread(void* pThis)
	{
		((FileManager*)pThis)->workerThread();
	}	
#elif defined(LINUX)
	static void* startWorkerThread(void* pThis)
	{
		((FileManager*)pThis)->workerThread();
		return nullptr;
	}
#endif 


	fileContents loadFileContents(string filename);
	bool writeFileContents(string filename, fileContents contents); //deletes contents after writing!!!
	bool writeFileContents(string filename, shared_ptr<file> f, bool async);

	shared_ptr<file> parseFile(string filename, fileContents data);
	fileContents serializeFile(shared_ptr<file> f);

	void parseBinaryFile(shared_ptr<binaryFile> f, fileContents data);
	void parseTextFile(shared_ptr<textFile> f, fileContents data);
	void parseIniFile(shared_ptr<iniFile> f, fileContents data);
	void parseZipFile(shared_ptr<zipFile> f, fileContents data);
	void parseBmpFile(shared_ptr<textureFile> f, fileContents data);
	void parseTgaFile(shared_ptr<textureFile> f, fileContents data);
	void parsePngFile(shared_ptr<textureFile> f, fileContents data);
	void parseObjFile(shared_ptr<modelFile> f, fileContents data);

	fileContents serializeBinaryFile(shared_ptr<binaryFile> f);
	fileContents serializeTextFile(shared_ptr<textFile> f);
	fileContents serializeIniFile(shared_ptr<iniFile> f);
	fileContents serializeZipFile(shared_ptr<zipFile> f);
	fileContents serializeBmpFile(shared_ptr<textureFile> f);
	fileContents serializeTgaFile(shared_ptr<textureFile> f);
	fileContents serializePngFile(shared_ptr<textureFile> f);

	FileManager();
	~FileManager(){}

	queue<shared_ptr<file>> fileQueue;
	queue<shared_ptr<file>> fileWriteQueue;
	mutex fileQueueMutex;
	bool terminateFlag;
};

extern FileManager& fileManager;
