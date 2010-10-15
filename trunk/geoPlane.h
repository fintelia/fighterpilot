// Plane.h
//
//////////////////////////////////////////////////////////////////////

#pragma once 

#ifndef _PLANE_
#define _PLANE_

class geoPlane  
{

public:

	Vec3f normal,point;
	float d;


	geoPlane::geoPlane( Vec3f &v1,  Vec3f &v2,  Vec3f &v3);
	geoPlane::geoPlane(void);
	geoPlane::~geoPlane();

	void set3Points( Vec3f &v1,  Vec3f &v2,  Vec3f &v3);
	void setNormalAndPoint(Vec3f &normal, Vec3f &point);
	void setCoefficients(float a, float b, float c, float d);
	float distance(Vec3f &p);

	void print();

};

#endif