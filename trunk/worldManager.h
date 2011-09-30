class WorldManager: public objectList, public Terrain
{
public:
	static WorldManager& getInstance()
	{
		static WorldManager* pInstance = new WorldManager();
		return *pInstance;
	}
	void create(std::shared_ptr<Level> lvl);
	//void create();
	void destroy();

	float elevation(Vec2f v) const;
	float elevation(float x, float z) const;
	float altitude(Vec3f v) const;
	float altitude(float x, float y, float z) const;
	bool isLand(Vec2f v) const;
	bool isLand(float x, float z) const;

	Level::heightmapGL* const WorldManager::ground() const;

	//const map<objId,object*>& objects()const;
	//const map<objId,nPlane*>& planes()const;
	//const map<objId,aaGun*>& aaGuns()const;
	//const map<objId,missile*>& missiles()const;
	//const map<objId,bomb*>& bombs()const;

	void update();

	std::shared_ptr<Level>	level;
//	vector<bullet>		bullets;

	GameTime			time;

private:
	WorldManager(){}

};

extern WorldManager& world;