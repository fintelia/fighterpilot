
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
class ObjectStats
{
public:
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
	void load(string filename);
};
extern ObjectStats settings;