
#include "main.h"

void Settings::loadMap(char *filename)
{
	string line;
	ifstream fin(filename, ios::in);
	if(fin.is_open())
	{
		while(!fin.eof())
		{	
			getline(fin,line);
			if(line.find_first_of("=")==-1) continue;

			int f=line.find_first_of("=");				
			string var = line.substr(line.find_first_not_of(" \t"),f);
			string value = line.substr(f+1,line.size());
			if (var.compare("MAP_FILE") == 0)
			{
				MAP_FILE=value;
			}
			else if(var.compare("ENEMY_PLANES") == 0)
			{
				ENEMY_PLANES=lexical_cast<int>(value);
			}
			else if(var.compare("ON_HIT")==0)
			{
				if(value.compare("RESPAWN")==0)
					ON_HIT=RESPAWN;
				else if(value.compare("RESTART")==0)
					ON_HIT=RESTART;
				else if(value.compare("DIE")==0)
					ON_HIT=DIE;
			}
			else if(var.compare("ON_AI_HIT")==0)
			{
				if(value.compare("RESPAWN")==0)
					ON_AI_HIT=RESPAWN;
				else if(value.compare("RESTART")==0)
					ON_AI_HIT=RESTART;
				else if(value.compare("DIE")==0)
					ON_AI_HIT=DIE;
			}
			else if(var.compare("GAME_TYPE")==0)
			{
				if(value.compare("FFA")==0)
					GAME_TYPE=FFA;
				else if(value.compare("TEAMS")==0)
					GAME_TYPE=TEAMS;
				else if(value.compare("PLAYER_VS")==0)
					GAME_TYPE=PLAYER_VS;
			}
			else if(var.compare("MAP_TYPE")==0)
			{
				if(value.compare("WATER")==0)
					MAP_TYPE=WATER;
				else if(value.compare("LAND")==0)
					MAP_TYPE=LAND;
				else if(value.compare("SNOW")==0)
					MAP_TYPE=SNOW;
			}
			else if(var.compare("SEA_FLOOR_TYPE")==0)
			{
				if(value.compare("ROCK")==0)
					SEA_FLOOR_TYPE=ROCK;
				else if(value.compare("SAND")==0)
					SEA_FLOOR_TYPE=SAND;
			}
			else if(var.compare("LEVEL_NAME")==0)
			{
				LEVEL_NAME=value;
			}
			else if(var.compare("KILL_PERCENT_NEEDED")==0)
			{
				KILL_PERCENT_NEEDED=lexical_cast<int>(value);
			}
			else if(var.compare("MIN_X")==0)		MIN_X=lexical_cast<int>(value);
			else if(var.compare("MIN_Y")==0)		MIN_Y=lexical_cast<int>(value);
			else if(var.compare("MAX_X")==0)		MAX_X=lexical_cast<int>(value);
			else if(var.compare("MAX_Y")==0)		MAX_Y=lexical_cast<int>(value);
			else if(var.compare("HEIGHT_RANGE")==0)	HEIGHT_RANGE=(float)lexical_cast<int>(value);
			else if(var.compare("SEA_LEVEL")==0)	SEA_LEVEL=(float)lexical_cast<int>(value);
		}
		fin.close();
	}
}
void Settings::loadModelData(char* filename)
{
	string line;
	ifstream fin(filename, ios::in);

	planeType	cP=F18;//default to f18
	int			cH=-1,//hardpoint #
				cM=-1;//current missile #
	bool		hMirror=false;

	string		s;	
	char_separator<char> sep(" ,/", "");
		
	tokenizer<char_separator<char> >::iterator t;

	if(fin.is_open())
	{
		while(!fin.eof())
		{
			getline(fin,line);
			if(line.find_first_of("=")==line.npos || line.find_first_not_of(" \t")==line.npos) continue;

			line=line.substr(line.find_first_not_of(" \t"),line.npos);
			unsigned int f=line.find_first_of("=");
			string var = line.substr(0,f);

			unsigned int comment=line.find_first_of("'#");
			string value = line.substr(f+1,comment-f-1);


			try
			{
				//planeModels.push_back(load_model(lexical_cast<string>(value).c_str()));
				if(var.compare("PLANE") == 0)
				{
					if(value.compare("F12")==0) 				
						cP=F12;
					else if(value.compare("F16")==0)		
						cP=F16;
					else if(value.compare("F18")==0)		
						cP=F18;
					else if(value.compare("F22")==0)			
						cP=F22;
					else if(value.compare("UAV")==0)			
						cP=UAV;
					else if(value.compare("B2")==0)	
						cP=B2;
					else
						cP=(planeType)lexical_cast<int>(value); // Try to avoid
				}
				else if(var.compare("DEFAULT") == 0)
				{
					if(value.compare("F12")==0) 				
						defaultPlane=F12;
					else if(value.compare("F16")==0)		
						defaultPlane=F16;
					else if(value.compare("F18")==0)		
						defaultPlane=F18;
					else if(value.compare("F22")==0)			
						defaultPlane=F22;
					else if(value.compare("UAV")==0)			
						defaultPlane=UAV;
					else if(value.compare("B2")==0)	
						defaultPlane=B2;
					else
						defaultPlane=(planeType)lexical_cast<int>(value); // Try to avoid
					hMirror=false;
					cH=-1;
				}
				//else if(var.compare("MODEL") == 0)
				//{
				//	//int mId=objModel::objDisplayList((char*)lexical_cast<string>(value).c_str());//load_model((char*)lexical_cast<string>(value).c_str());
				//	int mId=dataManager.loadModel(value);
				//	planeModels.insert(pair<planeType,int>(cP,mId));
				//	planeStats[cP].dispList=mId;
				//	hMirror=false;
				//	cH=-1;
				//}
				else if(var.compare("MISSILE") == 0)
				{
					cM++;
					missileStats.push_back(missileStat());
					missileStats[cM].dispList=dataManager.loadModel(value);//objModel::objDisplayList((char*)lexical_cast<string>(value).c_str());
					int i=4;
				}
				else if(var.compare("H_MISSILE") == 0)
				{
					cH++;
					if(hMirror) cH++;
					planeStats[cP].hardpoints.push_back(planeStat::hardpoint());
					planeStats[cP].hardpoints[cH].missileNum=lexical_cast<int>(value);
					hMirror=false;
				}
				else if(var.compare("H_OFFSET") == 0 && cH != -1)
				{
					int d = 0;
					tokenizer<char_separator<char> > Tokenizer(value, sep);
					for (t = Tokenizer.begin(); t != Tokenizer.end(); t++, d++)
					{
						s=*t;
						planeStats[cP].hardpoints[cH].offset[d]=lexical_cast<float>(*t);
					}
					if(hMirror)
					{
						planeStats[cP].hardpoints[cH+1].offset=planeStats[cP].hardpoints[cH].offset;
						planeStats[cP].hardpoints[cH+1].offset.x *= -1.0;
					}
				}
				else if(var.compare("H_MIRROR") == 0 && cH != -1)
				{
					hMirror=value.compare("TRUE")==0;
					if(hMirror)
					{
						planeStats[cP].hardpoints.push_back(planeStat::hardpoint(planeStats[cP].hardpoints[cH]));
						planeStats[cP].hardpoints[cH+1].offset.x *= -1.0;
					}
				}
				//else if(var.compare("MIRROR") == 0 && cm != -1)
				//{
				//	missileStats[cm].mirror=value.compare("TRUE")==0;
				//}
				//else if (var.compare("NAME") == 0)
				//{
				//	while(value.find("_")!=value.npos)
				//		value[value.find("_")]=' ';
				//	getInstance()->units[ct].name		=value;
				//}
			}
			catch(bad_lexical_cast &)
			{
				debugBreak();//boost type conversion failed
				closingMessage("Error reading 'modelData.txt'. Fighter-Pilot will now close.","Error");
				return;
			}
		}
		fin.close();
	}
	else 
	{
		debugBreak();//could not open model data file
		closingMessage("'modelData.txt' not found. Fighter-Pilot will now close.","Missing File");
	}
}