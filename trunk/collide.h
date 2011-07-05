
class object;

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
	private:
		mutable bool radiusValid;
	public:
		Vec3f a,b,c;
		float pA,pB,pC,pD; // Plane normals
		mutable Vec3f center;
		mutable float radius;

		void findRadius() const;
		triangle(Vec3f A,Vec3f B,Vec3f C):radiusValid(false),a(A),b(B),c(C),center(0,0,0),radius(0.0){findRadius();}
		triangle():radiusValid(false),a(0,0,0),b(0,0,0),c(0,0,0),center(0,0,0),radius(0.0){}
	};
	class triangleList
	{
	private:
		triangle* triangles;
		unsigned int numTriangles;

		Vec3f center;
		float radius;
	public:
		Vec3f getCenter(){return center;}
		float getRadius(){return radius;}
		Vec3f getRandomVertex();
		triangleList(Vec3f* vertices, unsigned int* faces, unsigned int nVertices, unsigned int nFaces);
		triangleList(): triangles(0), numTriangles(0), center(0,0,0), radius(-999999999.9f) {}
		~triangleList(){if(triangles) delete[] triangles;}
		friend class CollisionChecker;
	};
private:
	CollisionChecker(){}
	~CollisionChecker(){}

	Vec3f linePlaneCollision(const Vec3f& a, const Vec3f& b, const triangle& tri1) const;
	bool pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1) const;
	bool pointInTriangle(const triangle& tri,const Vec3f& vert, bool x, bool y, bool z) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2) const;
public:
	
	//void findPolygonRadius(triangle& tri);
	bool boundingCollision(const triangle& tri1, const triangle& tri2) const;
	bool operator() (const triangleList& t1, const triangleList& t2) const;
	bool operator() (const triangleList& t1, Vec3f center, float radius) const;
	bool operator() (const triangleList& t1, Vec3f lineStart, Vec3f lineEnd) const;

	bool operator() (objectType t1, objectType t2) const;
	bool operator() (objectType t1, Vec3f center, float radius) const;
	bool operator() (objectType t1, Vec3f lineStart, Vec3f lineEnd) const;

	bool operator() (object* o1, object* o2) const;
};
extern CollisionChecker& collisionCheck;