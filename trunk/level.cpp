
#include "game.h"
#include "GL/glee.h"
#include <GL/glu.h>
#include "png/png.h"
#include <Windows.h>
//bool LevelFile::load(string filename)
//{
//	ifstream fin(filename, ios::binary);
//	if(!fin.is_open())
//		return false;
//
//	fin.read((char*)&header,sizeof(Header));
//	if(header.magicNumber == 0x454c49465f4c564c && header.version == 1)
//	{
//		if(info != NULL) delete info;
//		if(objects != NULL) delete objects;
//		if(heights != NULL) delete heights;
//
//		info = new Info;
//		fin.read((char*)info,sizeof(Info));
//
//		objects = new Object[info->numObjects];
//		fin.read((char*)objects,info->numObjects*sizeof(Object));
//
//		regions = new Region[info->numRegions];
//		fin.read((char*)regions,info->numRegions*sizeof(Region));
//
//		heights = new float[info->mapResolution.x*info->mapResolution.y];
//		fin.read((char*)heights,info->mapResolution.x*info->mapResolution.y*sizeof(float));
//		fin.close();
//
//		return true;
//	}
//	else
//	{
//		fin.close();
//		return false;
//	}
//}
//bool LevelFile::save(string filename)
//{
//	ofstream fout(filename, ios::binary);
//	fout.write((char*)&header,sizeof(Header));
//	if(header.version == 1 && info!=NULL && heights!=NULL && (objects!=NULL || info->numObjects==0))
//	{
//		fout.write((char*)info,sizeof(Info));
//		fout.write((char*)objects,info->numObjects*sizeof(Object));
//		fout.write((char*)heights,info->mapResolution.x*info->mapResolution.y*sizeof(float));
//	}
//	fout.close();
//	return true;
//}
LevelFile::Object::Object():type(F22), team(NEUTRAL), controlType(PLAYER_HUMAN), startloc(), startRot()
{

}
//bool LevelFile::savePNG(string filename)
//{
//	int tWidth = 32;
//	int tHeight = 32;
//	int size = (3*tWidth+tWidth%4)*sh + 3*tWidth*tHeight%4;
//
//	FILE *fp;
//	if((fp = fopen((filename).c_str(), "wb")) != nullptr)
//	{
//		unsigned char* colors = new unsigned char[size];
//		memset(colors,255,size);
//		png_bytepp rows = new unsigned char*[tHeight];
//		for(int i=0;i<tHeight;i++) rows[i] = colors +  (3*tWidth+tWidth%4) * (tHeight-i);
//
//		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,	NULL, NULL);
//		if (!png_ptr)
//		{
//			fclose(fp);
//			delete[] colors;
//			return false;
//		}
//		png_infop info_ptr = png_create_info_struct(png_ptr);
//		if (!info_ptr || setjmp(png_jmpbuf(png_ptr)))
//		{
//			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
//			fclose(fp);
//			delete[] colors;
//			return false;
//		}
//		png_init_io(png_ptr, fp);
//		png_set_IHDR(png_ptr, info_ptr, tWidth, tHeight, 8, PNG_COLOR_TYPE_RGB , PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//		png_set_rows(png_ptr,info_ptr,rows);
//
//		//////////////////////////////////////////WRITE SPECIAL CHUNKS/////////////////////////////////////////////
//		png_unknown_chunk chunks[2];
//		float minHeight = 0.0;
//		float maxHeight = 1.0;
//		unsigned short* sHeights = NULL;
//
//		auto pngUnknownChunk = [](png_unknown_chunk &chunk, string name, void* data, unsigned int size)
//		{
//			if(name.size() < 4)
//				return;
//
//			chunk.name[0] = name[0];
//			chunk.name[1] = name[1];
//			chunk.name[2] = name[2];
//			chunk.name[3] = name[3];
//			chunk.name[4] = 0;
//
//			chunk.data = (png_byte*)data;
//			chunk.size = size;
//
//			chunk.location = 0;//unused
//		};
//
//		if(info->mapResolution.x != 0 && info->mapResolution.y != 0)
//		{
//			sHeights = new unsigned short[info->mapResolution.x*info->mapResolution.y];
//			minHeight = maxHeight = heights[0];
//			for(int i=0; i<info->mapResolution.x*info->mapResolution.y;i++)
//			{
//				if(minHeight > heights[i]) minHeight = heights[i];
//				if(maxHeight < heights[i]) maxHeight = heights[i];
//			}
//			for(int i=0; i<info->mapResolution.x*info->mapResolution.y;i++)
//			{
//				sHeights[i] = (heights[i] - minHeight) / (maxHeight - minHeight) * UCHAR_MAX;
//			}
//			pngUnknownChunk(chunks[0], "mhTs", sHeights, info->mapResolution.x*info->mapResolution.y*sizeof(unsigned short));
//		}
//		pngUnknownChunk(chunks[1], "obJs", objects, info->numObjects*sizeof(Object));
//
//		png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_ALWAYS, NULL, 0);
//		png_set_unknown_chunks(png_ptr,info_ptr,chunks,2);
//		png_set_unknown_chunk_location(png_ptr, info_ptr, 0, 0x02);
//		png_set_unknown_chunk_location(png_ptr, info_ptr, 1, 0x02);
//
//
//
//		png_text textFields[9];
//		string	title = "untitled",
//				nextLevel = "",
//				shaderType,
//				resX = lexical_cast<string>(info->mapResolution.x),
//				resY =lexical_cast<string>(info->mapResolution.y),
//				sizeX = lexical_cast<string>(info->mapSize.x),
//				sizeY = lexical_cast<string>(info->mapSize.y),
//				minH = lexical_cast<string>(minHeight),
//				maxH = lexical_cast<string>(maxHeight);
//
//		if(info->shaderType == SHADER_ISLAND) shaderType = "island";
//		if(info->shaderType == SHADER_GRASS) shaderType = "grass";
//		if(info->shaderType == SHADER_SNOW) shaderType = "snow";
//		if(info->shaderType == SHADER_OCEAN) shaderType = "ocean";
//
//		auto pngTextField = [] (png_text* textField, const char* key, const char* text)
//		{
//			textField->compression = PNG_TEXT_COMPRESSION_NONE;
//			textField->key = (png_charp)key;
//			textField->text_length = strlen(text);
//			textField->text = (png_charp)text;
//
//			textField->itxt_length = 0;
//			textField->lang = 0;
//			textField->lang_key = 0;
//		};
//
//		pngTextField(&textFields[0],"Title",title.c_str());
//		pngTextField(&textFields[1],"Next Level",nextLevel.c_str());
//		pngTextField(&textFields[2],"Shader Type",shaderType.c_str());
//		pngTextField(&textFields[3],"Map X Resolution",resX.c_str());
//		pngTextField(&textFields[4],"Map Y Resolution",resY.c_str());
//		pngTextField(&textFields[5],"Map X Size",sizeX.c_str());
//		pngTextField(&textFields[6],"Map Y Size",sizeY.c_str());
//		pngTextField(&textFields[7],"Minimum Height",minH.c_str());
//		pngTextField(&textFields[8],"Maximum Height",maxH.c_str());
//		png_set_text(png_ptr,info_ptr,textFields,9);
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////
//		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
//
//		if (setjmp(png_jmpbuf(png_ptr))) debugBreak();
//		png_destroy_write_struct(&png_ptr, &info_ptr);
//
//		fclose(fp);
//		delete[] rows;
//		delete[] colors;
//		if(sHeights != NULL)
//			delete[] sHeights;
//	}
//	return true;
//}
//bool LevelFile::loadPNG(string filename)
//{
//	png_uint_32		i,
//					width,
//					height,
//					rowbytes;
//	int				bit_depth,
//					color_type;
//	unsigned char*	image_data;
//	png_bytep*		row_pointers;
//
//	/* Open the PNG file. */
//	FILE *infile;
//	infile = fopen(filename.c_str(), "rb");
//
//	if (!infile) {
//		return false;
//	}
//
//	unsigned char sig[8];
//	/* Check for the 8-byte signature */
//	fread(sig, 1, 8, infile);
//	if (!png_check_sig((unsigned char *) sig, 8)) {
//		fclose(infile);
//		return false;
//	}
//	/*
//	 * Set up the PNG structs
//	 */
//	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//	if (!png_ptr) {
//		fclose(infile);
//		return false; /* out of memory */
//	}
//
//	png_infop info_ptr = png_create_info_struct(png_ptr);
//	if (!info_ptr) {
//		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
//		fclose(infile);
//		return false; /* out of memory */
//	}
//
//	png_infop end_ptr = png_create_info_struct(png_ptr);
//	if (!end_ptr) {
//		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
//		fclose(infile);
//		return false; /* out of memory */
//	}
//
//	/*
//	 * block to handle libpng errors,
//	 * then check whether the PNG file had a bKGD chunk
//	 */
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
//		fclose(infile);
//		return false;
//	}
//
//	/*
//	 * takes our file stream pointer (infile) and
//	 * stores it in the png_ptr struct for later use.
//	 */
//	png_init_io(png_ptr, infile);
//
//	/*
//	 * lets libpng know that we already checked the 8
//	 * signature bytes, so it should not expect to find
//	 * them at the current file pointer location
//	 */
//	png_set_sig_bytes(png_ptr, 8);
//
//	png_set_keep_unknown_chunks(png_ptr,3,NULL,0);
//
//	png_read_info(png_ptr, info_ptr);
//	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
//
//
//	if (color_type == PNG_COLOR_TYPE_PALETTE)											png_set_expand(png_ptr);
//	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)								png_set_expand(png_ptr);
//	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))								png_set_expand(png_ptr);
//	if (bit_depth == 16)																png_set_strip_16(png_ptr);
//	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)	png_set_gray_to_rgb(png_ptr);
//
//	/* snipped out the color type code, see source pngLoad.c */
//	/* Update the png info struct.*/
//	png_read_update_info(png_ptr, info_ptr);
//
//	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
//	//colorChannels = (int)png_get_channels(png_ptr, info_ptr);
//
//	image_data = new unsigned char[rowbytes*height];
//	row_pointers = new png_bytep[height];
//
//	for (i = 0;  i < height;  i++)
//		row_pointers[i] = image_data + i*rowbytes;
//
//	png_read_image(png_ptr, row_pointers);
//	png_read_end(png_ptr, NULL);
//
//	//////////////////////////////////
//	if(info != NULL) delete info;
//	info = new Info();
//
//	string title, nextLevel, shaderType;
//	float minHeight = 0;
//	float maxHeight = 1.0;
//	png_text* text_ptr;
//	int num_text;
//	png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);
//	for(int i=0;i<num_text;i++)
//	{
//		try{
//			if(strcmp(text_ptr[i].key,"Title") == 0)			title = text_ptr[i].text;
//			if(strcmp(text_ptr[i].key,"Next Level") == 0)		info->nextLevel = text_ptr[i].text;
//			if(strcmp(text_ptr[i].key,"Shader Type") == 0)		shaderType = text_ptr[i].text;
//			if(strcmp(text_ptr[i].key,"Map X Resolution") == 0)	info->mapResolution.x = lexical_cast<int>(text_ptr[i].text);
//			if(strcmp(text_ptr[i].key,"Map Y Resolution") == 0)	info->mapResolution.y = lexical_cast<int>(text_ptr[i].text);
//			if(strcmp(text_ptr[i].key,"Map X Size") == 0)		info->mapSize.x = lexical_cast<float>(text_ptr[i].text);
//			if(strcmp(text_ptr[i].key,"Map Y Size") == 0)		info->mapSize.y = lexical_cast<float>(text_ptr[i].text);
//			if(strcmp(text_ptr[i].key,"Minimum Height") == 0)	minHeight = lexical_cast<float>(text_ptr[i].text);
//			if(strcmp(text_ptr[i].key,"Maximum Height") == 0)	maxHeight = lexical_cast<float>(text_ptr[i].text);
//		}catch(...)
//		{
//			debugBreak();
//		}
//	}
//	if(shaderType == "island")		info->shaderType = SHADER_ISLAND;
//	else if(shaderType == "grass")	info->shaderType = SHADER_GRASS;
//	else if(shaderType == "snow")	info->shaderType = SHADER_SNOW;
//	else if(shaderType == "ocean")	info->shaderType = SHADER_OCEAN;
//
//	png_unknown_chunk* unknowns;
//	int num_unknowns = png_get_unknown_chunks(png_ptr, info_ptr, &unknowns);
//
//	for(int i=0; i<num_unknowns; i++)
//	{
//		if(strcmp((const char*)unknowns[i].name,"mhTs") == 0)
//		{
//			unsigned int nSize = min((int)(unknowns[i].size/sizeof(unsigned short)), info->mapResolution.x*info->mapResolution.y);
//			unsigned short* sHeights = new unsigned short[nSize];
//			memcpy(sHeights, unknowns[i].data, nSize*sizeof(unsigned short));
//
//			if(heights != NULL) delete heights;
//			heights = new float[nSize];
//
//			for(int i=0; i<nSize;i++)
//			{
//				heights[i] = minHeight + (maxHeight - minHeight) * sHeights[i] / (float)UCHAR_MAX;
//			}
//
//			delete[] sHeights;
//		}
//		else if(strcmp((const char*)unknowns[i].name,"obJs") == 0)
//		{
//			if(objects != NULL) delete objects;
//			info->numObjects = unknowns[i].size / sizeof(Object);
//			objects = new Object[info->numObjects];
//			memcpy(objects, unknowns[i].data, info->numObjects*sizeof(Object));
//		}
//		else if(strcmp((const char*)unknowns[i].name,"reGn") == 0)
//		{
//			if(regions != NULL) delete regions;
//			info->numRegions = unknowns[i].size / sizeof(Region);
//			regions = new Region[info->numRegions];
//			memcpy(regions, unknowns[i].data, info->numRegions*sizeof(Region));
//		}
//	}
//
//	/////////////////////////////////
//
//	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
//	fclose(infile);
//
//	delete[] image_data;
//	delete[] row_pointers;
//	return true;
//}
bool LevelFile::saveZIP(string filename)
{
	//check that basic level data is valid
	if(heights == nullptr || info->mapResolution.x == 0 || info->mapResolution.y == 0)
		return 0;

	//create several FileManager::file to hold various aspects of our level
	shared_ptr<FileManager::zipFile>		lvlFile(new FileManager::zipFile(filename));
	shared_ptr<FileManager::binaryFile>		rawFile(new FileManager::binaryFile("heightmap.raw"));
	shared_ptr<FileManager::iniFile>		attributesFile(new FileManager::iniFile("attributes.ini"));
	shared_ptr<FileManager::textFile>		objectsFile(new FileManager::textFile("objects.txt"));

	//initialize local variables
	unsigned int width	= info->mapResolution.x;
	unsigned int height = info->mapResolution.y;
	float maxHeight		= heights[0]+0.001;
	float minHeight		= heights[0];

	//find minimum and maximum heights
	for(int x=0;x<width;x++)
	{
		for(int z=0;z<height;z++)
		{
			if(heights[x+z*width]>maxHeight) maxHeight = heights[x+z*width];
			if(heights[x+z*width]<minHeight) minHeight = heights[x+z*width];
		}
	}

	//store heightmap data in our "heightmap.raw" file 
	rawFile->size = 2 * width * height;
	rawFile->contents = new unsigned char[rawFile->size];
	memset(rawFile->contents,0,rawFile->size);
	for(int x=0; x<width; x++)
	{
		for(int z=0; z<height; z++)
		{
			if(heights[x + z*width] > maxHeight) debugBreak();
			if(heights[x + z*width] < minHeight) debugBreak();

			*((unsigned short*)rawFile->contents + (x + z*width)) = USHRT_MAX * (heights[x + z*width] - minHeight) / (maxHeight - minHeight);
		}
	}

	//place information about our level in attributes.ini
	attributesFile->bindings["heightmap"]["resolutionX"] = lexical_cast<string>(width);
	attributesFile->bindings["heightmap"]["resolutionY"] = lexical_cast<string>(height);
	attributesFile->bindings["heightmap"]["minHeight"] = lexical_cast<string>(minHeight);
	attributesFile->bindings["heightmap"]["maxHeight"] = lexical_cast<string>(maxHeight);
	attributesFile->bindings["heightmap"]["sizeX"] = lexical_cast<string>(info->mapSize.x);
	attributesFile->bindings["heightmap"]["sizeZ"] = lexical_cast<string>(info->mapSize.y);

	attributesFile->bindings["level"]["nextLevel"] = info->nextLevel;

	if(info->shaderType == SHADER_ISLAND)		attributesFile->bindings["shaders"]["shaderType"] = "ISLAND";
	else if(info->shaderType == SHADER_GRASS)	attributesFile->bindings["shaders"]["shaderType"] = "GRASS";
	else if(info->shaderType == SHADER_SNOW)	attributesFile->bindings["shaders"]["shaderType"] = "SNOW";
	else if(info->shaderType == SHADER_OCEAN)	attributesFile->bindings["shaders"]["shaderType"] = "OCEAN";
	else										attributesFile->bindings["shaders"]["shaderType"] = "NONE";

	for(int i = 0; i < info->numObjects; i++)
	{
		objectsFile->contents += "object\n{\n";
		objectsFile->contents += string("\ttype=") + objectTypeString(objects[i].type) + "\n";
		objectsFile->contents += string("\tteam=") + lexical_cast<string>(objects[i].team) + "\n";
		objectsFile->contents += string("\tspawnPos=(") + lexical_cast<string>(objects[i].startloc.x) + "," + lexical_cast<string>(objects[i].startloc.y) + "," + lexical_cast<string>(objects[i].startloc.z) + ")\n";
		objectsFile->contents += "}\n";
	}
	//make all these file part of the lvl (zip) file
	lvlFile->files["heightmap.raw"] = rawFile;
	lvlFile->files["attributes.ini"] = attributesFile;
	lvlFile->files["objects.txt"] = objectsFile;
	//attempt to write the lvl file and return whether we were successful 
	return fileManager.writeZipFile(lvlFile);
}
bool LevelFile::loadZIP(string filename)
{
	auto f = fileManager.loadZipFile(filename);

	auto rFile = f->files.find("heightmap.raw");
	auto aFile = f->files.find("attributes.ini");
	auto oFile = f->files.find("objects.txt");

	if(rFile == f->files.end() || aFile == f->files.end() || oFile == f->files.end())
		return false;

	shared_ptr<FileManager::binaryFile>	rawFile(dynamic_pointer_cast<FileManager::binaryFile>(rFile->second));
	shared_ptr<FileManager::iniFile> attributesFile(dynamic_pointer_cast<FileManager::iniFile>(aFile->second));
	shared_ptr<FileManager::textFile> objectsFile(dynamic_pointer_cast<FileManager::textFile>(oFile->second));

	delete info;		info = nullptr;
	delete[] objects;	objects = nullptr;
	delete[] regions;	regions = nullptr;
	delete[] heights;	heights = nullptr;

	info = new Info;
	attributesFile->readValue("heightmap", "resolutionX", info->mapResolution.x);
	attributesFile->readValue("heightmap", "resolutionY", info->mapResolution.y);
	attributesFile->readValue("heightmap", "minHeight", info->minHeight);
	attributesFile->readValue("heightmap", "maxHeight", info->maxHeight);
	attributesFile->readValue("heightmap", "sizeX", info->mapSize.x);
	attributesFile->readValue("heightmap", "sizeZ", info->mapSize.y);
	attributesFile->readValue("level", "nextLevel", info->nextLevel);

	string sType;
	attributesFile->readValue("shaders", "shaderType", sType);
	if(sType == "ISLAND")		info->shaderType = SHADER_ISLAND;
	else if(sType == "GRASS")	info->shaderType = SHADER_GRASS;
	else if(sType == "SNOW")	info->shaderType = SHADER_SNOW;
	else if(sType == "OCEAN")	info->shaderType = SHADER_OCEAN;
	else						info->shaderType = SHADER_GRASS; //grass is default

	if(info->mapResolution.x == 0 || info->mapResolution.y == 0)
	{
		delete info;
		info = nullptr;
		return false;
	}
	heights = new float[info->mapResolution.x * info->mapResolution.y];
	memset(heights, 0, info->mapResolution.x * info->mapResolution.y * sizeof(float));
	for(int i = 0; i < info->mapResolution.x * info->mapResolution.y && i * 2 < rawFile->size; i++)
	{
		heights[i] = info->minHeight + ((float)*((unsigned short*)rawFile->contents + i)) * (info->maxHeight - info->minHeight) / USHRT_MAX ;
	}

	parseObjectFile(objectsFile);

	return true;
}
bool LevelFile::parseObjectFile(shared_ptr<FileManager::textFile> f)
{
	if(!f)
	{
		return false;
	}

	string& str = f->contents;
	info->numObjects=0;
	int pos=0;
	unsigned int objectNum=0;

	auto readSubString = [str, &pos](char* c)->bool
	{
		int p=0;
		while(p+pos < str.size())
		{
			if(*(c+p) == 0)
			{
				pos += p;
				return true;
			}
			else if(*(c+p) != str[pos+p])
			{
				return false;
			}
			else
			{
				p++;
			}

		}
		return true;
	};
	auto readLine = [str, &pos]()->string
	{
		int p=0;
		while(p+pos < str.size() && str[p+pos] != '\n')
		{
			p++;
		}

		pos += p+1;
		return str.substr(pos-p-1,p);
	};

	while(pos < str.size())
	{
		if(readSubString("object"))
		{
			++info->numObjects;
			while(str[pos] != '\n' && pos < str.size())
				++pos;
		}
		else
		{
			while(str[pos] != '\n' && pos < str.size())
				++pos;			
		}
		++pos;
	}

	//try
	{
		pos = 0;
		objects = new Object[info->numObjects];
		enum {SEARHING,PARSING_OBJECT}state = SEARHING;
		while(pos < str.size())
		{
			if(state == SEARHING)
			{
				if(readSubString("object\n{\n"))
				{
					state = PARSING_OBJECT;
				}
				else
				{
					while(str[pos] != '\n' && pos < str.size())
						++pos;
					pos++;
				}
			}
			else if(state == PARSING_OBJECT)
			{
				if(readSubString("\ttype="))
				{
					string s = readLine();
					objects[objectNum].type = objectTypeFromString(s);
				}
				else if(readSubString("\tteam="))
				{
					string s = readLine();
					objects[objectNum].team = lexical_cast<int>(s) - 1;
				}
				else if(readSubString("\tspawnPos="))
				{
					string s = readLine();
					objects[objectNum].startloc = lexical_cast<Vec3f>(s);
					int i=0;
				}
				else if(readSubString("}\n"))
				{
					state = SEARHING;
					objectNum++;
				}
			}
		}
		return true;
	}
	//catch(...)
	//{
	//	debugBreak();
	//	return false;
	//}
}
void LevelFile::initializeWorld(unsigned int humanPlayers)
{
	players.resetPlayers();

	int w = min(info->mapResolution.x, info->mapResolution.y);
	if(w != 0 && heights != nullptr)
	{
		float maxHeight=heights[0]+0.001;
		float minHeight=heights[0];
		for(int x=0;x<info->mapResolution.x;x++)
		{
			for(int z=0;z<info->mapResolution.y;z++)
			{
				if(heights[x+z*info->mapResolution.x]>maxHeight) maxHeight=heights[x+z*info->mapResolution.x];
				if(heights[x+z*info->mapResolution.x]<minHeight) minHeight=heights[x+z*info->mapResolution.x];
			}
		}

		unsigned short* h = new unsigned short[w*w];
		for(int i=0;i<w*w;i++) h[i] = ((heights[(i%w)+(i/w)*info->mapResolution.x]-minHeight)/(maxHeight-minHeight)) * USHRT_MAX;
		world.initTerrain(h, w,Vec3f(0,minHeight,0),Vec3f(info->mapSize.x,maxHeight - minHeight,info->mapSize.y));
	}

	bullets = world.newObject(new bulletCloud);
	particleManager.addEmitter(new particle::bulletEffect, bullets);

	for(int i=0; i<info->numObjects; i++)
	{
		if(objects[i].type & PLANE)
		{
			if(/*obj.controlType == CONTROL_HUMAN &&*/ players.numPlayers() < humanPlayers)
			{	
				auto id = world.newObject(new nPlane(objects[i].startloc, objects[i].startRot, objects[i].type,objects[i].team));
				players.addHumanPlayer(id);
			}
			else
			{
				auto id = world.newObject(new nPlane(objects[i].startloc,objects[i].startRot,objects[i].type,objects[i].team));
				players.addAIplayer(id);
			}
		}
		else if(objects[i].type == AA_GUN)	
		{
			world.newObject(new AAgun(objects[i].startloc,objects[i].startRot,objects[i].type, objects[i].team));
		}
		else if(objects[i].type == SAM_BATTERY)	
		{
			world.newObject(new SAMbattery(objects[i].startloc,objects[i].startRot,objects[i].type, objects[i].team));
		}
		else if(objects[i].type == FLAK_CANNON)	
		{
			world.newObject(new flakCannon(objects[i].startloc,objects[i].startRot,objects[i].type, objects[i].team));
		}
	}
}
LevelFile::LevelFile(): info(NULL), objects(NULL), regions(NULL), heights(NULL)
{
	header.magicNumber = 0;
	header.version = 0;
}

