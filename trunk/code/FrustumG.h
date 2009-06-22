/* ------------------------------------------------------

 View Frustum - Lighthouse3D

  -----------------------------------------------------*/


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


	Vec3f ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle,tang;
	float nw,nh,fw,fh;

	void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD);
	void FrustumG::setCamDef(Vec3f &p, Vec3f &l, Vec3f &u);
	int FrustumG::pointInFrustum(Vec3f &p);
	int FrustumG::sphereInFrustum(Vec3f &p, float raio);
	int FrustumG::boxInFrustum(AABox &b);

	void FrustumG::drawPoints();
	void FrustumG::drawLines();
	void FrustumG::drawgeoPlanes();
	void FrustumG::drawNormals();
};
#define ANG2RAD 3.14159265358979323846/180.0

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

void FrustumG::setCamDef(Vec3f &p, Vec3f &l, Vec3f &u) {

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

int FrustumG::pointInFrustum(Vec3f &p) {

	int result = INSIDE;
	for(int i=0; i < 6; i++) {

		if (pl[i].distance(p) < 0)
			return OUTSIDE;
	}
	return(result);

}


int FrustumG::sphereInFrustum(Vec3f &p, float raio) {

	int result = INSIDE;
	float distance;

	for(int i=0; i < 6; i++) {
		distance = pl[i].distance(p);
		if (distance < -raio)
			return OUTSIDE;
		else if (distance < raio)
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

#endif