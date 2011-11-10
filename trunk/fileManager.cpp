
#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
	#define WINDOWS
#else
	#error operating system not currently supported by fileManager
#endif

#include "zlib/zlib.h"

using namespace std;

#include "enums.h"
#include "fileManager.h"
/////////////////////////////////THREADING///////////////////////////////////////////////////
//template<class T>
//struct threadData{
//	void( *func )( T );
//	T val;
//};
//
//template <class T>
//void newThreadStart(void* t)
//{
//	threadData<T>* m = (threadData<T>*)t;
//	((threadData<T>*)t)->func( ((threadData<T>*)t)->val );
//	delete t;
//}
//
//template <class T> 
//void startInNewThread(void(*func)( T ), T val)
//{
//	threadData<T>* t = new threadData<T>();
//	t->func = func;
//	t->val = val;
//	_beginthread(newThreadStart<T>,0,(void*)t);
//}

///////////////////////////////////////////////////////////////////////////////////////////
#ifdef WINDOWS
FileManager::FileManager()
{
	_beginthread(startWorkerThread,0,this);
}
#endif
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
#ifdef WINDOWS
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

		if(!(d & FILE_ATTRIBUTE_DIRECTORY))
			return CreateDirectoryA(directory.c_str(), NULL) != 0;
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
#endif
//
//void FileManager::lTextFile(std::shared_ptr<textFile> f)
//{
//	try{
//		ifstream fin(f->filename,ios::in|ios::ate);
//		if(fin.is_open())
//		{
//			int size = fin.tellg();
//			std::unique_ptr<char> data(new char[size+1]);
//			memset(data.get(),0,size+1);
//			fin.seekg (0, ios::beg);
//			fin.read (data.get(), size);
//			fin.close();
//
//			f->contents = data.get();
//
//			f->completeLoad(true);
//			return;
//		}
//	}catch(...){}
//	f->completeLoad(false);
//}
//void FileManager::lBinaryFile(std::shared_ptr<binaryFile> f)
//{
//	try{
//		ifstream fin(f->filename,ios::in|ios::ate|ios::binary);
//		if(fin.is_open())
//		{
//			int size = fin.tellg();
//			unsigned char* data = new unsigned char[size];
//			memset(data,0,size);
//			f->contents = data;
//			f->size = size;
//
//			fin.seekg (0, ios::beg);
//			fin.read ((char*)data, size);
//			fin.close();
//
//			f->completeLoad(true);
//			return;
//		}
//	}catch(...){}
//	f->completeLoad(false);
//}
//void FileManager::lIniFile(std::shared_ptr<iniFile> f)
//{
//	try{
//		ifstream fin(f->filename,ios::in|ios::ate);
//		if(fin.is_open())
//		{
//			int size = fin.tellg();
//			std::unique_ptr<char> data(new char[size]);
//			memset(data.get(),0,size);
//			fin.seekg (0, ios::beg);
//			fin.read (data.get(), size);
//			fin.close();
//
//			string section="", key, value;
//
//			char* d = data.get();
//
//			int i=0;
//			auto advanceToNextLine = [&i, &size,&d]()
//			{
//				while(i < size && d[i] != '\n')
//					++i;
//				++i;
//			};
//
//
//			while(i < size)
//			{
//				key = "";
//				value = "";
//				if(d[i] == '[')
//				{
//					++i;
//					while(i < size && d[i] != ']' && d[i] != '\n')
//					{
//						section += d[i];
//						++i;
//					}
//				}
//				else
//				{
//					if(d[i] == ';' || d[i] == '#'){ advanceToNextLine(); continue;}
//
//					while(i < size && d[i] != '\n' && d[i] != '=' && d[i] != ';' && d[i] != '#')
//					{
//						key += d[i];
//						++i;
//					}
//
//					if(d[i] == ';' || d[i] == '#'){ advanceToNextLine();continue;}
//					else if(d[i] == '\n'){++i; continue;}
//					else if(i >= size) break;
//					else if(d[i] == '=') i++;
//
//					while(i < size && d[i] != '\n' && d[i] != ';' && d[i] != '#')
//					{
//						value += d[i];
//						++i;
//					}
//
//					f->bindings[section][key] = value;
//					
//					if(d[i] != '\n')
//						advanceToNextLine();
//				}
//
//			}
//
//			f->completeLoad(true);
//			return;
//		}
//	}catch(...){}
//	f->completeLoad(false);
//}
shared_ptr<FileManager::textFile> FileManager::loadTextFile(string filename, bool asinc)
{
	std::shared_ptr<textFile> f(new textFile(filename));

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
	std::shared_ptr<binaryFile> f(new binaryFile(filename));

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
	std::shared_ptr<iniFile> f(new iniFile(filename));

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
	std::shared_ptr<zipFile> f(new zipFile(filename));

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
		shared_ptr<zipFile> f(new zipFile(filename));
		parseZipFile(f, data);
		return f;
	}
	else
	{
		shared_ptr<binaryFile> f(new binaryFile(filename));
		parseBinaryFile(f, data);
		return f;
	}

}
void FileManager::parseBinaryFile(std::shared_ptr<binaryFile> f, fileContents data)
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
void FileManager::parseTextFile(std::shared_ptr<textFile> f, fileContents data)
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
void FileManager::parseIniFile(std::shared_ptr<iniFile> f, fileContents data)
{
	if(data.contents != nullptr && data.size != 0)
	{
		string section="", key, value;

		char* d = (char*)data.contents;
		unsigned int size = data.size;

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
FileManager::fileContents FileManager::loadFileContents(string filename)
{
	fileContents f;

	try{
		ifstream fin(filename,ios::in|ios::ate|ios::binary);
		if(fin.is_open())
		{
			f.size = fin.tellg();
			f.contents = new unsigned char[f.size];
			memset(f.contents,0,f.size);

			fin.seekg(0, ios::beg);
			fin.read((char*)f.contents, f.size);
			fin.close();
		}
	}catch(...){}
	return f;
}
void FileManager::workerThread()
{
	bool empty;
	while(true)
	{
		fileQueueMutex.lock();
		empty = fileQueue.empty();
		
		if(!empty)
		{
			shared_ptr<file> f = fileQueue.front();
			fileQueue.pop();
			fileQueueMutex.unlock();

			fileContents data = loadFileContents(f->filename);

 			if(f->type == file::BINARY)
			{
				//parseBinaryFile(dynamic_pointer_cast<binaryFile>(f),data);     //no need to 'parse' a binary file
				dynamic_pointer_cast<binaryFile>(f)->contents = data.contents;
				dynamic_pointer_cast<binaryFile>(f)->size = data.size;
				f->completeLoad(true);
				continue;
			}
			else if(f->type == file::TEXT)	parseTextFile(dynamic_pointer_cast<textFile>(f),data);
			else if(f->type == file::INI)	parseIniFile(dynamic_pointer_cast<iniFile>(f),data);
			else if(f->type == file::ZIP)	parseZipFile(dynamic_pointer_cast<zipFile>(f),data);
#ifdef _DEBUG
			else __debugbreak();
#endif
			delete[] data.contents;
		}
		else
		{
			fileQueueMutex.unlock();
			sleep(15);
		}
	}
}