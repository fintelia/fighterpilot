
enum TerrainType{TERRAIN_ISLAND, TERRAIN_MOUNTAINS, TERRAIN_SNOW, TERRAIN_DESERT};


class Terrain
{
public:
	/**
	 * A ClipMap represents a square region of `sideLength` x `sideLength` 
	 * meters. It is composed of a number of layers. Layer 0 represents the
	 * entire region, and each successive layer represents half the previous
	 * one.
	 * 
	 * Each layer is composed of `layerResolution` x `layerResolution` floats
	 * stored in a 1D array, as well as a texture representation used for 
	 * rendering.
	 *
	 * Unlike  traditional implementations of a clipmap, this does not support
	 * loading new data. All layers are, and must remain, centered at (0,0). 
	 */
	class ClipMap
	{
	private:
		float sideLength;
		unsigned int layerResolution;
		struct Layer
		{
			float minHeight;
			float maxHeight;
			unique_ptr<float[]> heights;
			shared_ptr<GraphicsManager::texture2D> texture;

			Layer(const Layer&)/*=delete*/;
			Layer(Layer&& layer);

		public:
			Layer(){}
		};
		vector<Layer> layers;
		
		/**
		 * Uses the data from heights to create textures for all levels that do
		 * not have them.
		 */
//		void generateTextures();

		friend class Terrain;
		friend class FractalNode;

	public:
		ClipMap(unsigned int layerResolution, float sideLength);
		void addLayer(unique_ptr<float[]> heights);
	};

private:
	class FractalNode
	{
	public:
		static unsigned int frameNumber;
		static const unsigned int tileResolution;
		static const unsigned int textureResolution;
	private:
		static unsigned int totalNodes;
		static shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
		static std::array<shared_ptr<GraphicsManager::indexBuffer>, 16> indexBuffers;
		static shared_ptr<GraphicsManager::texture2D> subdivideTexture;
		static shared_ptr<GraphicsManager::texture2D> queryTexture;

		enum Directions{LEFT=1, RIGHT=2, TOP=4, BOTTOM=8};
		enum DivisionLevel{FRUSTUM_CULLED, SUBDIVIDED, LEVEL_USED, COMBINED};
		//enum TileType{LAND, SHORE, OCEAN};
		DivisionLevel divisionLevel;
		DivisionLevel waterDivisionLevel;

		//TileType tileType;

		unsigned int level; //0 = top level node, 1+ = subdivided
		Vec2i coordinates; //from `0` to `2^level-1`
		float sideLength; //in meters
		float minHeight;
		float maxHeight;

		//the minimum distance to camera for which this level is
		//allowed to be used
		float minDistance;
		//float maxError;
		//BoundingBox<float> bounds;
		Vec2f origin; //center in XZ plane

		//will be different than above due to curvature of the earth
		//BoundingBox<float> worldBounds;
		Vec3f worldCenter;
		BoundingBox<float> worldBounds;

		//last frameNumber on which this node was rendered
		unsigned int lastUseFrame;

		FractalNode* parent;
		std::array<weak_ptr<FractalNode>,4> neighbors; // = {L, R, T, B}
		std::array<shared_ptr<FractalNode>,4> children; // = {TL, TR, BL, BR}

		shared_ptr<GraphicsManager::texture2D> texture;

		shared_ptr<GraphicsManager::vertexBuffer> treesVBO;
		shared_ptr<GraphicsManager::indexBuffer> treesIBO;

		shared_ptr<ClipMap> clipMap;
		unsigned int clipMapLayer;
		Vec2f clipMapOrigin;
		float clipMapStep;
		
		void recursiveEnvoke(std::function<void(FractalNode*)> func);
		void reverseRecursiveEnvoke(std::function<void(FractalNode*)> func);
		
		void computeError();
		void subdivide();
		void pruneGrandchildren();
		float getHeight(unsigned int x, unsigned int z) const;

	public:
		FractalNode(FractalNode* parent, unsigned int level, Vec2i coordinates, shared_ptr<ClipMap> clipMap);
		~FractalNode();
		void render(shared_ptr<GraphicsManager::View> view, shared_ptr<GraphicsManager::shader> shader);
		void renderWater(shared_ptr<GraphicsManager::View> view, shared_ptr<GraphicsManager::shader> shader);
		float getWorldHeight(Vec2f worldPos) const;

		unsigned int computeSubdivision(shared_ptr<GraphicsManager::View> view, unsigned int maxDivisions);
		unsigned int computeWaterSubdivision(shared_ptr<GraphicsManager::View> view, unsigned int maxDivisions);
		void patchEdges();

		static void initialize();
		static void cleanUp();
	};

	template<unsigned int N>
	struct WaveGroup{
		float amplitudes[N];
		float frequencies[N];
		float speeds[N];
		Vec2f directions[N];
		unsigned int size()const{return N;}
	};
		
	WaveGroup<16> waves;
	shared_ptr<GraphicsManager::texture2D> waveTexture;
	static const unsigned int waveTextureResolution;
	static const float waveTextureScale;

	static const double earthRadius;
/*	struct decal
	{
		string texture;
		shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
		shared_ptr<GraphicsManager::indexBuffer> indexBuffer;
		double startTime;
		double fadeLength;

		decal(string tex, shared_ptr<GraphicsManager::vertexBuffer> vbo, shared_ptr<GraphicsManager::indexBuffer> ibo, double sTime, double fLength);
	};
	vector<shared_ptr<decal>> decals;*/

	shared_ptr<GraphicsManager::textureCube> skyTexture;
	shared_ptr<GraphicsManager::texture3D> oceanTexture;
	shared_ptr<GraphicsManager::texture2D> treeTexture;

	shared_ptr<GraphicsManager::vertexBuffer> waterVBO;
	shared_ptr<GraphicsManager::indexBuffer> waterIBO;

	unique_ptr<FractalNode> fractalTerrain;

	double currentTime;
	bool wireframe;


	void generateOceanTexture();
	void resetTerrain();
	void generateSky(Vec3f sunDirection);
	void generateTreeTexture(shared_ptr<SceneManager::mesh> treeMeshPtr);

	void renderFractalTerrain(shared_ptr<GraphicsManager::View> view) const;
	void renderFractalWater(shared_ptr<GraphicsManager::View> view) const;

protected:
	void terrainFrameUpdate(double currentTime);

public:
	Terrain(shared_ptr<ClipMap> clipMap);
	virtual ~Terrain();

	void renderTerrain(shared_ptr<GraphicsManager::View> view) const;
	void renderFoliage(shared_ptr<GraphicsManager::View> view) const;

	void addDecal(Vec2f center, float width, float height, string texture, double startTime, double fadeInLength=500.0);

//	const Circle<float>& bounds() const{return mBounds;}

	void setWireframe(bool w);

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
