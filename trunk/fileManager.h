
class FileManager
{
public:	
	enum FileType{NO_FILE_TYPE, BINARY_FILE,TEXT_FILE,INI_FILE,ZIP_FILE,TEXTURE_FILE,MODEL_FILE};
	enum FileFormat{NO_FILE_FORMAT, BIN, TXT, INI, ZIP, BMP, PNG, TGA, OBJ, MESH};
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
		bool writeFile;
		void completeLoad(bool s);
		friend class FileManager;

	public:
		const FileType type;
		FileFormat format;
		string filename;

		bool valid(){return mComplete && mValid;}
		bool invalid(){return mComplete && !mValid;}
		bool complete(){return mComplete;}
		virtual ~file(){}

	protected:
		file(FileType t, FileFormat f):mComplete(false),mValid(false),writeFile(false), type(t), format(f){}
		file(string fName, FileType t, FileFormat f):mComplete(false),mValid(false),writeFile(false), type(t), format(f), filename(fName){}

		friend class FileManager;
		virtual void parseFile(fileContents data)=0;
		virtual fileContents serializeFile()=0;
	};
	struct binaryFile: public file
	{
		unsigned int size;
		unsigned char* contents;

		binaryFile():file(BINARY_FILE,BIN),size(0),contents(nullptr){}
		binaryFile(string fName):file(fName,BINARY_FILE,BIN),size(0),contents(nullptr){}
		~binaryFile(){delete[] contents;}
	private:
		friend class FileManager;
		void parseFile(fileContents data);
		fileContents serializeFile();
	};
	struct textFile: public file
	{
		string contents;

		textFile():file(TEXT_FILE,TXT){}
		textFile(string fName):file(fName,TEXT_FILE,TXT){}
	private:
		friend class FileManager;
		void parseFile(fileContents data);
		fileContents serializeFile();
	};
	struct iniFile: public file
	{
		map<string,map<string, string>> bindings;

		iniFile(): file(INI_FILE,INI){}
		iniFile(string fName): file(fName,INI_FILE,INI){}
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
	private:
		friend class FileManager;
		void parseFile(fileContents data);
		fileContents serializeFile();
	};	
	struct zipFile: public file
	{
		map<string,shared_ptr<file>> files;

		zipFile(): file(ZIP_FILE,ZIP){}
		zipFile(string fName): file(fName,ZIP_FILE,ZIP){}
	private:
		friend class FileManager;
		void parseFile(fileContents data);
		fileContents serializeFile();
	};
	struct textureFile: public file
	{
		char channels;
		unsigned int width;
		unsigned int height;
		unsigned char* contents; // contents is in BGR(A) order
		GraphicsManager::texture::Format textureFormat;


		textureFile():file(TEXTURE_FILE,NO_FILE_FORMAT),channels(0),width(0),height(0),contents(nullptr),textureFormat(GraphicsManager::texture::NONE){}
		textureFile(string fName,FileFormat format):file(fName,TEXTURE_FILE,format),channels(0),width(0),height(0),contents(nullptr),textureFormat(GraphicsManager::texture::NONE){}
		~textureFile(){delete[] contents;}
	private:
		friend class FileManager;
		void parseFile(fileContents data);
		fileContents serializeFile();
	};
	struct modelFile: public file
	{
		struct material
		{
		//	shared_ptr<textureFile> tex;
		//	shared_ptr<textureFile> specularMap;
		//	shared_ptr<textureFile> normalMap;
			string textureMap_filename;
			string specularMap_filename;
			string normalMap_filename;

			Color4 diffuse;
			Color3 specular;
			float hardness;
			vector<unsigned int> indices; //indices are offsets in the vertices vector, every 3 indices represent a triangle
		};

		vector<normalMappedVertex3D> vertices;
		vector<material> materials;
		Sphere<float> boundingSphere;

		modelFile(): file(MODEL_FILE,NO_FILE_FORMAT){}
		modelFile(string fName,FileFormat format): file(fName,MODEL_FILE,format){}
	private:
		friend class FileManager;
		void parseFile(fileContents data);
		fileContents serializeFile();
	};

	template<class T> shared_ptr<T> loadFile(string filename, bool async=false)
	{
		static_assert(std::is_base_of<file,T>::value, "Type T must derive from FileManager::file");
		
		shared_ptr<T> f(new T());
		f->filename = filename;

		if(async)
		{
			fileQueueMutex.lock();
			fileQueue.push(f);
			fileQueueMutex.unlock();
		}
		else
		{
			fileContents data = fileManager.loadFileContents(filename);
			f->parseFile(data);
			delete[] data.contents;
		}
		return f;
	}
	template<class T> bool writeFile(shared_ptr<T> f, bool async=false)
	{
		static_assert(std::is_base_of<file,T>::value, "Type T must derive from FileManager::file");
		if(async)
		{
			fileQueueMutex.lock();
			fileWriteQueue.push(f);
			fileQueueMutex.unlock();
			return true;
		}
		else
		{
			return writeFileContents(f->filename, f->serializeFile());
		}
	}
	//shared_ptr<binaryFile> loadBinaryFile(string filename, bool async = false);
	//shared_ptr<textFile> loadTextFile(string filename, bool async = false);
	//shared_ptr<iniFile> loadIniFile(string filename, bool async = false);
	//shared_ptr<zipFile> loadZipFile(string filename, bool async = false);
	//shared_ptr<textureFile> loadTextureFile(string filename, bool async = false);
	//shared_ptr<textureFile> loadBmpFile(string filename, bool async = false);
	//shared_ptr<textureFile> loadTgaFile(string filename, bool async = false);
	//shared_ptr<textureFile> loadPngFile(string filename, bool async = false);
	//shared_ptr<modelFile> loadMeshFile(string filename, bool async = false);
	//shared_ptr<modelFile> loadObjFile(string filename, bool async = false);

	//bool writeBinaryFile(shared_ptr<binaryFile> f, bool async = false);
	//bool writeTextFile(shared_ptr<textFile> f, bool async = false);
	//bool writeIniFile(shared_ptr<iniFile> f, bool async = false);
	//bool writeZipFile(shared_ptr<zipFile> f, bool async = false);
	//bool writeBmpFile(shared_ptr<textureFile> f, bool async = false);
	//bool writeTgaFile(shared_ptr<textureFile> f, bool async = false);
	//bool writePngFile(shared_ptr<textureFile> f, bool async = false);
	//bool writeMeshFile(shared_ptr<modelFile> f, bool async = false);					//uses custom format

	//template<typename T> shared_ptr<T> loadFile(string filename, bool async);
	//template<> shared_ptr<binaryFile>	loadFile<binaryFile>(string filename, bool async);
	//template<> shared_ptr<textFile>	loadFile<textFile>(string filename, bool async);
	//template<> shared_ptr<iniFile>	loadFile<iniFile>(string filename, bool async);
	//template<> shared_ptr<zipFile>	loadFile<zipFile>(string filename, bool async);
