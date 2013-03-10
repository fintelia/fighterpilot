
#include "engine.h"

//
//	Much of the code for PhysicsManager is from "Real-Time Collision Detection" by Christer Ericson
//

Vec3f CollisionManager::cpPlanePoint(Plane<float> plane, Vec3f point) const
{
	float t = (plane.normal).dot(point) - plane.d; // t = signed distance between plane and point
	return point - plane.normal * t;
}
Vec3f CollisionManager::cpSegmentPoint(Vec3f sInitial, Vec3f sFinal, Vec3f point) const
{
	Vec3f s = sFinal - sInitial;
	float t = (point-sInitial).dot(s);
	if(t <= 0.0f)
	{
		//t = 0.0;
		return sInitial;
	}
	else
	{
		float denom = s.magnitudeSquared();
		if(t > denom)
		{
			//t = 1.0;
			return sFinal;
		}
		else
		{
			t = t / denom;
			return sInitial + s * t;
		}
	}
}
Vec3f CollisionManager::cpTrianglePoint(Vec3f a, Vec3f b, Vec3f c, Vec3f point) const
{
	Vec3f ab = b - a;
	Vec3f ac = c - a;

	Vec3f ap = point - a;
	float d1 = ab.dot(ap);
	float d2 = ac.dot(ap);
	if(d1 <= 0.0f && d2 <= 0.0f)
		return a;

	Vec3f bp = point - b;
	float d3 = ab.dot(bp);
	float d4 = ac.dot(bp);
	if(d3 >= 0.0f && d4 <= d3)
		return b;

	Vec3f cp = point - c;
	float d5 = ab.dot(cp);
	float d6 = ac.dot(cp);
	if(d6 >= 0.0f && d5 <= d6)
		return c;

	float vc = d1*d4 - d3*d2;
	if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return a + ab * v;
	}

	float vb = d5*d2 - d1*d6;
	if(vb < 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return a + ac * w;
	}

	float va = d3*d6 - d5*d4;
	if(va <= 0.0f && (d4-d3) >= 0.0f && (d5-d6) >= 0.0f)
	{
		float w = (d4-d3) / ((d4-d3) + (d5-d6));
		return b + (c - b) * w;
	}

	float invDenom = 1.0 / (va+vb+vc);
	float v = vb * invDenom;
	float w = vc * invDenom;
	return a + ab * v + ac * w;
}
float CollisionManager::squareDistanceSegmentSegment(Vec3f s1_initial, Vec3f s1_final, Vec3f s2_initial, Vec3f s2_final, Vec3f& s1_closest, Vec3f& s2_closest) const //returns closest point on s1
{
	Vec3f d1 = s1_final - s1_initial;
	Vec3f d2 = s2_final - s2_initial;
	Vec3f r = s1_initial - s2_initial;
	float a = d1.magnitudeSquared();
	float e = d2.magnitudeSquared();
	float f = d2.dot(r);

	float s;
	float t;

	if(a <= 1e-5 && e <= 1e-5)
	{
		s1_closest = s1_initial;
		s2_closest = s2_initial;
		return s1_initial.distanceSquared(s2_initial);
	}
	else if(a <= 1e-5)
	{
		s = 0.0;
		t = f / e;
		s = clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = d1.dot(d2);
		if(e <= 1e-5)
		{
			t = 0.0f;
			s = clamp(-c/a, 0.0,1.0);
		}
		else
		{
			float b = d1.dot(d2);
			float denom = a*e - b*b; //always nonnegative

			if(denom != 0.0f)
			{
				s = clamp((b*f-c*e) / denom, 0.0, 1.0);
			}
			else
			{
				s = 0.0f;
			}

			float tnom = b*s + f;
			if(tnom < 0.0f)
			{
				t = 0.0f;
				s = clamp(-c/a, 0.0, 1.0);
			}
			else if(tnom > e)
			{
				t = 1.0;
				s = clamp((b-c) / a, 0.0, 1.0);
			}
			else
			{
				t = tnom / e;
			}
		}
	}
	s1_closest = s1_initial + d1 * s;
	s2_closest = s2_initial + d1 * t;
	return s1_closest.distanceSquared(s2_closest);
}
float CollisionManager::squareDistanceSegmentPoint(Vec3f sInitial, Vec3f sFinal, Vec3f point) const
{
	Vec3f s = sFinal - sInitial;
	Vec3f ip = point - sInitial;
	Vec3f fp = point - sFinal;

	float e = ip.dot(s);
	if(e < 0.0f) return ip.magnitudeSquared();

	float f = s.magnitudeSquared();
	if(e >= f) return fp.magnitudeSquared();

	return ip.magnitudeSquared() - e * e / f;
}
bool CollisionManager::sweptSphereSphere(Sphere<float> s1, Vec3f v1, Sphere<float> s2, Vec3f v2) const
{
	Vec3f s = s2.center - s1.center;
	Vec3f v = v2 - v1;
	float r = s2.radius + s1.radius;
	float c = s.magnitudeSquared() - r*r;

	if(c < 0.0f)
	{
		return true; //spheres initially overlap
	}

	float a = v.magnitudeSquared();
	if(a <= 1e-5) return false; //spheres not moving

	float b = v.dot(s);
	if(b >= 0.0f) return false; //spheres not moving towards each other

	float d = b*b - a*c;
	if(d < 0.0f) return false; //no real root: spheres do not intersect

	float t = (-b - sqrt(d)) / a;
	return t <= 1.0;
}
bool CollisionManager::testSegmentPlane(Vec3f sInitial, Vec3f sFinal, Plane<float> plane, Vec3f& intersectionPoint) const
{
	Vec3f s = sFinal - sInitial;
	float t = (plane.d - (plane.normal).dot(sInitial)) / (plane.normal).dot(s);
	if(t >= 0.0f && t <= 1.0f)
	{
		intersectionPoint = sInitial + s * t;
		return true;
	}
	return false;
}
bool CollisionManager::testRaySphere(Vec3f p, Vec3f d, Sphere<float> s, Vec3f& intersectionPoint) const
{
	Vec3f m = p - s.center;
	float b = m.dot(d);
	float c = m.magnitudeSquared() - s.radius*s.radius;

	if(c > 0.0f && b > 0.0f)	// ray is outside of and pointing away from sphere
		return false;

	float discr = b*b - c;
	if(discr < 0.0f)			//ray misses sphere
		return false;

	float t = -b - sqrt(discr);
	if(t < 0.0f)				//ray started inside sphere
		t = 0.0f;

	intersectionPoint = p + d*t;
	return true;
}
// see:http://people.cs.clemson.edu/~dhouse/courses/881/papers/raytriangle-moeller02.pdf
// u,v = barycentric coordinates
// t = distance from ray origin
bool CollisionManager::testRayTriangle(Vec3f o, Vec3f d, Vec3f v0, Vec3f v1, Vec3f v2, Vec3f& intersectionPoint) const
{
	Vec3f e1 = v1 - v0;
	Vec3f e2 = v2 - v0;
	Vec3f p = d.cross(e2);
	float a = e1.dot(p);

	if(a > -0.0001 && a < 0.0001)
		return false;

	double f = 1.0/a;
	Vec3f s = o - v0;
	float u = f * s.dot(p);

	if(u < 0.0 || u > 1.0)
		return false;

	Vec3f q = s.cross(e1);
	float v = f * d.dot(q);

	if(v < 0.0 || u+v > 1.0)
		return false;

	//double t = f * e2.dot(q); // t is distance along ray to intesection point

	intersectionPoint = v0 + e1*u + e2*v; //o + t * d;

//	Vec3f intersectionPoint2 = (v2 + v1 + v0) / 3.0; //point that *should* be on the triangle


	//check distance to triangle plane
	//Vec3f normal = ((v1-v0).cross(v2-v0)).normalize();
	//float distancesToPlane[] = {normal.dot(intersectionPoint - v0),
	//							normal.dot(intersectionPoint - v1),
	//							normal.dot(intersectionPoint - v2),
	//							normal.dot(intersectionPoint2 - v0),
	//							normal.dot(intersectionPoint2 - v1),
	//							normal.dot(intersectionPoint2 - v2)};
	//
	//////////////////////////////////

	return true;
}
bool CollisionManager::testRayAABB(Vec3f p, Vec3f d, BoundingBox<float> AABB, Vec3f& intersectionPoint)
{// see:http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

	debugAssert(d.magnitudeSquared() < 1.001 && d.magnitudeSquared() > 0.999);

	Vec3f dirfrac = Vec3f(1.0f / d.x, 1.0f / d.y, 1.0f / d.z); 

	float t1 = (AABB.minXYZ.x - p.x)*dirfrac.x;
	float t2 = (AABB.maxXYZ.x - p.x)*dirfrac.x;
	float t3 = (AABB.minXYZ.y - p.y)*dirfrac.y;
	float t4 = (AABB.maxXYZ.y - p.y)*dirfrac.y;
	float t5 = (AABB.minXYZ.z - p.z)*dirfrac.z;
	float t6 = (AABB.maxXYZ.z - p.z)*dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0 || tmin > tmax)
	{
		return false;
	}

	intersectionPoint = p  + d * tmin;
	return true;
}
bool CollisionManager::testTriangleTriangle(Vec3f t1_a, Vec3f t1_b, Vec3f t1_c, Vec3f t2_a, Vec3f t2_b, Vec3f t2_c) const
{	//	based on "A Fast Triangle-Triangle Intersection Test" by Tomas Moller
	//	http://knight.temple.edu/~lakamper/courses/cis350_2004/etc/moeller_triangle.pdf

/* NOT YET COMPLETE!!!! */
	debugBreak();
	return false;

/*  commented out so that it is clear this never detects a collision*/
	
/*	Vec3f N1 = (t1_b - t1_a).cross(t1_c - t1_a);
	Vec3f N2 = (t2_b - t2_a).cross(t2_c - t2_a);
	float d1 = -N1.dot(t1_a);
	float d2 = -N2.dot(t2_a);

	float dv1a = N2.dot(t1_a) + d2;
	float dv1b = N2.dot(t1_b) + d2;
	float dv1c = N2.dot(t1_c) + d2;

	if((dv1a > 0.0f && dv1b > 0.0f && dv1c > 0.0f) || (dv1a < 0.0f && dv1b < 0.0f && dv1c < 0.0f)) //if all three points of triangle 1 are on the same side of the plane of triangle 2
	{
		return false;
	}
	else if(dv1a == 0.0f && dv1b == 0.0f && dv1c == 0.0f)
	{
		//triangles are co-planar
		debugBreak();
		return false;
	}

	float dv2a = N1.dot(t2_a) + d1;
	float dv2b = N1.dot(t2_b) + d1;
	float dv2c = N1.dot(t2_c) + d1;

	if((dv2a > 0.0f && dv2b > 0.0f && dv2c > 0.0f) || (dv2a < 0.0f && dv2b < 0.0f && dv2c < 0.0f)) //if all three points of triangle 2 are on the same side of the plane of triangle 1
	{
		return false;
	}

	float N1dotN1 = N1.magnitudeSquared();
	float N2dotN2 = N2.magnitudeSquared();
	float N1dotN2 = N1.dot(N2);

	Vec3f D = N1.cross(N2);
	Vec3f O = (N1*(d1*N1dotN1-d2*N1dotN2) + N2*(d2*N2dotN2-d1*N1dotN2)) / (N1dotN1*N2dotN2-N1dotN2*N1dotN2);

	//TODO: finish writing code...
	float p1a = D.dot(t1_a - O);
	float p1b = D.dot(t1_b - O);
	float p1c = D.dot(t1_c - O);
	float p2a = D.dot(t2_a - O);
	float p2b = D.dot(t2_b - O);
	float p2c = D.dot(t2_c - O);

	//float t1 = p1a + (p1b - p1a) * d1a / (d1a - d1b); -- are these right? (Do we need to choose specific verticies...)
	//float t2 = p2a + (p2b - p2a) * d2a / (d2a - d2b); 
	
	return false;*/
}
//bool PhysicsManager::testLineTriangle(Vec3f lStart, Vec3f lEnd, Vec3f t1, Vec3f t2, Vec3f t3, Vec3f& intersectionPoint) const
//{									// not "double sided"?
//	Vec3f line = lEnd - lStart;
//	Vec3f pa = t1 - lStart;
//	Vec3f pb = t2 - lStart;
//	Vec3f pc = t3 - lStart;
//
//	Vec3f m = line.cross(pc);
//	float u = pb.dot(m);				//scalar triple product [line,pc,pb]
//	if(u < 0.0f) return false;
//	float v = -(pa.dot(m));				//scalar triple product [line,pa,pc]
//	if(v < 0.0f) return false;
//	float w = line.cross(pb).dot(pa);	//scalar triple product [line,pb,pa]
//	if(w < 0.0f) return false;
//
//	float invDenom = u + v + w;
//	if(invDenom == 0.0f)
//	{
//		intersectionPoint = lStart;
//		return true;
//	}
//
//	float denom = 1.0f / invDenom;
//	u *= denom;
//	v *= denom;
//	w *= denom;
//	intersectionPoint = t1 * u + t2 * v + t3 * w;
//	return true;
//}
Vec3f CollisionManager::closestPointOnSegment(Vec3f s1, Vec3f s2, Vec3f point) const //see http://pisa.ucsd.edu/cse125/2003/cse190g2/Collision.pdf
{
	// Determine t (the length of the vector from ‘a’ to ‘p’)
	Vec3f v = (s2 - s1).normalize();
	float d = s1.distanceSquared(s2);
	float t = v.dot(point - s1);

	if(t < 0) return s1;
	if(t*t > d) return s2;
	return s1 + v * t;
}
bool CollisionManager::sphereTriangleCollision(const triangle& t, const Mat4f& mat, const Sphere<float>& s) const 
{//see: http://realtimecollisiondetection.net/blog/?p=103
	Vec3f a = mat * t.vertices[0] - s.center;
	Vec3f b = mat * t.vertices[1] - s.center;
	Vec3f c = mat * t.vertices[2] - s.center;
	float rr = s.radius * s.radius;
	Vec3f AB = b - a; 
	Vec3f BC = c - b;
	Vec3f CA = a - c;
	
	Vec3f v = AB.cross(CA);
	float d = a.dot(v);
	float e = v.magnitudeSquared();
	if(d * d > rr * e)
		return false;
	
	float aa = a.dot(a);
	float ab = a.dot(b);
	float ac = a.dot(c);
	float bb = b.dot(b);
	float bc = b.dot(c);
	float cc = c.dot(c);
	if((aa > rr && ab > aa && ac > aa) || (bb > rr && ab > bb && bc > bb) || (cc > rr && ac > cc && bc > cc))
		return false;
	
	float d1 = a.dot(AB);
	float e1 = AB.magnitudeSquared();
	float d2 = b.dot(BC);
	float e2 = BC.magnitudeSquared();
	float d3 = c.dot(CA);
	float e3 = CA.magnitudeSquared();
	Vec3f Q1 = a * e1 - AB * d1;
	Vec3f QC = c * e1 - Q1;
	Vec3f Q2 = b * e2 - BC * d2;
	Vec3f QA = a * e2 - Q2;
	Vec3f Q3 = c * e3 - CA * d3;
	Vec3f QB = b * e3 - Q3;
	return !((Q1.dot(Q1) > rr * e1*e1 && Q1.dot(QC) > 0) || (Q2.dot(Q2) > rr * e2*e2 && Q2.dot(QA) > 0) || (Q3.dot(Q3) > rr * e3*e3 && Q3.dot(QB) > 0));
}
//bool PhysicsManager::triangleSweptSphere(const triangle& a, const Mat4f& m, const Sphere<float>& s, Vec3f sphereInitial, Vec3f sphereFinal, float radius)const
//{
//
//}
bool CollisionManager::segmentPlaneCollision(const Vec3f& a, const Vec3f& b, const Plane3f& p, Vec3f& collisionPoint) const
{
	//see: http://paulbourke.net/geometry/planeline/

	float numerator = (p.normal).dot(a) + p.d;
	float denominator = (p.normal).dot(a-b);

	if(abs(denominator) > 0.001) //make sure line and plane are not parallel
	{
		float u = numerator/denominator;
		collisionPoint = lerp(a, b, u);
		return u >= 0.0 && u <= 1.0;
	}
	else //if they are parallel
	{
		collisionPoint = lerp(a, b, 0.5);
		return abs(numerator) < 0.001; //return whether the line lies entirely in the plane
	}
}
//bool CollisionChecker::pointBetweenVertices(const Vec3f& a,const Vec3f& b, const triangle& tri1) const
//{
//	float t,i,final_t;
//
//	t=0.0f; i=0.0f;
//	i+=(tri1.pA*b.x)+(tri1.pB*b.y)+(tri1.pC*b.z)+(tri1.pD);
//	t+=(tri1.pA*(b.x*-1))+(tri1.pB*(b.y*-1))+(tri1.pC*(b.z*-1));
//	t+=(tri1.pA*a.x)+(tri1.pB*a.y)+(tri1.pC*a.z);
//
//	// Be wary of possible divide-by-zeros here (i.e. if t==0)
//	final_t = (-i)/t;
//
//	return  (final_t >= 0) && (final_t <= 1);
//}
bool CollisionManager::pointInTriangle(const triangle& tri,const Vec3f& vert) const
{
	float ang = acos((tri.vertices[0]-vert).normalize().dot((tri.vertices[0]-vert).normalize())) +
				acos((tri.vertices[1]-vert).normalize().dot((tri.vertices[1]-vert).normalize())) +
				acos((tri.vertices[2]-vert).normalize().dot((tri.vertices[2]-vert).normalize()));
	return ang > 2.0 * PI - 0.01;
}
//bool CollisionChecker::triangleCollision(const triangle& tri1, const triangle& tri2) const
//{
//	Vec3f p;
//	bool temp = false;
//
//	// Scroll thru 3 line segments of the other triangle
//
//	// First iteration  (a,b)
//	p=linePlaneCollision(tri2.a, tri2.b, tri1);
//	// Determine which axis to project to
//	if		((abs(tri1.pA) >= abs(tri1.pB)) && (abs(tri1.pA) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, false, true, true);	// X is greatest
//	else if ((abs(tri1.pB) >= abs(tri1.pA)) && (abs(tri1.pB) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, true,false,true);	// Y is greatest
//	else if ((abs(tri1.pC) >= abs(tri1.pA)) && (abs(tri1.pC) >= abs(tri1.pB)))	temp = pointInTriangle(tri1, p, true, true, false);	// Z is greatest
//	if (temp) // Point needs to be checked to see if it lies between the two vertices, First check for the special case of vertical line segments,
//		return (tri2.a.x == tri2.b.x && tri2.a.z==tri2.b.z && ( (tri2.a.y<=p.y && p.y<=tri2.b.y) || (tri2.b.y<=p.y && p.y <= tri2.a.y) )) || pointBetweenVertices(tri2.a, tri2.b, tri1);
//
//
//
//	// Second iteration (b,c)
//	p=linePlaneCollision(tri2.b, tri2.c, tri1);
//	if		((abs(tri1.pA) >= abs(tri1.pB)) && (abs(tri1.pA) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, false, true, true);	// Determine which axis to project to
//	else if ((abs(tri1.pB) >= abs(tri1.pA)) && (abs(tri1.pB) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, true, false, true);
//	else if ((abs(tri1.pC) >= abs(tri1.pA)) && (abs(tri1.pC) >= abs(tri1.pB)))	temp = pointInTriangle(tri1, p, true, true, false);
//	if (temp) return ((tri2.b.x == tri2.c.x) && (tri2.b.z == tri2.c.z) && (((tri2.b.y <= p.y) && (p.y <= tri2.c.y)) || ((tri2.c.y <= p.y) && (p.y <= tri2.b.y)))) || pointBetweenVertices(tri2.b, tri2.c, tri1);
//
//
//
//	// Third iteration  (c,a)
//	p=linePlaneCollision( tri2.c, tri2.a, tri1);
//	if		((abs(tri1.pA) >= abs(tri1.pB)) && (abs(tri1.pA) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, false, true, true);	// Determine which axis to project to
//	else if ((abs(tri1.pB) >= abs(tri1.pA)) && (abs(tri1.pB) >= abs(tri1.pC)))	temp = pointInTriangle(tri1, p, true, false, true);
//	else if ((abs(tri1.pC) >= abs(tri1.pA)) && (abs(tri1.pC) >= abs(tri1.pB)))	temp = pointInTriangle(tri1, p, true, true, false);
//	if (temp) return ((tri2.c.x == tri2.a.x) && (tri2.c.z == tri2.a.z) && (((tri2.c.y <= p.y) && (p.y <= tri2.a.y)) || ((tri2.a.y <= p.y) && (p.y <= tri2.c.y)))) || pointBetweenVertices(tri2.c, tri2.a, tri1);
//
//
//	return false; // Default value/no collision
//}
bool CollisionManager::triangleCollision(const triangle& tri1, const triangle& tri2) const
{
	Vec3f p;

	Plane3f plane1(tri1.vertices[0], tri1.vertices[1], tri1.vertices[2]);
	if(	(segmentPlaneCollision(tri2.vertices[0], tri2.vertices[1], plane1, p) && pointInTriangle(tri1,p)) ||
		(segmentPlaneCollision(tri2.vertices[1], tri2.vertices[2], plane1, p) && pointInTriangle(tri1,p)) ||
		(segmentPlaneCollision(tri2.vertices[2], tri2.vertices[0], plane1, p) && pointInTriangle(tri1,p)))
			return true;

	Plane3f plane2(tri2.vertices[0], tri2.vertices[1], tri2.vertices[2]);
	if(	(segmentPlaneCollision(tri1.vertices[0], tri1.vertices[1], plane2, p) && pointInTriangle(tri2,p)) ||
		(segmentPlaneCollision(tri1.vertices[1], tri1.vertices[2], plane2, p) && pointInTriangle(tri2,p)) ||
		(segmentPlaneCollision(tri1.vertices[2], tri1.vertices[0], plane2, p) && pointInTriangle(tri2,p)))
			return true;

	return false;
}
bool CollisionManager::triangleCollision(const triangle& tri1, const triangle& tri2, Mat4f rot1, Mat4f rot2) const
{
	triangle T1 = tri1;
	triangle T2 = tri2;

	T1.vertices[0] = rot1 * T1.vertices[0];
	T1.vertices[1] = rot1 * T1.vertices[1];
	T1.vertices[2] = rot1 * T1.vertices[2];

	
	T2.vertices[0] = rot2 * T2.vertices[0];
	T2.vertices[1] = rot2 * T2.vertices[1];
	T2.vertices[2] = rot2 * T2.vertices[2];

	return triangleCollision(T1, T2);
}
//Vec3f CollisionChecker::triangleList::getRandomVertex()
//{
//	int r = random<int>(numTriangles*3);
//	if(r % 3 == 0)		return triangles[r / 3].vertices[0];
//	else if(r % 3 == 1)	return triangles[r / 3].vertices[1];
//	else				return triangles[r / 3].vertices[2];
//}
//CollisionChecker::triangleList::triangleList(Vec3f* vertices, unsigned int* faces, unsigned int nVertices, unsigned int nFaces):triangles(NULL), numTriangles(0), center(0.0f,0.0f,0.0f), radius(-99999999.9f)
//{
//	if(nVertices == 0 || nFaces == 0)
//		return;
//
//	float minx,miny,minz,maxx,maxy,maxz;
//	Vec3f cross_v1xv2,p;
//	int i;
//
//	triangles = new triangle[nFaces];
//	numTriangles = nFaces;
//	for(i=0;i<nFaces;i++)
//	{
//		triangles[i].vertices[0]=vertices[faces[i*3+0]];
//		triangles[i].vertices[1]=vertices[faces[i*3+1]];
//		triangles[i].vertices[2]=vertices[faces[i*3+2]];
//		triangles[i].findRadius();
//
//		//Vec3f cross_v1xv2 = (triangles[i].b - triangles[i].a).cross(triangles[i].c - triangles[i].a);
//		//triangles[i].pA = cross_v1xv2.x;
//		//triangles[i].pB = cross_v1xv2.y;
//		//triangles[i].pC = cross_v1xv2.z;
//		//triangles[i].pD += (-(triangles[i].a.x))*(cross_v1xv2.x);
//		//triangles[i].pD += (-(triangles[i].a.y))*(cross_v1xv2.y);
//		//triangles[i].pD += (-(triangles[i].a.z))*(cross_v1xv2.z);
//
//
//	}
//	minx = maxx = vertices[0].x;
//	miny = maxy = vertices[0].y;
//	minz = maxz = vertices[0].z;
//	for(i=1;i<nVertices;i++)
//	{
//		if(vertices[i].x<minx) minx=vertices[i].x;
//		if(vertices[i].y<miny) miny=vertices[i].y;
//		if(vertices[i].z<minz) minz=vertices[i].z;
//		if(vertices[i].x>maxx) maxx=vertices[i].x;
//		if(vertices[i].y>maxy) maxy=vertices[i].y;
//		if(vertices[i].z>maxz) maxz=vertices[i].z;
//	}
//	center = Vec3f((minx+maxx)/2,((miny+maxy)/2),(minz+maxz)/2);
//	float radiusSquared = center.distanceSquared(vertices[0]);
//	for(i=1;i<nVertices;i++)
//	{
//		if (center.distanceSquared(vertices[i]) > radiusSquared) radiusSquared = center.distanceSquared(vertices[i]);
//	}
//	radius = sqrt(radiusSquared);
//}
void CollisionManager::triangle::findRadius() const
{
	if(radiusValid) return;
	float radiusSquared, minx,miny,minz,maxx,maxy,maxz;

	minx = maxx = vertices[0].x;
	miny = maxy = vertices[0].y;
	minz = maxz = vertices[0].z;

	if(vertices[1].x < minx)	minx = vertices[1].x;
	if(vertices[2].x < minx)	minx = vertices[2].x;
	if(vertices[1].y < miny)	miny = vertices[1].y;
	if(vertices[2].y < miny)	miny = vertices[2].y;
	if(vertices[1].z < minz)	minz = vertices[1].z;
	if(vertices[2].z < minz)	minz = vertices[2].z;

	if(vertices[1].x > maxx)	maxx = vertices[1].x;
	if(vertices[2].x > maxx)	maxx = vertices[2].x;
	if(vertices[1].y > maxy)	maxy = vertices[1].y;
	if(vertices[2].y > maxy)	maxy = vertices[2].y;
	if(vertices[1].z > maxz)	maxz = vertices[1].z;
	if(vertices[2].z > maxz)	maxz = vertices[2].z;

	center.x = ((minx+maxx)/2);
	center.y = ((miny+maxy)/2);
	center.z = ((minz+maxz)/2);

	radiusSquared = center.distanceSquared(vertices[0]);
	if (center.distanceSquared(vertices[1]) > radiusSquared)
		radiusSquared = center.distanceSquared(vertices[1]);
  	if (center.distanceSquared(vertices[2]) > radiusSquared)
		radiusSquared = center.distanceSquared(vertices[2]);

	radius = sqrt(radiusSquared);
	radiusValid=true;
}

