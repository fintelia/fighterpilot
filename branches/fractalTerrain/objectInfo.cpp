
#include "game.h"
#include "xml/tinyxml2.h"

using namespace tinyxml2;

ObjectInfo& objectInfo = ObjectInfo::getInstance();

shared_ptr<SceneManager::meshInstance> ObjectInfo::objectData::newMeshInstance(Mat4f transformation) const
{
	if(!mesh.expired())
	{
		return sceneManager.newMeshInstance(mesh.lock(), transformation);
	}
	else
	{
		return nullptr;
	}
}
shared_ptr<CollisionManager::collisionInstance> ObjectInfo::objectData::newCollisionInstance(Mat4f transformation) const
{
	if(!collisionMesh.expired())
	{
		return collisionManager.newCollisionInstance(collisionMesh.lock(), transformation);
	}
	else
	{
		return nullptr;
	}
}
shared_ptr<SceneManager::meshInstance> ObjectInfo::aaaObjectData::turret::newMeshInstance(weak_ptr<SceneManager::meshInstance> parentMeshInstance) const
{
	if(!mesh.expired())
	{
		return sceneManager.newChildMeshInstance(mesh.lock(), parentMeshInstance);
	}
	else
	{
		return nullptr;
	}
}
shared_ptr<SceneManager::meshInstance> ObjectInfo::aaaObjectData::cannon::newMeshInstance(weak_ptr<SceneManager::meshInstance> parentMeshInstance) const
{
	if(!mesh.expired())
	{
		return sceneManager.newChildMeshInstance(mesh.lock(), parentMeshInstance);
	}
	else
	{
		return nullptr;
	}
}
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
			shared_ptr<objectData> obj(new objectData);
			if(strcmp(weaponElement->Value(), "missile") == 0 && ++missileCount <= MINOR_OBJECT_TYPE)
			{
				obj->type = MISSILE + missileCount;	
			}
			else if(strcmp(weaponElement->Value(), "bomb") == 0 && ++bombCount <= MINOR_OBJECT_TYPE)
			{
				obj->type = BOMB + bombCount;
			}
			else
			{
				debugBreak(); //unrecongized type, or too many objects for selected type
				continue;
			}
			obj->name = getAttribute(weaponElement, "name");
			obj->meshFilename = getAttribute(weaponElement, "model");
			if(obj->meshFilename != "")				assetLoader.addModel(obj->meshFilename);
			obj->collisionMeshFilename = getAttribute(weaponElement, "collisionMesh");
			if(obj->collisionMeshFilename != "")	assetLoader.addCollisionMesh(obj->collisionMeshFilename, obj->type);
			objectMap[obj->type] = obj;

			weaponElement = weaponElement->NextSiblingElement();
		}
	}
	if((node=objectsNode->FirstChildElement("AAA")) != nullptr && node->ToElement() != nullptr && (node=node->ToElement()->FirstChildElement()) != nullptr)
	{
		XMLElement* aaaElement = node->ToElement();
		while(aaaElement != nullptr)
		{
			shared_ptr<aaaObjectData> obj(new aaaObjectData);
			if(strcmp(aaaElement->Value(), "SAM") == 0)
			{
				obj->type = SAM_BATTERY;				
			}
			else if(strcmp(aaaElement->Value(), "AAgun") == 0)
			{
				obj->type = AA_GUN;
			}
			else if(strcmp(aaaElement->Value(), "flakCannon") == 0)
			{
				obj->type = FLAK_CANNON;
			}
			else
			{
				debugBreak(); //unrecongized type
				continue;
			}

			obj->meshFilename = getAttribute(aaaElement, "model");
			if(obj->meshFilename != "")				assetLoader.addModel(obj->meshFilename);
			obj->collisionMeshFilename = getAttribute(aaaElement, "collisionMesh");
			if(obj->collisionMeshFilename != "")	assetLoader.addCollisionMesh(obj->collisionMeshFilename, obj->type);

			obj->name = getAttribute(aaaElement, "name");
			obj->textName = getAttribute(aaaElement, "textName");



			
			XMLElement* aaaProperties = aaaElement->FirstChildElement();
			while(aaaProperties != nullptr)
			{
				if(strcmp(aaaProperties->Value(), "turret") == 0)
				{
					aaaObjectData::turret t;
					t.meshFilename = getAttribute(aaaProperties, "model");
					if(t.meshFilename != "") assetLoader.addModel(t.meshFilename);
					t.rotationCenter = Vec3f(getFloatAttribute(aaaProperties,"rotationOffsetX"), 0, getFloatAttribute(aaaProperties,"rotationOffsetZ"));
					obj->turrets.push_back(t);
				}
				else if(strcmp(aaaProperties->Value(), "cannon") == 0)
				{
					aaaObjectData::cannon c;
					c.meshFilename = getAttribute(aaaProperties, "model");
					if(c.meshFilename != "") assetLoader.addModel(c.meshFilename);
					c.rotationCenter = Vec3f(getFloatAttribute(aaaProperties,"rotationOffsetX"),getFloatAttribute(aaaProperties,"rotationOffsetY"), getFloatAttribute(aaaProperties,"rotationOffsetZ"));
					obj->cannons.push_back(c);
				}
				aaaProperties = aaaProperties->NextSiblingElement();
			}


			objectMap[obj->type] = obj;	
			if(getAttribute(aaaElement,"placeable") == "true")
			{
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
			shared_ptr<objectData> obj(new objectData);
			if(strcmp(shipElement->Value(), "ship") == 0 && ++shipCount <= MINOR_OBJECT_TYPE)
			{
				obj->type = SHIP + shipCount;
			}
			else
			{
				debugBreak();
				continue;
			}

			obj->meshFilename = getAttribute(shipElement, "model");
			if(obj->meshFilename != "")				assetLoader.addModel(obj->meshFilename);
			obj->collisionMeshFilename = getAttribute(shipElement, "collisionMesh");
			if(obj->collisionMeshFilename != "")	assetLoader.addCollisionMesh(obj->collisionMeshFilename, obj->type);

			obj->name = getAttribute(shipElement, "name");
			obj->textName = getAttribute(shipElement, "textName");
			objectMap[obj->type] = obj;	
			if(getAttribute(shipElement,"placeable") == "true")
			{
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
				shared_ptr<planeObjectData> obj(new planeObjectData);

				obj->type = PLANE + planeCount;
				obj->meshFilename = getAttribute(planeElement, "model");
				if(obj->meshFilename != "")				assetLoader.addModel(obj->meshFilename);
				obj->collisionMeshFilename = getAttribute(planeElement, "collisionMesh");
				if(obj->collisionMeshFilename != "")	assetLoader.addCollisionMesh(obj->collisionMeshFilename, obj->type);


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
				{
					placeableObjects.push_back(obj->type);
				}
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
void ObjectInfo::linkObjectMeshes()
{
	for(auto i = objectMap.begin(); i != objectMap.end(); i++)
	{
		if(i->second->collisionMeshFilename != "")
		{
		//	shared_ptr<CollisionManager::collisionBounds> mesh  //needed since i->second->mesh is actually a weak ptr
			i->second->collisionMesh = dataManager.getCollisionBounds(i->second->collisionMeshFilename);
		}
		if(i->second->meshFilename != "")
		{
			shared_ptr<SceneManager::mesh> mesh = dataManager.getModel(i->second->meshFilename); //needed since i->second->mesh is actually a weak ptr
			i->second->mesh = mesh;
			if(i->second->collisionMesh.expired())
			{
				dataManager.addCollisionBounds(i->second->meshFilename + "_boundingSphere", mesh->boundingSphere);
				i->second->collisionMesh = dataManager.getCollisionBounds(i->second->meshFilename + "_boundingSphere");
			}
			//collisionManager.setCollsionBounds(i->first, mesh->boundingSphere); //only changes bounding volume if a collision mesh was not loaded
		}
		if(i->second->type & ANTI_AIRCRAFT_ARTILLARY)
		{
			auto& turrets = static_pointer_cast<aaaObjectData>(i->second)->turrets;
			for(auto t = turrets.begin(); t != turrets.end(); t++)
			{
				shared_ptr<SceneManager::mesh> mesh = dataManager.getModel(t->meshFilename); //needed since i->second->mesh is actually a weak ptr
				t->mesh = mesh;
			}
			auto& cannons = static_pointer_cast<aaaObjectData>(i->second)->cannons;
			for(auto c = cannons.begin(); c != cannons.end(); c++)
			{
				shared_ptr<SceneManager::mesh> mesh = dataManager.getModel(c->meshFilename); //needed since i->second->mesh is actually a weak ptr
				c->mesh = mesh;
			}
		}
	}
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
shared_ptr<const ObjectInfo::planeObjectData> ObjectInfo::planeData(objectType t)
{
	if(!(t & PLANE))
	{
		debugBreak();
		t = (t & MINOR_OBJECT_TYPE) | PLANE;
	}
	return static_pointer_cast<planeObjectData>(objectMap[t]);
}
shared_ptr<const ObjectInfo::aaaObjectData> ObjectInfo::aaaData(objectType t)
{
	if(!(t & ANTI_AIRCRAFT_ARTILLARY))
	{
		debugBreak();
		t = (t & MINOR_OBJECT_TYPE) | ANTI_AIRCRAFT_ARTILLARY;
	}
	return static_pointer_cast<aaaObjectData>(objectMap[t]);
}
objectType objectTypeFromString(string s)
{
	return objectInfo.typeFromString(s);
}
string objectTypeString(objectType t)
{
	return objectInfo.typeString(t);
}
