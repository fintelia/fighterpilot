
struct LevelFile
{
	struct Header{
		__int64 magicNumber;
		unsigned int version;
	}header;

	struct Info{//V1
		float			seaLevel;	
		Vec3f			mapSize;
		Vec2u			mapResolution;
		unsigned int	numObjects;
		Info(): seaLevel(0), mapSize(1,1,1), mapResolution(0,0), numObjects(0){}
	}*info;
	struct Object{
		int				type;
		int				team;
		Vec3f			startloc;
		Quat4f			startRot;
	}*objects;

	float* heights;//in a V1 file this there would then just be a (mapSize.x) x (mapSize.y) grid of floats

	void load(string filename);
	void save(string filename);
	LevelFile();
	LevelFile(string filename);
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
		virtual unsigned int resolutionX() const {return mResolution.x;}
		virtual unsigned int resolutionZ() const {return mResolution.y;}
		virtual Vec2u resolution() const {return mResolution;}
		virtual Vec3f size()const{return mSize;}
		virtual Vec2f sizeXZ()const{return Vec2f(mSize.x,mSize.z);}
		virtual float sizeX()const{return mSize.x;}
		virtual float sizeY()const{return mSize.y;}
		virtual float sizeZ()const{return mSize.z;}
		virtual float rasterHeight(unsigned int x,unsigned int z) const;
		virtual float interpolatedHeight(float x, float z) const;
		virtual float height(float x, float z) const;
		virtual Vec3f rasterNormal(unsigned int x, unsigned int z) const;
		virtual Vec3f interpolatedNormal(float x, float z) const;
		virtual Vec3f normal(float x, float z) const;

		virtual void setMinMaxHeights() const;


		virtual void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] = height;}
		virtual void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] += height;}
		virtual void setPos(Vec3f pos){mPosition = pos;}
		virtual void setSize(Vec3f s){mSize = s;}
		virtual void setSizeY(float s){mSize.y=s;}
		virtual void setSizeXZ(Vec2f s){mSize.x=s.x,mSize.z=s.y;}
		

		heightmapBase(Vec2u Resolution);
		heightmapBase(Vec2u Resolution, float* heights);
		~heightmapBase(){delete[] heights;}
		virtual void render() const =0;
	protected:
		Vec3f					mPosition,
								mSize;
		Vec2u					mResolution;//y value is z axis
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
		void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] = height; valid=false;}
		void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] += height; valid=false;}		
		void render() const;

		heightmapGL(Vec2u Resolution):heightmapBase(Resolution),valid(false),shader(0),dispList(0){init();}
		heightmapGL(Vec2u Resolution, float* heights):heightmapBase(Resolution,heights),valid(false),shader(0),dispList(0){init();}
		~heightmapGL(){glDeleteLists(dispList,1);}
		friend class Level;
	};
	class waterPlane
	{
	public:
		float seaLevel;
		string shader;
		int oceanID;
	};
	class object
	{
	public:
		int				type;
		int				team;
		Vec3f			startloc;
		Quat4f			startRot;
	};
	struct levelSettings
	{
		//textureType seaFloor;
		bool water;
	};
protected:
	heightmapBase*	mGround;
	waterPlane		mWater;
	levelSettings	mSettings;
	vector<object>	mObjects;
	int				groundShader;

	Level(): mGround(NULL), groundShader(0) {}
public:
	Level(LevelFile file);
	Level(string BMP);
	const heightmapBase* const ground() const;
	void render();

	LevelFile getLevelFile();
	void exportBMP(string filename);
};

class editLevel: public Level
{
public:
	editLevel();
	heightmapBase*	ground();
	levelSettings*	settings();

	void addZone(float x, float z, float width, float length);
	void newGround(unsigned int x, unsigned int z, float* heights=NULL);
	void addObject(int type,int team, Vec3f pos);
	void renderObjects();


	editLevel(LevelFile file):Level(file){}
	editLevel(string BMP):Level(BMP){}
};