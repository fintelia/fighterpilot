
#include "game.h"
LevelFile::Object::Object():type(0), team(NEUTRAL), startloc(), startRot()
{

}
bool LevelFile::Trigger::checkComparison(int value) const
{
	if(comparison == ALWAYS)				return true;
	else if(comparison == NEVER)			return false;
	else if(comparison == LESS)				return value < comparisonValue;
	else if(comparison == LESS_EQUAL)		return value <= comparisonValue;
	else if(comparison == EQUAL)			return value == comparisonValue;
	else if(comparison == GREATER_EQUAL)	return value >= comparisonValue;
	else if(comparison == GREATER)			return value > comparisonValue;
	
	debugBreak();
	return false;
}

bool LevelFile::saveZIP(string filename)
{
	shared_ptr<FileManager::zipFile> lvlFile(new FileManager::zipFile(filename));

	for (unsigned int layer = 0; layer < clipMap->getNumLayers(); layer++){
		string rawFilename = "heightmap" + lexical_cast<string>(layer) + ".raw";

		auto rawFile = std::make_shared<FileManager::binaryFile>(rawFilename);
		unsigned int width = clipMap->getLayerResolution();
		unsigned int height = clipMap->getLayerResolution();
		float maxHeight = clipMap->getMinHeight();
		float minHeight = clipMap->getMaxHeight();

		//store heightmap data in our "heightmap.raw" file
		rawFile->size = 2 * width * height;
		rawFile->contents = new unsigned char[rawFile->size];
		memset(rawFile->contents, 0, rawFile->size);
		for (unsigned int x = 0; x < width; x++)
		{
			for (unsigned int z = 0; z < height; z++)
			{
				float h = clipMap->getHeight(layer, x, z);
				if (h > maxHeight) debugBreak();
				if (h < minHeight) debugBreak();

				*((unsigned short*)rawFile->contents + (x + z*width)) = USHRT_MAX * (h - minHeight) / (maxHeight - minHeight);
			}
		}
		lvlFile->files[rawFilename] = rawFile;
	}

	//place information about our level in attributes.ini
	auto attributesFile = std::make_shared<FileManager::iniFile>("attributes.ini");

	attributesFile->bindings["heightmap"]["resolution"] = lexical_cast<string>(clipMap->getLayerResolution());
	attributesFile->bindings["heightmap"]["minHeight"] = lexical_cast<string>(clipMap->getMinHeight());
	attributesFile->bindings["heightmap"]["maxHeight"] = lexical_cast<string>(clipMap->getMaxHeight());
	attributesFile->bindings["heightmap"]["numLayers"] = lexical_cast<string>(clipMap->getNumLayers());
	attributesFile->bindings["heightmap"]["sideLength"] = lexical_cast<string>(clipMap->getSideLength());
	attributesFile->bindings["heightmap"]["foliageDensity"] = lexical_cast<string>(info.foliageDensity);
	attributesFile->bindings["level"]["nextLevel"] = info.nextLevel;
	attributesFile->bindings["level"]["night"] = info.night ? "true" : "false";
	lvlFile->files["attributes.ini"] = attributesFile;

	auto objectsFile = std::make_shared<FileManager::textFile>("objects.txt");
	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		objectsFile->contents += "object\n{\n";
		objectsFile->contents += string("\ttype=") + objectInfo.typeString(i->type) + "\n";
		objectsFile->contents += string("\tteam=") + lexical_cast<string>(i->team) + "\n";
		objectsFile->contents += string("\tspawnPos=(") + lexical_cast<string>(i->startloc.x) + "," + lexical_cast<string>(i->startloc.y) + "," + lexical_cast<string>(i->startloc.z) + ")\n";
		objectsFile->contents += "}\n";
	}
	lvlFile->files["objects.txt"] = objectsFile;

	//attempt to write the lvl file and return whether we were successful
	return fileManager.writeFile(lvlFile);
}
bool LevelFile::loadZIP(string filename)
{
	auto f = fileManager.loadFile<FileManager::zipFile>(filename);

	auto aFile = f->files.find("attributes.ini");
	auto oFile = f->files.find("objects.txt");

	if(aFile == f->files.end())
	{
		messageBox("attributes.ini file not found!");
		return false;
	}
	if(oFile == f->files.end())
	{
		messageBox("objects.txt file not found!");
		return false;
	}

	shared_ptr<FileManager::iniFile> attributesFile(dynamic_pointer_cast<FileManager::iniFile>(aFile->second));
	shared_ptr<FileManager::textFile> objectsFile(dynamic_pointer_cast<FileManager::textFile>(oFile->second));

	if(attributesFile->getValue<int>("heightmap", "resolutionX") <= 2 || attributesFile->getValue<int>("heightmap", "resolutionY") <= 2)
	{
		messageBox("invalid heightmap size!");
		return false;
	}

	clipMap.reset();
	objects.clear();
	regions.clear();
	info = Info();

	unsigned int resolution, numLayers;
	attributesFile->readValue("heightmap", "resolution", resolution);
	attributesFile->readValue("heightmap", "numLayers", numLayers);

	float minHeight, maxHeight;
	attributesFile->readValue("heightmap", "minHeight", minHeight);
	attributesFile->readValue("heightmap", "maxHeight", maxHeight);

	float sideLength;
	attributesFile->readValue("heightmap", "sideLength", sideLength);
	attributesFile->readValue("heightmap", "foliageDensity", info.foliageDensity);
	attributesFile->readValue("level", "nextLevel", info.nextLevel);

	info.night = attributesFile->getValue<string>("level","night","false") == "true";

	vector<unique_ptr<float[]>> layers;
	for (unsigned int layer = 0; layer < numLayers; layer++){
		layers.emplace_back(new float[resolution * resolution]);

		auto rFile = f->files.find("heightmap" + lexical_cast<string>(layer) + ".raw");
		if (rFile == f->files.end())
		{
			messageBox("heightmap.raw file not found!");
			return false;
		}
		
		shared_ptr<FileManager::binaryFile>	rawFile(dynamic_pointer_cast<FileManager::binaryFile>(rFile->second));
		for (int i = 0; i < resolution * resolution && i * 2 < rawFile->size; i++)
		{
			layers[layer][i] = minHeight + ((float)*((unsigned short*)rawFile->contents + i)) * (maxHeight - minHeight) / USHRT_MAX;
		}
	}
	clipMap = std::make_unique<Terrain::ClipMap>(sideLength, resolution, layers);

	debugAssert(isPowerOfTwo(resolution - 1));

	if(!parseObjectFile(objectsFile))
	{
		messageBox("Failed to parse objects file!");
		return false;
	}

	return true;
}
bool LevelFile::parseObjectFile(shared_ptr<FileManager::textFile> f) //TODO: add improved error handling and reporting
{
	if(!f) //make sure the file pointer is not null
	{
		return false;
	}

	string& str = f->contents;
	int pos=0;
	vector<Object>::iterator object=objects.end();
	vector<Trigger>::iterator trigger=triggers.end();
	vector<Path>::iterator path=paths.end();

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

#if !defined(_DEBUG) || !defined(VISUAL_STUDIO)
	try
	{
#endif
		pos = 0;
		enum {SEARHING,PARSING_OBJECT,PARSING_TRIGGER,PARSING_PATH}state = SEARHING;
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
				else if(readSubString("trigger\n{\n"))
				{
					state = PARSING_TRIGGER;
					triggers.push_back(Trigger());	//add a new trigger
					trigger = (triggers.end()-1);	//get an iterator to the last element
				}
				else if(readSubString("path\n{\n"))
				{
					state = PARSING_PATH;
					paths.push_back(Path());	//add a new trigger
					path = (paths.end()-1);	//get an iterator to the last element
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
					object->team = lexical_cast<int>(s);
				}
				else if(readSubString("\tspawnPos="))
				{
					string s = readLine();
					object->startloc = lexical_cast<Vec3f>(s);
				}
				else if(readSubString("\tspawnAngle="))
				{
					string s = readLine();
					Angle angle = lexical_cast<float>(s) * PI / 180.0;
					object->startRot = Quat4f(Vec3f(0,1,0),angle);
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
			else if(state == PARSING_TRIGGER)
			{
				if(readSubString("\tconditionType="))
				{
					string s = readLine();
	
					vector<string> parameters;
					int openParen=s.find_first_of('('),
						closeParen=s.find_last_of(')');

					debugAssert(openParen <= closeParen); // close parenthesis precedes open parenthesis
					
					string conditionName=s.substr(0,openParen);
					
					if(openParen != s.npos && closeParen != s.npos && openParen+1 < closeParen)
					{
						string parametersString = s.substr(openParen+1, closeParen-openParen-1);
						boost::split(parameters,parametersString,boost::is_any_of(","));
					}

					if(conditionName == "NUM_OBJECTS")
						trigger->condition = shared_ptr<Trigger::Condition>(new Trigger::Condition(Trigger::Condition::NUM_OBJECTS));
					else if(conditionName == "NUM_OBJECTS_ON_TEAM")
						trigger->condition = shared_ptr<Trigger::Condition>(new Trigger::ConditionNumObjectsOnTeam(lexical_cast<int>(parameters[0])));
					else
						debugBreak(); //conditionName not recognized...
				}
				else if(readSubString("\tconditionComparison="))
				{
					string s = readLine();
					if(s == "LESS")					trigger->comparison = Trigger::LESS;
					else if(s == "LESS_EQUAL")		trigger->comparison = Trigger::LESS_EQUAL;
					else if(s == "EQUAL")			trigger->comparison = Trigger::EQUAL;
					else if(s == "GREATER_EQUAL")	trigger->comparison = Trigger::GREATER_EQUAL;
					else if(s == "GREATER")			trigger->comparison = Trigger::GREATER;
					else if(s == "NEVER")			trigger->comparison = Trigger::NEVER;
					else if(s == "ALWAYS")			trigger->comparison = Trigger::ALWAYS;
					else
					{
						debugBreak();	//condition not recognized...
						trigger->comparison = Trigger::NEVER;
					}
				}
				else if(readSubString("\tconditionValue="))
				{
					string s = readLine();
					trigger->comparisonValue = lexical_cast<int>(s);
				}
				else if(readSubString("\taction="))
				{
					string s = readLine();

					vector<string> parameters;
					int openParen=s.find_first_of('('),
						closeParen=s.find_last_of(')');

					debugAssert(openParen <= closeParen); // close parenthesis precedes open parenthesis
					
					string actionName=s.substr(0,openParen);
					
					if(openParen != s.npos && closeParen != s.npos && openParen+1 < closeParen)
					{
						string parametersString = s.substr(openParen+1, closeParen-openParen-1);
						boost::split(parameters,parametersString,boost::is_any_of(","));
					}

					if(actionName == "START_PATH" && parameters.size() >= 2)
						trigger->actions.push_back(shared_ptr<Trigger::Action>(new Trigger::ActionStartPath(lexical_cast<int>(parameters[0]), lexical_cast<int>(parameters[1]))));
					else
						debugBreak(); //unrecognized action or too few parameters

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
			else if(state == PARSING_PATH)
			{
				if(readSubString("\twaypoint="))
				{
					string s = readLine();
					int pipe = s.find_first_of('|');
					Vec3f position = lexical_cast<Vec3f>(s.substr(0,pipe));
					float time = lexical_cast<float>(s.substr(pipe+1,s.npos));
					path->waypoints.push_back(Path::waypoint(position,time));
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
#if !defined(_DEBUG) || !defined(VISUAL_STUDIO)
	}
	catch(...)
	{
		debugBreak();
		return false;
	}
#endif
}
void LevelFile::initializeWorld(unsigned int humanPlayers) const
{
	players.resetPlayers();

	debugAssert(clipMap->getNumLayers() != 0 && clipMap->getLayerResolution() != 0 && clipMap->getSideLength() != 0)

	Circle<float> bounds{Vec2f(0,0), clipMap->getSideLength() / (2 << clipMap->getNumLayers())};
	world->setBounds(bounds);

	bullets = world->newObject(new bulletCloud);
	particleManager.addEmitter(new particle::bulletEffect, bullets);

	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		if(i->type == PLAYER_PLANE && players.numPlayers() < humanPlayers)
		{
			auto id = world->newObject(new plane(i->startloc, i->startRot, objectInfo.getDefaultPlane(), i->team));
			players.addHumanPlayer(id);
		}
	}

	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		if(i->type & PLANE && i->type != PLAYER_PLANE)
		{
			if(players.numPlayers() == 0) //if the number of objects marked PLAYER_PLANE is less than the number of human players
			{
				auto id = world->newObject(new plane(i->startloc, i->startRot, objectInfo.getDefaultPlane(), i->team)); //keep creating objects as though they were marked PLAYER_PLANE
				players.addHumanPlayer(id);
			}
			else
			{
				auto id = world->newObject(new plane(i->startloc, i->startRot, i->type, i->team)); //create AI object
				players.addAIplayer(id);
			}
		}
		else if(i->type == AA_GUN)
		{
			world->newObject(new AAgun(i->startloc, i->startRot, i->type, i->team));
		}
		else if(i->type == SAM_BATTERY)
		{
			world->newObject(new SAMbattery(i->startloc, i->startRot, i->type, i->team));
		}
		else if(i->type == FLAK_CANNON)
		{
			world->newObject(new flakCannon(i->startloc, i->startRot, i->type, i->team));
		}
		else if(i->type & SHIP)
		{
			world->newObject(new ship(i->startloc, i->startRot, i->type, i->team));
		}
	}

	if(players.numPlayers() > 0 && players.numPlayers() < humanPlayers) //add additional human players if necessary
	{
		for(int i=players.numPlayers(); i<humanPlayers; i++)
		{
			shared_ptr<plane> ally = players[players.numPlayers()-1]->getObject();
			auto id = world->newObject(new plane(ally->position+ally->rotation * Vec3f(-75, 0, -50), ally->rotation, ally->type, ally->team));
			players.addHumanPlayer(id);
		}
	}

	world->time.reset();
}
bool LevelFile::checkValid()
{
	if(objects.size() == 0)
	{
		messageBox("Error: no objects found");
		return false;
	}

	bool playerObjectFound = false;
	for(auto i = objects.begin(); i != objects.end(); i++)
	{
		playerObjectFound = playerObjectFound || (i->type & PLANE);
	}
	if(!playerObjectFound)
	{
		messageBox("Error: No player object found");
		return false;
	}
	return true;
}