//________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________//
float Level::heightmapBase::rasterHeight(unsigned int x,unsigned int z) const
{
	return heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()];
}
float Level::heightmapBase::interpolatedHeight(float x, float z) const
{
	if(x-floor(x)+z-floor(z)<1.0)
	{
		float A = rasterHeight(floor(x),floor(z));
		float B = rasterHeight(floor(x),floor(z+1));
		float D = rasterHeight(floor(x+1),floor(z));
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x));
	}
	else
	{
		float B = rasterHeight(floor(x),floor(z+1));
		float C = rasterHeight(floor(x+1),floor(z+1));
		float D = rasterHeight(floor(x+1),floor(z));
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}
float Level::heightmapBase::height(float x, float z) const
{	//    A------D		.-----x+
	//	  |      |		|
	//    |      |		|
	//    B------C		z+

	x -= mPosition.x;
	z -= mPosition.z;
	x *= mResolution.x/mSize.x;
	z *= mResolution.y/mSize.y;
	return interpolatedHeight(x,z);
}

Vec3f Level::heightmapBase::rasterNormal(unsigned int h, unsigned int k) const
{
	unsigned int x=h; unsigned int z=k;
	float Cy = (z < resolutionZ()-1)	? ((float)heights[x	+(z+1)*resolutionX()]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	float Ay = (x < resolutionX()-1)	? ((float)heights[(x+1)+z*resolutionX()	]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	float Dy = (  z > 0	)				? ((float)heights[x	+(z-1)*resolutionX()]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	float By = (  x > 0	)				? ((float)heights[(x-1)+z*resolutionX()	]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	//if(z >= resolutionZ()-1) Cy=Dy;
	//if(x >= resolutionX()-1) Ay=By;
	//if(z <= 0) Dy = Cy;
	//if(x <= 0) By = Ay;
	return Vec3f(Cy - Ay, 3.0 / 15.0, Dy - By).normalize();
}
Vec3f Level::heightmapBase::interpolatedNormal(float x, float z) const
{
	if(x-floor(x)+z-floor(z)<1.0)
	{
		Vec3f A = rasterNormal(floor(x),floor(z));
		Vec3f B = rasterNormal(floor(x),floor(z+1));
		Vec3f D = rasterNormal(floor(x+1),floor(z));
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x));
	}
	else
	{
		Vec3f B = rasterNormal(floor(x),floor(z+1));
		Vec3f C = rasterNormal(floor(x+1),floor(z+1));
		Vec3f D = rasterNormal(floor(x+1),floor(z));
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}
Vec3f Level::heightmapBase::normal(float x, float z) const
{	//    A------D		.-----x+
	//	  |      |		|
	//    |      |		|
	//    B------C		z+

	x -= mPosition.x;
	z -= mPosition.z;
	x *= mResolution.x/mSize.x;
	z *= mResolution.y/mSize.y;
	return interpolatedNormal(x,z);
}
Level::heightmapBase::heightmapBase(Vec2u Resolution): mPosition(0,0,0), mSize(Resolution.x,Resolution.y), mResolution(Resolution),  heights(NULL), shaderType(SHADER_NONE)
{
	heights = new float[mResolution.x*mResolution.y];
	memset(heights,0,mResolution.x*mResolution.y*sizeof(float));
}
Level::heightmapBase::heightmapBase(Vec2u Resolution, float* hts): mPosition(0,0,0), mSize(Resolution.x,Resolution.y), mResolution(Resolution), heights(NULL), shaderType(SHADER_NONE)
{
	heights = new float[mResolution.x*mResolution.y];
	memcpy(heights,hts,mResolution.x*mResolution.y*sizeof(float));
}
void Level::heightmapGL::init()
{
	setMinMaxHeights();
	groundTex = graphics->genTexture2D();
	groundValues = new unsigned char[uPowerOfTwo(mResolution.x) * uPowerOfTwo(mResolution.y) * 4];
	memset(groundValues,0,uPowerOfTwo(mResolution.x)*uPowerOfTwo(mResolution.y)*sizeof(unsigned char)*4);
	groundTex->setData(uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), GraphicsManager::texture::RGBA, groundValues, true);
	//glBindTexture(GL_TEXTURE_2D, groundTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, 4 , uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//setTex();
	//if(!dynamic)
	//{
	//	delete groundValues;
	//	groundValues = NULL;
	//}
}
Level::heightmapGL::~heightmapGL()
{
	glDeleteLists(dispList,1);
	if(groundValues)
		delete[] groundValues;
}
void Level::heightmapGL::setTex() const
{	//////////////////////////////////
	//		B		//		-z		//
	//	C		A	//	-x		+x	//
	//		D		//		+z		//
	//////////////////////////////////
	int x,z;
	Vec3f n;
	int xPOT = uPowerOfTwo(mResolution.x);
	int zPOT = uPowerOfTwo(mResolution.y);
	Vec2f bSize(((float)mResolution.x-1)/xPOT, ((float)mResolution.y-1)/zPOT);

	for(x=0; x < xPOT; x += 1)
	{
		for(z=0; z< zPOT; z += 1)
		{
			n = interpolatedNormal(bSize.x*x,bSize.y*z);
			//if(x > 0.99)					n += getNormal(bSize.x*x-1,	bSize.y*z) * 0.5;
			//if(x < resolutionX()-0.99)	n += getNormal(bSize.x*x+1,	bSize.y*z) * 0.5;
			//if(z > 0.99)					n += getNormal(bSize.x*x,	bSize.y*z-1) * 0.5;
			//if(z < resolutionZ()-0.99)	n += getNormal(bSize.x*x,	bSize.y*z+1) * 0.5;
			if(n.magnitudeSquared() < 0.001)n = Vec3f(0.0,1.0,0.0);
			n = n.normalize();

			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+1] = (unsigned char)(n.y*255.0);
			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+3] = (unsigned char)(255.0*(interpolatedHeight(bSize.x*x,bSize.y*z)-minHeight)/(maxHeight-minHeight));//(255.0*(getHeight(bSize.x*x,bSize.y*z)/size.y-minHeight)/(maxHeight-minHeight));
		}
	}
	groundTex->setData(uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), GraphicsManager::texture::RGBA, groundValues, true);
	//glBindTexture(GL_TEXTURE_2D, groundTex);
	////glTexImage2D(GL_TEXTURE_2D, 0, 4 , uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
}
void Level::heightmapGL::createList() const
{
	int skipX = max(mResolution.x / 128,1);
	int skipZ = max(mResolution.y / 128,1);
	dispList = glGenLists(1);
	glNewList(dispList,GL_COMPILE);
	for(unsigned int z=0;z<mResolution.y-skipZ; z += skipZ)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int x=0;x<mResolution.x; x += skipX)
		{
			glVertex3f((float)x/(mResolution.x-1),heights[x+z*mResolution.x],	(float)(z)/(mResolution.y-1));
			glVertex3f((float)x/(mResolution.x-1),heights[x+(z+skipZ)*mResolution.x],(float)(z+skipZ)/(mResolution.y-1));
		}
		glEnd();
	}
	glEndList();

}
//void Level::heightmapGL::render() const
//{
//	if(!valid)
//	{
//		glDeleteLists(dispList,1);
//		createList();
//		setMinMaxHeights();
//		setTex();
//		valid=true;
//	}
//
//	if(shaderType == SHADER_ISLAND || shaderType == SHADER_GRASS || shaderType == SHADER_NONE)
//	{
//		if(shaderType == SHADER_ISLAND)	dataManager.bind("island new terrain");
//		else							dataManager.bind("grass new terrain");
//
//		dataManager.bind("sand",0);
//		dataManager.bind("grass",1);
//		dataManager.bind("rock",2);
//		dataManager.bind("LCnoise",3);
//		groundTex->bind(4);
//
//		dataManager.setUniform1f("maxHeight",	maxHeight);
//		dataManager.setUniform1f("minHeight",	0);
//		dataManager.setUniform1f("XZscale",		1);
//		dataManager.setUniform1f("time",		world.time());
//
//		dataManager.setUniform1i("sand",		0);
//		dataManager.setUniform1i("grass",		1);
//		dataManager.setUniform1i("rock",		2);
//		dataManager.setUniform1i("LCnoise",		3);
//		dataManager.setUniform1i("groundTex",	4);
//	}
//	else if(shaderType == SHADER_SNOW)
//	{
//		dataManager.bind("snow terrain");
//
//		dataManager.bind("snow",0);
//		dataManager.bind("LCnoise",1);
//		groundTex->bind(2);
//
//		dataManager.setUniform1f("maxHeight",	maxHeight);
//		dataManager.setUniform1f("minHeight",	0);
//		dataManager.setUniform1f("XZscale",		1);
//		dataManager.setUniform1f("time",		world.time());
//		dataManager.setUniform1i("snow",		0);
//		dataManager.setUniform1i("LCnoise",		1);
//		dataManager.setUniform1i("groundTex",	2);
//	}
//
// 	if(shaderType == SHADER_NONE || shaderType == SHADER_ISLAND || shaderType == SHADER_GRASS || shaderType == SHADER_SNOW)
//	{
//		glPushMatrix();
//			glTranslatef(mPosition.x,mPosition.y,mPosition.z);
//			glScalef(mSize.x,1,mSize.y);
//			glCallList(dispList);
//		glPopMatrix();
//
//
//		dataManager.unbindTextures();
//		dataManager.unbindShader();
//	}
//	//glPushMatrix();
//	//glColor4f(0.1,0.3,1.0,0.6);
//	//glTranslatef(position.x,position.y,position.z);
//	//glScalef(size.x,size.y,size.z);
//	//for(unsigned int z=0;z<resolution.y-1;z++)
//	//{
//	//	glBegin(GL_TRIANGLE_STRIP);
//	//	for(unsigned int x=0;x<resolution.x;x++)
//	//	{
//	//		glVertex3f(x,heights[x+z*resolution.x],z);
//	//		glVertex3f(x,heights[x+(z+1)*resolution.x],z+1);
//	//	}
//	//	glEnd();
//	//}
//	//glColor4f(0.3,0.5,1.0,0.8);
//	//glBegin(GL_LINES);
//	//for(int x=0;x<resolution.x; x+=4)
//	//{
//	//	for(int y=0;y<resolution.y-1; y+=1)
//	//	{
//	//		glVertex3f(x,rasterHeight(x,y),y);
//	//		glVertex3f(x,rasterHeight(x,y+1),y+1);
//	//	}
//	//}
//	//for(int y=0;y<resolution.y; y+=4)
//	//{
//	//	for(int x=0;x<resolution.x-1; x+=1)
//	//	{
//	//		glVertex3f(x,rasterHeight(x,y),y);
//	//		glVertex3f(x+1,rasterHeight(x+1,y),y);
//	//	}
//	//}
//	//glEnd();
//	//glPopMatrix();
//}
void Level::heightmapGL::renderPreview(float scale, float seaLevelOffset) const
{
	//if(seaLevelOffset >= maxHeight)
	//	return;

	if(!valid)
	{
		glDeleteLists(dispList,1);
		createList();
		setMinMaxHeights();
		setTex();
		valid=true;
	}
	if(shaderType == SHADER_ISLAND || shaderType == SHADER_GRASS || shaderType == SHADER_NONE)
	{
		if(shaderType == SHADER_ISLAND)		dataManager.bind("island preview terrain");
		else								dataManager.bind("grass preview terrain");

		dataManager.bind("sand",0);
		dataManager.bind("grass",1);
		dataManager.bind("rock",2);
		dataManager.bind("LCnoise",3);
		groundTex->bind(4);

		dataManager.setUniform1f("heightScale",	scale);

		dataManager.setUniform1f("maxHeight",	maxHeight);
		dataManager.setUniform1f("minHeight",	0);
		dataManager.setUniform1f("XZscale",		1);
		dataManager.setUniform1f("time",		world.time());
		dataManager.setUniform1i("sand",		0);
		dataManager.setUniform1i("grass",		1);
		dataManager.setUniform1i("rock",		2);
		dataManager.setUniform1i("LCnoise",		3);
		dataManager.setUniform1i("groundTex",	4);
	}
	else if(shaderType == SHADER_SNOW)
	{
		dataManager.bind("snow preview terrain");

		dataManager.bind("snow",0);
		dataManager.bind("LCnoise",1);
		groundTex->bind(2);

		dataManager.setUniform1f("heightScale",	scale);

		dataManager.setUniform1f("maxHeight",	maxHeight);
		dataManager.setUniform1f("minHeight",	0);
		dataManager.setUniform1f("XZscale",		1);
		dataManager.setUniform1f("time",		world.time());
		dataManager.setUniform1i("snow",		0);
		dataManager.setUniform1i("LCnoise",		1);
		dataManager.setUniform1i("groundTex",	2);
	}

	if(shaderType == SHADER_NONE || shaderType == SHADER_ISLAND || shaderType == SHADER_GRASS || shaderType == SHADER_SNOW)
	{
		glPushMatrix();
			glTranslatef(mPosition.x,mPosition.y,mPosition.z);
			glScalef(mSize.x,scale,mSize.y);
			glCallList(dispList);
		glPopMatrix();

		dataManager.unbindTextures();
		dataManager.unbindShader();
	}
	dataManager.bind("model");
}
void Level::heightmapBase::setMinMaxHeights() const
{
	if(heights == NULL || resolutionX()==0 || resolutionZ()==0) return;
	maxHeight=heights[0]+0.001;
	minHeight=heights[0];
	for(int x=0;x<resolutionX();x++)
	{
		for(int z=0;z<resolutionZ();z++)
		{
			if(heights[x+z*resolutionX()]>maxHeight) maxHeight=heights[x+z*resolutionX()];
			if(heights[x+z*resolutionX()]<minHeight) minHeight=heights[x+z*resolutionX()];
		}
	}
}
void Level::heightmapGL::increaseHeights(float amount)
{
	if(heights == NULL || resolutionX()==0 || resolutionZ()==0) return;
	int x,z;
	for(x=0;x<resolutionX();x++)
	{
		for(z=0;z<resolutionZ();z++)
		{
			heights[x+z*resolutionX()] += amount;
		}
	}
}
//________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________//

