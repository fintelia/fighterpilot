#include "main.h"
void FrustumG::setCamInternals(float _angle, float _ratio, float _nearD, float _farD) {

	ratio = _ratio;
	angle = _angle;
	nearD = _nearD;
	farD = _farD;

	tang = (float)tan(_angle* ANG2RAD * 0.5) ;
	nh = _nearD * tang;
	nw = nh * _ratio; 
	fh = _farD  * tang;
	fw = fh * _ratio;

}

void FrustumG::setCamDef(Vec3f &p, Vec3f &l, Vec3f &u) 
{
	eye = p;
	center = l;
	up = u;

	Vec3f dir,nc,fc,X,Y,Z;

	Z = p - l;
	Z=Z.normalize();

	X = u.cross(Z);
	X=X.normalize();

	Y = Z.cross(X);

	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	pl[TOP].set3Points(ntr,ntl,ftl);
	pl[BOTTOM].set3Points(nbl,nbr,fbr);
	pl[LEFT].set3Points(ntl,nbl,fbl);
	pl[RIGHT].set3Points(nbr,ntr,fbr);
	pl[NEARP].set3Points(ntl,ntr,nbr);
	pl[FARP].set3Points(ftr,ftl,fbl);
}

Vec2f FrustumG::project(Vec3f p)  //seems only to work in first person mode???
{
	p -= eye;

	Vec3f f = (center - eye).normalize();
	Vec3f UP = up.normalize();

	Vec3f s = (f.cross(UP)).normalize();
	Vec3f u = (s.cross(f)).normalize();
	
	float F = 1.0/tan((angle*PI/180.0) / 2.0);

	Vec3f v = Vec3f(s.dot(p)*F/ratio,   -u.dot(p)*F,   f.dot(p)*(nearD+farD)-2.0*nearD*farD) / (f.dot(p));

	return Vec2f( (v.x + 1.0) / 2.0, (v.y + 1.0) / 2.0 );
}

int FrustumG::pointInFrustum(Vec3f &p) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(p) < 0)
			return OUTSIDE;
	}
	return(result);

}


int FrustumG::sphereInFrustum(Vec3f p, float radius) {

	int result = INSIDE;
	float distance;

	for(int i=0; i < 6; i++) {
		distance = pl[i].distance(p);
		if (distance < -radius)
			return OUTSIDE;
		else if (distance < radius)
			result =  INTERSECT;
	}
	return(result);

}


int FrustumG::boxInFrustum(AABox &b) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(b.getVertexP(pl[i].normal)) < 0)
			return OUTSIDE;
		else if (pl[i].distance(b.getVertexN(pl[i].normal)) < 0)
			result =  INTERSECT;
	}
	return(result);

 }