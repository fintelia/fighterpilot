
const int RESPAWN	=0;
const int RESTART	=1;
const int DIE		=2;

const int FFA		=0;
const int TEAMS		=1;
const int PLAYER_VS	=2;

const int LAND		=0;
const int WATER		=1;
const int SNOW		=3;

const int ROCK		=0;
const int SAND		=1;
class SettingsManager
{
	SettingsManager(){}
public:
	static SettingsManager& getInstance()
	{
		static SettingsManager* pInstance = new SettingsManager();
		return *pInstance;
	}
	//string MAP_FILE;
	//string LEVEL_NAME;
	//int KILL_PERCENT_NEEDED;
	//int ENEMY_PLANES;
	//int ON_HIT;
	//int ON_AI_HIT;
	//int GAME_TYPE;
	//int MIN_X;
	//int MIN_Y;
	//int MAX_X;
	//int MAX_Y;
	//int MAP_TYPE;
	//int SEA_FLOOR_TYPE;
	//float HEIGHT_RANGE;
	//float SEA_LEVEL;
	map<string,map<string,string>> categories;

	struct planeStat
	{
		struct hardpoint
		{
			objectType mType;
			Vec3f offset;
			hardpoint(): mType(MISSILE), offset(0,0,0) {}
			hardpoint(objectType t,Vec3f Offset): mType(t), offset(Offset) {}
		};
		vector<hardpoint> hardpoints;//offsets
		vector<Vec3f> machineGuns;
		planeStat(){}
	};
	map<planeType,planeStat> planeStats;
	//struct missileStat
	//{
	//	//int dispList;
	//	missileType type;
	//	missileStat(): type(MISSILE) {}
	//};
	//vector<missileStat> missileStats;

	//void loadMap(char *filename);
//	void loadModelData(char* filename);
	template <class T> T get (string category, string name)
	{
		try{
			if(categories.find(category)!=categories.end() && categories[category].find(name)!=categories[category].end())
				return lexical_cast<T>(categories[category][name]);
			else
				return T();
		}catch(...){
			return T();
		}
	}
	string operator() (string category, string name)
	{
		if(categories.find(category)!=categories.end() && categories[category].find(name)!=categories[category].end())
			return categories[category][name];
		else
			return "";
	}
	void load(string filename);
	void load(const map<string,map<string,string>>& m);
};
extern SettingsManager& settings;