//	template<> shared_ptr<textureFile>	loadFile<textureFile>(string filename, bool async);
//	template<> shared_ptr<modelFile>	loadFile<modelFile>(string filename, bool async);

	//template<class T> shared_ptr<T>		loadFile(string filename){return loadFile<T>(filename, false);}

	//shared_ptr<file> loadFile(string filename, bool async=false);
	//bool writeFile(shared_ptr<file> f, bool async, string ext);
	//bool writeFile(shared_ptr<file> f, bool async=false);


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
	queue<shared_ptr<file>> fileQueue;
	queue<shared_ptr<file>> fileWriteQueue;
	mutex fileQueueMutex;
	bool terminateFlag;	

	shared_ptr<file> parseFile(string filename, fileContents data);

	fileContents loadFileContents(string filename);
	bool writeFileContents(string filename, fileContents contents); //delete[]'s contents after writing!!!
	//bool writeFileContents(string filename, shared_ptr<file> f, bool async);

	//shared_ptr<file> parseFile(string filename, fileContents data);
	//fileContents serializeFile(shared_ptr<file> f);

	//void parseBinaryFile(shared_ptr<binaryFile> f, fileContents data);
	//void parseTextFile(shared_ptr<textFile> f, fileContents data);
	//void parseIniFile(shared_ptr<iniFile> f, fileContents data);
	//void parseZipFile(shared_ptr<zipFile> f, fileContents data);
	//void parseBmpFile(shared_ptr<textureFile> f, fileContents data);
	//void parseTgaFile(shared_ptr<textureFile> f, fileContents data);
	//void parsePngFile(shared_ptr<textureFile> f, fileContents data);
	//void parseObjFile(shared_ptr<modelFile> f, fileContents data);
	//void parseMeshFile(shared_ptr<modelFile> f, fileContents data);

	//fileContents serializeBinaryFile(shared_ptr<binaryFile> f);
	//fileContents serializeTextFile(shared_ptr<textFile> f);
	//fileContents serializeIniFile(shared_ptr<iniFile> f);
	//fileContents serializeZipFile(shared_ptr<zipFile> f);
	//fileContents serializeBmpFile(shared_ptr<textureFile> f);
	//fileContents serializeTgaFile(shared_ptr<textureFile> f);
	//fileContents serializePngFile(shared_ptr<textureFile> f);
	//fileContents serializeMeshFile(shared_ptr<modelFile> f);

	FileManager();
	~FileManager(){}

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
};

extern FileManager& fileManager;