//bool CollisionChecker::operator() (const triangleList& t1, const triangleList& t2) const
//{
//	if(t1.center.distanceSquared(t2.center) < (t1.radius+t2.radius)*(t1.radius+t2.radius))
//	{
//		int i1,i2;
//		for(i1=0; i1 < t1.numTriangles; i1++)
//		{
//			if(t1.triangles[i1].center.distanceSquared(t2.center) < (t1.triangles[i1].radius+t2.radius)*(t1.triangles[i1].radius+t2.radius))
//			{
//				for(i2 = 0; i2 < t2.numTriangles; i2++)
//				{
//					if(t1.triangles[i1].center.distanceSquared(t2.triangles[i2].center) < (t1.triangles[i1].radius+t2.triangles[i2].radius)*(t1.triangles[i1].radius+t2.triangles[i2].radius))
//					{
//						if(triangleCollision(t1.triangles[i1],t2.triangles[i2]))
//							return true;
//					}
//				}
//			}
//		}
//	}
//	return false;
//}
//bool CollisionChecker::operator() (const triangleList& t1, Vec3f center, float radius) const
//{
//	if(t1.center.distanceSquared(center) < (t1.radius+radius)*(t1.radius+radius))
//	{
//		int i1;
//		for(i1=0; i1 < t1.numTriangles; i1++)
//		{
//			if(t1.triangles[i1].center.distanceSquared(center) < (t1.triangles[i1].radius+radius)*(t1.triangles[i1].radius+radius))
//			{
//				if(t1.triangles[i1].center.distanceSquared(center) < (t1.triangles[i1].radius+radius)*(t1.triangles[i1].radius+radius))
//				{
//					//we should also check that the triangle itself (not just its bounding sphere) intersects the shpere
//						return true;
//				}
//			}
//		}
//	}
//	return false;
//}
//bool CollisionChecker::operator() (const triangleList& t1, Vec3f lineStart, Vec3f lineEnd) const
//{
//	double d1 = t1.center.distanceSquared(lineStart);
//	double d2 = t1.center.distanceSquared(lineEnd);
//
//	//if(d1 < smallestD) smallestD = d1;
//	//if(d2 < smallestD) smallestD = d2;
//	//Profiler.setOutput("d value",sqrt(smallestD));
//
// 	if(d1 < t1.radius*t1.radius || d2 < t1.radius*t1.radius)
//		return true;
//
//	float u = (t1.center - lineStart).dot(lineEnd - lineStart) / (lineEnd - lineStart).dot(lineEnd - lineStart);
//
//	if(u < 0.0 || u > 1.0) return false;
//	double d3 = t1.center.distanceSquared(lineStart+(lineEnd-lineStart)*u);
//	//if(d3 < smallestD) smallestD = d3;
//	return d3 < t1.radius*t1.radius;
//}
//
//bool CollisionChecker::operator() (objectType t1, objectType t2) const
//{
//	return operator()(*dataManager.getModel(t1)->trl,*dataManager.getModel(t2)->trl);
//}
//bool CollisionChecker::operator() (objectType t1, Vec3f center, float radius) const
//{
//	return operator()(*dataManager.getModel(t1)->trl, center, radius);
//}
bool CollisionManager::operator() (shared_ptr<object> o1, Vec3f lineStart, Vec3f lineEnd) const
{
	auto o = objectBounds.find(o1->type);
	if(o != objectBounds.end() && o->second->type == collisionBounds::SPHERE)
	{
		Sphere<float> sphere = o->second->sphere;
		sphere.center = o1->rotation * sphere.center + o1->position;

		//early detection
		double d1 = sphere.center.distanceSquared(lineStart);
		double d2 = sphere.center.distanceSquared(lineEnd);
 		if(d1 < sphere.radius*sphere.radius || d2 < sphere.radius*sphere.radius)
			return true;

		//fallback
		float u = (sphere.center - lineStart).dot(lineEnd - lineStart) / (lineEnd - lineStart).dot(lineEnd - lineStart);
		if(u < 0.0 || u > 1.0) return false;
		return sphere.center.distanceSquared(lineStart+(lineEnd-lineStart)*u) < sphere.radius*sphere.radius;
	}
	else if(o != objectBounds.end() && o->second->type == collisionBounds::MESH) //right now just uses boundin sphere
	{
		Sphere<float> sphere = o->second->sphere;
		sphere.center = o1->rotation * sphere.center + o1->position;

		//early detection
		double d1 = sphere.center.distanceSquared(lineStart);
		double d2 = sphere.center.distanceSquared(lineEnd);
 		if(d1 < sphere.radius*sphere.radius || d2 < sphere.radius*sphere.radius)
			return true;

		//fallback
		float u = (sphere.center - lineStart).dot(lineEnd - lineStart) / (lineEnd - lineStart).dot(lineEnd - lineStart);
		if(u < 0.0 || u > 1.0) return false;
		return sphere.center.distanceSquared(lineStart+(lineEnd-lineStart)*u) < sphere.radius*sphere.radius;
	}
	return false;
}

