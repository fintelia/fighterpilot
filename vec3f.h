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

#pragma once

class Vec3f {
	public:
		float x,y,z;
		Vec3f();
		Vec3f(float x, float y, float z);

		float &operator[](int index);
		float operator[](int index) const;
		
		Vec3f operator*(float scale) const;
		Vec3f operator/(float scale) const;
		Vec3f operator+(const Vec3f &other) const;
		Vec3f operator-(const Vec3f &other) const;
		Vec3f operator-() const;

		const Vec3f &operator*=(float scale);
		const Vec3f &operator/=(float scale);
		const Vec3f &operator+=(const Vec3f &other);
		const Vec3f &operator-=(const Vec3f &other);
		
		bool operator!=(const Vec3f &other);
		bool operator==(const Vec3f &other);

		float magnitude() const;
		float magnitudeSquared() const;
		float signedMagnitude() const;
		Vec3f normalize() const;
		Vec3f normalizeSigned() const;
		float dot(const Vec3f &other) const;
		Vec3f cross(const Vec3f &other) const;
		void set(float x, float y, float z);
		bool equal(Vec3f v,float maxDifference=0.01);
		float distance(Vec3f v);
		float distanceSquared(Vec3f v);
};

Vec3f operator*(float scale, const Vec3f &v);
//std::ostream &operator<<(std::ostream &output, const Vec3f &v);
void upAndRight(Vec3f fwd,Angle roll,Vec3f& up,Vec3f& right);
float dist(Vec3f P1,Vec3f P2);
float dist_Point_to_Segment( Vec3f P, Vec3f S1,Vec3f S2);
double intersect(Vec3f rOrigin, Vec3f rNormal, Vec3f pOrigin, Vec3f pNormal);
Vec3f collide(Vec3f rOrigin, Vec3f rNormal, Vec3f pOrigin, Vec3f pNormal);
float dist_squared(Vec3f P1,Vec3f P2);
bool SegmentSphereIntersect(Vec3f A, Vec3f B, Vec3f P, float r);
Vec3f randVector();
Vec3f randNormal();
//template<class T>
//int sign(T f) <Vec3f>
//{
//	return (f.x+f.y+f.z)/abs(f.x+f.y+f.z);
//}
class Vec2f 
{
public:
	float x,y;
	Vec2f(){}
	Vec2f(float _x, float _y)
	{
		x=_x;
		y=_y;
	}
	void set(float _x, float _y)
	{
		x=_x;
		y=_y;
	}
	bool operator!=(const Vec2f &other) const {
		return (x!=other.x || y!=other.y);
	}
};


