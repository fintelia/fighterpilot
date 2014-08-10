
struct LevelFile
{
	struct Info
	{
		TerrainType		shaderType;
		Vec2f			mapSize;
		Vec2u			mapResolution;
		string			nextLevel;
		float			minHeight;
		float			maxHeight;
		float			foliageDensity; //in trees per km^2
		unsigned int	LOD;
		bool			night;
		Info(): shaderType(TERRAIN_ISLAND), mapSize(1,1), mapResolution(0,0), foliageDensity(0), LOD(1), night(false){}
	};
	struct Path
	{
		struct waypoint{
			Vec3f position;
			float time;
			waypoint(Vec3f pos, float t):position(pos), time(t){}
		};
		vector<waypoint> waypoints;
	};
	struct Trigger
	{
		enum Comparision{NEVER,ALWAYS,LESS,LESS_EQUAL,EQUAL,GREATER_EQUAL,GREATER} comparison;
		int comparisonValue;
		struct Condition
		{
			enum ConditionType{NUM_OBJECTS,NUM_OBJECTS_ON_TEAM} conditionType;
			Condition(ConditionType type):conditionType(type){}
		};
		struct ConditionNumObjectsOnTeam: public Condition
		{
			int teamNumber;
			ConditionNumObjectsOnTeam(int teamNum):Condition(NUM_OBJECTS_ON_TEAM), teamNumber(teamNum){}
		};
		struct Action
		{
			enum ActionType{START_PATH};
			const ActionType actionType;

			Action(ActionType type):actionType(type){}
		};
		struct ActionStartPath: public Action
		{
			int pathNumber;
			objId objectNumber;
			ActionStartPath(int pathNum, int objectNum):Action(START_PATH),pathNumber(pathNum),objectNumber(objectNum){}
		};

		bool destroyTrigger;
		shared_ptr<Condition> condition;
		vector<shared_ptr<Action>> actions;

		Trigger():destroyTrigger(true){}
		bool checkComparison(int value) const;
	};
	struct Object
	{
		int				type;			//the type of object
		int				team;			//what team it is part of
		Vec3f			startloc;		//the location where the object will spawn
		Quat4f			startRot;		//what direction the object will face when it spawns
		Object();
	};
	struct Region
	{
		int shape;					//shape: 0 = circle; 1 = rectangle;
		int type;					//type of region: 0 = map bounds; 1 = off limits;
//		float maxTime;				//maximum amount of time in seconds inside/outside of the region before the place explodes

		union{
			float centerXYZ[3];		//Vec3f's are not allowed in unions because they have constructors
			float minXYZ[3];
		};
		union{
			struct{
				float radius;		//for circle
				float minHeight;
				float maxHeight;
			};
			float maxXYZ[3];		//for rectangle
		};

		float colorR;
		float colorG;
		float colorB;
		bool operator() (Vec3f v) // returns true if v is in the region
		{
			if(type==0)
			{
				return (v.y > minHeight && v.y < maxHeight && (v.x-centerXYZ[0])*(v.x-centerXYZ[0])+(v.z-centerXYZ[2])*(v.z-centerXYZ[2]) <= radius*radius);
			}
			else if(type==1)
			{
				return (v.x > minXYZ[0] && v.y > minXYZ[1] && v.z > minXYZ[2] && v.x < maxXYZ[0] && v.y < maxXYZ[1] && v.z < maxXYZ[2]);
			}
			return false;

		}
	};

	Info			info;
	vector<Object>	objects;
	vector<Region>	regions;
	vector<Trigger>	triggers;
	vector<Path>	paths;
	float*			heights;

	bool saveZIP(string filename, float heightScale, float seaLevelOffset);
	bool loadZIP(string filename);

	bool parseObjectFile(shared_ptr<FileManager::textFile> f);
	void initializeWorld(unsigned int humanPlayers) const;//creates objects stored in level file
	shared_ptr<Terrain::ClipMap> generateClipMap() const;

	bool checkValid();

	LevelFile();
	~LevelFile();
};
