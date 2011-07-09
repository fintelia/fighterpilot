
typedef int ShaderType;
const ShaderType SHADER_NONE		= 0;
const ShaderType SHADER_ISLAND		= 1;
const ShaderType SHADER_GRASS		= 2;
const ShaderType SHADER_SNOW		= 3;
const ShaderType SHADER_OCEAN		= 4;

struct LevelFile
{
	struct Header{
		__int64 magicNumber;
		unsigned int version;
	}header;

	struct Info{//V1
		ShaderType		shaderType;	
		Vec2f			mapSize;
		Vec2u			mapResolution;
		unsigned int	numObjects;
		Info(): mapSize(1,1), mapResolution(0,0), numObjects(0){}
	}*info;

	struct Object{
		int				type;			//the type of object
		int				team;			//what team it is part of
		int				controlType;	//how this object is being controlled
		Vec3f			startloc;		//the location where the object will spawn
		Quat4f			startRot;		//what direction the object will face when it spawns
	}*objects;

	float* heights;						//in a V1 file this there would then just be a (mapSize.x) x (mapSize.y) grid of floats

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
						TERRAINMAP	= 0x20};
	struct shaderData
	{
		string name;
		textureType textures;
		shaderData():name(""),textures(textureType(0)){}
	};

	class heightmapBase
	{
	public:
		virtual unsigned int resolutionX() const {return mResolution.x;}
		virtual unsigned int resolutionZ() const {return mResolution.y;}
		virtual Vec2u resolution() const {return mResolution;}
		virtual Vec2f size()const{return Vec2f(mSize);}
		virtual float sizeX()const{return mSize.x;}
		virtual float sizeZ()const{return mSize.y;}
		virtual float rasterHeight(unsigned int x,unsigned int z) const;
		virtual float interpolatedHeight(float x, float z) const;
		virtual float height(float x, float z) const;
		virtual Vec3f rasterNormal(unsigned int x, unsigned int z) const;
		virtual Vec3f interpolatedNormal(float x, float z) const;
		virtual Vec3f normal(float x, float z) const;
		virtual float getMinHeight()const{return minHeight;}
		virtual float getMaxHeight()const{return maxHeight;}

		virtual void setMinMaxHeights() const;


		virtual void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] = height;}
		virtual void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] += height;}
		virtual void increaseHeights(float amount)=0;
		virtual void setPos(Vec3f pos){mPosition = pos;}
		virtual void setSize(Vec2f s){mSize = s;}
		virtual void setSizeX(float s){mSize.x=s;}
		virtual void setSizeZ(float s){mSize.y=s;}

		heightmapBase(Vec2u Resolution);
		heightmapBase(Vec2u Resolution, float* heights);
		~heightmapBase(){delete[] heights;}
		virtual void render() const=0;
		virtual void renderPreview(float seaLevelOffset=0.0) const=0;
		virtual void init()=0;

		float operator() (unsigned int x, unsigned int z) const{return height(x,z);}
		void operator() (unsigned int x, unsigned int z, float height) {setHeight(x,height,z);}
	protected:
		Vec3f					mPosition;
		Vec2f					mSize;
		Vec2u					mResolution;//y value is z axis
		float*					heights;
		mutable float			minHeight,
								maxHeight;
		ShaderType				shaderType;
		friend class Level;
		friend class modeMapBuilder;
	};
	class heightmapGL: public heightmapBase
	{
	private:
		mutable bool valid;
		mutable int dispList;
		//int shader;
		int	groundTex;
		mutable unsigned char *groundValues;

		void init();
		void setTex() const;
		void createList() const;
	public:
		void setShader(ShaderType t){shaderType=t;}
		void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] = height; valid=false;}
		void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] += height; valid=false;}		
		void increaseHeights(float amount);
		void renderPreview(float seaLevelOffset=0.0) const;
		void render() const;

		heightmapGL(Vec2u Resolution):heightmapBase(Resolution),valid(false),dispList(0){init();}
		heightmapGL(Vec2u Resolution, float* heights):heightmapBase(Resolution,heights),valid(false),dispList(0){init();}
		~heightmapGL();
		friend class Level;
		friend class modeDogFight;
		friend class modeMapBuilder;
	};

protected:
	heightmapBase*				mGround;
	vector<LevelFile::Object>	mObjects;

	string						nextLevel;
	shaderData					water;

	Level(): mGround(NULL) {}
	
public:
	Level(LevelFile file);
	Level(string BMP, Vec3f size, float seaLevel);
	~Level(){if(mGround)delete mGround;}

	heightmapBase* const ground() const{return mGround;}
	const vector<LevelFile::Object>& objects() const {return mObjects;}
	void renderPreview(bool drawWater, float seaLevelOffset=0.0);//for mapbuilder
	void renderObjectsPreview();
	void render(Vec3f eye);

	LevelFile getLevelFile();
	LevelFile getLevelFile(float seaLevelOffset);
	void exportBMP(string filename);
};

class editLevel: public Level
{
public:
	editLevel();
	heightmapBase*	ground();

	void newGround(unsigned int x, unsigned int z, float* heights=NULL);
	void addObject(int type,int team, int controlType, Vec3f pos, Quat4f rot=Quat4f());
	LevelFile::Object* getObject(int id){return &mObjects[id];}

	void setWater(string shaderName);

	editLevel(LevelFile file):Level(file){}
	editLevel(string BMP):Level(BMP){}
};