Level::Level(): mGround(NULL)
{

}
Level::~Level()
{
	if(mGround)
	{
		delete mGround;
	}
}
bool Level::init(string filename)
{
	LevelFile file;
	if(!file.loadZIP(filename))
		return false;

	if(	(file.info == NULL) ||
		(file.info->mapResolution.x != 0 && file.info->mapResolution.y != 0 && file.heights == NULL) ||
		(file.info->numObjects != 0 && file.objects == NULL) ||
		(file.info->numRegions != 0 && file.regions == NULL))
	{
		return false;
	}

	if(file.info->mapResolution.x != 0 && file.info->mapResolution.y != 0)
	{
		mGround = new heightmapGL(file.info->mapResolution,file.heights);
		mGround->setSize(file.info->mapSize);
		mGround->shaderType = file.info->shaderType;
	}

	nextLevel = file.info->nextLevel;

	for(int i=0; i < file.info->numObjects; i++)
		mObjects.push_back(file.objects[i]);

	return true;
}
void Level::initializeWorld()
{
	for(auto i=mObjects.begin(); i!=mObjects.end(); i++)
	{
		if(i->type & PLANE)
		{
			if(/*obj.controlType == CONTROL_HUMAN &&*/ players.numPlayers() < 2)
			{	
				auto id = world.newObject(new nPlane(i->startloc, i->startRot, i->type,i->team));
				players.addHumanPlayer(id);
			}
			else
			{
				auto id = world.newObject(new nPlane(i->startloc, i->startRot, i->type,i->team));
				players.addAIplayer(id);
			}
		}
		else if(i->type == AA_GUN)// can't be player controlled
		{
			world.newObject(new AAgun(i->startloc,i->startRot,i->type, i->type));
		}
	}

	bullets = world.newObject(new bulletCloud);
	particleManager.addEmitter(new particle::bulletEffect, bullets);


	int w = min(mGround->resolutionX(), mGround->resolutionZ());
	float minHeight = mGround->getMinHeight();
	float maxHeight = mGround->getMaxHeight();
	unsigned short* h = new unsigned short[w*w];
	for(int i=0;i<w*w;i++) h[i] = ((mGround->rasterHeight(i%w,i/w)-minHeight)/(maxHeight-minHeight)) * USHRT_MAX;//(unsigned short)((mGround->rasterHeight(i%w,i/w)-minHeight)/maxHeight * ((unsigned short)USHRT_MAX));
	world.initTerrain(h, w-1,Vec3f(0,mGround->getMinHeight(),0),Vec3f(mGround->sizeX(),mGround->getMaxHeight() - mGround->getMinHeight(),mGround->sizeZ()));
}
//void Level::render(Vec3f eye)
//{
//	//glDisable(GL_CULL_FACE);
//	glPushMatrix();
//
//	mGround->render();
//
////	glDepthMask(false);
//
//	if(mGround->shaderType == SHADER_ISLAND || mGround->shaderType == SHADER_OCEAN || mGround->shaderType == SHADER_GRASS)
//	{
//		Vec3d center(eye.x,0,eye.z);
//		double radius = (eye.y)*tan(asin(6000000/(6000000+eye.y)));
//		float cAng,sAng;
//
//		dataManager.bind("horizon2");
//		dataManager.bind("hardNoise",0);
//
//		dataManager.setUniform1i("bumpMap",	0);
//		dataManager.setUniform1f("time",	world.time());
//		dataManager.setUniform1f("seaLevel",mGround->minHeight/(mGround->maxHeight-mGround->minHeight));
//		dataManager.setUniform2f("center",	center.x,center.z);
//		dataManager.setUniform3f("eyePos", eye.x, eye.y, eye.z);
//
//		glBegin(GL_TRIANGLE_FAN);
//			glTexCoord2f(center.x/mGround->sizeX(),center.z/mGround->sizeZ());
//			glVertex3f(center.x,center.y,center.z);
//
//			for(float ang = 0; ang < PI*2.0+0.01; ang +=PI/8.0)
//			{
//				cAng=cos(ang);
//				sAng=sin(ang);
//				glTexCoord2f((center.x-cAng*radius)/mGround->sizeX(),(center.z-sAng*radius)/mGround->sizeZ());
//				glVertex3f(center.x-cAng*radius,center.y,center.z-sAng*radius);
//			}
//		glEnd();
//
//		dataManager.unbindTextures();
//		dataManager.unbindShader();
//
//		glDepthMask(true);
//	}
//	//else if(mGround->shaderType == SHADER_GRASS)
//	//{
//	//	glDepthMask(false);
//	//	dataManager.bind("ocean");
//
//	//	dataManager.bind("hardNoise",0);
//	//	if(mGround->shaderType == SHADER_OCEAN)		dataManager.bindTex(0,1);
//	//	else										dataManager.bindTex(((heightmapGL*)mGround)->groundTex,1);
//	//	dataManager.bind("rock",2);
//	//	dataManager.bind("sand",3);
//
//	//	dataManager.setUniform1i("bumpMap", 0);
//	//	dataManager.setUniform1i("groundTex", 1);
//	//	dataManager.setUniform1i("rock", 2);
//	//	dataManager.setUniform1i("sand", 3);
//	//	dataManager.setUniform1f("time", world.time());
//	//	dataManager.setUniform1f("seaLevel", -(mGround->minHeight)/(mGround->maxHeight-mGround->minHeight));
//	//	dataManager.setUniform1f("XZscale", mGround->mSize.x);
//
//	//	//glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->mResolution.x)/uPowerOfTwo(mGround->mResolution.x),(float)(mGround->mResolution.y)/uPowerOfTwo(mGround->mResolution.y));
//
//	//	glBegin(GL_QUADS);
//	//		glVertex3f(0,0,0);
//	//		glVertex3f(0,0,mGround->mSize.y);
//	//		glVertex3f(mGround->mSize.x,0,mGround->mSize.y);
//	//		glVertex3f(mGround->mSize.x,0,0);
//	//	glEnd();
//
//	//	dataManager.unbindTextures();
//	//	dataManager.unbindShader();
//	//	glDepthMask(true);
//	//}
//	glPopMatrix();
//}
//________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________//
Level::heightmapBase* editLevel::ground()
{
	return mGround;
}
void editLevel::newGround(unsigned int x, unsigned int z, float* heights)
{
	if(mGround != NULL)
		delete mGround;
	if(heights != NULL)
		mGround = new heightmapGL(Vec2u(x,z),heights);
	else
		mGround = new heightmapGL(Vec2u(x,z));
}
void editLevel::addObject(int type,int team,int controlType,Vec3f pos, Quat4f rot)
{
	LevelFile::Object o;
	o.type=type;
	o.team=team;
	o.controlType=controlType;
	o.startloc=pos;
	o.startRot=rot;
	mObjects.push_back(o);
}
void editLevel::addRegionCircle(Vec2f c, float rad)
{
	LevelFile::Region r;
	r.shape = 0;			//circle
	r.type = 0;				//map bounds
	r.centerXYZ[0] = c.x;
	r.centerXYZ[2] = c.y;
	r.radius = rad;
	r.colorR = 0.0;
	r.colorG = 1.0;
	r.colorB = 0.0;
	mRegions.push_back(r);
}
void editLevel::addRegionRect(Rect rect)
{
	LevelFile::Region r;
	r.shape = 1;			//rect
	r.type = 0;				//map bounds
	r.minXYZ[0] = rect.x;
	r.minXYZ[2] = rect.y;
	r.maxXYZ[0] = rect.x + rect.w;
	r.minXYZ[2] = rect.y + rect.h;
	r.colorR = 0.0;
	r.colorG = 1.0;
	r.colorB = 0.0;
	mRegions.push_back(r);
}
void editLevel::setWater(string shaderName)
{
	water.name = shaderName;
}
bool editLevel::init(string BMP, Vec3f size, float seaLevel)
{
	auto image = fileManager.loadBmpFile(BMP);
//	Image* image = loadBMP(BMP.c_str());

	if(!image->valid() || image->height == 0 || image->width==0)
		return false;

	float* t = new float[image->width * image->height];

	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			t[y * image->width + x] = (float)((unsigned char)image->contents[image->channels * (y * image->width + x)])/255.0*size.y - seaLevel;
		}
	}


	mGround = new heightmapGL(Vec2u(image->height,image->width),t);
	mGround->setSize(Vec2f(size.x,size.z));
	mGround->init();

	mGround->setMinMaxHeights();

	delete[] t;

	return true;
}
void editLevel::save(string filename)
{
	LevelFile f;
	f.header.magicNumber = 0x454c49465f4c564c;
	f.header.version = 1;
	f.info = new LevelFile::Info;
	f.info->shaderType = mGround->shaderType;
	f.info->mapSize = mGround->size();
	f.info->mapResolution = mGround->resolution();
	f.info->numObjects = mObjects.size();
	f.info->numRegions = 0;
	f.heights = mGround->heights;
	f.objects = NULL;
	if(!mObjects.empty()){
		f.objects = new LevelFile::Object[mObjects.size()];
		memcpy(f.objects, &(*mObjects.begin()),mObjects.size()*sizeof(LevelFile::Object));
	}
	f.saveZIP(filename);
}
void editLevel::save(string filename, float seaLevelOffset)
{
	LevelFile f;
	f.header.magicNumber = 0x454c49465f4c564c;
	f.header.version = 1;
	f.info = new LevelFile::Info;
	f.info->shaderType = mGround->shaderType;
	f.info->mapSize = mGround->size();
	f.info->mapResolution = mGround->resolution();
	f.info->numObjects = mObjects.size();
	f.info->numRegions = 0;
	f.heights = mGround->heights;
	if(mObjects.size() > 0)
	{
		f.objects = new LevelFile::Object[mObjects.size()];
		memcpy(f.objects, &(*mObjects.begin()),mObjects.size()*sizeof(LevelFile::Object));
	}

	int x,z;
	for(x=0;x<mGround->resolutionX();x++)
	{
		for(z=0;z<mGround->resolutionZ();z++)
		{
			f.heights[x+z*mGround->resolutionX()] -= seaLevelOffset;
		}
	}
	f.saveZIP(filename);
}


