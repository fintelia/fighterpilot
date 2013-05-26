
#include "engine.h"
#include <boost/crc.hpp>

#if defined(WINDOWS)
	#include <Windows.h>
	#include <process.h>
	#include <Shlobj.h>
#elif defined(LINUX)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <pthread.h>
#endif 

#include "zlib/zlib.h"
#include "png/png.h"

#if defined(WINDOWS)
	HANDLE pWorkerThread;
#elif defined(LINUX)
	pthread_t pWorkerThread;
#endif

/**
 *  The FileManager class provides in interface to load and save files to a wide variety of formats, it also has several methods
 *  to query information about the filesystem and to minipulate filenames.
 *  
 *  When adding additional files, be sure to update FileManage::parseFile is that they can be recognized when found inside zip files
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//															FileManager methods																			 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileManager::FileManager(): terminateFlag(false)
{
#if defined(WINDOWS)
	pWorkerThread = (HANDLE)_beginthread(startWorkerThread,0,this);
#elif defined(LINUX)
	int errorCode = pthread_create(&pWorkerThread, nullptr, startWorkerThread, this);
	if(errorCode)
		cout << "fileManager worker thread creation failed with error code" << errorCode << endl;
#endif
}
void FileManager::shutdown()
{
	fileQueueMutex.lock();
	while(!fileQueue.empty())
		fileQueue.pop();
	fileQueueMutex.unlock();
	terminateFlag = true;
#if defined(WINDOWS)
	WaitForSingleObject(pWorkerThread, INFINITE);
#elif defined(LINUX)
	pthread_join(pWorkerThread, NULL);
#endif
	this->~FileManager();
}
void FileManager::workerThread()
{
	bool empty;
	bool writeEmpty;
	while(true)
	{
		fileQueueMutex.lock();
		empty = fileQueue.empty();
		writeEmpty = fileWriteQueue.empty();
		if(!empty)
		{
			shared_ptr<file> f = fileQueue.front();
			fileQueue.pop();
			fileQueueMutex.unlock();

			fileContents data = loadFileContents(f->filename);
			f->parseFile(data);
			delete[] data.contents;
		}
		else if(!writeEmpty)
		{
			shared_ptr<file> f = fileWriteQueue.front();
			fileWriteQueue.pop();
			fileQueueMutex.unlock();

			writeFileContents(f->filename, f->serializeFile());
		}
		else
		{
			fileQueueMutex.unlock();
			if(terminateFlag)
				break;
			threadSleep(15);
		}
	}
}
void FileManager::file::completeLoad(bool s)
{
	mValid = s;
	mComplete=true;
	debugAssert(s);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//															Utility Functions																			 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
string FileManager::filename(string filename)
{
	size_t nDot = filename.find_last_of('.');
	size_t nSlash = filename.find_last_of("/\\");
	if(nSlash != filename.npos)
	{
		if(nDot != filename.npos && nDot > nSlash)
			return filename.substr(nSlash+1,nDot);
		else
			return filename.substr(nSlash+1,filename.npos);
	}
	else
	{
		if(nDot != filename.npos)
			return filename.substr(0,nDot);
		else
			return filename;
	}
}
string FileManager::extension(string filename)
{
	size_t nDot = filename.find_last_of('.');
	size_t nSlash = filename.find_last_of("/\\");

	if (nDot != filename.npos && (nSlash == filename.npos || nSlash < nDot))
		return filename.substr(nDot);
	else
		return string();
}
string FileManager::directory(string filename)
{
	size_t nSlash = filename.find_last_of("/\\");

	if(nSlash != filename.npos)
		return filename.substr(0,nSlash+1);
	else
		return string();
}
string FileManager::changeExtension(string filename, string newExtension)
{
	size_t nDot = filename.find_last_of('.');
	size_t nSlash = filename.find_last_of("/\\");

	if(nDot == filename.npos || (nSlash != filename.npos && nDot < nSlash))
		return filename + newExtension;
	else
		return filename.substr(0,nDot) + newExtension;
}
#if defined(WINDOWS)
	string FileManager::getAppDataDirectory()
	{
		char szPath[MAX_PATH];
		if (SHGetFolderPathA( NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath) == S_OK)
		{
			string dir(szPath);
			dir += "\\FighterPilot\\";

			createDirectory(dir); // checks to see if file exists and creates it if it does not
			return dir;
		}
		else
		{
			debugBreak();
			return ""; //use the current directory instead
		}
	}
	bool FileManager::fileExists(string filename)
	{
		DWORD d = GetFileAttributesA(filename.c_str());
		return d != INVALID_FILE_ATTRIBUTES && !(d & FILE_ATTRIBUTE_DIRECTORY);
	}
	bool FileManager::directoryExists(string directory)
	{
		DWORD d = GetFileAttributesA(directory.c_str());
		return d != INVALID_FILE_ATTRIBUTES && (d & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	bool FileManager::createDirectory(string directory)
	{
		DWORD d = GetFileAttributesA(directory.c_str());
		if(!(d & FILE_ATTRIBUTE_DIRECTORY) || d == INVALID_FILE_ATTRIBUTES)
		{
			bool b = CreateDirectoryA(directory.c_str(), NULL) != 0;
			DWORD lError = GetLastError();
			return b;
		}
		else
			return true;
	}
	vector<string> FileManager::getAllFiles(string directory)
	{
		vector<string> v;
		if(!directoryExists(directory))
			return v;

		directory += "/*";

		HANDLE h;
		WIN32_FIND_DATAA d;

		h = FindFirstFileA(directory.c_str(), &d);
		if(h == INVALID_HANDLE_VALUE)
			return v;

		do
		{
			if(!(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				v.push_back(string(d.cFileName));
			}
		}
		while(FindNextFileA(h, &d) != 0);

		FindClose(h);
		return v;
	}
	vector<string> FileManager::getAllFiles(string directory,string ext)
	{
		string s;
		vector<string> v;
		if(!directoryExists(directory))
			return v;

		directory += "/*";

		HANDLE h;
		WIN32_FIND_DATAA d;

		h = FindFirstFileA(directory.c_str(), &d);
		if(h == INVALID_HANDLE_VALUE)
			return v;

		do
		{
			if(!(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				s = string(d.cFileName);
				if(extension(s) == ext)
					v.push_back(s);
			}
		}
		while(FindNextFileA(h, &d) != 0);

		FindClose(h);
		return v;
	}
	vector<string> FileManager::getAllFiles(string directory,set<string> ext)
	{
		string s;
		vector<string> v;
		if(!directoryExists(directory))
			return v;

		directory += "/*";

		HANDLE h;
		WIN32_FIND_DATAA d;

		h = FindFirstFileA(directory.c_str(), &d);
		if(h == INVALID_HANDLE_VALUE)
			return v;

		do
		{
			if(!(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				s = string(d.cFileName);
				if(ext.count(extension(s)) > 0)
					v.push_back(s);
			}
		}
		while(FindNextFileA(h, &d) != 0);

		FindClose(h);
		return v;
	}
	vector<string> FileManager::getAllDirectories(string directory)
	{
		vector<string> v;
		if(!directoryExists(directory))
			return v;

		directory += "/*";

		HANDLE h;
		WIN32_FIND_DATAA d;

		h = FindFirstFileA(directory.c_str(), &d);
		if(h == INVALID_HANDLE_VALUE)
			return v;

		do
		{
			if(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				v.push_back(string(d.cFileName));
			}
		}
		while(FindNextFileA(h, &d) != 0);

		FindClose(h);
		return v;
	}
	vector<string> FileManager::getAllFilesAndDirectories(string directory)
	{
		vector<string> v;
		if(!directoryExists(directory))
			return v;

		directory += "/*";

		HANDLE h;
		WIN32_FIND_DATAA d;

		h = FindFirstFileA(directory.c_str(), &d);
		if(h == INVALID_HANDLE_VALUE)
			return v;

		do{
			v.push_back(string(d.cFileName));
		}while(FindNextFileA(h, &d) != 0);

		FindClose(h);
		return v;
	}
#elif defined(LINUX)
	string FileManager::getAppDataDirectory()
	{
		return "./";
	}
	bool FileManager::fileExists(string filename)
	{
		struct stat sb;
		if (stat(filename.c_str(), &sb) == -1)
			return false;
		
		return (sb.st_mode & S_IFMT) == S_IFREG;
	}
	bool FileManager::directoryExists(string directory)
	{
		struct stat sb;
		if (stat(directory.c_str(), &sb) == -1)
			return false;
		
		return (sb.st_mode & S_IFMT) == S_IFDIR;
	}
	bool FileManager::createDirectory(string directory)
	{
		int status = mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		return status==0 || errno == EEXIST;
	}
	vector<string> FileManager::getAllFiles(string directory)
	{
		DIR *dp = opendir(directory.c_str());
		if(dp == nullptr)
		{
			return vector<string>();
		}

		struct dirent *dirp;
		vector<string> files;
		while((dirp = readdir(dp)) != nullptr)
		{
			if(dirp->d_type == DT_REG)
			{
				files.push_back(string(dirp->d_name));
			}
		}
		closedir(dp);
		return files;
	}
	vector<string> FileManager::getAllFiles(string directory,string ext)
	{
		DIR *dp = opendir(directory.c_str());
		if(dp == nullptr)
		{
			return vector<string>();
		}

		struct dirent *dirp;
		vector<string> files;
		while((dirp = readdir(dp)) != nullptr)
		{
			if(dirp->d_type == DT_REG && extension(string(dirp->d_name)) == ext)
			{
				files.push_back(string(dirp->d_name));
			}
		}
		closedir(dp);
		return files;
	}
	vector<string> FileManager::getAllFiles(string directory,set<string> ext)
	{
		DIR *dp = opendir(directory.c_str());
		if(dp == nullptr)
		{
			return vector<string>();
		}

		struct dirent *dirp;
		vector<string> files;
		while((dirp = readdir(dp)) != nullptr)
		{
			if(dirp->d_type == DT_REG && ext.count(extension(string(dirp->d_name))) != 0)
			{
				files.push_back(string(dirp->d_name));
			}
		}
		closedir(dp);
		return files;
	}
	vector<string> FileManager::getAllDirectories(string directory)
	{
		DIR *dp = opendir(directory.c_str());
		if(dp == nullptr)
		{
			return vector<string>();
		}

		struct dirent *dirp;
		vector<string> directories;
		while((dirp = readdir(dp)) != nullptr)
		{
			if(dirp->d_type == DT_DIR)
			{
				directories.push_back(string(dirp->d_name));
			}
		}
		closedir(dp);
		return directories;
	}
	vector<string> FileManager::getAllFilesAndDirectories(string directory)
	{
		DIR *dp = opendir(directory.c_str());
		if(dp == nullptr)
		{
			return vector<string>();
		}

		struct dirent *dirp;
		vector<string> files;
		while((dirp = readdir(dp)) != nullptr)
		{
			if(dirp->d_type == DT_REG || dirp->d_type == DT_DIR)
			{
				files.push_back(string(dirp->d_name));
			}
		}
		closedir(dp);
		return files;
	}
#else
	#error OS not supported by fileManager
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//																Load File																				 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileManager::fileContents FileManager::loadFileContents(string filename)
{
	fileContents f;

	try{
		std::ifstream fin(filename,std::ios::in|std::ios::ate|std::ios::binary);
		if(fin.is_open())
		{
			f.size = (unsigned long)fin.tellg();
			f.contents = new unsigned char[f.size];
			//memset(f.contents,0,f.size);
			fin.seekg(0, std::ios::beg);
			fin.read((char*)f.contents, f.size);
			fin.close();
		}
	}catch(...){}
	return f;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//																Parse Files																				 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
shared_ptr<FileManager::file> FileManager::parseFile(string filename, fileContents data)
{
	string ext = extension(filename);
	if(data.contents && data.size>=4 && readAs<unsigned int>(data.contents) == 0x04034b50) //zip file
	{
		static int recursionLevel=0;
		if(recursionLevel >= 8)
		{
			return shared_ptr<zipFile>(new zipFile(filename));
		}

		recursionLevel++;
		shared_ptr<zipFile> f(new zipFile(filename));
		f->parseFile(data);
		recursionLevel--;
		return f;
	}
	else if(data.contents && data.size>=2 && readAs<unsigned short>(data.contents) == 0x4d42) //bmp file
	{
		shared_ptr<textureFile> f(new textureFile(filename,BMP));
		f->parseFile(data);
		return f;
	}
	else if(data.contents && data.size>=4 && readAs<unsigned int>(data.contents) == 0x474e5089) //png file
	{
		shared_ptr<textureFile> f(new textureFile(filename,PNG));
		f->parseFile(data);
		return f;
	}
	else if(ext == ".mesh")
	{
		shared_ptr<modelFile> f(new modelFile(filename,MESH));
		f->parseFile(data);
		return f;
	}
	else if(ext == ".txt")
	{
		shared_ptr<textFile> f(new textFile(filename));
		f->parseFile(data);
		return f;
	}
	else if(ext == ".ini")
	{
		shared_ptr<iniFile> f(new iniFile(filename));
		f->parseFile(data);
		return f;
	}
	else if(ext == ".tga")
	{
		shared_ptr<textureFile> f(new textureFile(filename,TGA));
		f->parseFile(data);
		return f;
	}
	else if(ext == ".raw")
	{
		shared_ptr<binaryFile> f(new binaryFile(filename));
		f->parseFile(data);
		return f;
	}
	else if(ext == ".obj")
	{
		shared_ptr<modelFile> f(new modelFile(filename,OBJ));
		f->parseFile(data);
		return f;
	}
	else //default to binary file
	{
		shared_ptr<binaryFile> f(new binaryFile(filename));
		f->parseFile(data);
		return f;
	}
}
void FileManager::binaryFile::parseFile(fileContents data)
{
	if(data.contents != nullptr && data.size != 0)
	{
		contents = new unsigned char[data.size];
		size = data.size;

		memcpy(contents, data.contents, data.size);
		completeLoad(true);
	}
	else if(data.size == 0)
	{
		contents = nullptr;
		size = 0;
		completeLoad(true);
	}
	else
	{
		completeLoad(false);
	}
}
void FileManager::textFile::parseFile(fileContents data)
{
	if(data.contents != nullptr || data.size == 0)
	{
		for(unsigned int i=0; i < data.size; i++)
		{
			if(data.contents[i] != '\r')
				contents += (char)data.contents[i];
		}
		completeLoad(true);
	}
	else
	{
		completeLoad(false);
	}
}
void FileManager::iniFile::parseFile(fileContents data)
{
	if(data.contents != nullptr && data.size != 0)
	{
		string section="", key, value;

		char* d = (char*)data.contents;
		int size = data.size;

		int i=0;
		auto advanceToNextLine = [&i, &size,&d]()
		{
			while(i < size && d[i] != '\n')
				++i;
			++i;
		};


		while(i < size)
		{
			key = "";
			value = "";
			if(d[i] == '[')
			{
				section = "";
				++i;
				while(i < size && d[i] != ']' && d[i] != '\n')
				{
					if(d[i] != '\r')
						section += d[i];
					++i;
				}
			}
			else
			{
				if(d[i] == ';' || d[i] == '#'){ advanceToNextLine(); continue;}

				while(i < size && d[i] != '\n' && d[i] != '=' && d[i] != ';' && d[i] != '#')
				{
					if(d[i] != '\r')
						key += d[i];
					++i;
				}

				if(d[i] == ';' || d[i] == '#'){ advanceToNextLine();continue;}
				else if(d[i] == '\n'){++i; continue;}
				else if(i >= size) break;
				else if(d[i] == '=') i++;

				while(i < size && d[i] != '\n' && d[i] != ';' && d[i] != '#')
				{
					if(d[i] != '\r')
						value += d[i];
					++i;
				}

				bindings[section][key] = value;

				if(d[i] != '\n')
					advanceToNextLine();
			}

		}

		completeLoad(true);
	}
	else if(data.size == 0)
	{
		completeLoad(true);
	}
	else
	{
		completeLoad(false);
	}
}
void FileManager::zipFile::parseFile(fileContents data)
{
	struct LocalHeader
	{
		unsigned int	signature;
		unsigned short	version;
		unsigned short	flag;
		unsigned short	compression;
		unsigned short	modificationTime;
		unsigned short	modificationDate;
		//signed short	padding
		unsigned int	crc32;
		unsigned int	compressedSize;
		unsigned int	uncompressedSize;
		unsigned short	nameLength;
		unsigned short	extraLength;
	};

	if(data.contents != nullptr && data.size >= sizeof(LocalHeader))
	{
		unsigned int position = 0;
		string name, extra;
		LocalHeader localHeader;

		while(readAs<unsigned int>(data.contents + position) != 0x04034b50 && position < data.size)
			position++;

		while(data.size - position > sizeof(LocalHeader) && position < data.size)
		{
			double t = GetTime();
			while(position+4 <= data.size && readAs<unsigned int>(data.contents + position) != 0x04034b50)
				position++;

			if(data.size - position <= sizeof(LocalHeader))
				break;

			localHeader.signature		 = readAs<unsigned int>  (data.contents + position + 0);
			localHeader.version			 = readAs<unsigned short>(data.contents + position + 4);
			localHeader.flag			 = readAs<unsigned short>(data.contents + position + 6);
			localHeader.compression		 = readAs<unsigned short>(data.contents + position + 8);
			localHeader.modificationTime = readAs<unsigned short>(data.contents + position + 10);
			localHeader.modificationDate = readAs<unsigned short>(data.contents + position + 12);
			localHeader.crc32			 = readAs<unsigned int>  (data.contents + position + 14);
			localHeader.compressedSize	 = readAs<unsigned int>  (data.contents + position + 18);
			localHeader.uncompressedSize = readAs<unsigned int>  (data.contents + position + 22);
			localHeader.nameLength		 = readAs<unsigned short>(data.contents + position + 26);
			localHeader.extraLength		 = readAs<unsigned short>(data.contents + position + 28);

			position += 30;

			name.assign((const char*)(data.contents + position), localHeader.nameLength);
			position += localHeader.nameLength;

			extra.assign((const char*)(data.contents + position), localHeader.extraLength);
			position += localHeader.extraLength;


			t = GetTime() - t;
			t = GetTime();

			if(localHeader.uncompressedSize != 0 && (localHeader.compression == 0 || localHeader.compression == 8)) //make sure size and compression are valid
			{
				fileContents d;
				d.size = localHeader.uncompressedSize;
				
				if(localHeader.compression == 0)
				{
					d.contents = data.contents + position;


						t = GetTime() - t;
						t = GetTime();

					//memcpy(d.contents, data.contents + position, d.size);
					files[name] = fileManager.parseFile(name, d);

					t = GetTime() - t;
					t = GetTime();
				}
				else if(localHeader.compression == 8)
				{
					d.contents = new unsigned char[localHeader.uncompressedSize];

					Bytef *dest			= d.contents;
					uLongf *destLen		= &d.size;
					const Bytef *source = data.contents + position;
					uLong sourceLen		= localHeader.compressedSize;
					z_stream stream;
					int err;

					stream.next_in = (Bytef*)source;
					stream.avail_in = (uInt)sourceLen;
					/* Check for source > 64K on 16-bit machine: */

					stream.next_out = dest;
					stream.avail_out = (uInt)*destLen;

					stream.zalloc = (alloc_func)0;
					stream.zfree = (free_func)0;

					err = inflateInit2(&stream,-15);
					if (err != Z_OK)
					{
						delete[] d.contents;
						continue;
					}

					err = inflate(&stream, Z_FINISH);
					if (err != Z_STREAM_END) {
						inflateEnd(&stream);
						delete[] d.contents;
						continue;
					}
					//*destLen = stream.total_out;

					err = inflateEnd(&stream);
					if(err != Z_OK)
					{
						delete[] d.contents;
						continue;
					}
					files[name] = fileManager.parseFile(name, d);
					delete[] d.contents;
				}
			}
			else if(localHeader.uncompressedSize == 0)
			{
				fileContents d;
				d.contents = nullptr;
				d.size = 0;
				files[name] = fileManager.parseFile(name, d);
			}
			position += localHeader.compressedSize;
		}
		completeLoad(true);
	}
	else if(data.size == 0)
	{
		completeLoad(true);
	}
	else
	{
		completeLoad(false);
	}
}
void pngStreamRead(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
	FileManager::fileContents* ptr = (FileManager::fileContents*)png_get_io_ptr(png_ptr);
	if(ptr  != nullptr && ptr->size >= byteCountToRead)
	{
		memcpy(outBytes, ptr->contents, byteCountToRead);

		ptr->contents += byteCountToRead;
		ptr->size -= byteCountToRead;
	}
	else if(ptr != nullptr)
	{
		memcpy(outBytes, ptr->contents, byteCountToRead);

		ptr->contents += ptr->size;
		ptr->size = 0;
	}
};
void FileManager::textureFile::parseFile(fileContents data)
{
	if(data.contents == nullptr || data.size == 0)
	{
		completeLoad(false);
		return;
	}

	if(readAs<unsigned short>(data.contents) == 0x4d42)			format = BMP;
	else if(readAs<unsigned int>(data.contents) == 0x474e5089)	format = PNG;
	else if(fileManager.extension(filename) == ".tga")			format = TGA;
	else
	{
		completeLoad(false);
		return;
	}

	if(format == BMP)
	{
		struct Header
		{
			unsigned short	type;
			unsigned long	size;
			unsigned short	reserved1;
			unsigned short	reserved2;
			unsigned long	offBits;
		}header;
		struct InfoHeader
		{
			unsigned long	size;
			long			width;
			long			height;
			unsigned short	planes;
			unsigned short	bitCount;
			unsigned long	compression;
			unsigned long	sizeImage;
			long			xPelsPerMeter;
			long			yPelsPerMeter;
			unsigned long	clrUsed;
			unsigned long	clrImportant;
		}infoHeader;


		if(data.size <= sizeof(header) + sizeof(infoHeader))
		{
			completeLoad(false);
			return;
		}

		unsigned int position=0;

		header.type			= readAs<unsigned short>(data.contents+position);	position +=2;
		header.size			= readAs<unsigned long>(data.contents+position);	position +=4;
		header.reserved1	= readAs<unsigned short>(data.contents+position);	position +=2;
		header.reserved2	= readAs<unsigned short>(data.contents+position);	position +=2;
		header.offBits		= readAs<unsigned long>(data.contents+position);	position +=4;

		infoHeader.size				= readAs<unsigned long>(data.contents+position);	position +=4;
		infoHeader.width			= readAs<long>(data.contents+position);				position +=4;
		infoHeader.height			= readAs<long>(data.contents+position);				position +=4;
		infoHeader.planes			= readAs<unsigned short>(data.contents+position);	position +=2;
		infoHeader.bitCount			= readAs<unsigned short>(data.contents+position);	position +=2;
		infoHeader.compression		= readAs<unsigned long>(data.contents+position);	position +=4;
		infoHeader.sizeImage		= readAs<unsigned long>(data.contents+position);	position +=4;
		infoHeader.xPelsPerMeter	= readAs<long>(data.contents+position);				position +=4;
		infoHeader.yPelsPerMeter	= readAs<long>(data.contents+position);				position +=4;
		infoHeader.clrUsed			= readAs<unsigned long>(data.contents+position);	position +=4;
		infoHeader.clrImportant		= readAs<unsigned long>(data.contents+position);	position +=4;

		if(infoHeader.width <= 0 || infoHeader.height <= 0 || !(infoHeader.bitCount == 8 || infoHeader.bitCount == 24 || infoHeader.bitCount == 32))
		{
			completeLoad(false);
			return;
		}

		width = infoHeader.width;
		height = infoHeader.height;

		if(infoHeader.bitCount == 8)		channels = 1;
		else if(infoHeader.bitCount == 24)	channels = 3;
		else if(infoHeader.bitCount == 32)	channels = 4;

		contents = new unsigned char[width * height * channels];

		int rowExtra = 3 - (width*channels - 1) % 4;

		if(infoHeader.bitCount == 8)
		{
			for(unsigned long y = 0; y < height; y++)
			{
				memcpy(contents + width*y, contents + position, width);
				position += width * 3 + rowExtra;
			}
			textureFormat = GraphicsManager::texture::INTENSITY;
		}
		else if(infoHeader.bitCount == 24)
		{
			for(unsigned long y = 0; y < height; y++)
			{
				memcpy(contents + 3*width*y, contents + position, 3*width);
				position += width * 3 + rowExtra;
			}
			textureFormat = GraphicsManager::texture::BGR;
		}
		else if(infoHeader.bitCount == 32)
		{
			for(unsigned long y = 0; y < height; y++)
			{
				memcpy(contents + 4*width*y, contents + position, 4*width);
				position += width * 4 + rowExtra;
			}
			textureFormat = GraphicsManager::texture::BGRA;
		}
		completeLoad(true);
	}
	else if(format == TGA)
	{
		width = readAs<short>(data.contents + 12);
		height = readAs<short>(data.contents + 14);
		char bpp = readAs<char>(data.contents + 16);

		channels = bpp / 8;

		if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp !=32)))
		{
			completeLoad(false);
			return;
		}

		contents = new unsigned char[width*height*channels];
		if(bpp == 24)	textureFormat = GraphicsManager::texture::BGR;
		if(bpp == 32)	textureFormat = GraphicsManager::texture::BGRA;
		memcpy(contents, data.contents + 18, width*height*channels);

		//unsigned char tmpSwap;
		//for(int cswap = 0; cswap < (int)(f->width*f->height*f->channels); cswap += f->channels)
		//{
		//	tmpSwap = f->contents[cswap+2];
		//	f->contents[cswap+2] = f->contents[cswap];
		//	f->contents[cswap] = f->contents[cswap+2];
		//}

		completeLoad(true);
	}
	else if(format == PNG)
	{
		png_uint_32		i,
						png_width,
						png_height,
						rowbytes;
		int				bit_depth,
						color_type,
						colorChannels;
		unsigned char*	image_data;
		png_bytep*		row_pointers;

		if(!png_check_sig(data.contents, 8)) 
		{
			completeLoad(false);
			return;
		}

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!png_ptr) //out of memory
		{
			completeLoad(false);
			return;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr) //out of memory
		{
			png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
			completeLoad(false);
			return;
		}

		fileContents streamPtr = data;

		// lets libpng know that we already checked the 8 signature bytes, 
		// so it should not expect to find them at the current file pointer location
		streamPtr.contents += 8;
		streamPtr.size -= 8;
		png_set_sig_bytes(png_ptr, 8);


		png_set_read_fn(png_ptr, &streamPtr, pngStreamRead);

		png_infop end_ptr = png_create_info_struct(png_ptr);
		if (!end_ptr) //out of memory
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			completeLoad(false);
			return;
		}

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
			completeLoad(false);
			return;
		}

		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &png_width, &png_height, &bit_depth, &color_type, NULL, NULL, NULL);


		if(color_type == PNG_COLOR_TYPE_PALETTE)											png_set_expand(png_ptr);
		if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)								png_set_expand(png_ptr);
		if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))									png_set_expand(png_ptr);
		if(bit_depth == 16)																	png_set_strip_16(png_ptr);
		if(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA)		png_set_bgr(png_ptr);

		//if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		//{
		//	png_set_gray_to_rgb(png_ptr);  //is this needed?
		//	png_set_bgr(png_ptr);
		//}

		png_read_update_info(png_ptr, info_ptr);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		colorChannels = (int)png_get_channels(png_ptr, info_ptr);

		image_data = new unsigned char[rowbytes*png_height];
		row_pointers = new png_bytep[png_height];

		for (i = 0;  i < png_height;  i++)
			row_pointers[i] = image_data + i*rowbytes;

		png_read_image(png_ptr, row_pointers);
		png_read_end(png_ptr, NULL);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		//fclose(infile);


		//int format;
		if(colorChannels == 1)		textureFormat = GraphicsManager::texture::INTENSITY;
		else if(colorChannels == 2)	textureFormat = GraphicsManager::texture::LUMINANCE_ALPHA;
		else if(colorChannels == 3) textureFormat = GraphicsManager::texture::BGR;
		else if(colorChannels == 4) textureFormat = GraphicsManager::texture::BGRA;

		channels = colorChannels;
		contents = image_data;
		height = png_height;
		width = png_width;

		delete[] row_pointers;
		completeLoad(true);
	}
}
		struct face{unsigned int v[3];unsigned int t[3];unsigned int n[3];int material;unsigned int combinedVertices[3];
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3, unsigned int n1, unsigned int n2, unsigned int n3) {	v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=n1;n[1]=n2;n[2]=n3;}
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=0;n[1]=0;n[2]=0;}
		face(unsigned int v1, unsigned int v2, unsigned int v3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}
		face() {v[0]=0;v[1]=0;v[2]=0;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}};
		struct triangle
		{
			Vec3f v1;
			Vec2f t1;
			Vec3f n1;

			Vec3f v2;
			Vec2f t2;
			Vec3f n2;

			Vec3f v3;
			Vec2f t3;
			Vec3f n3;
		};
		struct vertexIndices
		{
			unsigned int position;
			unsigned int texCoord;
			unsigned int normal;
			bool operator< (const vertexIndices& v)const{return v.position != position ? v.position < position : (v.texCoord != texCoord ? v.texCoord < texCoord : v.normal < normal);}
			vertexIndices(unsigned int p, unsigned int t, unsigned int n): position(p), texCoord(t), normal(n){}
		};
