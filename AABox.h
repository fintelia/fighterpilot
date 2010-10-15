/* ------------------------------------------------------

 Axis Aligned Boxes - Lighthouse3D

  -----------------------------------------------------*/

#pragma once 

#ifndef _AABOX_
#define _AABOX_

//#ifndef _Vec3f_
//#include "vec3f.h"
//#endif


class AABox 
{

public:

	Vec3f corner;
	float x,y,z;


	AABox::AABox( Vec3f &corner, float x, float y, float z);
	AABox::AABox(void);
	AABox::~AABox();

	void AABox::setBox( Vec3f &corner, float x, float y, float z);

	// for use in frustum computations
	Vec3f AABox::getVertexP(Vec3f &normal);
	Vec3f AABox::getVertexN(Vec3f &normal);


};


#endif