class WorldManager
{
public:
	static WorldManager& getInstance()
	{
		static WorldManager* pInstance = new WorldManager();
		return *pInstance;
	}
	
	Level			*level;
	vector<bullet>	bullets;
	vector<missile> missiles;
	map<int,planeBase*> planes;


private:
	WorldManager(){}

};

extern WorldManager& worldManager;