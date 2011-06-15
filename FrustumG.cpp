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





void FrustumG::drawPoints() {


	glBegin(GL_POINTS);

		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(nbl[0],nbl[1],nbl[2]);
		glVertex3f(nbr[0],nbr[1],nbr[2]);

		glVertex3f(ftl[0],ftl[1],ftl[2]);
		glVertex3f(ftr[0],ftr[1],ftr[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);

	glEnd();
}


void FrustumG::drawLines() {

	glBegin(GL_LINE_LOOP);
	//near plane
		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(nbr[0],nbr[1],nbr[2]);
		glVertex3f(nbl[0],nbl[1],nbl[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//far plane
		glVertex3f(ftr[0],ftr[1],ftr[2]);
		glVertex3f(ftl[0],ftl[1],ftl[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//bottom plane
		glVertex3f(nbl[0],nbl[1],nbl[2]);
		glVertex3f(nbr[0],nbr[1],nbr[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//top plane
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(ftl[0],ftl[1],ftl[2]);
		glVertex3f(ftr[0],ftr[1],ftr[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	//left plane
		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(nbl[0],nbl[1],nbl[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);
		glVertex3f(ftl[0],ftl[1],ftl[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	// right plane
		glVertex3f(nbr[0],nbr[1],nbr[2]);
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(ftr[0],ftr[1],ftr[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);

	glEnd();
}


void FrustumG::drawgeoPlanes() {

	glBegin(GL_QUADS);

	//near plane
		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(nbr[0],nbr[1],nbr[2]);
		glVertex3f(nbl[0],nbl[1],nbl[2]);

	//far plane
		glVertex3f(ftr[0],ftr[1],ftr[2]);
		glVertex3f(ftl[0],ftl[1],ftl[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);

	//bottom plane
		glVertex3f(nbl[0],nbl[1],nbl[2]);
		glVertex3f(nbr[0],nbr[1],nbr[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);

	//top plane
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(ftl[0],ftl[1],ftl[2]);
		glVertex3f(ftr[0],ftr[1],ftr[2]);

	//left plane

		glVertex3f(ntl[0],ntl[1],ntl[2]);
		glVertex3f(nbl[0],nbl[1],nbl[2]);
		glVertex3f(fbl[0],fbl[1],fbl[2]);
		glVertex3f(ftl[0],ftl[1],ftl[2]);

	// right plane
		glVertex3f(nbr[0],nbr[1],nbr[2]);
		glVertex3f(ntr[0],ntr[1],ntr[2]);
		glVertex3f(ftr[0],ftr[1],ftr[2]);
		glVertex3f(fbr[0],fbr[1],fbr[2]);

	glEnd();

}

void FrustumG::drawNormals() {

	Vec3f a,b;

	glBegin(GL_LINES);

		// near
		a = (ntr + ntl + nbr + nbl) * 0.25;
		b = a + pl[NEARP].normal;
		glVertex3f(a[0],a[1],a[2]);
		glVertex3f(b[0],b[1],b[2]);

		// far
		a = (ftr + ftl + fbr + fbl) * 0.25;
		b = a + pl[FARP].normal;
		glVertex3f(a[0],a[1],a[2]);
		glVertex3f(b[0],b[1],b[2]);

		// left
		a = (ftl + fbl + nbl + ntl) * 0.25;
		b = a + pl[LEFT].normal;
		glVertex3f(a[0],a[1],a[2]);
		glVertex3f(b[0],b[1],b[2]);
		
		// right
		a = (ftr + nbr + fbr + ntr) * 0.25;
		b = a + pl[RIGHT].normal;
		glVertex3f(a[0],a[1],a[2]);
		glVertex3f(b[0],b[1],b[2]);
		
		// top
		a = (ftr + ftl + ntr + ntl) * 0.25;
		b = a + pl[TOP].normal;
		glVertex3f(a[0],a[1],a[2]);
		glVertex3f(b[0],b[1],b[2]);
		
		// bottom
		a = (fbr + fbl + nbr + nbl) * 0.25;
		b = a + pl[BOTTOM].normal;
		glVertex3f(a[0],a[1],a[2]);
		glVertex3f(b[0],b[1],b[2]);

	glEnd();


}
