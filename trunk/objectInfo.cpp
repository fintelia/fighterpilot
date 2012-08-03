
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
		return b ? f : 0;
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

	if((node=objectsNode->FirstChildElement("weapons")) != nullptr && node->ToElement() != nullptr && (node=node->ToElement()->FirstChildElement()) != nullptr)
	{
		XMLElement* weaponElement = node->ToElement();
		while(weaponElement != nullptr)
		{
			if(strcmp(weaponElement->Value(), "missile") == 0)
			{
				if(++missileCount > MINOR_OBJECT_TYPE)	//to many missiles
				{
					debugBreak();
					continue;
				}
				
				shared_ptr<objectData> obj(new objectData);
				obj->type = MISSILE + missileCount;		
				obj->bounds = nullptr;
				obj->mesh = nullptr;
				obj->name = getAttribute(weaponElement, "name");
				objectMap[obj->type] = obj;	
			}
			else if(strcmp(weaponElement->Value(), "bomb") == 0)
			{
				if(++bombCount > MINOR_OBJECT_TYPE)	//to many bombs
				{
					debugBreak();
					continue;
				}

				shared_ptr<objectData> obj(new objectData);
				obj->type = BOMB + bombCount;
				obj->bounds = nullptr;
				obj->mesh = nullptr;
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
				shared_ptr<objectData> obj(new objectData);
				obj->type = SAM_BATTERY;	
				obj->bounds = nullptr;
				obj->mesh = nullptr;
				obj->name = getAttribute(aaaElement, "name");
				objectMap[obj->type] = obj;	
			}
			else if(strcmp(aaaElement->Value(), "AAgun") == 0)
			{
				if(++bombCount > MINOR_OBJECT_TYPE)	//to many bombs
				{
					debugBreak();
					continue;
				}

				shared_ptr<objectData> obj(new objectData);
				obj->type = AA_GUN;
				obj->bounds = nullptr;
				obj->mesh = nullptr;
				obj->name = getAttribute(aaaElement, "name");
				objectMap[obj->type] = obj;	
			}
			else if(strcmp(aaaElement->Value(), "flakCannon") == 0)
			{
				shared_ptr<objectData> obj(new objectData);
				obj->type = FLAK_CANNON;
				obj->bounds = nullptr;
				obj->mesh = nullptr;
				obj->name = getAttribute(aaaElement, "name");
				objectMap[obj->type] = obj;	
			}
			aaaElement = aaaElement->NextSiblingElement();
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
				obj->bounds = nullptr;
				obj->mesh = nullptr;
				obj->name = getAttribute(planeElement, "name");


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
						Vec3f v(getFloatAttribute(planeProperties,"x"), getFloatAttribute(planeProperties,"y"), getFloatAttribute(planeProperties,"z"));
						obj->engines.push_back(v);
						if(getAttribute(planeProperties,"mirror") == "true")
							obj->engines.push_back(Vec3f(-v.x,v.y,v.z));
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
			}
			planeElement = planeElement->NextSiblingElement();
		}
	}
	return true;
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
	auto i = objectMap.find(t);
	return i!=objectMap.end() ? i->second->name : "";
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
//	if(s=="f12")				return F12;
//	if(s=="f16")				return F16;
//	if(s=="f18")				return F18;
//	if(s=="f22")				return F22;
//	if(s=="UAV")				return UAV;
//	if(s=="b2")					return B2;
//	if(s=="mirage")				return MIRAGE;
//	if(s=="j37")				return J37;
//	if(s=="missile1")			return BOMB1;
//	if(s=="bomb1")				return BOMB1;
//	if(s=="missile2")			return BOMB2;
//	if(s=="bomb2")				return BOMB2;
//	if(s=="missile3")			return MISSILE3;
//	if(s=="missile4")			return MISSILE4;
//	if(s=="r550 magic missile")	return R550_MAGIC_MISSILE;
//	if(s=="super530 missile")	return SUPER_530_MISSILE;
//	if(s=="j37 missile")		return J37_MISSILE;
//	if(s=="SAM missile")		return SAM_MISSILE;
//	if(s=="flak missile")		return FLAK_MISSILE;
//	if(s=="AA gun")				return AA_GUN;
//	if(s=="SAM battery")		return SAM_BATTERY;
//	if(s=="flak cannon")		return FLAK_CANNON;
//	if(s=="bullet cloud")		return BULLET_CLOUD;
//	if(s=="PLAYER_PLANE")		return PLAYER_PLANE;
////	debugBreak();
////	cout << "objectType: " << s << " not found" << endl;
//	return 0;
//}
string objectTypeString(objectType t)
{
	return objectInfo.typeString(t);
}
//	if(t==F12)					return "f12";
//	if(t==F16)					return "f16";
//	if(t==F18)					return "f18";
//	if(t==F22)					return "f22";
//	if(t==UAV)					return "UAV";
//	if(t==B2)					return "b2";
//	if(t==MIRAGE)				return "mirage";
//	if(t==J37)					return "j37";
//	if(t==MISSILE1)				return "missile1";
//	if(t==MISSILE2)				return "missile2";
//	if(t==MISSILE3)				return "missile3";
//	if(t==MISSILE4)				return "missile4";
//	if(t==BOMB1)				return "missile1";
//	if(t==BOMB2)				return "missile2";
//	if(t==R550_MAGIC_MISSILE)	return "r550 magic missile";
//	if(t==SUPER_530_MISSILE)	return "super530 missile";
//	if(t==J37_MISSILE)			return "j37 missile";
//	if(t==SAM_MISSILE)			return "SAM missile";
//	if(t==FLAK_MISSILE)			return "flak missile";
//	if(t==AA_GUN)				return "AA gun";
//	if(t==SAM_BATTERY)			return "SAM battery";
//	if(t==FLAK_CANNON)			return "flak cannon";
//	if(t==BULLET_CLOUD)			return "bullet cloud";
//	if(t==PLAYER_PLANE)			return "PLAYER_PLANE";
//	debugBreak();
//	return "";
//}