

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

#include "vec3f.h"

using namespace std;

Vec3f::Vec3f() {
	
}

Vec3f::Vec3f(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

float &Vec3f::operator[](int index) {
	return v[index];
}

float Vec3f::operator[](int index) const {
	return v[index];
}

Vec3f Vec3f::operator*(float scale) const {
	return Vec3f(v[0] * scale, v[1] * scale, v[2] * scale);
}

Vec3f Vec3f::operator/(float scale) const {
	return Vec3f(v[0] / scale, v[1] / scale, v[2] / scale);
}

Vec3f Vec3f::operator+(const Vec3f &other) const {
	return Vec3f(v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}

Vec3f Vec3f::operator-(const Vec3f &other) const {
	return Vec3f(v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}

Vec3f Vec3f::operator-() const {
	return Vec3f(-v[0], -v[1], -v[2]);
}

const Vec3f &Vec3f::operator*=(float scale) {
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
	return *this;
}

const Vec3f &Vec3f::operator/=(float scale) {
	v[0] /= scale;
	v[1] /= scale;
	v[2] /= scale;
	return *this;
}

const Vec3f &Vec3f::operator+=(const Vec3f &other) {
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];
	return *this;
}

const Vec3f &Vec3f::operator-=(const Vec3f &other) {
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];
	return *this;
}

float Vec3f::magnitude() const {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float Vec3f::magnitudeSquared() const {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

Vec3f Vec3f::normalize() const {
	float m = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return Vec3f(v[0] / m, v[1] / m, v[2] / m);
}

float Vec3f::dot(const Vec3f &other) const {
	return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
}

Vec3f Vec3f::cross(const Vec3f &other) const {
	return Vec3f(v[1] * other.v[2] - v[2] * other.v[1],
				 v[2] * other.v[0] - v[0] * other.v[2],
				 v[0] * other.v[1] - v[1] * other.v[0]);
}

Vec3f operator*(float scale, const Vec3f &v) {
	return v * scale;
}

ostream &operator<<(ostream &output, const Vec3f &v) {
	cout << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
	return output;
}
void Vec3f::set(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
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

    //double numer = pNormal.dot(rOrigin) + D;
    //double denom = pNormal.dot(rNormal);
    //return -(numer / denom);
	double numer = pNormal.dot(pOrigin-rOrigin) + D;
	double denom = pNormal.dot(-rNormal);
	return (numer / denom);
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

