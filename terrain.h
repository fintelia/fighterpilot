
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


#ifdef VISUAL_STUDIO //since we don't have full C++11 support
			Layer(const Layer&)/*=delete*/;
#else
			Layer(const Layer&)=delete;
#endif
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
		static const unsigned int tileResolution;
		static const unsigned int textureResolution;
		class no_indices_remaining{};

	private:
		friend class Terrain;
		static unsigned int maxNodes;
		static unsigned int totalNodes;
		static unsigned int frameNumber;
		static shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
		static std::array<shared_ptr<GraphicsManager::indexBuffer>, 16> indexBuffers;
		static shared_ptr<GraphicsManager::texture2D> subdivideTexture;
		static shared_ptr<GraphicsManager::texture2D> queryTexture;
		static vector<unsigned int> unassignedTextureIndices;

		enum Directions{LEFT=1, RIGHT=2, TOP=4, BOTTOM=8};
		enum DivisionLevel{FRUSTUM_CULLED, SUBDIVIDED, LEVEL_USED, COMBINED};
		DivisionLevel divisionLevel;

		unsigned int level; //0 = top level node, 1+ = subdivided
		Vec2i coordinates; //from `0` to `2^level-1`
		float sideLength; //in meters
		float minHeight;
		float maxHeight;

		float minDistance;
		//BoundingBox<float> bounds;
		Vec2f origin; //center in XZ plane

		//will be different than above due to curvature of the earth
		//BoundingBox<float> worldBounds;
		Vec3f worldCenter;
		BoundingBox<float> worldBounds;

		//last frameNumber on which this node was rendered
		unsigned int lastUseFrame;

		FractalNode* parent;

		/**
		 *  ordering: {Left, Right, Top, Bottom}
		 */
		std::array<weak_ptr<FractalNode>,4> neighbors;

		/**
		 * Invariant: either all children must be valid, or none are
		 *
		 * ordering: {Top Left, Top Right, Bottom Left, Bottom Right}
		 */
		std::array<shared_ptr<FractalNode>,4> children;

		shared_ptr<GraphicsManager::texture2D> texture;
		shared_ptr<GraphicsManager::texture2D> treeTexture;
		shared_ptr<GraphicsManager::vertexBuffer> treesVBO;
		unsigned int treesVBOcount;

		shared_ptr<ClipMap> clipMap;
		unsigned int clipMapLayer;
		Vec2f clipMapOrigin;
		float clipMapStep;

		unsigned int textureArrayIndex;
		
		void recursiveEnvoke(std::function<void(FractalNode*)> func);
		void reverseRecursiveEnvoke(std::function<void(FractalNode*)> func);
		
		void computeError();
		void subdivide();
		void generateTrees();
		void generateTreeTexture();
		void generateTreeDensityTexture();
		void pruneChildren();
		float getHeight(unsigned int x, unsigned int z) const;

#ifdef VISUAL_STUDIO //since we don't have full C++11 support
		FractalNode(const FractalNode&)/*=delete*/;
		void operator=(const FractalNode&)/*=delete*/;
#else
		FractalNode(const FractalNode&)=delete;
		void operator=(const FractalNode&)=delete;
#endif

	public:
		FractalNode(FractalNode* parent, unsigned int level, Vec2i coordinates, shared_ptr<ClipMap> clipMap);
		~FractalNode();
		void render(shared_ptr<GraphicsManager::View> view, shared_ptr<GraphicsManager::shader> shader);
		void renderTrees(shared_ptr<GraphicsManager::View> view);
		float getWorldHeight(Vec2f worldPos) const;
		static void initialize();
		static void cleanUp();
	};

	template<unsigned int N>
	struct WaveGroup{
		float amplitudes[N];
		float frequencies[N];
		float speeds[N];
		Vec2f directions[N];
		float amplitudeSum;
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

	unsigned int computeFractalSubdivision(shared_ptr<GraphicsManager::View> view, unsigned int maxDivisions) const;

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
