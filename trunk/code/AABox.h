/* ------------------------------------------------------

 Axis Aligned Boxes - Lighthouse3D

  -----------------------------------------------------*/


#ifndef _AABOX_
#define _AABOX_

#ifndef _Vec3f_
#include "vec3f.h"
#endif


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



AABox::AABox( Vec3f &corner,  float x, float y, float z) {

	setBox(corner,x,y,z);
}



AABox::AABox(void) {


	corner[0] = 0; corner[1] = 0; corner[2] = 0;

	x = 1.0f;
	y = 1.0f;
	z = 1.0f;
	
}


AABox::~AABox() {}

	

void AABox::setBox( Vec3f &v_corner,  float _x, float _y, float _z) {


	corner=v_corner;

	if (_x < 0.0) {
		_x = -_x;
		corner[0] -= _x;
	}
	if (_y < 0.0) {
		_y = -_y;
		corner[1] -= _y;
	}
	if (_z < 0.0) {
		_z = -_z;
		corner[2] -= _z;
	}
	x = _x;
	y = _y;
	z = _z;


}



Vec3f AABox::getVertexP(Vec3f &normal) {

	Vec3f res = corner;

	if (normal[0] > 0)
		res[0] += x;

	if (normal[1] > 0)
		res[1] += y;

	if (normal[2] > 0)
		res[2] += z;

	return(res);
}



Vec3f AABox::getVertexN(Vec3f &normal) {

	Vec3f res = corner;

	if (normal[0] < 0)
		res[0] += x;

	if (normal[1] < 0)
		res[1] += y;

	if (normal[2] < 0)
		res[2] += z;

	return(res);
}

	



#endif