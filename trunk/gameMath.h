#pragma once 

#include "basicMath.h"
#include "angle.h"
//#include "vec3f.h"
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"
#include "geoPlane.h"//-------------------------//
#include "AABox.h"/////---for frustum culling---//
#include "FrustumG.h"//-------------------------//
#include "collide.h"

class SVertex
{
public:
	float x,y,z;
	float r,g,b,a;
	float s,t;
//	float padding[28];
};

template <class T>
class rectangle
{
public:
	T x, y, w, h;

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
		return rectangle(Cx-W/2,Cy-W/2,W,H);
	}
	static rectangle CWH(Vector2<T> center, Vector2<T> size)
	{
		return rectangle(center.x-size.x/2,center.y-size.y/2,size.x,size.y);
	}


	Vector2<T> bl()
	{
		return Vector2<T>(x + (w >= 0) ? 0 : w,y + (h >= 0) ? 0 : h);
	}
	Vector2<T> tr()
	{
		return Vector2<T>(x + (w <= 0) ? 0 : w,y + (h <= 0) ? 0 : h);
	}
	Vector2<T> tl()
	{
		return Vector2<T>(x + (w <= 0) ? 0 : w,y + (h >= 0) ? 0 : h);
	}
	Vector2<T> br()
	{
		return Vector2<T>(x + (w >= 0) ? 0 : w,y + (h <= 0) ? 0 : h);
	}
	Vector2<T> origin()
	{
		return Vector2<T>(x ,y);
	}
	Vector2<T> size()
	{
		return Vector2<T>(w, h);
	}

	T area()
	{
		return abs(w * h);
	}

};

typedef rectangle<float>		Rect;

typedef rectangle<float>		Rect4f;
typedef rectangle<double>		Rect4d;
typedef rectangle<int>			Rect4i;
typedef rectangle<unsigned int>	Rect4u;
typedef rectangle<long>			Rect4l;

template <class T>
void upAndRight(Vector3<T> fwd,Angle roll,Vector3<T>& up,Vector3<T>& right)
{
	if(fwd!=Vec3f(0,1,0))
	{
		Vec3f axis1=-((fwd.cross(Vec3f(0,1,0))).cross(fwd)).normalize();
		Vec3f axis2=fwd.cross(axis1).normalize();
		Vec3f s1=axis1*sin(-roll);
		right=s1+axis2*cos(-roll);
	}//+(float)PI/2
	else
		right=Vec3f(sin(roll+PI/2.0),0,cos(roll+PI/2.0));

	up=(fwd.cross(right)).normalize();
}

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

template<class T>
class circle
{
public:
	Vector2<T> center;
	T radius;

	circle(): center(0,0), radius(0) {}
	circle(Vector2<T> c, T r): center(c), radius(r) {}
};