#include "main.h" 
geoPlane::geoPlane( Vec3f &v1,  Vec3f &v2,  Vec3f &v3) {

	set3Points(v1,v2,v3);
}


geoPlane::geoPlane() {}

geoPlane::~geoPlane() {}


void geoPlane::set3Points( Vec3f &v1,  Vec3f &v2,  Vec3f &v3) {


	Vec3f aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = aux2.cross(aux1);

	normal=normal.normalize();
	point=v2;
	d = -1.0*normal.dot(point);
}

void geoPlane::setNormalAndPoint(Vec3f &_normal, Vec3f &_point) {

	normal=_normal;
	normal=normal.normalize();
	d = -1.0*(normal.dot(_point));
}

void geoPlane::setCoefficients(float a, float b, float c, float _d) {

	// set the normal vector
	normal.set(a,b,c);
	//compute the lenght of the vector
	float l = normal.magnitude();
	// normalize the vector
	normal.set(a/l,b/l,c/l);
	// and divide d by th length as well
	d = _d/l;
}


	

float geoPlane::distance(Vec3f &p) {

	return (d + normal.dot(p));
}

void geoPlane::print() {

	//no need
}
