
#include "game.h"
#include "xml/tinyxml2.h"

using namespace tinyxml2;

ObjectInfo& objectInfo = ObjectInfo::getInstance();

bool ObjectInfo::loadObjectData(string filename)
{
	auto  getAttribute = [](XMLElement* element, const char* attribute)->string
	{
		const char* c = element->Attribute(attribute);
		string s = (c != NULL) ? string(c) : string("");
		return s;
	};
	auto getFloatAttribute = [](XMLElement* element, const char* attribute)->float //returns 0 on failure
	{
		float f;
		bool b = element->QueryFloatAttribute(attribute, &f) == XML_SUCCESS;
		if(!b)
			debugBreak();
		return b ? f : 0.0f;
	};

	XMLDocument doc;
	if(doc.LoadFile(filename.c_str()))
	{
		return false;
	}

	XMLElement* node				= nullptr;
	XMLElement* objectsNode			= doc.FirstChildElement("objects");
	if(!objectsNode) return false;

	unsigned int missileCount	= 0;
	unsigned int bombCount		= 0;
	unsigned int planeCount		= 0;
	unsigned int shipCount		= 0;
	if((node=objectsNode->FirstChildElement("weapons")) != nullptr && node->ToElement() != nullptr && (node=node->ToElement()->FirstChildElement()) != nullptr)
	{
		XMLElement* weaponElement = node->ToElement();
		while(weaponElement != nullptr)
		{
			if(strcmp(weaponElement->Value(), "missile") == 0)
			{
				string cmName = getAttribute(weaponElement, "collisionMesh");
				string mName = getAttribute(weaponElement, "model");

				if(++missileCount > MINOR_OBJECT_TYPE)	//to many missiles
				{
					debugBreak();
					continue;
				}
				
				shared_ptr<objectData> obj(new objectData);
				obj->type = MISSILE + missileCount;		
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(weaponElement, "name");
				objectMap[obj->type] = obj;	
			}
			else if(strcmp(weaponElement->Value(), "bomb") == 0)
			{
				string cmName = getAttribute(weaponElement, "collisionMesh");
				string mName = getAttribute(weaponElement, "model");

				if(++bombCount > MINOR_OBJECT_TYPE)	//to many bombs
				{
					debugBreak();
					continue;
				}

				shared_ptr<objectData> obj(new objectData);
				obj->type = BOMB + bombCount;
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(weaponElement, "name");
				objectMap[obj->type] = obj;
			}
			weaponElement = weaponElement->NextSiblingElement();
		}
	}
	if((node=objectsNode->FirstChildElement("AAA")) != nullptr && node->ToElement() != nullptr && (node=node->ToElement()->FirstChildElement()) != nullptr)
	{
		XMLElement* aaaElement = node->ToElement();
		while(aaaElement != nullptr)
		{
			if(strcmp(aaaElement->Value(), "SAM") == 0)
			{
				string cmName = getAttribute(aaaElement, "collisionMesh");
				string mName = getAttribute(aaaElement, "model");

				shared_ptr<objectData> obj(new objectData);
				obj->type = SAM_BATTERY;	
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(aaaElement, "name");
				obj->textName = getAttribute(aaaElement, "textName");
				objectMap[obj->type] = obj;	
				if(getAttribute(aaaElement,"placeable") == "true")
					placeableObjects.push_back(obj->type);
			}
			else if(strcmp(aaaElement->Value(), "AAgun") == 0)
			{
				string cmName = getAttribute(aaaElement, "collisionMesh");
				string mName = getAttribute(aaaElement, "model");

				shared_ptr<objectData> obj(new objectData);
				obj->type = AA_GUN;
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(aaaElement, "name");
				obj->textName = getAttribute(aaaElement, "textName");
				objectMap[obj->type] = obj;	
				if(getAttribute(aaaElement,"placeable") == "true")
					placeableObjects.push_back(obj->type);
			}
			else if(strcmp(aaaElement->Value(), "flakCannon") == 0)
			{
				string cmName = getAttribute(aaaElement, "collisionMesh");
				string mName = getAttribute(aaaElement, "model");

				shared_ptr<objectData> obj(new objectData);
				obj->type = FLAK_CANNON;
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(aaaElement, "name");
				obj->textName = getAttribute(aaaElement, "textName");
				objectMap[obj->type] = obj;	
				if(getAttribute(aaaElement,"placeable") == "true")
					placeableObjects.push_back(obj->type);
			}
			aaaElement = aaaElement->NextSiblingElement();
		}
	}
	if((node=objectsNode->FirstChildElement("ships")) != nullptr && node->ToElement() != nullptr && (node=node->ToElement()->FirstChildElement()) != nullptr)
	{
		XMLElement* shipElement = node->ToElement();
		while(shipElement != nullptr)
		{
			if(strcmp(shipElement->Value(), "ship") == 0)
			{
				if(++shipCount > MINOR_OBJECT_TYPE)	//to many bombs
				{
					debugBreak();
					continue;
				}

				string cmName = getAttribute(shipElement, "collisionMesh");
				string mName = getAttribute(shipElement, "model");

				shared_ptr<objectData> obj(new objectData);
				obj->type = SHIP + shipCount;	
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(shipElement, "name");
				obj->textName = getAttribute(shipElement, "textName");
				objectMap[obj->type] = obj;	
				if(getAttribute(shipElement,"placeable") == "true")
					placeableObjects.push_back(obj->type);
			}
			shipElement = shipElement->NextSiblingElement();
		}
	}
	if((node=objectsNode->FirstChildElement("planes")) != nullptr && node->ToElement() != nullptr && (node=node->ToElement()->FirstChildElement()) != nullptr)
	{
		XMLElement* planeElement = node->ToElement();
		while(planeElement != nullptr)
		{
			if(strcmp(planeElement->Value(), "plane") == 0)
			{
				if(++planeCount > MINOR_OBJECT_TYPE || planeCount==PLAYER_PLANE)//to many planes
				{
					debugBreak();
					continue;
				}

				string cmName = getAttribute(planeElement, "collisionMesh");
				string mName = getAttribute(planeElement, "model");

				shared_ptr<planeObjectData> obj(new planeObjectData);
				obj->type = PLANE + planeCount;
				obj->collisionMeshFile = cmName != "" ? fileManager.loadObjFile(cmName, true) : nullptr;
				obj->meshFile = mName != "" ? fileManager.loadObjFile(mName, true) : nullptr;
				obj->name = getAttribute(planeElement, "name");
				obj->textName = getAttribute(planeElement, "textName");

				if(planeElement->QueryFloatAttribute("cameraDistance", &obj->cameraDistance) != XML_SUCCESS)
					obj->cameraDistance = 30.0;



				XMLElement* planeProperties = planeElement->FirstChildElement();
				while(planeProperties != nullptr)
				{
					if(strcmp(planeProperties->Value(), "g_offset") == 0)
					{
						Vec3f v(getFloatAttribute(planeProperties,"x"), getFloatAttribute(planeProperties,"y"), getFloatAttribute(planeProperties,"z"));
						obj->machineGuns.push_back(v);
						if(getAttribute(planeProperties,"mirror") == "true")
							obj->machineGuns.push_back(Vec3f(-v.x,v.y,v.z));
					}
					else if(strcmp(planeProperties->Value(), "e_offset") == 0)
					{
						planeObjectData::engine e;
						e.offset = Vec3f(getFloatAttribute(planeProperties,"x"), getFloatAttribute(planeProperties,"y"), getFloatAttribute(planeProperties,"z"));
						if(planeProperties->QueryFloatAttribute("radius", &e.radius) != XML_SUCCESS)
							e.radius = 1.0;

						if(e.radius == 0.0f)
							e.radius = 1.0;

						obj->engines.push_back(e);

						if(getAttribute(planeProperties,"mirror") == "true")
						{
							e.offset.x *= -1;
							obj->engines.push_back(e);
						}
					}
					else if(strcmp(planeProperties->Value(), "w_offset") == 0)
					{
						planeObjectData::hardpoint h;
						h.mType = typeFromString(getAttribute(planeProperties, "weapon"));
						h.offset = Vec3f(getFloatAttribute(planeProperties,"x"), getFloatAttribute(planeProperties,"y"), getFloatAttribute(planeProperties,"z"));
						obj->hardpoints.push_back(h);
						if(getAttribute(planeProperties,"mirror") == "true")
						{
							h.offset.x *= -1;
							obj->hardpoints.push_back(h);
						}
					}
					planeProperties = planeProperties->NextSiblingElement();
				}

				objectMap[obj->type] = obj;	
				if(getAttribute(planeElement,"placeable") == "true")
					placeableObjects.push_back(obj->type);
			}
			planeElement = planeElement->NextSiblingElement();
		}
		defaultPlane = typeFromString(getAttribute(objectsNode->FirstChildElement("planes"), "default"));
		if(!(defaultPlane & PLANE))
		{
			defaultPlane = 0;
			for(auto i=objectMap.begin(); i!=objectMap.end(); i++)
			{
				if(i->first & PLANE)
				{
					defaultPlane = i->first;
					break;
				}
			}
		}
	}
	return true;
}
int ObjectInfo::loadObjectMeshes()
{
	int numLeft=0;
	bool timeUp=false;
	double startTime = GetTime();
	for(auto i = objectMap.begin(); i != objectMap.end(); i++)
	{
		if(i->second->meshFile)
		{
			if(!i->second->meshFile->complete() || timeUp)
			{
				numLeft++;
			}
			else if(i->second->meshFile->valid())
			{
				i->second->mesh = sceneManager.createMesh(i->second->meshFile);
				i->second->meshFile.reset();
				physics.setCollsionBounds(i->first, i->second->mesh->getBoundingSphere());
				if(GetTime() - startTime > 33.3)
					timeUp = true;
			}
			else
			{
				debugBreak();
				i->second->meshFile.reset();
			}
		}

		if(i->second->collisionMeshFile)
		{
			if(!i->second->collisionMeshFile->complete() || timeUp)
			{
				numLeft++;
			}
			else if(i->second->collisionMeshFile->valid())
			{
				if(!i->second->collisionMeshFile->materials.empty())
				{
					vector<Vec3f> vertices;
					for(auto v=i->second->collisionMeshFile->vertices.begin(); v!=i->second->collisionMeshFile->vertices.end(); v++)
						vertices.push_back(v->position);
					physics.setCollsionBounds(i->first, i->second->collisionMeshFile->boundingSphere, vertices, i->second->collisionMeshFile->materials[0].indices);
				}
				i->second->collisionMeshFile.reset();
				if(GetTime() - startTime > 33.3)
					timeUp = true;
			}
			else
			{
				debugBreak();
				i->second->collisionMeshFile.reset();
			}
		}
	}
	return numLeft;
}
objectType ObjectInfo::typeFromString(string s)
{
	if(s == "PLAYER_PLANE")
		return PLAYER_PLANE;

	for(auto i=objectMap.begin(); i!=objectMap.end(); i++)
	{
		if(i->second->name == s)
		{
			return i->first;
		}
	}
	return 0;
}
string ObjectInfo::typeString(objectType t)
{
	if(t == PLAYER_PLANE)
		return "PLAYER_PLANE";

	auto i = objectMap.find(t);
	return i!=objectMap.end() ? i->second->name : "";
}
string ObjectInfo::textName(objectType t)
{
	if(t == PLAYER_PLANE)
		return "<player plane>";

	auto i = objectMap.find(t);
	if(i!=objectMap.end())
	{
		return i->second->textName != "" ? i->second->textName : i->second->name;
	}
	else
	{
		return "";
	}
}
shared_ptr<ObjectInfo::objectData> ObjectInfo::operator[] (objectType t)
{
	auto i = objectMap.find(t);
	return i!=objectMap.end() ? i->second : shared_ptr<objectData>();
}
const ObjectInfo::planeObjectData& ObjectInfo::planeStats(objectType t)
{
	if(!(t & PLANE))
	{
		debugBreak();
		t = t & MINOR_OBJECT_TYPE | PLANE;
	}
	return *static_pointer_cast<planeObjectData>(objectMap[t]);
}
objectType objectTypeFromString(string s)
{
	return objectInfo.typeFromString(s);
}
string objectTypeString(objectType t)
{
	return objectInfo.typeString(t);
}