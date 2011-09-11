
#include "main.h"

bool CollisionChecker::boundingCollision(const triangle& tri1, const triangle& tri2) const
{
	tri1.findRadius();
	tri2.findRadius();
	return tri1.center.distanceSquared(tri2.center) < (tri1.radius+tri2.radius)*(tri1.radius+tri2.radius);
}
Vec3f CollisionChecker::linePlaneCollision(const Vec3f& a, const Vec3f& b, const triangle& tri1) const
{
	float final_x,final_y,final_z,final_t;
	float t,i;
	Vec3f temp;

	t=0.0f; i=0.0f;
	i+=(tri1.pA*b.x)+(tri1.pB*b.y)+(tri1.pC*b.z)+(tri1.pD);
	t+=(tri1.pA*(b.x*-1))+(tri1.pB*(b.y*-1))+(tri1.pC*(b.z*-1));
	t+=(tri1.pA*a.x)+(tri1.pB*a.y)+(tri1.pC*a.z);

	// Be wary of possible divide-by-zeros here (i.e. if t==0)
	final_t = (-i)/t;

	// Vertical Line Segment
	if ((a.x == b.x)&&(a.z == b.z)) { // vertical line segment
		temp.x = a.x;
		temp.y = (-((tri1.pA*a.x)+(tri1.pC*a.z)+(tri1.pD)))/(tri1.pB);
		temp.z = a.z;

		return(temp);
	}

	final_x = (((a.x)*(final_t))+((b.x)*(1-final_t)));
	final_y = (((a.y)*(final_t))+((b.y)*(1-final_t)));
	final_z = (((a.z)*(final_t))+((b.z)*(1-final_t)));

	temp.x = final_x;
	temp.y = final_y;
	temp.z = final_z;

	return(temp);
}
bool CollisionChecker::pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1) const
{
	float t,i,final_t;

	t=0.0f; i=0.0f;
	i+=(tri1.pA*b.x)+(tri1.pB*b.y)+(tri1.pC*b.z)+(tri1.pD);
	t+=(tri1.pA*(b.x*-1))+(tri1.pB*(b.y*-1))+(tri1.pC*(b.z*-1));
	t+=(tri1.pA*a.x)+(tri1.pB*a.y)+(tri1.pC*a.z);

	// Be wary of possible divide-by-zeros here (i.e. if t==0)
	final_t = (-i)/t;

	return  (final_t >= 0) && (final_t <= 1);
}
bool CollisionChecker::pointInTriangle(const triangle& tri,const Vec3f& vert, bool x, bool y, bool z) const
{
	triangle tri2=tri;
	Vec3f p=vert;
	if(!x)
	{
		p.x=0;
		tri2.a.x=0;
		tri2.b.x=0;
		tri2.c.x=0;
	}
	else if(!y)
	{
		p.y=0;
		tri2.a.y=0;
		tri2.b.y=0;
		tri2.c.y=0;
	}
	else if(!z)
	{
		p.z=0;
		tri2.a.z=0;
		tri2.b.z=0;
		tri2.c.z=0;
	}
	Angle ang = acosA((tri2.a-p).normalize().dot((tri2.b-p).normalize())) + 
				acosA((tri2.b-p).normalize().dot((tri2.c-p).normalize())) + 
				acosA((tri2.c-p).normalize().dot((tri2.a-p).normalize()));
	return ang > 2.0 * PI - 0.01;
}
bool CollisionChecker::triangleCollision(const triangle& tri1, const triangle& tri2) const
{
	Vec3f p;
	bool temp = false;
  
	// Scroll thru 3 line segments of the other triangle

	// First iteration  (a,b)
	p=linePlaneCollision(tri2.a, tri2.b, tri1);
	// Determine which axis to project to
	if		((abs(tri1.pA) >= abs(tri1.pB)) && (abs(tri1.pA) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, false, true, true);	// X is greatest
	else if ((abs(tri1.pB) >= abs(tri1.pA)) && (abs(tri1.pB) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, true,false,true);	// Y is greatest
	else if ((abs(tri1.pC) >= abs(tri1.pA)) && (abs(tri1.pC) >= abs(tri1.pB)))	temp = pointInTriangle(tri1, p, true, true, false);	// Z is greatest
	if (temp) // Point needs to be checked to see if it lies between the two vertices, First check for the special case of vertical line segments,
		return (tri2.a.x == tri2.b.x && tri2.a.z==tri2.b.z && ( (tri2.a.y<=p.y && p.y<=tri2.b.y) || (tri2.b.y<=p.y && p.y <= tri2.a.y) )) || pointBetweenVertices(tri2.a, tri2.b, tri1);



	// Second iteration (b,c)
	p=linePlaneCollision(tri2.b, tri2.c, tri1);
	if		((abs(tri1.pA) >= abs(tri1.pB)) && (abs(tri1.pA) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, false, true, true);	// Determine which axis to project to
	else if ((abs(tri1.pB) >= abs(tri1.pA)) && (abs(tri1.pB) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, true, false, true);
	else if ((abs(tri1.pC) >= abs(tri1.pA)) && (abs(tri1.pC) >= abs(tri1.pB)))	temp = pointInTriangle(tri1, p, true, true, false);
	if (temp) return ((tri2.b.x == tri2.c.x) && (tri2.b.z == tri2.c.z) && (((tri2.b.y <= p.y) && (p.y <= tri2.c.y)) || ((tri2.c.y <= p.y) && (p.y <= tri2.b.y)))) || pointBetweenVertices(tri2.b, tri2.c, tri1);



	// Third iteration  (c,a)
	p=linePlaneCollision( tri2.c, tri2.a, tri1);
	if		((abs(tri1.pA) >= abs(tri1.pB)) && (abs(tri1.pA) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, false, true, true);	// Determine which axis to project to
	else if ((abs(tri1.pB) >= abs(tri1.pA)) && (abs(tri1.pB) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, true, false, true);
	else if ((abs(tri1.pC) >= abs(tri1.pA)) && (abs(tri1.pC) >= abs(tri1.pB)))	temp = pointInTriangle(tri1, p, true, true, false);
	if (temp) return ((tri2.c.x == tri2.a.x) && (tri2.c.z == tri2.a.z) && (((tri2.c.y <= p.y) && (p.y <= tri2.a.y)) || ((tri2.a.y <= p.y) && (p.y <= tri2.c.y)))) || pointBetweenVertices(tri2.c, tri2.a, tri1);


	return false; // Default value/no collision
}
Vec3f CollisionChecker::triangleList::getRandomVertex()
{
	int r = random<int>(numTriangles*3);
	if(r % 3 == 0)		return triangles[r / 3].a;
	else if(r % 3 == 1)	return triangles[r / 3].b;
	else				return triangles[r / 3].c;
}
CollisionChecker::triangleList::triangleList(Vec3f* vertices, unsigned int* faces, unsigned int nVertices, unsigned int nFaces):triangles(NULL), numTriangles(0), center(0.0f,0.0f,0.0f), radius(-99999999.9f)
{
	if(nVertices == 0 || nFaces == 0)
		return;

	float minx,miny,minz,maxx,maxy,maxz;
	Vec3f cross_v1xv2,p;
	int i;

	triangles = new triangle[nFaces];
	numTriangles = nFaces;
	for(i=0;i<nFaces;i++)
	{
		triangles[i].a=vertices[faces[i*3+0]];
		triangles[i].b=vertices[faces[i*3+1]];
		triangles[i].c=vertices[faces[i*3+2]];
		triangles[i].findRadius();

		Vec3f cross_v1xv2 = (triangles[i].b - triangles[i].a).cross(triangles[i].c - triangles[i].a);
		triangles[i].pA = cross_v1xv2.x;
		triangles[i].pB = cross_v1xv2.y;
		triangles[i].pC = cross_v1xv2.z;
		triangles[i].pD += (-(triangles[i].a.x))*(cross_v1xv2.x);
		triangles[i].pD += (-(triangles[i].a.y))*(cross_v1xv2.y);
		triangles[i].pD += (-(triangles[i].a.z))*(cross_v1xv2.z);


	}
	minx = maxx = vertices[0].x;
	miny = maxy = vertices[0].y;
	minz = maxz = vertices[0].z;
	for(i=1;i<nVertices;i++)
	{
		if(vertices[i].x<minx) minx=vertices[i].x;
		if(vertices[i].y<miny) miny=vertices[i].y;
		if(vertices[i].z<minz) minz=vertices[i].z;
		if(vertices[i].x>maxx) maxx=vertices[i].x;
		if(vertices[i].y>maxy) maxy=vertices[i].y;
		if(vertices[i].z>maxz) maxz=vertices[i].z;
	}
	center = Vec3f((minx+maxx)/2,((miny+maxy)/2),(minz+maxz)/2);
	float radiusSquared = center.distanceSquared(vertices[0]);
	for(i=1;i<nVertices;i++)
	{
		if (center.distanceSquared(vertices[i]) > radiusSquared) radiusSquared = center.distanceSquared(vertices[i]);
	}
	radius = sqrt(radiusSquared);
}
void CollisionChecker::triangle::findRadius() const
{
	if(radiusValid) return;
	float radiusSquared, minx,miny,minz,maxx,maxy,maxz;

	minx = maxx = a.x;
	miny = maxy = a.y;
	minz = maxz = a.z;

	if (b.x < minx)	minx = b.x;
	if (c.x < minx)	minx = c.x;
	if (b.y < miny)	miny = b.y;
	if (c.y < miny)	miny = c.y;
	if (b.z < minz)	minz = b.z;
	if (c.z < minz)	minz = c.z;
    
	if (b.x > maxx)	maxx = b.x;
	if (c.x > maxx)	maxx = c.x;
	if (b.y > maxy)	maxy = b.y;
	if (c.y > maxy)	maxy = c.y;
	if (b.z > maxz)	maxz = b.z;
	if (c.z > maxz)	maxz = c.z;

	center.x = ((minx+maxx)/2);
	center.y = ((miny+maxy)/2);
	center.z = ((minz+maxz)/2);

	radiusSquared = center.distanceSquared(a);
	if (center.distanceSquared(b) > radiusSquared)
		radiusSquared = center.distanceSquared(b);
  	if (center.distanceSquared(c) > radiusSquared)
		radiusSquared = center.distanceSquared(c);

	radius = sqrt(radiusSquared);
	radiusValid=true;
}

bool CollisionChecker::operator() (const triangleList& t1, const triangleList& t2) const
{
	if(t1.center.distanceSquared(t2.center) < (t1.radius+t2.radius)*(t1.radius+t2.radius))
	{
		int i1,i2;
		for(i1=0; i1 < t1.numTriangles; i1++)
		{
			if(t1.triangles[i1].center.distanceSquared(t2.center) < (t1.triangles[i1].radius+t2.radius)*(t1.triangles[i1].radius+t2.radius))
			{
				for(i2 = 0; i2 < t2.numTriangles; i2++)
				{
					if(t1.triangles[i1].center.distanceSquared(t2.triangles[i2].center) < (t1.triangles[i1].radius+t2.triangles[i2].radius)*(t1.triangles[i1].radius+t2.triangles[i2].radius))
					{
						if(triangleCollision(t1.triangles[i1],t2.triangles[i2]))
							return true;
					}
				}
			}
		}
	}
	return false;
}
bool CollisionChecker::operator() (const triangleList& t1, Vec3f center, float radius) const
{
	if(t1.center.distanceSquared(center) < (t1.radius+radius)*(t1.radius+radius))
	{
		int i1;
		for(i1=0; i1 < t1.numTriangles; i1++)
		{
			if(t1.triangles[i1].center.distanceSquared(center) < (t1.triangles[i1].radius+radius)*(t1.triangles[i1].radius+radius))
			{
				if(t1.triangles[i1].center.distanceSquared(center) < (t1.triangles[i1].radius+radius)*(t1.triangles[i1].radius+radius))
				{
					//we should also check that the triangle itself (not just its bounding sphere) intersects the shpere
						return true;
				}
			}
		}
	}
	return false;
}
bool CollisionChecker::operator() (const triangleList& t1, Vec3f lineStart, Vec3f lineEnd) const
{
	static double smallestD = 1000000000000000000.0;
	double d1 = t1.center.distanceSquared(lineStart);
	double d2 = t1.center.distanceSquared(lineEnd);

	//if(d1 < smallestD) smallestD = d1;
	//if(d2 < smallestD) smallestD = d2;
	//Profiler.setOutput("d value",sqrt(smallestD));

 	if(d1 < t1.radius*t1.radius || d2 < t1.radius*t1.radius)
		return true;

	float u = (t1.center - lineStart).dot(lineEnd - lineStart) / (lineEnd - lineStart).dot(lineEnd - lineStart);

	if(u < 0.0 || u > 1.0) return false;
	double d3 = t1.center.distanceSquared(lineStart+(lineEnd-lineStart)*u);
	//if(d3 < smallestD) smallestD = d3;
	return d3 < t1.radius*t1.radius;
}

bool CollisionChecker::operator() (objectType t1, objectType t2) const
{
	return operator()(*dataManager.getModel(t1)->trl,*dataManager.getModel(t2)->trl);
}
bool CollisionChecker::operator() (objectType t1, Vec3f center, float radius) const
{
	return operator()(*dataManager.getModel(t1)->trl, center, radius);
}
bool CollisionChecker::operator() (objectType t1, Vec3f lineStart, Vec3f lineEnd) const
{
	return operator()(*dataManager.getModel(t1)->trl, lineStart, lineEnd);
}

bool CollisionChecker::operator() (object* o1, object* o2) const
{
	auto tr1 = dataManager.getModel(o1->type)->trl;
	auto tr2 = dataManager.getModel(o2->type)->trl;

	Quat4f rot1 = o1->rotation;
	Quat4f rot2 = o2->rotation;

	Vec3f pos1 = o1->position;
	Vec3f pos2 = o2->position;

	Vec3f center1 = o1->position + rot1 * tr1->center;
	Vec3f center2 = o2->position + rot2 * tr2->center;

	if(center1.distanceSquared(center2) < (tr1->radius+tr2->radius)*(tr1->radius+tr2->radius))
	{
		int i1,i2;
		for(i1=0; i1 < tr1->numTriangles; i1++)
		{
			if((pos1 + rot1*tr1->triangles[i1].center).distanceSquared(center2) < (tr1->triangles[i1].radius+tr2->radius)*(tr1->triangles[i1].radius+tr2->radius))
			{
				for(i2 = 0; i2 < tr2->numTriangles; i2++)
				{
					if((pos1 + rot1*tr1->triangles[i1].center).distanceSquared(pos2 + rot2*tr2->triangles[i2].center) < (tr1->triangles[i1].radius+tr2->triangles[i2].radius)*(tr1->triangles[i1].radius+tr2->triangles[i2].radius))
					{
						if(triangleCollision(tr1->triangles[i1],tr2->triangles[i2]))
							return true;
					}
				}
			}
		}
	}
	return false;
}