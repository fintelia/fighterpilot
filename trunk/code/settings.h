
const int RESPAWN	=0;
const int RESTART	=1;
const int DIE		=2;

const int FFA		=0;
const int TEAMS		=1;
const int PLAYER_VS	=2;

class Settings
{
public:
	string MAP_FILE;
	string LEVEL_NAME;
	int KILL_PERCENT_NEEDED;
	int ENEMY_PLANES;
	int ON_HIT;
	int ON_AI_HIT;
	int GAME_TYPE;
	void loadMap(char *filename)
	{
		string line;
		ifstream fin(filename, ios::in);
		if(fin.is_open())
		{
			while(!fin.eof())
			{	
				getline(fin,line);
				int f=line.find_first_of("=");
				string var = line.substr(0,f);
				string value = line.substr(f+1,line.size());
				if (var.compare("MAP_FILE") == 0)
				{
					MAP_FILE=value;
				}
				else if(var.compare("ENEMY_PLANES") == 0)
				{
					ENEMY_PLANES=atoi(value.c_str());
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
				else if(var.compare("LEVEL_NAME")==0)
				{
					LEVEL_NAME=value;
				}
				else if(var.compare("KILL_PERCENT_NEEDED")==0)
				{
					KILL_PERCENT_NEEDED=atoi(value.c_str());
				}
			}
			fin.close();
		}
	}
	Settings(){}
	Settings(char *filename)
	{
		loadMap(filename);
	}
};