
struct LevelFile
{
	float* heights;

	enum levelEnum {DIE,RESPAWN,RESTART,FFA,TEAMS,PLAYER_VS,WATER,LAND,SAND,ROCK }
					onHit,
					onAiHit,
					gameType,
					mapType,
					seaFloorType;

	unsigned int	mapSize,
					enemyPlanes,
					killPercentNeeded,
					heightRange;

	int				minX, minY,
					maxX, maxY,
					seaLevel;		

	string			levelName;
};

class Level
{
public:
	enum textureType{	GRASS		= 0x01,
						ROCK		= 0x02,
						SAND		= 0x04,
						SNOW		= 0x08,
						LCNOISE		= 0x10,
						HEIGHTMAP	= 0x20,
						NORMALMAP	= 0x40};

	class heightmap
	{
	private:
		typedef Vec3f vertex;
		typedef unsigned int index;

		vertex*			vertices;
		index*			indices;
		unsigned int	size,
						numVerts,
						numIdices;
		int				VBOvert,
						VBOindex;
		mutable float	minHeight,
						maxHeight;
		int				shader;
		void setVBO() const;
		void init(float* heights);

		bool dynamic;
		mutable bool VBOvalid;
		mutable bool minMaxValid;

	public:
		heightmap(int Size, bool Dynamic=false);
		heightmap(int Size, float* heights, bool Dynamic=false);
		void setHeight(unsigned int x, float height, unsigned int z);
		void increaseHeight(unsigned int x, float height, unsigned int z);
		void setShader(int s){shader=s;}

		float getHeight(unsigned int x, unsigned int z) const;
		void draw(int shader=0) const;
		unsigned int getSize() const;
		void setMinMaxHeights() const;


		friend class Level;
	};
	class waterPlane
	{
	public:
		float seaLevel;
		string shader;
		int oceanID;
	};
	struct zone
	{
		float x,z,width,length;
	};
	struct levelSettings
	{
		textureType seaFloor;
		bool water;
	};

protected:
	heightmap*		mGround;
	waterPlane		mWater;
	vector<zone>	mZones;
	levelSettings	mSettings;
	int				groundShader;

	Level(): mGround(NULL), groundShader(0){}
public:
	Level(LevelFile file);
	Level(string BMP);
	const heightmap* const ground() const;
	void render();
};

class editLevel: public Level
{
public:
	editLevel();
	heightmap*		ground();
	vector<zone>*	zones();
	levelSettings*	settings();

	void addZone(float x, float z, float width, float length);
	void newGround(unsigned int size, float* heights=NULL);
};