
#include "game.h"
LevelFile::Object::Object():type(0), team(NEUTRAL), startloc(), startRot()
{

}
bool LevelFile::saveZIP(string filename)
{
	//check that basic level data is valid
	if(heights == nullptr || info.mapResolution.x == 0 || info.mapResolution.y == 0)
		return 0;

	//create several FileManager::file to hold various aspects of our level
	shared_ptr<FileManager::zipFile>		lvlFile(new FileManager::zipFile(filename));
	shared_ptr<FileManager::binaryFile>		rawFile(new FileManager::binaryFile("heightmap.raw"));
	shared_ptr<FileManager::iniFile>		attributesFile(new FileManager::iniFile("attributes.ini"));
	shared_ptr<FileManager::textFile>		objectsFile(new FileManager::textFile("objects.txt"));

	//initialize local variables
	unsigned int width	= info.mapResolution.x;
	unsigned int height = info.mapResolution.y;
	float maxHeight		= heights[0]+0.001;
	float minHeight		= heights[0];

	//find minimum and maximum heights
	for(int x=0;x<width;x++)
	{
		for(int z=0;z<height;z++)
		{
			maxHeight = max(maxHeight, heights[x+z*width]);
			minHeight = min(minHeight, heights[x+z*width]);
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
	attributesFile->bindings["heightmap"]["sizeX"] = lexical_cast<string>(info.mapSize.x);
	attributesFile->bindings["heightmap"]["sizeZ"] = lexical_cast<string>(info.mapSize.y);
	attributesFile->bindings["heightmap"]["foliageAmount"] = lexical_cast<string>(info.foliageAmount);

	attributesFile->bindings["level"]["nextLevel"] = info.nextLevel;

	if(info.shaderType == TERRAIN_ISLAND)		attributesFile->bindings["shaders"]["shaderType"] = "ISLAND";
	else if(info.shaderType == TERRAIN_SNOW)	attributesFile->bindings["shaders"]["shaderType"] = "SNOW";
	else if(info.shaderType == TERRAIN_DESERT)	attributesFile->bindings["shaders"]["shaderType"] = "DESERT";
	else										attributesFile->bindings["shaders"]["shaderType"] = "NONE";

	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		objectsFile->contents += "object\n{\n";
		objectsFile->contents += string("\ttype=") + objectTypeString(i->type) + "\n";
		objectsFile->contents += string("\tteam=") + lexical_cast<string>(i->team) + "\n";
		objectsFile->contents += string("\tspawnPos=(") + lexical_cast<string>(i->startloc.x) + "," + lexical_cast<string>(i->startloc.y) + "," + lexical_cast<string>(i->startloc.z) + ")\n";
		objectsFile->contents += "}\n";
	}
	//add all these files to the lvl (zip) file
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

	delete[] heights;
	heights = nullptr;

	attributesFile->readValue("heightmap", "resolutionX", info.mapResolution.x);
	attributesFile->readValue("heightmap", "resolutionY", info.mapResolution.y);
	attributesFile->readValue("heightmap", "minHeight", info.minHeight);
	attributesFile->readValue("heightmap", "maxHeight", info.maxHeight);
	attributesFile->readValue("heightmap", "sizeX", info.mapSize.x);
	attributesFile->readValue("heightmap", "sizeZ", info.mapSize.y);
	attributesFile->readValue("heightmap", "foliageAmount", info.foliageAmount);
	attributesFile->readValue("level", "nextLevel", info.nextLevel);



	string sType;
	attributesFile->readValue("shaders", "shaderType", sType);
	if(sType == "ISLAND")		info.shaderType = TERRAIN_ISLAND;
	else if(sType == "GRASS")	info.shaderType = TERRAIN_ISLAND;
	else if(sType == "SNOW")	info.shaderType = TERRAIN_SNOW;
	else if(sType == "DESERT")	info.shaderType = TERRAIN_DESERT;
	else						info.shaderType = TERRAIN_ISLAND; //grass is default

	if(info.mapResolution.x == 0 || info.mapResolution.y == 0)
	{
		return false;
	}
	heights = new float[info.mapResolution.x * info.mapResolution.y];
	memset(heights, 0, info.mapResolution.x * info.mapResolution.y * sizeof(float));
	for(int i = 0; i < info.mapResolution.x * info.mapResolution.y && i * 2 < rawFile->size; i++)
	{
		heights[i] = info.minHeight + ((float)*((unsigned short*)rawFile->contents + i)) * (info.maxHeight - info.minHeight) / USHRT_MAX;
	}

	parseObjectFile(objectsFile);

	return true;
}
bool LevelFile::parseObjectFile(shared_ptr<FileManager::textFile> f)
{
	if(!f) //make sure the file pointer is not null
	{
		return false;
	}

	string& str = f->contents;
	int pos=0;
	vector<Object>::iterator object=objects.end();

	auto readSubString = [str, &pos](const char* c)->bool
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

	//while(pos < str.size())
	//{
	//	if(readSubString("object"))
	//	{
	//		++info.numObjects;
	//		while(str[pos] != '\n' && pos < str.size())
	//			++pos;
	//	}
	//	else
	//	{
	//		while(str[pos] != '\n' && pos < str.size())
	//			++pos;
	//	}
	//	++pos;
	//}

	//try
	{
		pos = 0;
		enum {SEARHING,PARSING_OBJECT}state = SEARHING;
		while(pos < str.size())
		{
			if(state == SEARHING)
			{
				if(readSubString("object\n{\n"))
				{
					state = PARSING_OBJECT;
					objects.push_back(Object());	//add a new object
					object = (objects.end()-1);		//get an iterator to the last element
				}
				else
				{
					while(pos < str.size() && str[pos] != '\n')
						++pos;
					pos++;
				}
			}
			else if(state == PARSING_OBJECT)
			{
				if(readSubString("\ttype="))
				{
					string s = readLine();
					object->type = objectInfo.typeFromString(s);
				}
				else if(readSubString("\tteam="))
				{
					string s = readLine();
					object->team = lexical_cast<int>(s) - 1;
				}
				else if(readSubString("\tspawnPos="))
				{
					string s = readLine();
					object->startloc = lexical_cast<Vec3f>(s);
				}
				else if(readSubString("}\n"))
				{
					state = SEARHING;
				}
				else
				{
					pos++; //just continue until we reach something we recognize
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

	int w = min(info.mapResolution.x, info.mapResolution.y);
	if(w != 0 && heights != nullptr)
	{
		float maxHeight=heights[0]+0.001;
		float minHeight=heights[0];
		for(int x=0;x<info.mapResolution.x;x++)
		{
			for(int z=0;z<info.mapResolution.y;z++)
			{
				if(heights[x+z*info.mapResolution.x]>maxHeight) maxHeight=heights[x+z*info.mapResolution.x];
				if(heights[x+z*info.mapResolution.x]<minHeight) minHeight=heights[x+z*info.mapResolution.x];
			}
		}

		unsigned short* h = new unsigned short[w*w];
		for(int i=0;i<w*w;i++) h[i] = ((heights[(i%w)+(i/w)*info.mapResolution.x]-minHeight)/(maxHeight-minHeight)) * USHRT_MAX;
		world.initTerrain(h, w,Vec3f(0,minHeight,0),Vec3f(info.mapSize.x,maxHeight - minHeight,info.mapSize.y),info.shaderType,info.foliageAmount, info.shaderType==TERRAIN_DESERT?4:1);
	}

	bullets = world.newObject(new bulletCloud);
	particleManager.addEmitter(new particle::bulletEffect, bullets);

	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		if(i->type == PLAYER_PLANE && players.numPlayers() < humanPlayers)
		{
			auto id = world.newObject(new nPlane(i->startloc, i->startRot, objectInfo.typeFromString("MIRAGE"), i->team));
			players.addHumanPlayer(id);
		}
	}

	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		if(i->type & PLANE && i->type != PLAYER_PLANE)
		{
			if(players.numPlayers() < humanPlayers) //if the number of objects marked PLAYER_PLANE is less than the number of human players
			{
				auto id = world.newObject(new nPlane(i->startloc, i->startRot, objectInfo.typeFromString("MIRAGE"), i->team)); //keep creating objects as though they were marked PLAYER_PLANE
				players.addHumanPlayer(id);
			}
			else
			{
				auto id = world.newObject(new nPlane(i->startloc, i->startRot, i->type, i->team)); //create AI object
				players.addAIplayer(id);
			}
		}
		else if(i->type == AA_GUN)
		{
			world.newObject(new AAgun(i->startloc, i->startRot, i->type, i->team));
		}
		else if(i->type == SAM_BATTERY)
		{
			world.newObject(new SAMbattery(i->startloc, i->startRot, i->type, i->team));
		}
		else if(i->type == FLAK_CANNON)
		{
			world.newObject(new flakCannon(i->startloc, i->startRot, i->type, i->team));
		}
	}
	world.time.reset();
}
LevelFile::LevelFile(): heights(nullptr)
{
	header.magicNumber = 0;
	header.version = 0;
}
LevelFile::~LevelFile()
{
	delete[] heights;
}
