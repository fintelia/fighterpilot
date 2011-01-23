
template <class T>
class Vector2
{
public:
	T x,y;

	Vector2();
	Vector2(T x, T y);

	T &operator[](int index);
	T operator[](int index) const;
	Vector2 operator*(T scale) const;
	Vector2 operator/(T scale) const;
	Vector2 operator+(const Vector2 &other) const;
	Vector2 operator-(const Vector2 &other) const;
	Vector2 operator-() const;

	const Vector2 &operator*=(T scale);
	const Vector2 &operator/=(T scale);
	const Vector2 &operator+=(const Vector2 &other);
	const Vector2 &operator-=(const Vector2 &other);

	bool operator!=(const Vector2 &other);
	bool operator==(const Vector2 &other);

	T magnitude() const;
	T magnitudeSquared() const;
	T signedMagnitude() const;
	Vector2 normalize() const;
	Vector2 normalizeSigned() const;
	T dot(const Vector2 &other) const;
	void set(T x, T y);
	bool equal(Vector2 v,T maxDifference=0.01);
	T distance(Vector2 v);
	T distanceSquared(Vector2 v);
};
typedef Vector2<float>			Vec2f;
typedef Vector2<double>			Vec2d;
typedef Vector2<int>			Vec2i;
typedef Vector2<unsigned int>	Vec2u;
typedef Vector2<long>			Vec2l;
template <class T>
class Vector3
{
public:
	T x,y,z;
	Vector3();
	Vector3(T x, T y, T z);

	T &operator[](int index);
	T operator[](int index) const;
		
	Vector3 operator*(T scale) const;
	Vector3 operator/(T scale) const;
	Vector3 operator+(const Vector3 &other) const;
	Vector3 operator-(const Vector3 &other) const;
	Vector3 operator-() const;

	const Vector3 &operator*=(T scale);
	const Vector3 &operator/=(T scale);
	const Vector3 &operator+=(const Vector3 &other);
	const Vector3 &operator-=(const Vector3 &other);

	bool operator!=(const Vector3 &other) const;
	bool operator==(const Vector3 &other) const;

	T magnitude() const;
	T magnitudeSquared() const;
	T signedMagnitude() const;
	Vector3 normalize() const;
	Vector3 normalizeSigned() const;
	T dot(const Vector3 &other) const;
	Vector3 cross(const Vector3 &other) const;
	void set(T x, T y, T z);
	bool equal(Vector3 v,T maxDifference=0.01) const;
	T distance(Vector3 v) const;
	T distanceSquared(Vector3 v) const;
};
typedef Vector3<float>			Vec3f;
typedef Vector3<double>			Vec3d;
typedef Vector3<int>			Vec3i;
typedef Vector3<unsigned int>	Vec3u;
typedef Vector3<long>			Vec3l;
/////////////////////////////////////////////////////////////definitions///////////////////////////////////////////////////////////////////////
template <class T>
Vector3<T>::Vector3(): x(0), y(0), z(0)
{
	
}

template <class T>
Vector3<T>::Vector3(T X, T Y, T Z): x(X), y(Y), z(Z)
{

}

template <class T>
T& Vector3<T>::operator[](int index)
{
	if(index==0)	return x;
	if(index==1)	return y;
	if(index==2)	return z;
	return z;
}

template <class T>
T Vector3<T>::operator[](int index) const
{
	if(index==0)	return x;
	if(index==1)	return y;
	if(index==2)	return z;
	return 0;
}

template <class T>
Vector3<T> Vector3<T>::operator*(T scale) const
{
	return Vector3(x * scale, y * scale, z * scale);
}
template <class T>
Vector3<T> Vector3<T>::operator/(T scale) const
{
	return Vector3(x / scale, y / scale, z / scale);
}
template <class T>
Vector3<T> Vector3<T>::operator+(const Vector3 &other) const
{
	return Vector3(x + other.x, y + other.y, z + other.z);
}
template <class T>
Vector3<T> Vector3<T>::operator-(const Vector3 &other) const
{
	return Vector3(x - other.x, y - other.y, z - other.z);
}
template <class T>
Vector3<T> Vector3<T>::operator-() const
{
	return Vector3(-x, -y, -z);
}
template <class T>
const Vector3<T>& Vector3<T>::operator*=(T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}
template <class T>
const Vector3<T>& Vector3<T>::operator/=(T scale)
{
	x /= scale;
	y /= scale;
	z /= scale;
	return *this;
}
template <class T>
const Vector3<T>& Vector3<T>::operator+=(const Vector3 &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}
template <class T>
const Vector3<T>& Vector3<T>::operator-=(const Vector3 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
template <class T>
bool Vector3<T>::operator!=(const Vector3 &other) const
{
	return !equal(other);
}
template <class T>
bool Vector3<T>::operator==(const Vector3 &other) const
{
	return equal(other);
}

template <class T>
T Vector3<T>::magnitude() const
{
	return sqrt(x * x + y * y + z * z);
}
template <class T>
T Vector3<T>::magnitudeSquared() const
{
	return x * x + y * y + z * z;
}
template <class T>
T Vector3<T>::signedMagnitude() const
{
	return sqrt(x*x*x/abs(x) + y*y*y/abs(y) + z*z*z/abs(z));
}
template <class T>
Vector3<T> Vector3<T>::normalize() const
{
	T m = sqrt(x * x + y * y + z * z);
	return Vector3(x / m, y / m, z / m);
}
template <class T>
Vector3<T> Vector3<T>::normalizeSigned() const
{
	float m = sqrt(x * x + y * y + z * z);
	if(x+y+z<0) m*=-1;
	return Vector3(x / m, y / m, z / m);
}
template <class T>
T Vector3<T>::dot(const Vector3 &other) const
{
	return x * other.x + y * other.y + z * other.z;
}
template <class T>
Vector3<T> Vector3<T>::cross(const Vector3 &other) const
{
	return Vec3f(y * other.z - z * other.y,
			 z * other.x - x * other.z,
			 x * other.y - y * other.x);
}
template <class T>
void Vector3<T>::set(T X, T Y, T Z)
{
	x=X;
	y=Y;
	z=Z;
}
template <class T>
bool Vector3<T>::equal(Vector3 v,T maxDifference=0.01) const
{
	return (abs(x-v.x)< maxDifference && abs(y-v.y)< maxDifference && abs(z-v.z)< maxDifference);
}
template <class T>
T Vector3<T>::distance(Vector3 v) const
{
	return sqrt((v.x-x)*(v.y-y)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z));
}
template <class T>
T Vector3<T>::distanceSquared(Vector3 v) const
{
	return (v.x-x)*(v.y-y)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z);
}

