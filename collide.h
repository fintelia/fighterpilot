
class CollisionChecker
{
public:
	static CollisionChecker& getInstance()
	{
		static CollisionChecker* pInstance = new CollisionChecker();
		return *pInstance;
	}
	struct triangle
	{
		Vec3f a,b,c;
		Vec3f center;
		float pA,pB,pC,pD; // Plane normals
		float radius;

		void findRadius();
	};
	class triangleList
	{
	private:
		triangle* triangles;
		unsigned int numTriangles;

		Vec3f center;
		float radius;
	public:
		triangleList(Vec3f* vertices, unsigned int* faces, unsigned int nVertices, unsigned int nFaces);
		triangleList(): triangles(0), numTriangles(0), center(0,0,0), radius(-999999999.9f) {}
		~triangleList(){if(triangles) delete[] triangles;}
		friend class CollisionChecker;
	};
private:
	CollisionChecker(){}
	~CollisionChecker(){}

	Vec3f linePlaneCollision(const Vec3f& a, const Vec3f& b, const triangle& tri1);
	bool pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1);
	bool pointInTriangle(const triangle& tri,const Vec3f& vert, bool x, bool y, bool z);
	bool triangleCollision(const triangle& tri1, const triangle& tri2);
public:
	
	//void findPolygonRadius(triangle& tri);
	bool boundingCollision(triangle& tri1, triangle& tri2);
	bool operator() (triangleList t1, triangleList t2);
};
extern CollisionChecker& collisionCheck;