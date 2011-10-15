
#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <set>

#ifdef _WIN32
	#include <Windows.h>
	#define WINDOWS
#else
	static_assert(0, "operating system not currently supported by fileManager");
#endif

using namespace std;

#include "fileManager.h"

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

void FileManager::lTextFile(std::shared_ptr<file> f)
{
	try{
		ifstream fin(f->filename,ios::in|ios::ate);
		if(fin.is_open())
		{
			int size = fin.tellg();
			std::shared_ptr<char> data(new char[size+1]);
			memset(data.get(),0,size+1);
			f->fileContents = data;
			fin.seekg (0, ios::beg);
			fin.read (data.get(), size);
			fin.close();

			f->loaded = true;
			return;
		}
	}catch(...){}
	f->loadFailed = true;
}
void FileManager::lBinaryFile(std::shared_ptr<file> f)
{
	try{
		ifstream fin(f->filename,ios::in|ios::ate|ios::binary);
		if(fin.is_open())
		{
			int size = fin.tellg();
			std::shared_ptr<char> data(new char[size]);
			memset(data.get(),0,size);
			f->fileContents = data;
			fin.seekg (0, ios::beg);
			fin.read (data.get(), size);
			fin.close();

			f->loaded = true;
			return;
		}
	}catch(...){}
	f->loadFailed = true;
}
std::shared_ptr<FileManager::file> FileManager::loadTextFile(string filename, bool asinc)
{
	std::shared_ptr<file> f(new file());
	f->loaded = false;
	f->loadFailed = false;
	f->filename = filename;
	f->type = file::TEXT;

	if(asinc)
	{
		lTextFile(f);
		return f;
	}
	else
	{
		lTextFile(f);
		return f;
	}
}
std::shared_ptr<FileManager::file> FileManager::loadBinaryFile(string filename, bool asinc)
{
	std::shared_ptr<file> f(new file());
	f->loaded = false;
	f->loadFailed = false;
	f->filename = filename;
	f->type = file::BINARY;

	if(asinc)
	{
		lBinaryFile(f);
		return f;
	}
	else
	{
		lBinaryFile(f);
		return f;
	}
}