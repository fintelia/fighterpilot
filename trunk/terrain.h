
enum TerrainType{TERRAIN_ISLAND, TERRAIN_MOUNTAINS, TERRAIN_SNOW, TERRAIN_DESERT};


class Terrain
{
protected:
	class Patch
	{
	public:

		//Patch* parent;
		//Patch* children[4];		//top left, top right, bottom left, bottom right
		//Patch* neighbors[4];		//left, right, top, bottom
		//unsigned int level;

		mutable enum DivisionLevel{SUBDIVIDED, LEVEL_USED, COMBINED} divisionLevel;
		mutable unsigned char edgeFlags;

		//unsigned int row, col;

		float maxError;
		float minDistanceSquared; //square of the minimum distance to camera for which this level is allowed to be used

		BoundingBox<float> bounds;
		Vec3f center;

//		shared_ptr<GraphicsManager::indexBuffer> foliageIBO;

		Patch();
		//void init(Patch* trunk, unsigned int levelsDeep, unsigned int totalLevels);

		//inline unsigned int levelOffset(unsigned int level) const
		//{
		//	unsigned int offset = 0;
		//	for(int i=0; i<level; i++)
		//	{
		//		offset += 1 << (2*i);
		//	}
		//	return offset;
		//}
		
	};
	class Page
	{
	public:
		struct foliagePatch
		{
			//struct plant
			//{
			//	Vec3f location;
			//	float height;
			//	mutable float screenDepth;
			//};
			BoundingBox<float> bounds;
			Vec3f center;
			shared_ptr<GraphicsManager::indexBuffer> plantIndexBuffer;
			//vector<plant> plants;
			mutable enum RenderType{DONT_RENDER,RENDER_BILLBAORD,RENDER_MODEL}renderType;
		};
		vector<foliagePatch> foliagePatches;
		unsigned int numTrees;

		Vec3f minXYZ;
		Vec3f maxXYZ;

		unsigned int height;
		unsigned int width;
		unsigned int LOD;

		unsigned int levelsDeep;
		unsigned int blockLength;

		unsigned short* heights;
		//Patch* trunk;
		shared_ptr<quadTree<Patch>> patches;


		//unsigned int VBO;
		shared_ptr<GraphicsManager::texture2D> texture;
		shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
		vector<vector<shared_ptr<GraphicsManager::indexBuffer>>> indexBuffers;

		shared_ptr<GraphicsManager::vertexBuffer> foliageVBO;

		//struct plant
		//{
		//	Vec3f location;
		//	float height;
		//	mutable float screenDepth;
		//};
		//vector<plant> foliage;

		//struct IndexBuffer
		//{
		//	unsigned int numVertices;
		//	unsigned int id;
		//}*indexBuffer;

		mutable vector<quadTree<Patch>::node*> renderQueue;

		Page(unsigned short* Heights, unsigned int patchResolution, Vec3f position, Vec3f scale, unsigned int lod=1);
		~Page();

		void render(shared_ptr<GraphicsManager::View> view, TerrainType shaderType) const;
		void renderFoliage(shared_ptr<GraphicsManager::View> view) const;

		void generateFoliage(float foliageDensity);

		void setSubdivided(quadTree<Patch>::node* n) const;
		void subdivide(quadTree<Patch>::node* n, const Vec3f& eye) const;
		void checkEdges() const;
		void patchEdges(quadTree<Patch>::node* n) const;

		Vec3f getNormal(Vec2f loc) const;
		float getHeight(Vec2f loc) const;
	protected:
		Vec3f interpolatedNormal(Vec2f loc) const;
		float interpolatedHeight(Vec2f loc) const;
		Vec3f rasterNormal(Vec2u loc) const;
		float rasterHeight(Vec2u loc) const;
	};



	Vec3f terrainPosition;
	Vec3f terrainScale;

	Circle<float> mBounds;
	bool waterPlane;
	
	//shared_ptr<GraphicsManager::vertexBuffer> foliageVBO;
	//shared_ptr<GraphicsManager::indexBuffer> foliageIBO;
	//vector<Vec3f> trees;
	//struct plant
	//{
	//	Vec3f location;
	//	float depth;
	//	plant(Vec3f l):location(l){}
	//};
	//mutable vector<plant> trees;

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
	shared_ptr<GraphicsManager::texture2D> treeTexture;
	//unsigned int skyTextureId; //id for the sky cube-map
	//unsigned int oceanTextureId; //id for the ocean normals cube map

	vector<shared_ptr<Page>> terrainPages;

	shared_ptr<Page> getPage(Vec2f position) const;
	shared_ptr<Page> getPage(Vec3f position) const;

	void generateOceanTexture();
	void resetTerrain();
public:
	Terrain():waterPlane(true){}
	virtual ~Terrain(){}
	void initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, TerrainType shader, float foliageDensity=0, unsigned int LOD=1);
	void renderTerrain(shared_ptr<GraphicsManager::View> view) const;
	void renderFoliage(shared_ptr<GraphicsManager::View> view) const;
	void generateSky(Angle theta, Angle phi, float zenithLumance);//theta = angle from up axis; phi = angle from south
	void generateTreeTexture(shared_ptr<SceneManager::mesh> treeMeshPtr);

	const Circle<float>& bounds() const{return mBounds;}

	float elevation(Vec2f v) const;
	float elevation(float x, float z) const;
	float altitude(Vec3f v) const;
	float altitude(float x, float y, float z) const;
	Vec3f terrainNormal(Vec2f) const;
	Vec3f terrainNormal(float x, float z) const;
	bool isLand(Vec2f v) const;
	bool isLand(float x, float z) const;
	//bool rayIntersection(Vec3f rayStart, Vec3f direction, Vec3f& intersectionPoint) const;
};