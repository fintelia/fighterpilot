
class object;

class CollisionManager
{
public:
	static CollisionManager& getInstance()
	{
		static CollisionManager* pInstance = new CollisionManager();
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
	//class triangleList
	//{
	//private:
	//	triangle* triangles;
	//	unsigned int numTriangles;

	//	Vec3f center;
	//	float radius;

	//	triangleList(const triangleList&);
	//	triangleList* operator=(const triangleList&);
	//public:
	//	Vec3f getCenter(){return center;}
	//	float getRadius(){return radius;}
	//	Vec3f getRandomVertex();
	//	triangleList(Vec3f* vertices, unsigned int* faces, unsigned int nVertices, unsigned int nFaces);
	//	triangleList(): triangles(0), numTriangles(0), center(0,0,0), radius(-999999999.9f) {}
	//	~triangleList(){if(triangles) delete[] triangles;}
	//	friend class CollisionChecker;
	//};

private:
	CollisionManager(){}
	~CollisionManager(){}

	struct collisionBounds
	{
	public:
		enum Type{SPHERE,MESH}type;
		Sphere<float> sphere;
		collisionBounds(Sphere<float> s):type(SPHERE),sphere(s){}
		virtual ~collisionBounds(){}
	protected:
		collisionBounds(Type t):type(t){}
	};
	//struct collisionSphere: public collisionBounds
	//{
	//	collisionSphere():collisionBounds(SPHERE){}
	//	collisionSphere(Sphere<float> s):collisionBounds(SPHERE),sphere(s){}
	//};
	struct collisionMesh: public collisionBounds
	{
		vector<Vec3f> vertices;
		vector<triangle> triangles;
		collisionMesh():collisionBounds(MESH){}
	};
	map<objectType, shared_ptr<collisionBounds>> objectBounds;

	//closest point
	Vec3f cpPlanePoint(Plane<float> plane, Vec3f point) const;
	Vec3f cpSegmentPoint(Vec3f sInitial, Vec3f sFinal, Vec3f point) const;
	Vec3f cpTrianglePoint(Vec3f t1, Vec3f t2, Vec3f t3, Vec3f point) const;

	//distance
	float squareDistanceSegmentPoint(Vec3f sInitial, Vec3f sFinal, Vec3f point)const;
	float squareDistanceSegmentSegment(Vec3f s1_initial, Vec3f s1_final, Vec3f s2_initial, Vec3f s2_final, Vec3f& s1_closest, Vec3f& s2_closest) const;

	//tests
	bool sweptSphereSphere(Sphere<float> s1, Vec3f v1, Sphere<float> s2, Vec3f v2) const;
	bool testSegmentPlane(Vec3f sInitial, Vec3f sFinal, Plane<float> plane, Vec3f& intersectionPoint) const;
	bool testRaySphere(Vec3f p, Vec3f d, Sphere<float> s, Vec3f& intersectionPoint) const;
	bool testTriangleTriangle(Vec3f t1_a, Vec3f t1_b, Vec3f t1_c, Vec3f t2_a, Vec3f t2_b, Vec3f t2_c) const;
	//bool testLineTriangle(Vec3f lStart, Vec3f lEnd, Vec3f t1, Vec3f t2, Vec3f t3, Vec3f& intersectionPoint) const;

	Vec3f closestPointOnSegment(Vec3f s1, Vec3f s2, Vec3f point) const;
	bool sphereTriangleCollision(const triangle& a, const Mat4f& m, const Sphere<float>& s) const;
	//Vec3f linePlaneCollision(const Vec3f& a, const Vec3f& b, const triangle& tri1) const;
	bool segmentPlaneCollision(const Vec3f& a, const Vec3f& b, const Plane3f& p, Vec3f& collisionPoint) const;
	//bool pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1) const;
	bool pointInTriangle(const triangle& tri,const Vec3f& vert) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2, Mat4f rot1, Mat4f rot2) const;

	//bool triangleSweptSphere(const triangle& a, const Mat4f& m, const Sphere<float>& s, Vec3f sphereInitial, Vec3f sphereFinal, float radius)const;

public:
	void setCollsionBounds(objectType type, Sphere<float> s);
	void setCollsionBounds(objectType type, Sphere<float> s, const vector<Vec3f>& vertices, const vector<unsigned int>& indices);

	bool groundCollsion(shared_ptr<object> o1) const;
	bool operator() (shared_ptr<object> o1, Vec3f lineStart, Vec3f lineEnd) const;
	bool operator() (shared_ptr<object> o1, shared_ptr<object> o2) const;

private:
	//friend class physicsInstance;
	//map<majorObjectType, vector<std::weak_ptr<physicsInstance>>> physicsInstances;
};
//typedef shared_ptr<PhysicsManager::physicsInstance> physicsInstancePtr;
extern CollisionManager& collisionManager;