void editLevel::exportBMP(string filename)
{
	int width = mGround->resolutionX();
	int size = (3*width+width%4)*mGround->resolutionZ() + 3*width*mGround->resolutionZ()%4;


	BITMAPFILEHEADER header;
	header.bfType					= 0x4D42;
	header.bfSize					= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + size;
	header.bfReserved1				= 0;
	header.bfReserved2				= 0;
	header.bfOffBits				= sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);

	BITMAPINFO info;
	info.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth			= mGround->resolutionX();
	info.bmiHeader.biHeight			= mGround->resolutionZ();
	info.bmiHeader.biPlanes			= 1;
	info.bmiHeader.biBitCount		= 24;
	info.bmiHeader.biCompression	= BI_RGB;
	info.bmiHeader.biSizeImage		= 0;
	info.bmiHeader.biXPelsPerMeter	= 100;
	info.bmiHeader.biYPelsPerMeter	= 100;
	info.bmiHeader.biClrUsed		= 0;
	info.bmiHeader.biClrImportant	= 0;
	memset(&info.bmiColors,0,sizeof(info.bmiColors));

	mGround->setMinMaxHeights();
	unsigned char* colors = new unsigned char[size];
	memset(colors,0,size);
	for(int z=0;z<mGround->resolutionZ();z++)
	{
		for(int x=0;x<mGround->resolutionX();x++)
		{
			colors[(z*(3*width+width%4) + 3*x) + 0] = 255.0*(mGround->rasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
			colors[(z*(3*width+width%4) + 3*x) + 1] = 255.0*(mGround->rasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
			colors[(z*(3*width+width%4) + 3*x) + 2] = 255.0*(mGround->rasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
		}
	}
	ofstream fout(filename,ios::out|ios::binary|ios::trunc);
	fout.write((char*)&header,sizeof(header));
	fout.write((char*)&info,sizeof(info));
	fout.write((char*)colors,size);
	delete[] colors;
	fout.close();
}
void editLevel::renderPreview(bool drawWater, float scale, float seaLevelOffset)
{
	glPushMatrix();
	mGround->renderPreview(scale, seaLevelOffset);

	if(seaLevelOffset != 0.0 && (mGround->shaderType == SHADER_ISLAND || mGround->shaderType == SHADER_OCEAN || mGround->shaderType == SHADER_GRASS))
	{
		dataManager.bind("ocean preview");

		dataManager.bind("ocean normals",0);
		if(mGround->shaderType == SHADER_OCEAN)		dataManager.bindTex(0,1);
		else										((heightmapGL*)mGround)->groundTex->bind(1);
		dataManager.bind("rock",2);
		dataManager.bind("sand",3);

		dataManager.setUniform1i("bumpMap", 0);
		dataManager.setUniform1i("groundTex", 1);
		dataManager.setUniform1i("rock", 2);
		dataManager.setUniform1i("sand", 3);
		dataManager.setUniform1f("time", world.time());
		dataManager.setUniform1f("seaLevel", (seaLevelOffset-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight));
		dataManager.setUniform1f("XZscale", mGround->mSize.x);

		//glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->mResolution.x)/uPowerOfTwo(mGround->mResolution.x),(float)(mGround->mResolution.y)/uPowerOfTwo(mGround->mResolution.y));
		graphics->drawQuad(	Vec3f(0,seaLevelOffset*scale,0),
							Vec3f(0,seaLevelOffset*scale,mGround->mSize.y),
							Vec3f(mGround->mSize.x,seaLevelOffset*scale,0),
							Vec3f(mGround->mSize.x,seaLevelOffset*scale,mGround->mSize.y));

		dataManager.bindTex(0,3);
		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		dataManager.bind("model");
	}



	if(mGround->shaderType == SHADER_GRASS || mGround->shaderType == SHADER_SNOW)
	{
		//glScalef(mGround->sizeX()/(mGround->resolutionX()-1),1,mGround->sizeZ()/(mGround->resolutionZ()-1));
		dataManager.bind("model");
		dataManager.setUniformMatrix("modelTransform", Mat4f(Quat4f(), Vec3f(), Vec3f(mGround->sizeX()/(mGround->resolutionX()-1),1,mGround->sizeZ()/(mGround->resolutionZ()-1))));
		dataManager.setUniform1i("tex", 0);

		float h = mGround->minHeight*scale-20.0;
		//float h = max(mGround->minHeight-20.0,seaLevelOffset);//needs to be adjusted for sea level
		dataManager.bind("layers");
		float t=0.0;

		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0,1,0);
		for(int i = 0; i < mGround->resolutionX()-1; i++)
		{
			glTexCoord2f(t,(mGround->rasterHeight(i,0)-mGround->minHeight+20)/256);		glVertex3f(i,h,0);
			glTexCoord2f(t,0);	glVertex3f(i,max(mGround->rasterHeight(i,0)*scale,h) ,0);
			t+=0.2;
		}
		for(int i = 0; i < mGround->resolutionZ()-1; i++)
		{
			glTexCoord2f(t,(mGround->rasterHeight(mGround->resolutionX()-1,i)-mGround->minHeight+20)/256);	glVertex3f(mGround->resolutionX()-1,h,i);
			glTexCoord2f(t,0);	glVertex3f(mGround->resolutionX()-1,max(mGround->rasterHeight(mGround->resolutionX()-1,i)*scale,h),i);
			t+=0.2;
		}
		t+=0.2;
		for(int i = mGround->resolutionX()-1; i > 0; i--)
		{
			t-=0.2;
			glTexCoord2f(t,(mGround->rasterHeight(i,mGround->resolutionZ()-1)-mGround->minHeight+20)/256);	glVertex3f(i,h,mGround->resolutionZ()-1);
			glTexCoord2f(t,0);	glVertex3f(i,max(mGround->rasterHeight(i,mGround->resolutionZ()-1)*scale,h),mGround->resolutionZ()-1);
		}
		for(int i = mGround->resolutionZ()-1; i >= 0; i--)
		{
			t-=0.2;
			glTexCoord2f(t,(mGround->rasterHeight(0,i)-mGround->minHeight+20)/256);		glVertex3f(0,h,i);
			glTexCoord2f(t,0);	glVertex3f(0,max(mGround->rasterHeight(0,i)*scale,h),i);
		}
		glEnd();

		//dataManager.bindTex(0);
		dataManager.bind("white");

		glColor3f(0.73,0.6,0.47);
		glBegin(GL_QUADS);
			glVertex3f(0,h,0);
			glVertex3f(0,h,mGround->resolutionZ()-1);
			glVertex3f(mGround->resolutionX()-1,h,mGround->resolutionZ()-1);
			glVertex3f(mGround->resolutionX()-1,h,0);
		glEnd();
		glColor3f(1,1,1);

		dataManager.setUniformMatrix("modelTransform", Mat4f());
	}
	glPopMatrix();
}
void editLevel::renderObjectsPreview()
{
	for(auto i=mObjects.begin();i!=mObjects.end();i++)
	{
		//if(i->type & PLANE)
		//{
			//auto b = dataManager.getModel(i->type)->boundingSphere;
			//graphics->drawSphere(i->startloc + i->startRot * b.center, b.radius * 10.0);
			graphics->drawModel(i->type,i->startloc,i->startRot,10.0);
		//}
	}
}
