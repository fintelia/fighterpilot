

/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <cmath>
#include <cstdlib>
#include "Angle.h"
#include "vec3f.h"

using namespace std;

Vec3f::Vec3f() {
}

Vec3f::Vec3f(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

bool Vec3f::equal(Vec3f v,float maxDifference)
{
	if(abs(x-v.x)< maxDifference && abs(y-v.y)< maxDifference && abs(z-v.z)< maxDifference)
		return true;
	return false;
}

float &Vec3f::operator[](int index) {
	if(index==0)
		return x;
	if(index==1)
		return y;
	if(index==2)
		return z;
	return z;//ERROR!!!
}

float Vec3f::operator[](int index) const {
	if(index==0)
		return x;
	if(index==1)
		return y;
	if(index==2)
		return z;
	return z;//ERROR!!!
}

Vec3f Vec3f::operator*(float scale) const {
	return Vec3f(x * scale, y * scale, z * scale);
}

Vec3f Vec3f::operator/(float scale) const {
	return Vec3f(x / scale, y / scale, z / scale);
}

Vec3f Vec3f::operator+(const Vec3f &other) const {
	return Vec3f(x + other.x, y + other.y, z + other.z);
}

Vec3f Vec3f::operator-(const Vec3f &other) const {
	return Vec3f(x - other.x, y - other.y, z - other.z);
}

Vec3f Vec3f::operator-() const {
	return Vec3f(-x, -y, -z);
}

const Vec3f &Vec3f::operator*=(float scale) {
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

const Vec3f &Vec3f::operator/=(float scale) {
	x /= scale;
	y /= scale;
	z /= scale;
	return *this;
}

const Vec3f &Vec3f::operator+=(const Vec3f &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

const Vec3f &Vec3f::operator-=(const Vec3f &other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

bool Vec3f::operator!=(const Vec3f &other) {
	return !equal(other);
}

bool Vec3f::operator==(const Vec3f &other) {
	return equal(other);
}

float Vec3f::magnitude() const {
	return sqrt(x * x + y * y + z * z);
}

float Vec3f::magnitudeSquared() const {
	return x * x + y * y + z * z;
}
float Vec3f::signedMagnitude() const {
	return sqrt(x*x*x/abs(x) + y*y*y/abs(y) + z*z*z/abs(z));
}
Vec3f Vec3f::normalize() const {
	float m = sqrt(x * x + y * y + z * z);
	return Vec3f(x / m, y / m, z / m);
}
Vec3f Vec3f::normalizeSigned() const {
	float m = sqrt(x * x + y * y + z * z);
	if(x+y+z<0) m*=-1;
	return Vec3f(x / m, y / m, z / m);
}
float Vec3f::dot(const Vec3f &other) const {
	return x * other.x + y * other.y + z * other.z;
}

Vec3f Vec3f::cross(const Vec3f &other) const {
	return Vec3f(y * other.z - z * other.y,
				 z * other.x - x * other.z,
				 x * other.y - y * other.x);
}

Vec3f operator*(float scale, const Vec3f &v) {
	return v * scale;
}

//ostream &operator<<(ostream &output, const Vec3f &v) {
//	cout << '(' << v.x << ", " << v.y << ", " << v.z << ')';
//	return output;
//}
void Vec3f::set(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}
float dist(Vec3f P1,Vec3f P2)
{
	return sqrt((P1[0]-P2[0])*(P1[0]-P2[0])+(P1[1]-P2[1])*(P1[1]-P2[1])+(P1[2]-P2[2])*(P1[2]-P2[2]));
}
float dist_squared(Vec3f P1,Vec3f P2)
{
	return (P1[0]-P2[0])*(P1[0]-P2[0])+(P1[1]-P2[1])*(P1[1]-P2[1])+(P1[2]-P2[2])*(P1[2]-P2[2]);
}
float dist_Point_to_Segment( Vec3f P, Vec3f S1,Vec3f S2)
{
    Vec3f v = S2 - S1;
    Vec3f w = P - S1;

    double c1 = w.dot(v);
    if ( c1 <= 0 )
        return dist(P, S1);

    double c2 = v.dot(v);
    if ( c2 <= c1 )
        return dist(P, S2);

    double b = c1 / c2;
    Vec3f Pb = S1 + (float)b * v;
    return dist(P, Pb);
}
double intersect(Vec3f rOrigin, Vec3f rNormal, Vec3f pOrigin, Vec3f pNormal)
{
    // Calc D for the plane (this is usually pre-calculated
    // and stored with the plane)

    double D = pOrigin.dot(pNormal);

    // Determine the distance from rOrigin to the point of
    // intersection on the plane

	double numer = pNormal.dot(rOrigin) + D;
	double denom = pNormal.dot(rNormal);
	return -(numer / denom);
	//double numer = pNormal.dot(pOrigin-rOrigin) + D;
	//double denom = pNormal.dot(-rNormal);
	//return (numer / denom);
}

Vec3f collide(Vec3f rOrigin, Vec3f rNormal, Vec3f pOrigin, Vec3f pNormal)
{
    // Get the distance to the collision point

    double t = intersect(rOrigin, rNormal, pOrigin, pNormal);

    // Calculate the collision point

    Vec3f collisionPoint;
    collisionPoint[0] = rOrigin[0] + rNormal[0] * (float)t;
    collisionPoint[1] = rOrigin[1] + rNormal[1] * (float)t;
    collisionPoint[2] = rOrigin[2] + rNormal[2] * (float) t;

    // Here it is...

    return collisionPoint;
}

bool PointInSphere(Vec3f P, float r, Vec3f Q)
{
    Vec3f PQ = Q - P;
    float pq2 = PQ.magnitude();
    float r2  = r * r;

    if(pq2 > r2) return false; else return true;
}

bool SegmentSphereIntersect(Vec3f A, Vec3f B, Vec3f P, float r)
{
    Vec3f AB        = B - A;
    float ab2       = AB.magnitude();
    Vec3f AP        = P - A;
    float ap_dot_ab = AP.dot(AB);
    float t         = ap_dot_ab / ab2; // the projection parameter on the line


    // clamp parameter to segment extremities

    if (t < 0.0f) t = 0.0f; else if (t > 1.0f) t = 1.0f;

    // calculate the closest point

    Vec3f Q = A + AB * t;

    return PointInSphere(P,r,Q);;
}
Vec3f randVector()
{
	return Vec3f((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX).normalize()*((float)rand()/RAND_MAX);
}

Vec3f randNormal()
{
	return Vec3f((float)rand()/RAND_MAX+0.001,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX).normalize();
}
float Vec3f::distance(Vec3f v)
{
	return sqrt((v.x-x)*(v.y-y)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z));
}
float Vec3f::distanceSquared(Vec3f v)
{
	return (v.x-x)*(v.y-y)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z);
}
void upAndRight(Vec3f fwd,Angle roll,Vec3f& up,Vec3f& right)
{
	if(fwd!=Vec3f(0,1,0))
	{
		Vec3f axis1=-((fwd.cross(Vec3f(0,1,0))).cross(fwd)).normalize();
		Vec3f axis2=fwd.cross(axis1).normalize();
		Vec3f s1=axis1*sin(-roll);
		right=s1+axis2*cos(-roll);
	}//+(float)PI/2
	else
		right=Vec3f(sin(roll+PI/2.0),0,cos(roll+PI/2.0));

	up=(fwd.cross(right)).normalize();
}