void FileManager::modelFile::parseFile(fileContents data)
{
	if(data.contents == nullptr || data.size == 0)
	{
		completeLoad(false);
		return;
	}

	if(fileManager.extension(filename) == ".obj")			format = OBJ;
	else if(fileManager.extension(filename) == ".mesh")		format = MESH;
	else if(format != OBJ && format != MESH)
	{
		completeLoad(false);
		return;
	}


	if(format == OBJ)
	{

		auto safeStr = [](const char* c)->const char*
		{
			return c ? c : "";
		};

	////////////////////variables///////////////////////////
		if(data.size == 0 || data.contents == nullptr)
		{
			completeLoad(false);
			return;
		}

		unsigned int			numMtls=0;
		vector<Vec3f>			verts;
		vector<Vec3f>			normals;
		vector<Vec2f>			texCoords;
		vector<face>			faces;
		material*				mtls;
		map<string,int>			mtlNames;

		string dataString;
		dataString.reserve(data.size);
		for(unsigned int i=0; i < data.size; i++)
		{
			if(data.contents[i] != '\r')
				dataString += (char)data.contents[i];
		}

		std::stringstream dataStream;
		dataStream.str(dataString);
	

		map<vertexIndices, unsigned int> indexMap;
		map<string,modelFile::material>	mtl_map;
		string mtlFilename;

		char* token;
		char line[256];

		int stringPos = dataString.find("mtllib ");
		if(stringPos != dataString.npos)
		{
			mtlFilename = dataString.substr(stringPos+7,dataString.find("\n",stringPos+7)-(stringPos+7));
		}

	/////////////////////////new mtl////////////////////////////
		if(mtlFilename != "")
		{
			auto parseTexLine = [](char* line, Vec2f& scale, bool& tile)->string
			{
				if(!line) return "";
				string file;
				scale = Vec2f(1.0, 1.0);
				tile = true;
				char* c = strtok(line," ");
				do{
					if(strcmp(c,"-blenu") == 0)
					{
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-blenv") == 0)
					{
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-boost") == 0)
					{
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-cc") == 0)
					{
						strtok(NULL, " ");
					}
					else if(strcmp(c, "-clamp") == 0)
					{
						if(strcmp(strtok(NULL, " "),"on") == 0) 
							tile = false;
					}
					else if(strcmp(c,"-mm") == 0)
					{
						strtok(NULL, " ");
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-o") == 0)
					{
						strtok(NULL, " ");
						strtok(NULL, " ");
						strtok(NULL, " ");
					}
					else if(strcmp(c, "-s") == 0)
					{
						scale.x = lexical_cast<float>(strtok(NULL, " "));
						scale.y = lexical_cast<float>(strtok(NULL, " "));
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-t") == 0)
					{
						strtok(NULL, " ");
						strtok(NULL, " ");
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-texres") == 0)
					{
						strtok(NULL, " ");
					}
					else if(strcmp(c,"-bm") == 0)
					{
						strtok(NULL, " ");
					}
					else if(file == "")
					{
						file += c;
					}
					else
					{
						file += " ";
						file += c;
					}
				}while((c=strtok(NULL, " ")));
				return file;
			};

			auto mtlFile = fileManager.loadFile<textFile>(fileManager.directory(filename) + mtlFilename);

			dataStream.clear();
			dataStream.str(mtlFile->contents);

			if(mtlFile->valid())
			{
				enum State{SEARCHING_FOR_NEWMTL, READING_MTL}state=SEARCHING_FOR_NEWMTL;
				modelFile::material mMtl;
				string mtlName;
				while(dataStream.getline(line,256))
				{
					token = strtok(line, " \t");
					if(token == nullptr)
						continue;

					if(strcmp(token, "newmtl") == 0)
					{
						if(state == READING_MTL)
						{
							mtl_map[mtlName] = mMtl;
						}
						mMtl = modelFile::material();
						//mMtl.tex.reset();
						//mMtl.normalMap.reset();
						//mMtl.specularMap.reset();
						mtlName = safeStr(strtok(NULL, " #\n"));
						//mMtl.diffuse = white;
						//mMtl.specular = black;
						//mMtl.hardness = 40.0;
						state = READING_MTL;
					}
					else if(strcmp(token, "map_Kd") == 0 && state == READING_MTL)
					{
						bool tile;
						Vec2f scale;
						string texName = fileManager.directory(filename) + parseTexLine(strtok(NULL, "#\n"), scale, tile);
						//mMtl.tex = loadTextureFile(filename);
						mMtl.textureMap_filename = texName;
					}
					else if(strcmp(token, "map_Ns") == 0 && state == READING_MTL)
					{
						bool tile;
						Vec2f scale;
						string texName = fileManager.directory(filename) + parseTexLine(strtok(NULL, "#\n"), scale, tile);
						//mMtl.specularMap = loadTextureFile(filename);
						mMtl.specularMap_filename = texName;
					}
					else if(strcmp(token, "map_bump") == 0 && state == READING_MTL)
					{
						bool tile;
						Vec2f scale;
						string texName = fileManager.directory(filename) + parseTexLine(strtok(NULL, "#\n"), scale, tile);
						//mMtl.normalMap = loadTextureFile(filename);
						mMtl.normalMap_filename = texName;
					}
					else if(strcmp(token, "Kd") == 0 && state == READING_MTL)
					{
						float r,g,b;
						if(sscanf(safeStr(strtok(NULL, "#\n")), "%f%f%f", &r,&g,&b) == 3)
						{
							mMtl.diffuse = Color4(r,g,b,mMtl.diffuse.a);
						}
					}
					else if(strcmp(token, "Ks") == 0 && state == READING_MTL)
					{
						float r,g,b;
						if(sscanf(safeStr(strtok(NULL, "#\n")), "%f%f%f", &r,&g,&b) == 3)
						{
							mMtl.specular = Color3(r,g,b);
						}
					}
					else if(strcmp(token, "Ns") == 0 && state == READING_MTL)
					{
						sscanf(safeStr(strtok(NULL, "#\n")), "%f", &mMtl.hardness);
					}
					else if(strcmp(token, "d") == 0 && state == READING_MTL)
					{
						sscanf(safeStr(strtok(NULL, "#\n")), "%f", &mMtl.diffuse.a);
					}
					else if(strcmp(token, "Tr") == 0 && state == READING_MTL)
					{
						float Tr;
						if(sscanf(safeStr(strtok(NULL, "#\n")), "%f", &Tr))
						{
							mMtl.diffuse.a = 1.0 - Tr;
						}
					}
				}

				if(state == READING_MTL)
				{
					mtl_map[mtlName] = mMtl;
				}
			}

		}
	////////////////////////new mtl end////////////////////////////

		dataStream.clear();
		dataStream.str(dataString);

		mtls = new modelFile::material[mtl_map.size()];

		int i=0;
		for(auto itt=mtl_map.begin();itt!=mtl_map.end();itt++)
		{
			mtls[numMtls++]=itt->second;
			mtlNames[itt->first] = i++;
		}

		int cMtl=-1;
		Vec2f tmpVec2;
		Vec3f tmpVec3;
		face tmpFace;
		while(dataStream.getline(line,256))
		{
			token = strtok(line, " \t");
			if(token == nullptr)
				continue;

			if(strcmp(token, "v") == 0)
			{
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec3.x);
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec3.y);
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec3.z);
				tmpVec3.x = -tmpVec3.x;
				verts.push_back(tmpVec3);
			}
			else if(strcmp(token, "vt") == 0)
			{
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec2.x);
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec2.y);
				tmpVec2.y = 1.0f - tmpVec2.y;
				texCoords.push_back(tmpVec2);
			}
			else if(strcmp(token, "vn") == 0)
			{
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec3.x);
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec3.y);
				sscanf(safeStr(strtok(NULL, " ")), "%f", &tmpVec3.z);
				normals.push_back(tmpVec3.normalize());
			}
			else if(strcmp(token, "f") == 0)
			{
				int i, v = 0, t = 0, n = 0;

				for(i = 0; i<3; i++) //should check for faces that do not include both a vertex, normal and position
				{
					token = strtok(NULL, " \t");
					sscanf(safeStr(token), "%d/%d/%d", &v, &t, &n);

					tmpFace.n[i] = n;
					tmpFace.t[i] = t;
					tmpFace.v[i] = v;
				}
				tmpFace.material=cMtl;
				faces.push_back(tmpFace);
			}
			else if(strcmp(token, "usemtl") == 0)
			{
				string name=safeStr(strtok(NULL, " "));
				if(name != "")
				{
					auto n = mtlNames.find(name);
					if(n != mtlNames.end())
					{
						cMtl = n->second;
					}
				}
			}
		}
	////////////////////////////////////////////////bounding sphere///////////////////////////////////////////////////
		Vec3f center;
		float minx=0, maxx=0, miny=0, maxy=0, minz=0, maxz=0, radiusSquared=0;
		if(verts.size() >= 1)
		{
			minx = maxx = verts[0].x;
			miny = maxy = verts[0].y;
			minz = maxz = verts[0].z;
			for(i=1;i<verts.size();i++)
			{
				if(verts[i].x<minx) minx=verts[i].x;
				if(verts[i].y<miny) miny=verts[i].y;
				if(verts[i].z<minz) minz=verts[i].z;
				if(verts[i].x>maxx) maxx=verts[i].x;
				if(verts[i].y>maxy) maxy=verts[i].y;
				if(verts[i].z>maxz) maxz=verts[i].z;
			}
			boundingSphere.center = Vec3f((minx+maxx)/2,((miny+maxy)/2),(minz+maxz)/2);
			radiusSquared = center.distanceSquared(verts[0]);
			for(i=1;i<verts.size();i++)
			{
				if(center.distanceSquared(verts[i]) > radiusSquared)
				{
					radiusSquared = center.distanceSquared(verts[i]);
				}
			}
			boundingSphere.radius = sqrt(radiusSquared);
		}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		map<vertexIndices, unsigned int>::iterator vertexIndicesItt;
		float s1,t1,s2,t2,inv_det;
		Vec3f Q1, Q2, T, B, T_prime, B_prime;
		normalMappedVertex3D tmpVertex;
		Vec3f faceNormal(0,1,0), avgFaceNormal, faceTangent(0,0,1), faceBitangent(1,0,0);
		for(int i=0; i < faces.size(); i++)
		{
			if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0)
			{
				faceNormal = (verts[faces[i].v[1]-1]-verts[faces[i].v[0]-1]).cross(verts[faces[i].v[2]-1]-verts[faces[i].v[0]-1]);
				if(faces[i].n[0] != 0 && faces[i].n[1] != 0 && faces[i].n[2] != 0)
				{
					avgFaceNormal = normals[faces[i].n[0]-1] + normals[faces[i].n[1]-1] + normals[faces[i].n[2]-1];
					if(faceNormal.dot(avgFaceNormal) < 0.0)// correct triangle winding order
					{
						swap(faces[i].v[0], faces[i].v[1]);
						swap(faces[i].t[0], faces[i].t[1]);
						swap(faces[i].n[0], faces[i].n[1]);
					}
				}
			}	

			if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0 && faces[i].t[0] != 0 && faces[i].t[1] != 0 && faces[i].t[2] != 0)
			{//see: http://www.terathon.com/code/tangent.html

				Q1 = verts[faces[i].v[1]-1] - verts[faces[i].v[0]-1];					
				Q2 = verts[faces[i].v[2]-1] - verts[faces[i].v[0]-1];				

				s1 = (texCoords[faces[i].t[1]-1] - texCoords[faces[i].t[0]-1]).x;
				t1 = (texCoords[faces[i].t[1]-1] - texCoords[faces[i].t[0]-1]).y;
				s2 = (texCoords[faces[i].t[2]-1] - texCoords[faces[i].t[0]-1]).x;
				t2 = (texCoords[faces[i].t[2]-1] - texCoords[faces[i].t[0]-1]).y;

				T, B, T_prime, B_prime;

				inv_det = s1*t2 - s2*t1;

				T = inv_det * ( t2 * Q1 - t1 * Q2);
				B = inv_det * (-s1 * Q1 + s1 * Q2);

				float d1 = faceNormal.dot(T);
				float d2 = faceNormal.dot(B);
				float d3 = (T - faceNormal.dot(T) * faceNormal).dot(B);
				float tms = T.magnitudeSquared();

				T_prime = T - faceNormal.dot(T) * faceNormal;
				B_prime = B - faceNormal.dot(B) * faceNormal - T_prime.dot(B) * T_prime / T.magnitudeSquared();

				faceTangent = T_prime;
				faceBitangent = B_prime;

				//inv = (texCoords[faces[i].t[0]-1].x * texCoords[faces[i].t[1]-1].y - texCoords[faces[i].t[1]-1].x * texCoords[faces[i].t[0]-1].y);
				//faceTangent = abs(inv) < 0.001 ? Vec3f(1,0,0).cross(faceNormal) : (verts[faces[i].v[0]-1] * texCoords[faces[i].t[1]-1].y - verts[faces[i].v[1]-1] * texCoords[faces[i].t[0]-1].y) / inv;
			}


			for(int j = 0; j < 3; j++)
			{
				vertexIndicesItt = indexMap.find(vertexIndices(faces[i].v[j], faces[i].t[j], faces[i].n[j]));
				if(vertexIndicesItt == indexMap.end())
				{
					indexMap[vertexIndices(faces[i].v[j], faces[i].t[j], faces[i].n[j])] = vertices.size();
					faces[i].combinedVertices[j] = vertices.size();

					tmpVertex.position = (faces[i].v[j] != 0) ? verts[faces[i].v[j]-1] : Vec3f();
					tmpVertex.normal = (faces[i].n[j] != 0) ? normals[faces[i].n[j]-1] : faceNormal;
					tmpVertex.UV = (faces[i].t[j] != 0) ? texCoords[faces[i].t[j]-1] : Vec2f();
					tmpVertex.tangent = Vec3f(0,0,0);
					tmpVertex.bitangent = Vec3f(0,0,0);
					vertices.push_back(tmpVertex);
				}
				else
				{
					faces[i].combinedVertices[j] = vertexIndicesItt->second;
				}
			}
			vertices[faces[i].combinedVertices[0]].tangent += faceTangent;
			vertices[faces[i].combinedVertices[1]].tangent += faceTangent;
			vertices[faces[i].combinedVertices[2]].tangent += faceTangent;

			vertices[faces[i].combinedVertices[0]].bitangent += faceBitangent;
			vertices[faces[i].combinedVertices[1]].bitangent += faceBitangent;
			vertices[faces[i].combinedVertices[2]].bitangent += faceBitangent;
		}


		//unsigned int* indexBuffer = new unsigned int[totalFaces*3];
		//unsigned int lNum=0, vNum = 0;
		for(int m=0; m<numMtls; m++)
		{
			material mat = mtls[m];

			int n=0;
			for(int i=0; i < faces.size(); i++)
			{
				if(faces[i].material == m)
				{
					n++;
					mat.indices.push_back(faces[i].combinedVertices[0]);
					mat.indices.push_back(faces[i].combinedVertices[1]);
					mat.indices.push_back(faces[i].combinedVertices[2]);
				}
			}
			if(n > 0)
			{
				materials.push_back(mat);
			}
		}

		int n=0;
		material unnamedMat;
		unnamedMat.diffuse = white;
		unnamedMat.specular = white;
		unnamedMat.hardness = 40.0;
		for(int i=0; i < faces.size(); i++)
		{
			if(faces[i].material == -1)
			{
				n++;
				unnamedMat.indices.push_back(faces[i].combinedVertices[0]);
				unnamedMat.indices.push_back(faces[i].combinedVertices[1]);
				unnamedMat.indices.push_back(faces[i].combinedVertices[2]);
			}
		}
		if(n > 0)
		{
			materials.push_back(unnamedMat);
		}
		completeLoad(true);
	}
	else if(format == MESH)
	{
		if(data.size < 25)
		{
			completeLoad(false);
			return;
		}

		unsigned char version		= readAs<unsigned char>(data.contents);
		unsigned int numVertices	= readAs<unsigned int>(data.contents + 1);
		unsigned int numMaterials	= readAs<unsigned int>(data.contents + 5);
		float boundingSphereX		= readAs<float>(data.contents + 9);
		float boundingSphereY		= readAs<float>(data.contents + 13);
		float boundingSphereZ		= readAs<float>(data.contents + 17);
		float boundingSphereRadius	= readAs<float>(data.contents + 21);

		if(version != 1)
		{
			debugBreak();
			completeLoad(false);
			return;
		}
		
		if(data.size < 25LL + 228LL * numMaterials + 64LL * numVertices)
		{
			completeLoad(false);
			return; 
		}

		unsigned int totalNumMaterialIndicies = 0;
		vector<unsigned int> numMaterialIndicies;
		for(int i = 0; i < numMaterials; i++)
		{
			totalNumMaterialIndicies = readAs<unsigned int>(data.contents+25+228*i+224);
			numMaterialIndicies.push_back(readAs<unsigned int>(data.contents+25+228*i+224));
		}

		if(data.size < 25LL + 228LL * numMaterials + 64LL * numVertices + 4LL * totalNumMaterialIndicies)
		{
			completeLoad(false);
			return;
		}

		boundingSphere.center = Vec3f(boundingSphereX,boundingSphereY,boundingSphereZ);
		boundingSphere.radius = boundingSphereRadius;

		string textureFilename;
		unsigned int position = 25;
		for(int i = 0; i < numMaterials; i++)
		{
			modelFile::material m;

			textureFilename = "";
			for(int n=0; n < 64 && data.contents[position+n] != '\0'; n++)
				textureFilename += data.contents[position+n];
			if(textureFilename != "")
				m.textureMap_filename = textureFilename;//shared_ptr<textureFile>(new textureFile(textureFilename, NO_FILE_FORMAT));//loadTextureFile(textureFilename);

			textureFilename = "";
			for(int n=0; n < 64 && data.contents[position+64+n] != '\0'; n++)
				textureFilename += data.contents[position+64+n];
			if(textureFilename != "")
				m.specularMap_filename = textureFilename;//shared_ptr<textureFile>(new textureFile(textureFilename, NO_FILE_FORMAT));//loadTextureFile(textureFilename);

			textureFilename = "";
			for(int n=0; n < 64 && data.contents[position+128+n] != '\0'; n++)
				textureFilename += data.contents[position+128+n];
			if(textureFilename != "")
				m.normalMap_filename = textureFilename;//shared_ptr<textureFile>(new textureFile(textureFilename, NO_FILE_FORMAT));//loadTextureFile(textureFilename);

			m.diffuse.r = readAs<float>(data.contents+position+192);
			m.diffuse.g = readAs<float>(data.contents+position+196);
			m.diffuse.b = readAs<float>(data.contents+position+200);
			m.diffuse.a = readAs<float>(data.contents+position+204);

			m.specular.r = readAs<float>(data.contents+position+208);
			m.specular.g = readAs<float>(data.contents+position+212);
			m.specular.b = readAs<float>(data.contents+position+216);

			m.hardness = readAs<float>(data.contents+position+220);

			materials.push_back(m);
			position += 228;
		}

		vertices.assign(	(normalMappedVertex3D*)(data.contents+position), 
							(normalMappedVertex3D*)(data.contents+position+numVertices*sizeof(normalMappedVertex3D)));

		position += numVertices*sizeof(normalMappedVertex3D);

		for(int i=0; i<numMaterials; i++)
		{
			materials[i].indices.assign(	(unsigned int*)(data.contents+position),
											(unsigned int*)(data.contents+position+4*numMaterialIndicies[i]));
			position += 4*numMaterialIndicies[i];
		}
		completeLoad(true);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//																Write Files																				 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FileManager::writeFileContents(string filename, fileContents contents)
{
	std::unique_ptr<unsigned char[]> p(contents.contents); //makes sure that memory get deleted

#if defined(WINDOWS)
	if(filename.length() >= 2 && filename[0]=='.' && filename[1]=='/')
	{
		filename = filename.substr(2);
	}
#endif

	try{
		std::ofstream fout(filename,std::ios::binary);
		if(fout.is_open())
		{
			fout.write((const char*)contents.contents, contents.size);
			fout.close();
			return true;
		}
		else
		{
			return false;
		}
	}catch(...)
	{
		return false;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//															Serialize Files																				 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileManager::fileContents FileManager::binaryFile::serializeFile()
{
	fileContents c;
	c.contents = contents;
	c.size = size;
	return c;
}
FileManager::fileContents FileManager::textFile::serializeFile()
{
	fileContents c;
	c.size = contents.size();
	c.contents = new unsigned char[c.size];
	memcpy(c.contents, contents.c_str(), c.size);
	return c;
}
FileManager::fileContents FileManager::iniFile::serializeFile()
{
	string s;
	for(auto section = bindings.begin(); section != bindings.end(); section++)
	{
		s = s + "[" + section->first + "]\n";
		for(auto binding = section->second.begin(); binding != section->second.end(); binding++)
		{
			s += binding->first + "=" + binding->second + "\n";
		}
	}
	fileContents c;
	c.size = s.size();
	c.contents = new unsigned char[c.size];
	memcpy(c.contents, s.c_str(), c.size);
	return c;
}
FileManager::fileContents FileManager::zipFile::serializeFile()	//see: http://www.pkware.com/documents/casestudies/APPNOTE.TXT
{
	vector<unsigned char> data;
	auto write = [&data](const void* d, unsigned int l)
	{
		data.insert(data.end(), (unsigned char*)d, (unsigned char*)d + l);
	};

	struct centralFileHeader
	{
		unsigned short modTime;
		unsigned short modDate;
		unsigned int CRC32;
		unsigned int compressedSize;
		unsigned int size;
		unsigned int localHeaderOffset;
		bool isText;
		string filename;
	};
	vector<centralFileHeader> centralFileHeaders;


	fileContents fContents;
	centralFileHeader cFileHeader;


	for(auto file = files.begin(); file != files.end(); file++)
	{
		fContents = file->second->serializeFile();

		boost::crc_32_type crc;
		crc.process_block(fContents.contents, fContents.contents + fContents.size);

		cFileHeader.modTime = 0;
		cFileHeader.modDate = 0;
		cFileHeader.CRC32 = crc.checksum();
		cFileHeader.compressedSize = fContents.size;
		cFileHeader.size = fContents.size;
		cFileHeader.filename = file->second->filename;
		cFileHeader.localHeaderOffset = data.size();
		cFileHeader.isText = file->second->type == TEXT_FILE || file->second->type == INI_FILE;
		centralFileHeaders.push_back(cFileHeader);

		writeAs<unsigned int>		(data, 0x04034b50);							//local file header signature
		writeAs<unsigned short>		(data, 20);									//version needed to extract
		writeAs<unsigned short>		(data, 0);									//general purpose bit flag
		writeAs<unsigned short>		(data, 0);									//compression method
		writeAs<unsigned short>		(data, 0);									//last mod file time
		writeAs<unsigned short>		(data, 0);									//last mod file date
		writeAs<unsigned int>		(data, cFileHeader.CRC32);					//crc-32
		writeAs<unsigned int>		(data, fContents.size);						//compressed size
		writeAs<unsigned int>		(data, fContents.size);						//uncompressed size
		writeAs<unsigned short>		(data, file->second->filename.size());		//file name length
		writeAs<unsigned short>		(data, 0);									//extra field length

		write(file->second->filename.c_str(), file->second->filename.size());
		write(fContents.contents, fContents.size);
	}

	int cDirectoryStart = data.size();

	for(auto header = centralFileHeaders.begin(); header != centralFileHeaders.end(); header++)
	{
		writeAs<unsigned int>		(data, 0x02014b50);					//central file header signature
		writeAs<unsigned short>		(data, 20);							//version made by
		writeAs<unsigned short>		(data, 20);							//version needed to extract
		writeAs<unsigned short>		(data, 0);							//general purpose bit flag
		writeAs<unsigned short>		(data, 0);							//compression method
		writeAs<unsigned short>		(data, 0);							//last mod file time
		writeAs<unsigned short>		(data, 0);							//last mod file date
		writeAs<unsigned int>		(data, header->CRC32);				//crc-32
		writeAs<unsigned int>		(data, header->compressedSize);		//compressed size
		writeAs<unsigned int>		(data, header->size);				//uncompressed size
		writeAs<unsigned short>		(data, header->filename.size());	//file name length
		writeAs<unsigned short>		(data, 0);							//extra field length
		writeAs<unsigned short>		(data, 0);							//file comment length
		writeAs<unsigned short>		(data, 0);							//disk number start
		writeAs<unsigned short>		(data, header->isText ? 0x1 : 0x0);	//internal file attributes
		writeAs<unsigned int>		(data, 0x00000020);					//external file attributes
		writeAs<unsigned int>		(data, header->localHeaderOffset);	//relative offset of local header

		write(header->filename.c_str(), header->filename.size());
	}

	int cDirectoryEnd = data.size();

	writeAs<unsigned int>		(data, 0x06054b50);							//end of central dir signature
	writeAs<unsigned short>		(data, 0); 									//number of this disk
	writeAs<unsigned short>		(data, 0); 									//number of the disk with the start of the central directory
	writeAs<unsigned short>		(data, (short)centralFileHeaders.size());	//total number of entries in the central directory on this disk
	writeAs<unsigned short>		(data, (short)centralFileHeaders.size()); 	//total number of entries in the central directory
	writeAs<unsigned int>		(data, cDirectoryEnd - cDirectoryStart);	//size of the central directory
	writeAs<unsigned int>		(data, cDirectoryStart); 					//offset of start of central directory with respect to the starting disk number
	writeAs<unsigned short>		(data, 0); 									//ZIP file comment length

	if(data.size() > 0)
	{
		fileContents contents;
		contents.size = data.size();
		contents.contents = new unsigned char[contents.size];
		memcpy(contents.contents, &data[0], contents.size);
		return contents;
	}
	else
	{
		return fileContents();
	}
}
void pngStreamWrite(png_structp png_ptr, png_bytep data, png_size_t length)
{
	vector<unsigned char>& vec = *(vector<unsigned char>*)png_get_io_ptr(png_ptr);

	vec.reserve(vec.size() + length);
	vec.insert(vec.end(), (unsigned char*)data, (unsigned char*)data + length);
};
void pngStreamFlush(png_structp png_ptr){}
FileManager::fileContents FileManager::textureFile::serializeFile()
{
	if(format == BMP)
	{
		unsigned int bmp_width = width * channels;
		unsigned int pWidth = bmp_width + 3 - ((bmp_width - 1) % 4);//padded width

		fileContents c;
		c.size = 54 + pWidth * height;
		c.contents = new unsigned char[c.size];
		memset(c.contents, 0, c.size);

		writeAs<unsigned short>(	c.contents + 0,  0x4D42);			//type
		writeAs<unsigned long>(		c.contents + 2,  c.size);			//size
		writeAs<unsigned short>(	c.contents + 6,  0);				//reserved1
		writeAs<unsigned short>(	c.contents + 8,  0);				//reserved2
		writeAs<unsigned long>(		c.contents + 10, 54);				//offBits
		writeAs<unsigned long>(		c.contents + 14, 40);				//size of infoHeader
		writeAs<long>(				c.contents + 18, width);			//width
		writeAs<long>(				c.contents + 22, height);			//height
		writeAs<unsigned short>(	c.contents + 26, 1);				//planes
		writeAs<unsigned short>(	c.contents + 28, channels * 8);		//bit count
		writeAs<unsigned long>(		c.contents + 30, 0);				//compression
		writeAs<unsigned long>(		c.contents + 34, pWidth * height);	//size of image data
		writeAs<long>(				c.contents + 38, 3000);				//x pixels per meter
		writeAs<long>(				c.contents + 42, 3000);				//y pixels per meter
		writeAs<unsigned long>(		c.contents + 46, 0);				//colors used (or zero)
		writeAs<unsigned long>(		c.contents + 50, 0);				//important colors used (or zero)

		//unsigned char tmpChar;
		//int off1, off2;

		for(unsigned int y = 0; y < height; y++)
		{
			memcpy(c.contents+54+pWidth*y, contents+pWidth*y, bmp_width);

			//for(unsigned int x = 0; x < f->width; x++)
			//{
			//	off1 = 54 + pWidth * y + x * f->channels + 0;
			//	off2 = off1 + 2;
			//
			//	tmpChar = c.contents[off1];
			//	c.contents[off1] = c.contents[off2];
			//	c.contents[off2] = tmpChar;
			//}
		}
		return c;
	}
	else if(format == TGA)
	{
		fileContents c;
		c.size = 18 + width * height * channels;
		c.contents = new unsigned char[c.size];
		memset(c.contents, 0, c.size);

		writeAs<unsigned char>(		c.contents + 0,  0);				//size of id field
		writeAs<unsigned char>(		c.contents + 1,  0);				//type of color map
		writeAs<unsigned char>(		c.contents + 2,  2);				//type of image
		writeAs<unsigned short>(	c.contents + 3,  0);				//first color in palette
		writeAs<unsigned short>(	c.contents + 5,  0);				//number of colors in palette
		writeAs<unsigned char>(		c.contents + 7,  0);				//number of bits per entry
		writeAs<unsigned short>(	c.contents + 8,  0);				//x origin
		writeAs<unsigned short>(	c.contents + 10, 0);				//y origin
		writeAs<unsigned short>(	c.contents + 12, width);			//width
		writeAs<unsigned short>(	c.contents + 14, height);			//height
		writeAs<unsigned char>(		c.contents + 16, channels * 8);		//bits per pixel
		writeAs<unsigned char>(		c.contents + 17, 0);				//descriptor

		if(channels == 3)
		{
			for(unsigned int y = 0; y < height; y++)
			{
				for(unsigned int x = 0; x < width; x++)
				{
					c.contents[18 + (y * width + x) * 3 + 0] = contents[(y * width + x) * 3 + 0]; //B
					c.contents[18 + (y * width + x) * 3 + 1] = contents[(y * width + x) * 3 + 1]; //G
					c.contents[18 + (y * width + x) * 3 + 2] = contents[(y * width + x) * 3 + 2]; //R
				}
			}
		}
		else if(channels == 4)
		{
			for(unsigned int y = 0; y < height; y++)
			{
				for(unsigned int x = 0; x < width; x++)
				{
					c.contents[18 + (y * width + x) * 4 + 0] = contents[(y * width + x) * 4 + 0]; //B
					c.contents[18 + (y * width + x) * 4 + 1] = contents[(y * width + x) * 4 + 1]; //G
					c.contents[18 + (y * width + x) * 4 + 2] = contents[(y * width + x) * 4 + 2]; //R
					c.contents[18 + (y * width + x) * 4 + 3] = contents[(y * width + x) * 4 + 3]; //A
				}
			}
		}
		else
		{
			debugBreak(); //unsupported number of channels!!!
		}
		return c;
	}
	else if(format == PNG)
	{
		vector<unsigned char> data;

		int colorType;
		if(channels == 1)
			colorType = PNG_COLOR_TYPE_GRAY;
		else if(channels == 3)
			colorType = PNG_COLOR_TYPE_RGB;
		else if(channels == 4)
			colorType = PNG_COLOR_TYPE_RGB_ALPHA;
		else
		{
			debugBreak();
			return fileContents();
		}

		//png_bytepp rows = new unsigned char*[height];
		//for(unsigned int i=0;i<height;i++) rows[i] = contents + width * channels * (height - i - 1);

		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,	NULL, NULL);
		if (!png_ptr)
		{
		//	delete[] rows;
			return fileContents();
		}
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr || setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		//	delete[] rows;
			return fileContents();
		}

		png_set_write_fn(png_ptr, (void*)&data, pngStreamWrite, pngStreamFlush);
		png_set_flush(png_ptr, 32);
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		//png_set_rows(png_ptr,info_ptr,rows);
		//png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);

		png_set_bgr(png_ptr);
		png_write_info(png_ptr, info_ptr);
		png_bytep row;

		png_set_filter(png_ptr, 0, PNG_FILTER_PAETH);

		double t = GetTime();
		for(unsigned int i=0;i<height;i++)
		{
			row = contents + width * channels * (height - i - 1);
			png_write_row(png_ptr, row);
		}
		t = GetTime() - t;
		png_write_end(png_ptr, info_ptr);

		if (setjmp(png_jmpbuf(png_ptr))) debugBreak();
		png_destroy_write_struct(&png_ptr, &info_ptr);

		//delete[] rows;

		fileContents c;
		c.size = data.size();
		c.contents = new unsigned char[c.size];
		memcpy(c.contents, &data[0], c.size);
		return c;
	}
	else
	{
		debugBreak();
		return fileContents();
	}
}
FileManager::fileContents FileManager::modelFile::serializeFile()
{
	/*	 header (size = 25 bytes)
	 uint8		version
	 uint32		numVertices
	 uint32		numMaterials
	 float[3]	bounding sphere center
	 float		bounding sphere radius

	 material (size = 228 bytes)
	 char[64]	texture filename
	 char[64]	specular map filename
	 char[64]	normal map filename
	 float[4]	diffuse color
	 float[3]	specular color
	 float		hardness
	 uint32		numIndicies

	 vertex (size = 64 bytes)
	 float[3]	position
	 float[3]	normal
	 float[3]	tanget
	 float[2]	texcoord
	 float[5]	padding

	 index (size = 4 bytes)
	 uint32		index

	 */

	debugAssert(format == MESH);

	fileContents c;
	unsigned int totalIndices = 0;
	for(auto i = materials.begin(); i != materials.end(); i++)
	{
		totalIndices += i->indices.size();
	}

	c.size = 25 + 228 * materials.size() + 64 * vertices.size() + 4 * totalIndices;
	c.contents = new unsigned char[c.size];
	memset(c.contents, 0, c.size);


	writeAs<unsigned char>(c.contents,	1);
	writeAs<unsigned int>(c.contents + 1, vertices.size());
	writeAs<unsigned int>(c.contents + 5, materials.size());
	writeAs<float>(c.contents + 9, boundingSphere.center.x);
	writeAs<float>(c.contents + 13, boundingSphere.center.y);
	writeAs<float>(c.contents + 17, boundingSphere.center.z);
	writeAs<float>(c.contents + 21, boundingSphere.radius);


	unsigned int position = 25;
	for(auto i = materials.begin(); i != materials.end(); i++)
	{
		if((i->textureMap_filename.size() > 64) || (i->specularMap_filename.size() > 64) || (i->normalMap_filename.size() > 64))
			debugBreak(); // texture, specularMap or normalMap filename will be truncated!!!

		for(int n=0; n < 64 && n < i->textureMap_filename.size(); n++)
			writeAs<unsigned char>(c.contents+position+n, i->textureMap_filename[n]);
		for(int n=0; n < 64 && n < i->specularMap_filename.size(); n++)
			writeAs<unsigned char>(c.contents+position+64+n, i->specularMap_filename[n]);
		for(int n=0; n < 64 && n < i->normalMap_filename.size(); n++)
			writeAs<unsigned char>(c.contents+position+128+n, i->normalMap_filename[n]);

		writeAs<float>(c.contents+position+192,	i->diffuse.r);
		writeAs<float>(c.contents+position+196,	i->diffuse.g);
		writeAs<float>(c.contents+position+200,	i->diffuse.b);
		writeAs<float>(c.contents+position+204,	i->diffuse.a);

		writeAs<float>(c.contents+position+208,	i->specular.r);
		writeAs<float>(c.contents+position+212,	i->specular.g);
		writeAs<float>(c.contents+position+216,	i->specular.b);

		writeAs<float>(c.contents+position+220,	i->hardness);

		writeAs<unsigned int>(c.contents+position+224,	i->indices.size());
		position += 228;
	}

	if(!vertices.empty())
	{
		memcpy(c.contents+position, &vertices[0], vertices.size() * sizeof(normalMappedVertex3D));
		position += vertices.size() * sizeof(normalMappedVertex3D);
	}

	for(auto i = materials.begin(); i != materials.end(); i++)
	{
		if(!i->indices.empty())
		{
			memcpy(c.contents+position, &i->indices[0], i->indices.size() * 4);
			position += i->indices.size() * 4;
		}
	}
	
	return c;
}
