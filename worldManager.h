class WorldManager
{
public:
	static WorldManager& getInstance()
	{
		static WorldManager* pInstance = new WorldManager();
		return *pInstance;
	}
	void create(Level* lvl);
	//void create();
	void destroy();

	float elevation(Vec2f v) const;
	float elevation(float x, float z) const;
	float altitude(Vec3f v) const;
	float altitude(float x, float y, float z) const;

	Level::heightmapGL* const WorldManager::ground() const;

	const map<objId,object*>& objects()const;
	const map<objId,nPlane*>& planes()const;
	const map<objId,missile*>& missiles()const;

	void update();

	Level*				level;
	vector<bullet>		bullets;
	objectList			objectList;

	Smoke				smoke;
	Smoke				exaust;
	GameTime			time;

private:
	WorldManager():level(NULL){}

};

extern WorldManager& world;