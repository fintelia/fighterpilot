
typedef int ShaderType;
const ShaderType SHADER_NONE		= 0;
const ShaderType SHADER_ISLAND		= 1;
const ShaderType SHADER_GRASS		= 2;
const ShaderType SHADER_SNOW		= 3;
const ShaderType SHADER_OCEAN		= 4;

struct LevelFile
{
	//struct lhDr{
	//	__int64			magicNumber;
	//	unsigned int	version;
	//	ShaderType		shaderType;	
	//	Vec2f			mapSize;
	//	Vec2u			mapResolution;
	//};
	struct Header{
		__int64 magicNumber;
		unsigned int version;
	}header;

	struct Info{//V1
		ShaderType		shaderType;
		Vec2f			mapSize;
		Vec2u			mapResolution;
		unsigned short	numObjects;
		unsigned short	numRegions;
		string			nextLevel;
		float			minHeight;
		float			maxHeight;
		Info(): shaderType(SHADER_NONE), mapSize(1,1), mapResolution(0,0), numObjects(0), numRegions(0){}
	}*info;

	struct Object{
		int				type;			//the type of object
		int				team;			//what team it is part of
		int				controlType;	//how this object is being controlled
		Vec3f			startloc;		//the location where the object will spawn
		Quat4f			startRot;		//what direction the object will face when it spawns
		Object();
	}*objects;

	struct Region{
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
	}*regions;

	float* heights;						//in a V1 file this there would then just be a (mapSize.x) x (mapSize.y) grid of floats

	//bool load(string filename);
	//bool save(string filename);
	//bool savePNG(string filename);
	//bool loadPNG(string filename);

	bool saveZIP(string filename);
	bool loadZIP(string filename);

	bool parseObjectFile(shared_ptr<FileManager::textFile> f);

	void initializeWorld(unsigned int humanPlayers);//creates objects stored in level file

	LevelFile();
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
		virtual void renderPreview(float scale=1.0, float seaLevelOffset=0.0) const=0;
		virtual void init()=0;

		float operator() (unsigned int x, unsigned int z) const{return rasterHeight(x,z);}
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
		friend class editLevel;
		friend class modeMapBuilder;
		friend class gui::levelEditor;
	};
	class heightmapGL: public heightmapBase
	{
	private:
		mutable bool valid;
		mutable int dispList;
		//int shader;
		shared_ptr<GraphicsManager::texture2D>	groundTex;
		mutable unsigned char *groundValues;

		void init();
		void setTex() const;
		void createList() const;
	public:
		void setShader(ShaderType t){shaderType=t;}
		void setHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] = height; valid=false;}
		void increaseHeight(unsigned int x, float height, unsigned int z){heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()] += height; valid=false;}		
		void increaseHeights(float amount);
		void renderPreview(float scale=1.0, float seaLevelOffset=0.0) const;
		void render() const;

		heightmapGL(Vec2u Resolution):heightmapBase(Resolution),valid(false),dispList(0){init();}
		heightmapGL(Vec2u Resolution, float* heights):heightmapBase(Resolution,heights),valid(false),dispList(0){init();}
		~heightmapGL();
		friend class Level;
		friend class editLevel;
		friend class modeDogFight;
		friend class modeMapBuilder;
		friend class gui::levelEditor;
	};

protected:
	heightmapBase*				mGround;
	vector<LevelFile::Object>	mObjects;
	vector<LevelFile::Region>	mRegions;

	string						nextLevel;
	shaderData					water;

public:
	Level();
	~Level();
	virtual bool init(string filename);

	void initializeWorld();//creates objects stored in level file

	void render(Vec3f eye);

	heightmapBase* const ground() const{return mGround;}
	const vector<LevelFile::Object>& objects() const {return mObjects;}
	const vector<LevelFile::Region>& regions() const {return mRegions;}
	string getLevelNext(){return nextLevel;}
};

class editLevel: public Level
{
public:
	heightmapBase*	ground();

	bool init(string BMP, Vec3f size, float seaLevel);

	void newGround(unsigned int x, unsigned int z, float* heights=NULL);
	void addObject(int type,int team, int controlType, Vec3f pos, Quat4f rot=Quat4f());
	void addRegionCircle(Vec2f c, float r);
	void addRegionRect(Rect r);
	void setWater(string shaderName);

	void renderPreview(bool drawWater, float scale, float seaLevelOffset=0.0);//for mapbuilder
	void renderObjectsPreview();

	LevelFile::Object* getObject(int id){return &mObjects[id];}

	void save(string filename);
	void save(string filename, float seaLevelOffset);
	void exportBMP(string filename);
};