bool CollisionManager::operator() (shared_ptr<object> o1, shared_ptr<object> o2) const
{
	auto b1 = objectBounds.find(o1->type);
	auto b2 = objectBounds.find(o2->type);

	if(b1 == objectBounds.end() || b2 == objectBounds.end())
	{
		return false;
	}
	else if(b1->second->type == collisionBounds::SPHERE && b2->second->type == collisionBounds::SPHERE)
	{
		Sphere<float> s1 = b1->second->sphere;
		Sphere<float> s2 = b2->second->sphere;
		s1.center = o1->lastRotation * s1.center + o1->lastPosition;
		s2.center = o2->lastRotation * s2.center + o2->lastPosition;

		Vec3f s1_newCenter = o1->rotation * s1.center + o1->position;
		Vec3f s2_newCenter = o2->rotation * s2.center + o2->position;

		return sweptSphereSphere(s1, s1_newCenter - s1.center, s2, s2_newCenter - s2.center);
		//return (s1.center).distanceSquared(s2.center) < (s1.radius+s2.radius)*(s1.radius+s2.radius);
	}
	else if((b1->second->type == collisionBounds::MESH && b2->second->type == collisionBounds::SPHERE) || (b1->second->type == collisionBounds::SPHERE && b2->second->type == collisionBounds::MESH))
	{
		if(b1->second->type == collisionBounds::SPHERE && b2->second->type == collisionBounds::MESH) //make sure b1's type == MESH and b2's type == SPHERE
			swap(b1,b2);

		Sphere<float> s1 = b1->second->sphere;
		Sphere<float> s2 = b2->second->sphere;
		s1.center = o1->rotation * s1.center + o1->position;
		s2.center = o2->rotation * s2.center + o2->position;
		if((s1.center).distanceSquared(s2.center) > (s1.radius+s2.radius)*(s1.radius+s2.radius))
			return false;

		Mat4f mat1(o1->rotation, o1->position);
		auto m = static_pointer_cast<collisionMesh>(b1->second);
		for(auto i = m->triangles.begin(); i != m->triangles.end(); i++)
		{
			if(sphereTriangleCollision(*i, mat1, s2))
				return true;
		}
	}
	else if(b1->second->type == collisionBounds::MESH && b2->second->type == collisionBounds::MESH)
	{
		debugBreak(); // code not yet written!!!
	}
	return false;
	//auto tr1 = dataManager.getModel(o1->type)->trl;
	//auto tr2 = dataManager.getModel(o2->type)->trl;
	//
	//Mat4f mat1(o1->rotation, o1->position);
	//Mat4f mat2(o2->rotation, o2->position);
	//
	////Vec3f _center1 = o1->position + o1->rotation * tr1->center;
	////Vec3f _center2 = o2->position + o2->rotation * tr2->center;
	//
	//Vec3f center1 = mat1 * tr1->center;
	//Vec3f center2 = mat1 * tr2->center;
	//
	//if(center1.distanceSquared(center2) < (tr1->radius+tr2->radius)*(tr1->radius+tr2->radius))
	//{
	//	int i1;
	//	//int i2;
	//	for(i1=0; i1 < tr1->numTriangles; i1++)
	//	{
	//		if((mat1*tr1->triangles[i1].center).distanceSquared(center2) < (tr1->triangles[i1].radius+tr2->radius)*(tr1->triangles[i1].radius+tr2->radius))
	//		{
	//		//	for(i2 = 0; i2 < tr2->numTriangles; i2++)
	//		//	{
	//		//		if((mat1*tr1->triangles[i1].center).distanceSquared(mat2*tr2->triangles[i2].center) <= (tr1->triangles[i1].radius+tr2->triangles[i2].radius)*(tr1->triangles[i1].radius+tr2->triangles[i2].radius))
	//		//		{
	//		//			if(triangleCollision(tr1->triangles[i1],tr2->triangles[i2], mat1, mat2))
	//						return true;
	//		//		}
	//		//	}
	//		}
	//	}
	//}
	//return false;
}
bool CollisionManager::groundCollsion(shared_ptr<object> o1) const
{
	auto b1 = objectBounds.find(o1->type);
	if(b1 == objectBounds.end())
	{
		return false;
	}
	else if(b1->second->type == collisionBounds::SPHERE)
	{
		Sphere<float> s1 = b1->second->sphere;
		s1.center = o1->rotation * s1.center + o1->position;

		return world.altitude(s1.center) <= s1.radius;
	}
	else if(b1->second->type == collisionBounds::MESH)
	{
		Mat4f mat1(o1->rotation, o1->position);
		shared_ptr<collisionMesh> m = static_pointer_cast<collisionMesh>(b1->second);
		for(auto i = m->vertices.begin(); i != m->vertices.end(); i++)
		{
			if(world.altitude(mat1 * (*i)) < 0.0)
				return true;
		}
		return false;
	}
	return false;
}
void CollisionManager::setCollsionBounds(objectType type, Sphere<float> s)
{
	if(objectBounds.find(type) == objectBounds.end()) 
	{
		objectBounds[type] = shared_ptr<collisionBounds>(new collisionBounds(s));
	}
}
void CollisionManager::setCollsionBounds(objectType type, Sphere<float> s, const vector<Vec3f>& vertices, const vector<unsigned int>& indices)
{
	collisionMesh* m = new collisionMesh;
	m->sphere = s;
	m->vertices = vertices;
	for(int i=0; i<indices.size(); i += 3)
	{
		m->triangles.push_back(triangle(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]));
	}
	objectBounds[type] = shared_ptr<collisionBounds>(m);
}