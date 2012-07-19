
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
		Vec3f vertices[3];
		mutable Vec3f center;
		mutable float radius;

		void findRadius() const;
		triangle(Vec3f a,Vec3f b,Vec3f c):radiusValid(false),center(0,0,0),radius(0.0){vertices[0]=a;vertices[1]=b;vertices[2]=c;findRadius();}
		triangle():radiusValid(false),center(0,0,0),radius(0.0){vertices[0]=vertices[1]=vertices[2]=Vec3f(0,0,0);}
	};
	class triangleList
	{
	private:
		triangle* triangles;
		unsigned int numTriangles;

		Vec3f center;
		float radius;

		triangleList(const triangleList&);
		triangleList* operator=(const triangleList&);
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

	//Vec3f linePlaneCollision(const Vec3f& a, const Vec3f& b, const triangle& tri1) const;
	bool segmentPlaneCollision(const Vec3f& a, const Vec3f& b, const Plane3f& p, Vec3f& collisionPoint) const;
	//bool pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1) const;
	bool pointInTriangle(const triangle& tri,const Vec3f& vert) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2, Mat4f rot1, Mat4f rot2) const;
public:

	//void findPolygonRadius(triangle& tri);
	bool boundingCollision(const triangle& tri1, const triangle& tri2) const;
	bool operator() (const triangleList& t1, const triangleList& t2) const;
	bool operator() (const triangleList& t1, Vec3f center, float radius) const;
	bool operator() (const triangleList& t1, Vec3f lineStart, Vec3f lineEnd) const;

	bool operator() (objectType t1, objectType t2) const;
	bool operator() (objectType t1, Vec3f center, float radius) const;
	bool operator() (objectType t1, Vec3f lineStart, Vec3f lineEnd) const;

	bool operator() (shared_ptr<object> o1, shared_ptr<object> o2) const;
};
extern CollisionChecker& collisionCheck;
