
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
		float minHeight;
		float maxHeight;

		float sideLength;
		unsigned int layerResolution;

		shared_ptr<GraphicsManager::texture2DArray> texture;

		vector<unique_ptr<float[]>> layers;
		
		/**
		 * Uses the data from heights to create textures for all levels that do
		 * not have them.
		 */
//		void generateTextures();

		friend class Terrain;
		friend class FractalNode;

	public:
		ClipMap(float sideLength, unsigned int layerResolution, vector<unique_ptr<float[]>>&& layers);

		float getMinHeight() const { return minHeight; }
		float getMaxHeight() const { return maxHeight; }
		float getSideLength() const { return sideLength; }
		float getHeight(unsigned int layer, unsigned int x, unsigned int z) const { return layers[layer][x + z * layerResolution]; }
		float getHeight(float x, float z);
        
		void bindTexture(unsigned int textureUnit = 0) { texture->bind(textureUnit); }

		unsigned int getLayerResolution() const { return layerResolution; }
		unsigned int getNumLayers() const { return layers.size(); }
	};
private:
    class GpuClipMap
    {
    private:
        // Length of one side of the (square) clipMap in world space.
        const float sideLength;
        // Resolution of each of the layers. Must be a power of two.
        const unsigned int layerResolution;
        // Number of layers that are pinned. Pinned layers are not proceedurally
        // generated and must remain centered. Thus, all lower levels of the
        // clipMap are constrained to be entirely inside the area of the
        // smallest pinned layer.
        const unsigned int numPinnedLayers;
        // Resolution of the clipmap if the most detailed layer were present
        // across the entire range of the clipmap. 
        const unsigned int resolution;
        // Ratio of block resolution to the mesh resolution. Setting this value
        // greater than 1 allows us to leave additional information for the
        // normal map. Must be a power of two.
        const unsigned int blockStep;
        // Resolution of the mesh used to represent a block of a layer.
        const unsigned int meshResolution;
        // The resolution of a block. It is generally around half the size of a
        // layer. This leaves us a wide border around the block so we don't have
        // to update it every frame, but rather only when the viewer moves by a
        // significant amount.
        const unsigned int blockResolution;
        
        shared_ptr<GraphicsManager::vertexBuffer> clipMapVBO;
        shared_ptr<GraphicsManager::indexBuffer> clipMapIBO;
        unsigned int numRingIndices;
        unsigned int numCenterIndices;
        
        struct Layer{
            // Each layer represents a progressively higher resolution, but
            // smaller, area of the clipmap.  The first numPinnedLayers layers
            // are set at initialization and have a fixed location. All other
            // layers are generated dynamically from the most detailed pinned
            // layer.

            
            // Where the region is relative to the rest of the clipmap. This
            // value is relative to the coordinates of the most detailed layer
            // so it must be between (-resolution/2) and (resolution/2-1)
            // inclusive in each dimension. However, since this value is the
            // center of the layer, we also have to make sure that the layer
            // extents do not exceed that range either. Additionally, no
            // nonpinned layer can be outside the extents of the previous
            // layer. All pinned layers must remain at (0,0).
            Vec2i center;
            // This is the value that center would have if we regenerated the
            // layer to be centered around the eye position. Since we avoid
            // regenerating the whole thing when the camera only moves a little
            // bit, this might not be exactly the same as center.
            Vec2i targetCenter;
            // Which texel in the texture corresponds to the center of the
            // region. Since the texture wraps around, this may not actually be
            // the center of the texture.
            Vec2u textureCenter;
            
            // Texture arrays containing the current state of the clipMap.
            shared_ptr<GraphicsManager::texture2D> heights;
            shared_ptr<GraphicsManager::texture2D> normals;
            shared_ptr<GraphicsManager::texture2D> shoreDistance;
        };
        vector<Layer> layers;

        Vec2i worldCenterToLayerPosition(unsigned int layer, Vec2f center);

        void synthesizeHeightmap(unsigned int layer);
        void generateAuxiliaryMaps(unsigned int layer);
        void regenLayer(unsigned int layer);
        
    public:
        GpuClipMap(float sLength, unsigned int resolution, unsigned int num_layers, const vector<unique_ptr<float[]>>& pinnedLayers);
        void centerClipMap(Vec2f center);
        void render(shared_ptr<GraphicsManager::View> view);
    };
    mutable unique_ptr<GpuClipMap> gpuClipMap;
    shared_ptr<ClipMap> clipMap;
    
    struct TerrainData {
		/** space taken by a single vertex in the buffer */
		const unsigned int vertexSize;
        /** space taken by each tile in the vertex buffer */
        const unsigned int vertexStep;
        /** number of vertices in each tile */
        const unsigned int verticesPerTile;

        /** positions of each segment in the index buffer */
        vector<unsigned int> segmentOffsets;
        /** number of triangles for segment in the index buffer */
        vector<unsigned int> segmentCounts;

        shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
		shared_ptr<GraphicsManager::indexBuffer> indexBuffer;
        shared_ptr<GraphicsManager::multiDraw> multiDraw;
        IndexPool nodeIndices;
        
        TerrainData(unsigned int totalIndices, unsigned int tileResolution);
    } mutable terrainData;

	class FractalNode
	{
	public:
		static const unsigned int tileResolution;
		static const unsigned int textureResolution;
		class no_indices_remaining{};

	private:
		friend class Terrain;
		//static unsigned int maxNodes;
		static unsigned int totalNodes;
		static unsigned int frameNumber;
		//static shared_ptr<GraphicsManager::vertexBuffer> vertexBuffer;
		//static std::array<shared_ptr<GraphicsManager::indexBuffer>, 16> indexBuffers;
		//static shared_ptr<GraphicsManager::texture2D> subdivideTexture;
		//static shared_ptr<GraphicsManager::texture2D> queryTexture;
		//static vector<unsigned int> unassignedTextureIndices;

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

		shared_ptr<GraphicsManager::texture2D> treeTexture;
		shared_ptr<GraphicsManager::vertexBuffer> treesVBO;
		unsigned int treesVBOcount;

		shared_ptr<ClipMap> clipMap;
		unsigned int clipMapLayerIndex;
		Vec2f clipMapOrigin;
		float clipMapStep;

        IndexPool::IndexPtr index;
		
		unsigned int getLayerSize();
		unsigned int getLayerStep();

		void recursiveEnvoke(std::function<void(FractalNode*)> func);
		void reverseRecursiveEnvoke(std::function<void(FractalNode*)> func);
		
		void computeError();
		void subdivide(TerrainData& terrainData);
		void generateTrees();
		void generateTreeTexture();
		void generateTreeDensityTexture();
		void pruneChildren();
//		float getHeight(unsigned int x, unsigned int z) const;

		FractalNode(const FractalNode&)=delete;
		void operator=(const FractalNode&)=delete;

	public:
		FractalNode(FractalNode* parent, unsigned int level, Vec2i coordinates,
					shared_ptr<ClipMap> clipMap, TerrainData& terrainData);
		~FractalNode();
		void renderTrees(shared_ptr<GraphicsManager::View> view);
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

	void renderTerrain(shared_ptr<GraphicsManager::View> view) const;
	void renderFoliage(shared_ptr<GraphicsManager::View> view) const;

	void addDecal(Vec2f center, float width, float height, string texture,
				  double startTime, double fadeInLength=500.0);

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
