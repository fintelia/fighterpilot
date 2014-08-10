
class WorldManager: public objectList, public Terrain
{
private:
	Circle<float> mBounds;

public:
	GameTime time;

	WorldManager(shared_ptr<ClipMap> clipMap);
	~WorldManager();

	void simulationUpdate();
	void frameUpdate();

	void setBounds(Circle<float> c){mBounds = c;}
	const Circle<float>& bounds() const{return mBounds;}
};

extern unique_ptr<WorldManager> world;
