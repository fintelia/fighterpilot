
#include "debugBreak.h"

#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>

#include <boost/crc.hpp>

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
	#include <shlwapi.h>
	#include <ShlObj.h>
	#pragma comment(lib,"shlwapi.lib")
	#define WINDOWS
#else
	#error operating system not currently supported by fileManager
#endif

#include "zlib/zlib.h"
#include "png/png.h"

using namespace std;

#include "definitions.h"
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
#endif
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
shared_ptr<FileManager::textureFile> FileManager::loadTextureFile(string filename, bool asinc)
{
	string ext = extension(filename);
	if(ext == ".bmp")	return loadBmpFile(filename, asinc);
	if(ext == ".tga")	return loadPngFile(filename, asinc);
	if(ext == ".png")	return loadPngFile(filename, asinc);

	return shared_ptr<FileManager::textureFile>();
}
shared_ptr<FileManager::textureFile> FileManager::loadBmpFile(string filename, bool asinc)
{
	std::shared_ptr<textureFile> f(new bmpFile(filename));

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
	std::shared_ptr<textureFile> f(new tgaFile(filename));

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
	std::shared_ptr<textureFile> f(new pngFile(filename));

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
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
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
			#ifdef _DEBUG
				else __debugbreak();
			#endif
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
			sleep(15);
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
	unique_ptr<unsigned char[]> p(contents.contents); //makes sure that memory get deleted

	try{
		ofstream fout(filename,ios::out|ios::binary|ios::trunc);
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
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writeTgaFile(shared_ptr<textureFile> f, bool asinc)
{
	return writeFileContents(f->filename, f, asinc);
}
bool FileManager::writePngFile(shared_ptr<textureFile> f, bool asinc)
{
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

	for(unsigned int y = 0; y < f->height; y++)
	{
		for(unsigned int x = 0; x < f->width; x++)
		{
			c.contents[54 + pWidth * y + x * f->channels + 0] = f->contents[width * y + x * f->channels + 2];
			c.contents[54 + pWidth * y + x * f->channels + 1] = f->contents[width * y + x * f->channels + 1];
			c.contents[54 + pWidth * y + x * f->channels + 2] = f->contents[width * y + x * f->channels + 0];
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