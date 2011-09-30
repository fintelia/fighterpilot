
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

	inline unsigned int levelOffset(unsigned int level)
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
	int x;
	int y;
	unsigned int height;
	unsigned int width;

	unsigned int levelsDeep;
	unsigned int blockLength;

	unsigned char* heights;
	TerrainPatch* trunk;

	vector<TerrainPatch*> renderQueue;

	TerrainPage():heights(nullptr), trunk(nullptr){}
	~TerrainPage()
	{
		if(heights)	delete[] heights;
		if(trunk)	delete[] trunk;
	}
	void initQuadTree(unsigned int length_Pow2);
	TerrainPatch* getPatch(unsigned int level, unsigned int x, unsigned int y);

	void render();
};
class Terrain
{
protected:
	Vec3f terrainPosition;
	Vec3f terrainScale;

	vector<TerrainPage> terrainPages;
public:

	void initTerrain();
	void renderTerrain();
};