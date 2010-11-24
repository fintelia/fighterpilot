
struct LevelFile
{
	float*					heights;

	enum levelEnum			{DIE,RESPAWN,RESTART,FFA,TEAMS,PLAYER_VS,WATER,LAND,SAND,ROCK }
							onHit,
							onAiHit,
							gameType,
							mapType,
							seaFloorType;

	unsigned int			enemyPlanes,
							killPercentNeeded,
							heightRange;

	int						seaLevel;	

	Vector2<unsigned int>	min,
							max,
							mapSize;

	string					levelName;
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

	class heightmapBase
	{
	public:
		virtual unsigned int getResolutionX() const {return resolution.x;}
		virtual unsigned int getResolutionZ() const {return resolution.y;}
		virtual float getRasterHeight(unsigned int x,unsigned int z) const;
		virtual Vec3f getRasterNormal(unsigned int x, unsigned int z) const;
		virtual Vec3f getNormal(float x, float z) const;
		virtual float getHeight(float x, float z) const;
		virtual void setMinMaxHeights() const;

		virtual void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,getResolutionX()-1) + clamp(z,0,getResolutionZ()-1)*getResolutionX()] = height;}
		virtual void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,getResolutionX()-1) + clamp(z,0,getResolutionZ()-1)*getResolutionX()] += height;}
		virtual void setPos(Vec3f pos){position = pos;}
		virtual void setSize(Vec3f s){size = s;}
		virtual void setSizeY(float s){size.y=s;}
		virtual void setSizeXZ(Vec2f s){size.x=s.x,size.z=s.y;}
		

		heightmapBase(Vector2<unsigned int> Resolution);
		heightmapBase(Vector2<unsigned int> Resolution, float* heights);
		~heightmapBase(){delete[] heights;}
		virtual void render() const =0;
	protected:
		Vec3f position,			size;
		Vector2<unsigned int>	resolution;//y value is z axis
		float*					heights;
		mutable float			minHeight,
								maxHeight;
		friend class Level;
	};
	class heightmapGL: public heightmapBase
	{
	private:
		mutable bool valid;
		mutable int dispList;
		int shader;
		int	groundTex;
		mutable unsigned char *groundValues;

		void init();
		void setTex() const;
		void createList() const;
	public:
		void setShader(int s){shader=s;}
		void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,getResolutionX()-1) + clamp(z,0,getResolutionZ()-1)*getResolutionX()] = height; valid=false;}
		void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,getResolutionX()-1) + clamp(z,0,getResolutionZ()-1)*getResolutionX()] += height; valid=false;}		
		void render() const;

		heightmapGL(Vector2<unsigned int> Resolution):heightmapBase(Resolution),valid(false),shader(0),dispList(0){init();}
		heightmapGL(Vector2<unsigned int> Resolution, float* heights):heightmapBase(Resolution,heights),valid(false),shader(0),dispList(0){init();}
		~heightmapGL(){glDeleteLists(dispList,1);}
	};
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

		int				groundTex;
		mutable unsigned char *groundValues;//needed only if dynamic; (R,G,B) = normal and (A) = height

		void setVBO() const;
		void setTex() const;
		void init(float* heights);

		bool dynamic;
		mutable bool VBOvalid;
		mutable bool minMaxValid;
		mutable bool texValid;
	public:
		heightmap(int Size, bool Dynamic=false);
		heightmap(int Size, float* heights, bool Dynamic=false);
		void setHeight(unsigned int x, float height, unsigned int z);
		void increaseHeight(unsigned int x, float height, unsigned int z);
		void setShader(int s){shader=s;}

		float getHeight(unsigned int x, unsigned int z) const;
		Vec3f getNormal(unsigned int x, unsigned int z) const;
		//void draw(int shader=0) const;
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
	heightmapBase*	mGround;
	waterPlane		mWater;
	vector<zone>	mZones;
	levelSettings	mSettings;
	int				groundShader;

	Level(): mGround(NULL), groundShader(0) {}
public:
	Level(LevelFile file);
	Level(string BMP);
	const heightmapBase* const ground() const;
	void render();
};

class editLevel: public Level
{
public:
	editLevel();
	heightmapBase*	ground();
	vector<zone>*	zones();
	levelSettings*	settings();

	void addZone(float x, float z, float width, float length);
	void newGround(unsigned int x, unsigned int z, float* heights=NULL);
};