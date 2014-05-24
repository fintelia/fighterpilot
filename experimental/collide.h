
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
		Vec3f mCenter;
		float mRadius;
		Vec3f mVertices[3];
	public:	

		float radius() const;
		Vec3f center() const;
		Vec3f vertex(unsigned int i) const;
		void setVertices(Vec3f v1, Vec3f v2, Vec3f v3);
		triangle(Vec3f v1, Vec3f v2, Vec3f v3):mCenter(0,0,0),mRadius(0.0){setVertices(v1, v2, v3);}
		triangle():mCenter(0,0,0),mRadius(0.0){mVertices[0]=mVertices[1]=mVertices[2]=Vec3f(0,0,0);}
	};
	class collisionBounds
	{
	public:
		const enum Type{SPHERE,MESH}type;
		Sphere<float> sphere;
		collisionBounds(Sphere<float> s):type(SPHERE),sphere(s){}
		virtual ~collisionBounds(){}
	protected:
		collisionBounds(Type t):type(t){}
	};
	class collisionMesh: public collisionBounds
	{
	public:
		vector<Vec3f> vertices;
		vector<triangle> triangles;
		collisionMesh():collisionBounds(MESH){}
	};
	class collisionInstance
	{
	private:
		weak_ptr<collisionBounds> collisionBoundsPtr;
		Mat4f transformationMatrix;
		Mat4f lastTransformationMatrix;

		friend class CollisionManager;
		collisionInstance(shared_ptr<collisionBounds> boundsPtr, Mat4f transformation): collisionBoundsPtr(boundsPtr), transformationMatrix(transformation), lastTransformationMatrix(transformation) {}

	public:
		void update(const Mat4f& transformation, const Mat4f& lastTransformation);
		Sphere<float> boundingSphere() const; //does not factor in scale!
	};
private:
	CollisionManager(){}

public:

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
	bool testRayTriangle(Vec3f p, Vec3f d, Vec3f v0, Vec3f v1, Vec3f v2, Vec3f& intersectionPoint) const;
	bool testRayAABB(Vec3f p, Vec3f d, BoundingBox<float> AABB, Vec3f& intersectionPoint);
	bool testTriangleTriangle(Vec3f t1_a, Vec3f t1_b, Vec3f t1_c, Vec3f t2_a, Vec3f t2_b, Vec3f t2_c) const;
	//bool testLineTriangle(Vec3f lStart, Vec3f lEnd, Vec3f t1, Vec3f t2, Vec3f t3, Vec3f& intersectionPoint) const;
	bool testSphereSegment(Sphere<float> sphere, Vec3f lineStart, Vec3f lineEnd) const;

	Vec3f closestPointOnSegment(Vec3f s1, Vec3f s2, Vec3f point) const;
	bool sphereTriangleCollision(const triangle& a, const Mat4f& m, const Sphere<float>& s) const;
	//Vec3f linePlaneCollision(const Vec3f& a, const Vec3f& b, const triangle& tri1) const;
	bool segmentPlaneCollision(const Vec3f& a, const Vec3f& b, const Plane3f& p, Vec3f& collisionPoint) const;
	//bool pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1) const;
	bool pointInTriangle(const triangle& tri,const Vec3f& vert) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2) const;
	bool triangleCollision(const triangle& tri1, const triangle& tri2, Mat4f rot1, Mat4f rot2) const;

	//bool triangleSweptSphere(const triangle& a, const Mat4f& m, const Sphere<float>& s, Vec3f sphereInitial, Vec3f sphereFinal, float radius)const;


	//void setCollsionBounds(objectType type, Sphere<float> s);
	//void setCollsionBounds(objectType type, Sphere<float> s, const vector<Vec3f>& vertices, const vector<unsigned int>& indices);
	//shared_ptr<const collisionBounds> getCollisionBoundsPtr(objectType type) const;

	bool groundCollsion(shared_ptr<object> o1) const;
	bool operator() (shared_ptr<object> o1, Vec3f lineStart, Vec3f lineEnd) const;
	bool operator() (shared_ptr<object> o1, shared_ptr<object> o2) const;

	shared_ptr<collisionInstance> newCollisionInstance(shared_ptr<collisionBounds> collisionBoundsPtr, Mat4f transformation=Mat4f());

private:
	//friend class physicsInstance;
	//map<majorObjectType, vector<std::weak_ptr<physicsInstance>>> physicsInstances;
};
//typedef shared_ptr<PhysicsManager::physicsInstance> physicsInstancePtr;
typedef shared_ptr<CollisionManager::collisionInstance> collisionBoundsPtr;
extern CollisionManager& collisionManager;

