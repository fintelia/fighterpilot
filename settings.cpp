
#include "game.h"
void SettingsManager::load(string filename) //should be rewritten to use FileManager to load the modelData.txt file
{
	string line;
	ifstream fin(filename, ios::in);

	planeType cP = F18;//default to f18
	missileType	cWeaponType=0;

	//int			cH	= -1,//hardpoint #
	bool		mirror = false;

	boost::char_separator<char> sep(" ,/", "");
	
	boost::tokenizer<boost::char_separator<char> >::iterator t;

	if(fin.is_open())
	{
		while(!fin.eof())
		{
			getline(fin,line);
			if(line.find_first_of("=")==line.npos || line.find_first_not_of(" \t")==line.npos) continue;

			line=line.substr(line.find_first_not_of(" \t"),line.npos);
			unsigned int f=line.find_first_of("=");
			string var = line.substr(0,f);

			unsigned int comment=line.find_first_of("'#\r"); //by treating \r as qa comment we don't have to worry about eliminating it before newlines
			string value = line.substr(f+1,comment-f-1);


			try
			{
				if(var == "PLANE")
				{
					cP = objectTypeFromString(value);
					mirror=false;
				}
				else if(var == "DEFAULT")
				{
					defaultPlane = objectTypeFromString(value);
					mirror=false;
				}
				else if(var == "CURRENT_WEAPON")
				{
					cWeaponType = objectTypeFromString(value);
				}
				else if(var == "W_OFFSET" && cWeaponType != 0)
				{
					Vec3f v = lexical_cast<Vec3f>(value);

					planeStats[cP].hardpoints.push_back(planeStat::hardpoint(cWeaponType,v));
					if(mirror)	
						planeStats[cP].hardpoints.push_back(planeStat::hardpoint(cWeaponType, Vec3f(-v.x,v.y,v.z)));
				}
				else if(var.compare("G_OFFSET") == 0)
				{
					Vec3f v = lexical_cast<Vec3f>(value);

					planeStats[cP].machineGuns.push_back(v);
					if(mirror)	
						planeStats[cP].machineGuns.push_back(-v);
				}
				else if(var.compare("E_OFFSET") == 0)
				{
					Vec3f v = lexical_cast<Vec3f>(value);

					planeStats[cP].engines.push_back(v);
					if(mirror)	
						planeStats[cP].engines.push_back(Vec3f(-v.x,v.y,v.z));
				}
				else if(var == "MIRROR")
				{
					mirror = (value == "TRUE");
				}
			}
			catch(boost::bad_lexical_cast &)
			{
				cout << line << endl;
				debugBreak();//boost type conversion failed
				closingMessage(string("Error reading '") + filename + "'. Fighter-Pilot will now close.","Error");
				return;
			}
		}
		fin.close();
	}
	else 
	{
		debugBreak();//could not open model data file
		closingMessage(string("'") + filename + "' not found. Fighter-Pilot will now close.","Missing File");
	}
}
void SettingsManager::load(const map<string,map<string,string>>& m)
{
	for(auto i = m.begin(); i != m.end(); i++)
	{
		categories[i->first].insert(i->second.begin(), i->second.end());
	}
}