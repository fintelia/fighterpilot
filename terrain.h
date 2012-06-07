
enum TerrainType{TERRAIN_ISLAND, TERRAIN_SNOW, TERRAIN_DESERT};

class TerrainPatch
{
public:

	TerrainPatch* parent;
	TerrainPatch* children[4];		//top left, top right, bottom left, bottom right
	TerrainPatch* neighbors[4];		//left, right, top, bottom
	unsigned int level;

	mutable enum DivisionLevel{SUBDIVIDED, LEVEL_USED, COMBINED} divisionLevel;
	mutable unsigned char edgeFlags;

	unsigned int row, col;

	float maxError;
	float minDistanceSquared; //square of the minimum distance to camera for which this level is allowed to be used

	BoundingBox<float> bounds;
	Vec3f center;

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
	void subdivide(const Vec3f& eye) const;
	void patchEdges() const;
};
class TerrainPage
{
public:
	Vec3f minXYZ;
	Vec3f maxXYZ;

	unsigned int height;
	unsigned int width;
	unsigned int LOD;

	unsigned int levelsDeep;
	unsigned int blockLength;

	unsigned short* heights;
	TerrainPatch* trunk;

	//unsigned int VBO;
	shared_ptr<GraphicsManager::texture2D> texture;
	shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
	vector<shared_ptr<GraphicsManager::indexBuffer>> indexBuffers;


	//struct IndexBuffer
	//{
	//	unsigned int numVertices;
	//	unsigned int id;
	//}*indexBuffer;

	mutable vector<TerrainPatch*> renderQueue;

	TerrainPage(unsigned short* Heights, unsigned int patchResolution, Vec3f position, Vec3f scale, unsigned int lod=1);
	~TerrainPage();
	TerrainPatch* getPatch(unsigned int level, unsigned int x, unsigned int y) const;

	void render(shared_ptr<GraphicsManager::View> view) const;

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
	
	shared_ptr<GraphicsManager::vertexBuffer> foliageVBO;
	shared_ptr<GraphicsManager::indexBuffer> foliageIBO;
	//struct plant
	//{
	//	Vec3f location;
	//	float height;
	//};
	//vector<plant> foliage;
	//mutable vector<texturedVertex3D> foliageVertices;
	TerrainType shaderType;

	shared_ptr<GraphicsManager::textureCube> skyTexture;
	shared_ptr<GraphicsManager::texture3D> oceanTexture;
	//unsigned int skyTextureId; //id for the sky cube-map
	//unsigned int oceanTextureId; //id for the ocean normals cube map

	vector<std::shared_ptr<TerrainPage>> terrainPages;

	std::shared_ptr<TerrainPage> getPage(Vec2f position) const;
	std::shared_ptr<TerrainPage> getPage(Vec3f position) const;

	void generateOceanTexture();
	void generateFoliage(int count);
	void resetTerrain();
public:
	Terrain():waterPlane(true){}
	~Terrain();
	void initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, TerrainType shader, int foliageAmount=0, unsigned int LOD=1);
	void renderTerrain(shared_ptr<GraphicsManager::View> view) const;

	void generateSky(Angle theta, Angle phi, float zenithLumance);//theta = angle from up axis; phi = angle from south

	const Circle<float>& bounds() const{return mBounds;}

	float elevation(Vec2f v) const;
	float elevation(float x, float z) const;
	float altitude(Vec3f v) const;
	float altitude(float x, float y, float z) const;
	Vec3f terrainNormal(Vec2f) const;
	Vec3f terrainNormal(float x, float z) const;
	bool isLand(Vec2f v) const;
	bool isLand(float x, float z) const;

};