
class TerrainPatch
{
public:



	TerrainPatch* parent;
	TerrainPatch* children[4];		//top left, top right, bottom left, bottom right
	TerrainPatch* neighbors[4];		//left, right, top, bottom
	unsigned int level;
	unsigned char flags;

	float maxError;


	Vec3f minXYZ, maxXYZ;		//bounding box (for later)

	TerrainPatch();
	void init(TerrainPatch* trunk, unsigned int levelsDeep, unsigned int totalLevels);

	inline unsigned int levelOffset(unsigned int level) const
	{
		unsigned int offset = 0;
		for(int i=0; i<level; i++)
		{
			offset += 1 << (2*i);
		}
		return offset;
	}
};
class TerrainPage
{
public:
	Vec3f minXYZ;
	Vec3f maxXYZ;

	unsigned int height;
	unsigned int width;

	unsigned int levelsDeep;
	unsigned int blockLength;

	unsigned short* heights;
	TerrainPatch* trunk;

	unsigned int VBO;
	unsigned int texture;

	struct IndexBuffer
	{
		unsigned int numVertices;
		unsigned int id;
	}indexBuffer;

	mutable vector<TerrainPatch*> renderQueue;

	TerrainPage(unsigned short* Heights, unsigned int LevelsDeep, unsigned short patchResolution, Vec3f position, Vec3f scale);
	~TerrainPage()
	{
		delete[] heights;
		delete[] trunk;
	}
	TerrainPatch* getPatch(unsigned int level, unsigned int x, unsigned int y) const;

	void render(Vec3f eye) const;

	Vec3f getNormal(Vec2f loc) const;
	float getHeight(Vec2f loc) const;
protected:
	Vec3f interpolatedNormal(Vec2f loc) const;
	float interpolatedHeight(Vec2f loc) const;
	Vec3f rasterNormal(Vec2u loc) const;
	float rasterHeight(Vec2u loc) const;
};
class Terrain
{
protected:
	Vec3f terrainPosition;
	Vec3f terrainScale;

	Circle<float> mBounds;
	bool waterPlane;

	vector<std::shared_ptr<TerrainPage>> terrainPages;

	std::shared_ptr<TerrainPage> getPage(Vec2f position) const;
	std::shared_ptr<TerrainPage> getPage(Vec3f position) const;
public:
	Terrain():waterPlane(true){}

	void initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, bool water=true);
	void renderTerrain(Vec3f eye) const;

	const Circle<float>& bounds() const{return mBounds;}

	float elevation(Vec2f v) const;
	float elevation(float x, float z) const;
	float altitude(Vec3f v) const;
	float altitude(float x, float y, float z) const;
	bool isLand(Vec2f v) const;
	bool isLand(float x, float z) const;
};