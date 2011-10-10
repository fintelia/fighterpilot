
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
	Vec3f minXYZ;
	Vec3f maxXYZ;

	unsigned int height;
	unsigned int width;

	unsigned int levelsDeep;
	unsigned int blockLength;

	unsigned short* heights;
	TerrainPatch* trunk;

	GLuint VBO;

	struct IndexBuffer
	{
		unsigned int numVertices;
		GLuint id;
	}indexBuffer;

	vector<TerrainPatch*> renderQueue;

	TerrainPage(unsigned short* Heights, unsigned int LevelsDeep, unsigned short patchResolution, Vec3f position, Vec3f scale);
	~TerrainPage()
	{
		if(heights)	delete[] heights;
		if(trunk)	delete[] trunk;
	}
	TerrainPatch* getPatch(unsigned int level, unsigned int x, unsigned int y);

	void render(Vec3f eye);
};
class Terrain
{
protected:
	Vec3f terrainPosition;
	Vec3f terrainScale;

	vector<std::shared_ptr<TerrainPage>> terrainPages;
public:

	void initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale);
	void renderTerrain(Vec3f eye);
};