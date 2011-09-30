
template <class T>
class Plane
{
public:
	Vector3<T> normal,point;
	T d;

	Plane(): normal(0,1,0), point(0,0,0), d(0)
	{
		
	}
	Plane(Vector3<T> p1, Vector3<T> p2, Vector3<T> p3): normal(((p1-p2).cross(p3-p2)).normalize()), point(p2), d(-normal.dot(point))
	{
		
	}
	Plane(Vector3<T> Normal, Vector3<T> Point): normal(Normal.normalize()), point(Point), d(-normal.dot(point))
	{

	}
	Plane(T A, T B, T C, T D): normal(A,B,C), point(), d(D)
	{
		T l = normal.magnitude();

		normal /= l;
		d /= l;

		point = normal * d;
	}

	template<class U>
	T distance(Vector3<U> p)
	{
		return d + normal.dot(p);
	}
};

typedef Plane<float>			Plane3f;
typedef Plane<double>			Plane3d;