template <class T>
Vector3<T> lerp(const Vector3<T>& v1, const Vector3<T>& v2, T t)
{
	if (t <= (T)0.0)
		return v1;
	if (t >= (T)1.0)
		return v2;

	return (v1 * ((T)1.0 - t) + v2 * t);
};

////////////////////////////////////////////////////////////Vector2/////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Vector2<T>::Vector2(): x(0), y(0)
{

}
template <class T>
Vector2<T>::Vector2(T X, T Y): x(X), y(Y)
{

}

template <class T>
T &Vector2<T>::operator[](int index)
{
	if(index==0)	return x;
	if(index==1)	return y;
	return 0;
}
template <class T>
T Vector2<T>::operator[](int index) const
{
	if(index==0)	return x;
	if(index==1)	return y;
	return 0;
}
template <class T>
Vector2<T> Vector2<T>::operator*(T scale) const
{
	return Vector2(x * scale, y * scale);
}
template <class T>
Vector2<T> Vector2<T>::operator/(T scale) const
{
	return Vector2(x / scale, y / scale);
}
template <class T>
Vector2<T> Vector2<T>::operator+(const Vector2 &other) const
{
	return Vector2(x + scale, y + scale);
}
template <class T>
Vector2<T> Vector2<T>::operator-(const Vector2 &other) const
{
	return Vector2(x - scale, y - scale);
}
template <class T>
Vector2<T> Vector2<T>::operator-() const
{
	return Vector2(-x, -y);
}
template <class T>
const Vector2<T> &Vector2<T>::operator*=(T scale)
{
	x *= scale;
	y *= scale;
	return *this;
}
template <class T>
const Vector2<T> &Vector2<T>::operator/=(T scale)
{
	x /= scale;
	y /= scale;
	return *this;
}
template <class T>
const Vector2<T> &Vector2<T>::operator+=(const Vector2 &other)
{
	x += other.x;
	y += other.y;
	return *this;
}
template <class T>
const Vector2<T> &Vector2<T>::operator-=(const Vector2 &other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}
template <class T>
bool Vector2<T>::operator!=(const Vector2 &other)
{
	return !equal(other);
}
template <class T>
bool Vector2<T>::operator==(const Vector2 &other)
{
	return equal(other);
}
template <class T>
T Vector2<T>::magnitude() const
{
	return sqrt(x * x + y * y);
}
template <class T>
T Vector2<T>::magnitudeSquared() const
{
	return x * x + y * y;
}
template <class T>
T Vector2<T>::signedMagnitude() const
{
	T m sqrt(x*x+ y*y);
	if(x+y<0) m*=-1;
	return m;
}
template <class T>
Vector2<T> Vector2<T>::normalize() const
{
	T m = sqrt(x * x + y * y);
	return Vector2(x / m, y / m);
}
template <class T>
Vector2<T> Vector2<T>::normalizeSigned() const
{
	T m = sqrt(x * x + y * y);
	if(x+y<0) m*=-1;
	return Vector2(x / m, y / m);
}
template <class T>
T Vector2<T>::dot(const Vector2 &other) const
{
	return x * other.x + y * other.y;
}
template <class T>
void Vector2<T>::set(T X, T Y)
{
	x = X;
	y = Y;
}
template <class T>
bool Vector2<T>::equal(Vector2 v,T maxDifference=0.01)
{
	return (abs(x-v.x)< maxDifference && abs(y-v.y)< maxDifference);
}
template <class T>
T Vector2<T>::distance(Vector2 v)
{
	return sqrt((x-v.x)*(x-v.x)+(y-v.y)*(y-v.y));
}
template <class T>
T Vector2<T>::distanceSquared(Vector2 v)
{
	return (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y);
}
template <class T>
Vector2<T> lerp(const Vector2<T>& v1, const Vector2<T>& v2, T t)
{
	if (t <= (T)0.0)
		return v1;
	if (t >= (T)1.0)
		return v2;

	return (v1 * ((T)1.0 - t) + v2 * t).normalize();
};


//Vec3f operator*(float scale, const Vec3f &v);
//void upAndRight(Vec3f fwd,Angle roll,Vec3f& up,Vec3f& right);
//float dist(Vec3f P1,Vec3f P2);
//float dist_Point_to_Segment( Vec3f P, Vec3f S1,Vec3f S2);
//double intersect(Vec3f rOrigin, Vec3f rNormal, Vec3f pOrigin, Vec3f pNormal);
//Vec3f collide(Vec3f rOrigin, Vec3f rNormal, Vec3f pOrigin, Vec3f pNormal);
//float dist_squared(Vec3f P1,Vec3f P2);
//bool SegmentSphereIntersect(Vec3f A, Vec3f B, Vec3f P, float r);