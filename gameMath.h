#pragma once

#include "angle.h"
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"
#include "geoPlane.h"
#include "collide.h"

class SVertex
{
public:
	float x,y,z;
	float r,g,b,a;
	float s,t;
};

template<class T>
class Circle
{
public:
	Vector2<T> center;
	T radius;

	Circle(): center(0,0), radius(0) {}
	Circle(Vector2<T> c, T r): center(c), radius(r) {}
};

template<class T>
class Sphere
{
public:
	Vector3<T> center;
	T radius;

	Sphere(): center(0,0,0), radius(0) {}
	Sphere(Vector3<T> c, T r): center(c), radius(r) {}
	template<class U> Sphere operator+(const Vector3<U> &vec) const
	{
		return Sphere(center + vec,radius);
	}
	template<class U> Sphere operator-(const Vector3<U> &vec) const
	{
		return Sphere(center - vec,radius);
	}
	template<class U> Sphere operator*(const U &scale) const
	{
		return Sphere(center,radius*scale);
	}
	template<class U> Sphere operator/(const U &scale) const
	{
		return Sphere(center,radius/scale);
	}
};

template <class T>
class rectangle
{
public:
	T x, y, w, h;
	rectangle(): x(), y(), w(), h()
	{

	}
	rectangle(T X, T Y, T W, T H): x(X), y(Y), w(W), h(H)
	{

	}

	static rectangle XYWH(T X, T Y, T W, T H)
	{
		return rectangle(X,Y,W,H);
	}
	static rectangle XYWH(Vector2<T> origin, Vector2<T> size)
	{
		return rectangle(origin.x,origin.y,size.x,size.y);
	}
	static rectangle XYXY(T X, T Y, T X2, T Y2)
	{
		return rectangle(X,Y,X2-X,Y2-Y);
	}
	static rectangle XYXY(Vector2<T> p1, Vector2<T> p2)
	{
		return rectangle(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
	}
	static rectangle XXYY(T X, T X2, T Y, T Y2)
	{
		return rectangle(X,Y,X2-X,Y2-Y);
	}
	static rectangle XXYY(Vector2<T> xVals, Vector2<T> yVals)
	{
		return rectangle(xVals.x,yVals.x,xVals.y,yVals.y);
	}
	static rectangle CWH(T Cx, T Cy, T W, T H)
	{
		return rectangle(Cx-W/2,Cy-H/2,W,H);
	}
	static rectangle CWH(Vector2<T> center, Vector2<T> size)
	{
		return rectangle(center.x-size.x/2,center.y-size.y/2,size.x,size.y);
	}


	Vector2<T> bl() const
	{
		return Vector2<T>(x + (w >= 0 ? 0 : w), y + (h >= 0 ? 0 : h));
	}
	Vector2<T> tr() const
	{
		return Vector2<T>(x + (w <= 0 ? 0 : w), y + (h <= 0 ? 0 : h));
	}
	Vector2<T> tl() const
	{
		return Vector2<T>(x + (w <= 0 ? 0 : w), y + (h >= 0 ? 0 : h));
	}
	Vector2<T> br() const
	{
		return Vector2<T>(x + (w >= 0 ? 0 : w), y + (h <= 0 ? 0 : h));
	}
	Vector2<T> origin() const
	{
		return Vector2<T>(x ,y);
	}
	Vector2<T> size() const
	{
		return Vector2<T>(w, h);
	}

	T area() const
	{
		return abs(w * h);
	}

	template <class U>
	bool inRect(Vector2<U> v) const
	{
		return	((w >= 0 && v.x >= x && v.x <= x + w) || (w <= 0 && v.x <= x && v.x >= x + w)) && 
				((h >= 0 && v.y >= y && v.y <= y + h) || (h <= 0 && v.y <= y && v.y >= y + h));
	}

};

typedef rectangle<float>		Rect;

typedef rectangle<float>		Rect4f;
typedef rectangle<double>		Rect4d;
typedef rectangle<int>			Rect4i;
typedef rectangle<unsigned int>	Rect4u;
typedef rectangle<long>			Rect4l;

template <class T>
double dist_Point_to_Segment(Vector3<T> P, Vector3<T> S1,Vector3<T> S2)
{
	Vector3<T> v = S2 - S1;
	Vector3<T> w = P - S1;

	double c1 = w.dot(v);
	if ( c1 <= 0 )
		return P.distance(S1);

	double c2 = v.dot(v);
	if ( c2 <= c1 )
		return P.distance(S2);

	double b = c1 / c2;
	Vector3<T> Pb = S1 + v * b;
	return P.distance(Pb);
}

template <class T, class U>
bool SegmentSphereIntersect(Vector3<T> A, Vector3<T> B, Vector3<T> P, U r)
{
    Vector3<T> AB	= B - A;
    T ab2			= AB.magnitude();
    Vector3<T> AP	= P - A;
    T ap_dot_ab		= AP.dot(AB);
    T t				= ap_dot_ab / ab2; // the projection parameter on the line


    // clamp parameter to segment extremities

    if (t < 0.0f) t = 0.0f; else if (t > 1.0f) t = 1.0f;

    // calculate the closest point

    Vector3<T> Q = A + AB * t;

    return P.distanceSquared(Q) < r*r;
}

template <class T, class U, class V>
T lerp(T a, U b, V t)
{
	return a+(b-a)*t;
}

template <class T,class U,class V>
T clamp(T value, U v1, V v2)
{
	if(v1<v2)
	{
		if(value<v1) return (T)v1;
		if(value>v2) return (T)v2;
		else return value;
	}
	else
	{
		if(value>v1) return (T)v1;
		if(value<v2) return (T)v2;
		else return value;
	}
}

template <class T> T taylor(T t, T x, T dx)
{
	return x + dx * t;
}
template <class T> T taylor(T t, T x, T dx, T ddx)
{
	return x + dx * t + ddx * t*t;
}
template <class T> T taylor(T t, T x, T dx, T ddx, T dddx)
{
	return x + dx * t + ddx * t*t + dddx * t*t*t;
}
