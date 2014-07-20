
class WorldManager: public objectList, public Terrain
{
public:
	GameTime time;

	void create();
	void destroy();

	void simulationUpdate();
	void frameUpdate();
private:
	WorldManager(){}
	WorldManager(const WorldManager& w){}

public:
	static WorldManager& getInstance()
	{
		static WorldManager* pInstance = new WorldManager();
		return *pInstance;
	}
};

extern WorldManager& world;
