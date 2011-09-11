/* ------------------------------------------------------

 View Frustum - Lighthouse3D

  -----------------------------------------------------*/

  #pragma once 

#ifndef _FRUSTUMG_
#define _FRUSTUMG_

class FrustumG 
{
private:

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};


public:

	static enum {OUTSIDE, INTERSECT, INSIDE};

	geoPlane pl[6];

	Vec3f eye, center, up;
	Vec3f ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle, tang;
	float nw,nh,fw,fh;

	void setCamInternals(float angle, float ratio, float nearD, float farD);
	void setCamDef(Vec3f &p, Vec3f &l, Vec3f &u);
	int pointInFrustum(Vec3f &p);
	int sphereInFrustum(Vec3f p, float radius);
	int boxInFrustum(AABox &b);

	Vec2f project(Vec3f p);
	//Vec3f unProject(Vec2f p);
};
#define ANG2RAD 3.14159265358979323846/180.0

#endif