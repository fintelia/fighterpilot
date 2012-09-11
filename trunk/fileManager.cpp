
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
		return (d & FILE_ATTRIBUTE_DIRECTORY) != 0;
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
		directory += "/*";
		if(!directoryExists(directory))
			return v;

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
		directory += "/*";
		if(!directoryExists(directory))
			return v;

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
		directory += "/*";
		if(!directoryExists(directory))
			return v;

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
		directory += "/*";
		if(!directoryExists(directory))
			return v;

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
		directory += "/*";
		if(!directoryExists(directory))
			return v;

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
		return "";
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
shared_ptr<FileManager::textFile> FileManager::loadTextFile(string filename, bool asinc)
{
	shared_ptr<textFile> f(new textFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parseTextFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::binaryFile> FileManager::loadBinaryFile(string filename, bool asinc)
{
	shared_ptr<binaryFile> f(new binaryFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		f->contents = data.contents;
		f->size = data.size;
		f->completeLoad(true);
	}
	return f;
}
shared_ptr<FileManager::iniFile> FileManager::loadIniFile(string filename, bool asinc)
{
	shared_ptr<iniFile> f(new iniFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parseIniFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::zipFile> FileManager::loadZipFile(string filename, bool asinc)
{
	shared_ptr<zipFile> f(new zipFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parseZipFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::textureFile> FileManager::loadTextureFile(string filename, bool asinc)
{
	string ext = extension(filename);
	if(ext == ".bmp")	return loadBmpFile(filename, asinc);
	if(ext == ".tga")	return loadTgaFile(filename, asinc);
	if(ext == ".png")	return loadPngFile(filename, asinc);

	return shared_ptr<FileManager::textureFile>();
}
shared_ptr<FileManager::textureFile> FileManager::loadBmpFile(string filename, bool asinc)
{
	shared_ptr<textureFile> f(new bmpFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parseBmpFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::textureFile> FileManager::loadTgaFile(string filename, bool asinc)
{
	shared_ptr<textureFile> f(new tgaFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parseTgaFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::textureFile> FileManager::loadPngFile(string filename, bool asinc)
{
	shared_ptr<textureFile> f(new pngFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parsePngFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::modelFile> FileManager::loadObjFile(string filename, bool asinc)
{
	shared_ptr<modelFile> f(new objFile(filename));

	if(asinc)
	{
		fileQueueMutex.lock();
		fileQueue.push(f);
		fileQueueMutex.unlock();
	}
	else
	{
		fileContents data = loadFileContents(f->filename);
		parseObjFile(f, data);
		delete[] data.contents;
	}
	return f;
}
shared_ptr<FileManager::file> FileManager::parseFile(string filename, fileContents data)
{
	string ext = extension(filename);
	if(ext == ".txt")
	{
		shared_ptr<textFile> f(new textFile(filename));
		parseTextFile(f, data);
		return f;
	}
	else if(ext == ".ini")
	{
		shared_ptr<iniFile> f(new iniFile(filename));
		parseIniFile(f, data);
		return f;
	}
	else if(ext == ".zip")
	{
		static int recursionLevel=0;
		if(recursionLevel >= 8)
		{
			return shared_ptr<zipFile>(new zipFile(filename));
		}

		recursionLevel++;
		shared_ptr<zipFile> f(new zipFile(filename));
		parseZipFile(f, data);
		recursionLevel--;
		return f;
	}
	else if(ext == ".bmp")
	{
		shared_ptr<textureFile> f(new bmpFile(filename));
		parseBmpFile(f, data);
		return f;
	}
	else if(ext == ".tga")
	{
		shared_ptr<textureFile> f(new bmpFile(filename));
		parseBmpFile(f, data);
		return f;
	}
	else if(ext == ".png")
	{
		shared_ptr<textureFile> f(new pngFile(filename));
		parsePngFile(f, data);
		return f;
	}
	else if(ext == ".raw")
	{
		shared_ptr<binaryFile> f(new binaryFile(filename));
		parseBinaryFile(f, data);
		return f;
	}
	else
	{
		shared_ptr<binaryFile> f(new binaryFile(filename));
		parseBinaryFile(f, data);
		return f;
	}

}
FileManager::fileContents FileManager::serializeFile(shared_ptr<file> f)
{
	if(f->type == TEXT_FILE)
	{
		return serializeTextFile(dynamic_pointer_cast<textFile>(f));
	}
	else if(f->type == INI_FILE)
	{
		return serializeIniFile(dynamic_pointer_cast<iniFile>(f));
	}
	else if(f->type == ZIP_FILE)
	{
		return serializeZipFile(dynamic_pointer_cast<zipFile>(f));
	}
	else if(f->type == TEXTURE_FILE && f->format == BMP)
	{
		return serializeBmpFile(dynamic_pointer_cast<textureFile>(f));
	}
	else if(f->type == TEXTURE_FILE && f->format == TGA)
	{
		return serializeTgaFile(dynamic_pointer_cast<textureFile>(f));
	}
	else if(f->type == TEXTURE_FILE && f->format == PNG)
	{
		return serializePngFile(dynamic_pointer_cast<textureFile>(f));
	}
	else if(f->type == BINARY_FILE)
	{
		return serializeBinaryFile(dynamic_pointer_cast<binaryFile>(f));
	}
	else
	{
		debugBreak();
		return fileContents();
	}
}
void FileManager::parseBinaryFile(shared_ptr<binaryFile> f, fileContents data)
{
	if(data.contents != nullptr && data.size != 0)
	{
		f->contents = new unsigned char[data.size];
		f->size = data.size;

		memcpy(f->contents, data.contents, data.size);
		f->completeLoad(true);
	}
	else if(data.size == 0)
	{
		f->contents = nullptr;
		f->size = 0;
		f->completeLoad(true);
	}
	else
	{
		f->completeLoad(false);
	}
}
void FileManager::parseTextFile(shared_ptr<textFile> f, fileContents data)
{
	if(data.contents != nullptr)
	{
		for(unsigned int i=0; i < data.size; i++)
		{
			if(data.contents[i] != '\r')
				f->contents += (char)data.contents[i];
		}
		f->completeLoad(true);
	}
	else
	{
		f->completeLoad(false);
	}
}
void FileManager::parseIniFile(shared_ptr<iniFile> f, fileContents data)
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

				f->bindings[section][key] = value;

				if(d[i] != '\n')
					advanceToNextLine();
			}

		}

		f->completeLoad(true);
	}
	else if(data.size == 0)
	{
		f->completeLoad(true);
	}
	else
	{
		f->completeLoad(false);
	}
}
void FileManager::parseZipFile(shared_ptr<zipFile> f, fileContents data)
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
			while(readAs<unsigned int>(data.contents + position) != 0x04034b50 && position < data.size)
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




			if(localHeader.uncompressedSize != 0 && (localHeader.compression == 0 || localHeader.compression == 8)) //make sure size and compression are valid
			{
				fileContents d;
				d.size = localHeader.uncompressedSize;
				d.contents = new unsigned char[localHeader.uncompressedSize];
				if(localHeader.compression == 0)
				{
					memcpy(d.contents, data.contents + position, d.size);
				}
				else if(localHeader.compression == 8)
				{
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
				}
				f->files[name] = parseFile(name, d);
				delete[] d.contents;
			}
			else if(localHeader.uncompressedSize == 0)
			{
				fileContents d;
				d.contents = nullptr;
				d.size = 0;
				f->files[name] = parseFile(name, d);
			}
			position += localHeader.compressedSize;
		}
		f->completeLoad(true);
	}
	else if(data.size == 0)
	{
		f->completeLoad(true);
	}
	else
	{
		f->completeLoad(false);
	}
}
void FileManager::parseBmpFile(shared_ptr<textureFile> f, fileContents data)
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



	if(data.contents != nullptr && data.size > sizeof(header) + sizeof(infoHeader))
	{
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

		if(	infoHeader.width > 0 &&
			infoHeader.height > 0 &&
			(infoHeader.bitCount == 8 || infoHeader.bitCount == 24 || infoHeader.bitCount == 32)
			/*&& data.size >= position + ???? */)
		{
			f->width = infoHeader.width;
			f->height = infoHeader.height;

			if(infoHeader.bitCount == 8)		f->channels = 1;
			else if(infoHeader.bitCount == 24)	f->channels = 3;
			else if(infoHeader.bitCount == 32)	f->channels = 4;

			f->contents = new unsigned char[f->width * f->height * f->channels];

			int rowExtra = 3 - (f->width*f->channels - 1) % 4;

			for(unsigned long y = 0; y < f->height; y++)
			{
				for(unsigned long x = 0; x < f->width; x++)
				{
					for(char c = 0; c < f->channels; c++)
					{
						f->contents[f->channels * ((f->width) * y + x) + c] = readAs<unsigned char>(data.contents + position);
						position++;
					}
				}
				position += rowExtra;
			}
			f->completeLoad(true);
		}
		else
		{
			f->completeLoad(false);
		}
	}
	else
	{
		f->completeLoad(false);
	}
}
void FileManager::parseTgaFile(shared_ptr<textureFile> f, fileContents data)
{
	if(data.contents != nullptr)
	{
		f->width = readAs<short>(data.contents + 9);
		f->height = readAs<short>(data.contents + 11);
		char bpp = readAs<char>(data.contents + 13) / 8;

		f->channels = bpp / 8;

		if((f->width <= 0) || (f->height <= 0) || ((bpp != 24) && (bpp !=32)))
		{
			f->completeLoad(false);
			return;
		}

		f->contents = new unsigned char[f->width*f->height*f->channels];
		memcpy(f->contents, data.contents + 18, f->width*f->height*f->channels);

		for(int cswap = 0; cswap < (int)(f->width*f->height*f->channels); cswap += f->channels)
		{
			swap(f->contents[cswap], f->contents[cswap+2]);
		}

		f->completeLoad(true);
	}
	else
	{
		f->completeLoad(false);
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
void FileManager::parsePngFile(shared_ptr<textureFile> f, fileContents data)
{
	if(data.contents != nullptr)
	{
		png_uint_32		i,
						width,
						height,
						rowbytes;
		int				bit_depth,
						color_type,
						colorChannels;
		unsigned char*	image_data;
		png_bytep*		row_pointers;

		/* Open the PNG file. */
		//FILE *infile;
		//infile = fopen(filename.c_str(), "rb");

		if (!png_check_sig(data.contents, 8)) {
			//fclose(infile);
			f->completeLoad(false);
			return;
		}
		/*
		 * Set up the PNG structs
		 */
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) {
			//fclose(infile);
			f->completeLoad(false);
			return; /* out of memory */
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
			//fclose(infile);
			f->completeLoad(false);
			return; /* out of memory */
		}

		fileContents streamPtr = data;

		/*
		 * lets libpng know that we already checked the 8
		 * signature bytes, so it should not expect to find
		 * them at the current file pointer location
		 */
		streamPtr.contents += 8;
		streamPtr.size -= 8;
		png_set_sig_bytes(png_ptr, 8);


		png_set_read_fn(png_ptr, &streamPtr, pngStreamRead);

		png_infop end_ptr = png_create_info_struct(png_ptr);
		if (!end_ptr) {
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			//fclose(infile);
			f->completeLoad(false);
			return; /* out of memory */
		}

		/*
		 * block to handle libpng errors,
		 * then check whether the PNG file had a bKGD chunk
		 */
		if (setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
			//fclose(infile);
			f->completeLoad(false);
			return;
		}

		/*
		 * takes our file stream pointer (infile) and
		 * stores it in the png_ptr struct for later use.
		 */
		//png_init_io(png_ptr, infile);


		//png_set_sig_bytes(png_ptr, 8);

		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);


		if (color_type == PNG_COLOR_TYPE_PALETTE)											png_set_expand(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)								png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))								png_set_expand(png_ptr);
		if (bit_depth == 16)																png_set_strip_16(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)	png_set_gray_to_rgb(png_ptr);

		/* snipped out the color type code, see source pngLoad.c */
		/* Update the png info struct.*/
		png_read_update_info(png_ptr, info_ptr);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		colorChannels = (int)png_get_channels(png_ptr, info_ptr);

		image_data = new unsigned char[rowbytes*height];
		row_pointers = new png_bytep[height];

		for (i = 0;  i < height;  i++)
			row_pointers[i] = image_data + i*rowbytes;

		png_read_image(png_ptr, row_pointers);
		png_read_end(png_ptr, NULL);
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		//fclose(infile);


		//int format;
		//if(colorChannels == 1)		format = GL_LUMINANCE;
		//else if(colorChannels == 2)	format = GL_LUMINANCE_ALPHA;
		//else if(colorChannels == 3) format = GL_RGB;
		//else if(colorChannels == 4) format = GL_RGBA;

		f->channels = colorChannels;
		f->contents = image_data;
		f->height = height;
		f->width = width;

		delete[] row_pointers;

		//textureAsset* a = new textureAsset;
		//a->id = texV;
		//a->type = asset::TEXTURE;
		//a->width = width;
		//a->height = height;
		//a->bpp = colorChannels*8;
		//a->data = NULL;
		//assets[name] = a;


		f->completeLoad(true);
	}
	else
	{
		f->completeLoad(false);
	}
}
void FileManager::parseObjFile(shared_ptr<modelFile> f, fileContents data)
{
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

	auto safeStr = [](const char* c)->const char*
	{
		return c ? c : "";
	};

////////////////////variables///////////////////////////
	if(data.size == 0 || data.contents == nullptr)
	{
		f->completeLoad(false);
		return;
	}


	//unsigned int			numVertices=0,
	//						numTexcoords=0,
	//						numNormals=0,
	//						numFaces=0,
	unsigned int			numMtls=0;

	vector<Vec3f>			vertices;
	vector<Vec3f>			normals;
	vector<Vec2f>			texCoords;
	vector<face>			faces;

	modelFile::material*	mtls;

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
	
	//char* stringContents = new char[dataString.size()+1];
	//strcpy(stringContents, dataString.c_str());
	//char* stringContentsStart = stringContents;

	map<vertexIndices, unsigned int> indexMap;

	///////
	map<string,modelFile::material>	mtl_map;
	//string file(filename);
	//int i=file.find_last_of("/");
	//if(i==string::npos)
	//	file.assign("");
	//else
	//	file=file.substr(0,i+1);
	string mtlFilename;

	unsigned int position;

	char* token;
	char line[256];

	//auto gLine = [&stringContents, &line]()->bool
	//{
	//	line = stringContents;
	//	if(*stringContents == '\0')
	//		return false;
	//
	//	while(*stringContents != '\0')
	//	{
	//		if(*stringContents == '\n')
	//		{
	//			*stringContents = 0;
	//			stringContents++;
	//			return true;
	//		}
	//		stringContents++;
	//	}
	//	return true;
	//};

	//////
	//while(dataStream.getline(line,256))
	//{
	//	if((token = strtok(line, " \t")) != nullptr)
	//	{
	//		if(strcmp(token, "v") == 0) 		numVertices++;
	//		if(strcmp(token, "vt") == 0) 		numTexcoords++;
	//		if(strcmp(token, "f") == 0) 		numFaces++;
	//		if(strcmp(token, "vn") == 0)		numNormals++;
	//		if(strcmp(token, "mtllib") == 0)	mtlFilename = strtok(NULL, "\r\n");
	//	}
	//}
	//const char* tmpString = strstr(stringContents, "mtllib ");
	//if(tmpString)
	//{
	//	mtlFilename = string(tmpString+7,tmpString+7+strcspn(tmpString+7, "\n"));
	//}
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

		auto mtlFile = loadTextFile(directory(f->filename) + mtlFilename);

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
					mMtl.tex.reset();
					mMtl.normalMap.reset();
					mMtl.specularMap.reset();
					mtlName = safeStr(strtok(NULL, " #\n"));
					mMtl.diffuse = white;
					mMtl.specular = black;
					mMtl.hardness = 40.0;
					state = READING_MTL;
				}
				else if(strcmp(token, "map_Kd") == 0 && state == READING_MTL)
				{
					bool tile;
					Vec2f scale;
					string filename = directory(f->filename) + parseTexLine(strtok(NULL, "#\n"), scale, tile);
					mMtl.tex = loadTextureFile(filename);
				}
				else if(strcmp(token, "map_Ns") == 0 && state == READING_MTL)
				{
					bool tile;
					Vec2f scale;
					string filename = directory(f->filename) + parseTexLine(strtok(NULL, "#\n"), scale, tile);
					mMtl.specularMap = loadTextureFile(filename);
				}
				else if(strcmp(token, "map_bump") == 0 && state == READING_MTL)
				{
					bool tile;
					Vec2f scale;
					string filename = directory(f->filename) + parseTexLine(strtok(NULL, "#\n"), scale, tile);
					mMtl.normalMap = loadTextureFile(filename);
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

	//vertices	= new Vec3f[numVertices];
	//texCoords	= new Vec2f[numTexcoords];
	//faces		= new face[numFaces];
	//normals		= new Vec3f[numNormals];
	mtls		= new modelFile::material[mtl_map.size()];


	int i=0;
	for(auto itt=mtl_map.begin();itt!=mtl_map.end();itt++)
	{
		mtls[numMtls++]=itt->second;
		mtlNames[itt->first] = i++;
	}

	//numVertices=0;
	//numTexcoords=0;
	//numNormals=0;
	//numFaces=0;

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
			vertices.push_back(tmpVec3);
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
	if(vertices.size() >= 1)
	{
		minx = maxx = vertices[0].x;
		miny = maxy = vertices[0].y;
		minz = maxz = vertices[0].z;
		for(i=1;i<vertices.size();i++)
		{
			if(vertices[i].x<minx) minx=vertices[i].x;
			if(vertices[i].y<miny) miny=vertices[i].y;
			if(vertices[i].z<minz) minz=vertices[i].z;
			if(vertices[i].x>maxx) maxx=vertices[i].x;
			if(vertices[i].y>maxy) maxy=vertices[i].y;
			if(vertices[i].z>maxz) maxz=vertices[i].z;
		}
		f->boundingSphere.center = Vec3f((minx+maxx)/2,((miny+maxy)/2),(minz+maxz)/2);
		radiusSquared = center.distanceSquared(vertices[0]);
		for(i=1;i<vertices.size();i++)
		{
			if(center.distanceSquared(vertices[i]) > radiusSquared)
			{
				radiusSquared = center.distanceSquared(vertices[i]);
			}
		}
		f->boundingSphere.radius = sqrt(radiusSquared);
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	map<vertexIndices, unsigned int>::iterator vertexIndicesItt;
	float inv;
	normalMappedVertex3D tmpVertex;
	Vec3f faceNormal(0,1,0), avgFaceNormal, faceTangent(0,0,1);
	for(int i=0; i < faces.size(); i++)
	{
		if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0)
		{
			faceNormal = (vertices[faces[i].v[1]-1]-vertices[faces[i].v[0]-1]).cross(vertices[faces[i].v[2]-1]-vertices[faces[i].v[0]-1]);
		}
		if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0 && faces[i].t[0] != 0 && faces[i].t[1] != 0 && faces[i].t[2] != 0)
		{
			inv = (texCoords[faces[i].t[0]-1].x * texCoords[faces[i].t[1]-1].y - texCoords[faces[i].t[1]-1].x * texCoords[faces[i].t[0]-1].y);
			faceTangent = abs(inv) < 0.001 ? Vec3f(1,0,0).cross(faceNormal) : (vertices[faces[i].v[0]-1] * texCoords[faces[i].t[1]-1].y - vertices[faces[i].v[1]-1] * texCoords[faces[i].t[0]-1].y) / inv;
		}

		if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0 && faces[i].n[0] != 0 && faces[i].n[1] != 0 && faces[i].n[2] != 0)
		{
			avgFaceNormal = normals[faces[i].n[0]-1] + normals[faces[i].n[1]-1] + normals[faces[i].n[2]-1];
			if(faceNormal.dot(faceNormal) < 0.0)// correct triangle winding order
			{
				swap(faces[i].v[0], faces[i].v[1]);
				swap(faces[i].t[0], faces[i].t[1]);
				swap(faces[i].n[0], faces[i].n[1]);
			}
		}
		for(int j = 0; j < 3; j++)
		{
			vertexIndicesItt = indexMap.find(vertexIndices(faces[i].v[j], faces[i].t[j], faces[i].n[j]));
			if(vertexIndicesItt == indexMap.end())
			{
				indexMap[vertexIndices(faces[i].v[j], faces[i].t[j], faces[i].n[j])] = f->vertices.size();
				faces[i].combinedVertices[j] = f->vertices.size();

				tmpVertex.position = (faces[i].v[j] != 0) ? vertices[faces[i].v[j]-1] : Vec3f();
				tmpVertex.normal = (faces[i].n[j] != 0) ? normals[faces[i].n[j]-1] : faceNormal;
				tmpVertex.UV = (faces[i].t[j] != 0) ? texCoords[faces[i].t[j]-1] : Vec2f();
				tmpVertex.tangent = Vec3f(0,0,0);
				f->vertices.push_back(tmpVertex);
			}
			else
			{
				faces[i].combinedVertices[j] = vertexIndicesItt->second;
			}
		}
		f->vertices[faces[i].combinedVertices[0]].tangent += faceTangent;
		f->vertices[faces[i].combinedVertices[1]].tangent += faceTangent;
		f->vertices[faces[i].combinedVertices[2]].tangent += faceTangent;
	}


	//unsigned int* indexBuffer = new unsigned int[totalFaces*3];
	//unsigned int lNum=0, vNum = 0;
	for(int m=0; m<numMtls; m++)
	{
		modelFile::material mat;
		//if(m >= 0)
		//{
			mat.diffuse = mtls[m].diffuse;
			mat.specular = mtls[m].specular;
			mat.hardness = mtls[m].hardness;
			mat.specularMap = mtls[m].specularMap;
			mat.normalMap = mtls[m].normalMap;
			mat.tex = mtls[m].tex;
		//}
		//else
		//{
		//	mat.diffuse = white;
		//	mat.specular = white;
		//	mat.hardness = 40.0;
		//}

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
			f->materials.push_back(mat);
		}
	}

	int n=0;
	modelFile::material unnamedMat;
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
		f->materials.push_back(unnamedMat);
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	f->completeLoad(true);
}
FileManager::fileContents FileManager::loadFileContents(string filename)
{
	fileContents f;

	try{
		std::ifstream fin(filename,std::ios::in|std::ios::ate|std::ios::binary);
		if(fin.is_open())
		{
			f.size = (unsigned long)fin.tellg();
			f.contents = new unsigned char[f.size];
			memset(f.contents,0,f.size);

			fin.seekg(0, std::ios::beg);
			fin.read((char*)f.contents, f.size);
			fin.close();
		}
	}catch(...){}
	return f;
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

 			if(f->type == BINARY_FILE)
			{
				//parseBinaryFile(dynamic_pointer_cast<binaryFile>(f),data);     //no need to 'parse' a binary file
				dynamic_pointer_cast<binaryFile>(f)->contents = data.contents;
				dynamic_pointer_cast<binaryFile>(f)->size = data.size;
				f->completeLoad(true);
				continue;
			}
			else if(f->type == TEXT_FILE)	parseTextFile(dynamic_pointer_cast<textFile>(f),data);
			else if(f->type == INI_FILE)	parseIniFile(dynamic_pointer_cast<iniFile>(f),data);
			else if(f->type == ZIP_FILE)	parseZipFile(dynamic_pointer_cast<zipFile>(f),data);
			else if(f->type == TEXTURE_FILE && f->format == BMP)	parseBmpFile(dynamic_pointer_cast<textureFile>(f),data);
			else if(f->type == TEXTURE_FILE && f->format == TGA)	parseTgaFile(dynamic_pointer_cast<textureFile>(f),data);
			else if(f->type == TEXTURE_FILE && f->format == PNG)	parsePngFile(dynamic_pointer_cast<textureFile>(f),data);
			else if(f->type == MODEL_FILE && f->format == OBJ)		parseObjFile(dynamic_pointer_cast<modelFile>(f),data);
			else debugBreak();
			delete[] data.contents;
		}
		else if(!writeEmpty)
		{
			shared_ptr<file> f = fileWriteQueue.front();
			fileWriteQueue.pop();
			fileQueueMutex.unlock();

			writeFileContents(f->filename, serializeFile(f));
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																																						 //
//																Write Files																				 //
//																																						 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FileManager::writeFileContents(string filename, fileContents contents)
{
	std::unique_ptr<unsigned char[]> p(contents.contents); //makes sure that memory get deleted

	try{
		std::ofstream fout(filename,std::ios::out|std::ios::binary|std::ios::trunc);
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
bool FileManager::writeFileContents(string filename, shared_ptr<file> f, bool async)
{
	if(async)
	{
		f->filename = filename;
		fileQueueMutex.lock();
		fileWriteQueue.push(f);
		fileQueueMutex.unlock();
		return true;
	}
	else
	{
		return writeFileContents(filename, serializeFile(f));
	}
}
bool FileManager::writeBinaryFile(shared_ptr<binaryFile> f, bool asinc)
{
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writeTextFile(shared_ptr<textFile> f, bool asinc)
{
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writeIniFile(shared_ptr<iniFile> f, bool asinc)
{
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writeZipFile(shared_ptr<zipFile> f, bool asinc)
{
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writeBmpFile(shared_ptr<textureFile> f, bool asinc)
{
	f->format = BMP;
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writeTgaFile(shared_ptr<textureFile> f, bool asinc)
{
	f->format = TGA;
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writePngFile(shared_ptr<textureFile> f, bool asinc)
{
	f->format = PNG;
	return writeFileContents(f->filename, f, asinc);
}

FileManager::fileContents FileManager::serializeBinaryFile(shared_ptr<binaryFile> f)
{
	fileContents c;
	c.contents = f->contents;
	c.size = f->size;
	return c;
}
FileManager::fileContents FileManager::serializeTextFile(shared_ptr<textFile> f)
{
	fileContents c;
	c.size = f->contents.size();
	c.contents = new unsigned char[c.size];
	memcpy(c.contents, f->contents.c_str(), c.size);
	return c;
}
FileManager::fileContents FileManager::serializeIniFile(shared_ptr<iniFile> f)
{
	string s;
	for(auto section = f->bindings.begin(); section != f->bindings.end(); section++)
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
FileManager::fileContents FileManager::serializeZipFile(shared_ptr<zipFile> f)	//see: http://www.pkware.com/documents/casestudies/APPNOTE.TXT
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


	for(auto file = f->files.begin(); file != f->files.end(); file++)
	{
		fContents = serializeFile(file->second);

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
FileManager::fileContents FileManager::serializeBmpFile(shared_ptr<textureFile> f)
{
	unsigned int width = f->width * f->channels;
	unsigned int pWidth = width + 3 - ((width - 1) % 4);//padded width

	fileContents c;
	c.size = 54 + pWidth * f->height;
	c.contents = new unsigned char[c.size];
	memset(c.contents, 0, c.size);

	writeAs<unsigned short>(	c.contents + 0,  0x4D42);				//type
	writeAs<unsigned long>(		c.contents + 2,  c.size);				//size
	writeAs<unsigned short>(	c.contents + 6,  0);					//reserved1
	writeAs<unsigned short>(	c.contents + 8,  0);					//reserved2
	writeAs<unsigned long>(		c.contents + 10, 54);					//offBits
	writeAs<unsigned long>(		c.contents + 14, 40);					//size of infoHeader
	writeAs<long>(				c.contents + 18, f->width);				//width
	writeAs<long>(				c.contents + 22, f->height);			//height
	writeAs<unsigned short>(	c.contents + 26, 1);					//planes
	writeAs<unsigned short>(	c.contents + 28, f->channels * 8);		//bit count
	writeAs<unsigned long>(		c.contents + 30, 0);					//compression
	writeAs<unsigned long>(		c.contents + 34, pWidth * f->height);	//size of image data
	writeAs<long>(				c.contents + 38, 3000);					//x pixels per meter
	writeAs<long>(				c.contents + 42, 3000);					//y pixels per meter
	writeAs<unsigned long>(		c.contents + 46, 0);					//colors used (or zero)
	writeAs<unsigned long>(		c.contents + 50, 0);					//important colors used (or zero)

	unsigned char tmpChar;
	int off1, off2;

	for(unsigned int y = 0; y < f->height; y++)
	{
		memcpy(c.contents+54+pWidth*y, f->contents+pWidth*y, width);

		for(unsigned int x = 0; x < f->width; x++)
		{
			off1 = 54 + pWidth * y + x * f->channels + 0;
			off2 = off1 + 2;

			tmpChar = c.contents[off1];
			c.contents[off1] = c.contents[off2];
			c.contents[off2] = tmpChar;
		}
	}
	return c;
}
FileManager::fileContents FileManager::serializeTgaFile(shared_ptr<textureFile> f)
{
	fileContents c;
	c.size = 18 + f->width * f->height * f->channels;
	c.contents = new unsigned char[c.size];
	memset(c.contents, 0, c.size);

	writeAs<unsigned char>(		c.contents + 0,  0);					//size of id field
	writeAs<unsigned char>(		c.contents + 1,  0);					//type of color map
	writeAs<unsigned char>(		c.contents + 2,  2);					//type of image
	writeAs<unsigned short>(	c.contents + 3,  0);					//first color in palette
	writeAs<unsigned short>(	c.contents + 5,  0);					//number of colors in palette
	writeAs<unsigned char>(		c.contents + 7,  0);					//number of bits per entry
	writeAs<unsigned short>(	c.contents + 8,  0);					//x origin
	writeAs<unsigned short>(	c.contents + 10, 0);					//y origin
	writeAs<unsigned short>(	c.contents + 12, f->width);				//width
	writeAs<unsigned short>(	c.contents + 14, f->height);			//height
	writeAs<unsigned char>(		c.contents + 16, f->channels * 8);		//bits per pixel
	writeAs<unsigned char>(		c.contents + 17, 0);					//descriptor

	for(unsigned int y = 0; y < f->height; y++)
	{
		for(unsigned int x = 0; x < f->width; x++)
		{
			c.contents[18 + f->width * f->channels * y + x * f->channels + 0] = f->contents[f->width * f->channels * y + x * f->channels + 2];
			c.contents[18 + f->width * f->channels * y + x * f->channels + 1] = f->contents[f->width * f->channels * y + x * f->channels + 1];
			c.contents[18 + f->width * f->channels * y + x * f->channels + 2] = f->contents[f->width * f->channels * y + x * f->channels + 0];
		}
	}
	return c;
}
void pngStreamWrite(png_structp png_ptr, png_bytep data, png_size_t length)
{
	vector<unsigned char>& vec = *(vector<unsigned char>*)png_get_io_ptr(png_ptr);

	vec.reserve(vec.size() + length);
	vec.insert(vec.end(), (unsigned char*)data, (unsigned char*)data + length);
};
void pngStreamFlush(png_structp png_ptr){}
FileManager::fileContents FileManager::serializePngFile(shared_ptr<textureFile> f)
{
	vector<unsigned char> data;

	int colorType;
	if(f->channels == 1)
		colorType = PNG_COLOR_TYPE_GRAY;
	else if(f->channels == 3)
		colorType = PNG_COLOR_TYPE_RGB;
	else if(f->channels == 4)
		colorType = PNG_COLOR_TYPE_RGB_ALPHA;
	else
	{
		debugBreak();
		return fileContents();
	}

	png_bytepp rows = new unsigned char*[f->height];
	for(unsigned int i=0;i<f->height;i++) rows[i] = f->contents + f->width * f->channels * (f->height - i - 1);

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,	NULL, NULL);
	if (!png_ptr)
	{
		delete[] rows;
		return fileContents();
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr || setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		delete[] rows;
		return fileContents();
	}

	png_set_write_fn(png_ptr, (void*)&data, pngStreamWrite, pngStreamFlush);

	png_set_IHDR(png_ptr, info_ptr, f->width, f->height, 8, colorType , PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_set_rows(png_ptr,info_ptr,rows);

	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	if (setjmp(png_jmpbuf(png_ptr))) debugBreak();
	png_destroy_write_struct(&png_ptr, &info_ptr);

	delete[] rows;

	fileContents c;
	c.size = data.size();
	c.contents = new unsigned char[c.size];
	memcpy(c.contents, &data[0], c.size);
	return c;
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
}