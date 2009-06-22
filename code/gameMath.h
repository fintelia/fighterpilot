
#include "vec3f.h"
#include "geoPlane.h"//-------------------------//
#include "AABox.h"/////---for frustum culling---//
#include "FrustumG.h"//-------------------------//

class Angle
{
private:
	float ang;
public:
	Angle(float a)
	{
		ang=a;
		normalize();
	}
	void normalize()
	{
		while(ang>360)
			ang-=360;
		while(ang<0)
			ang+=360;
	}
	float getAngle()
	{
		return ang;
	}
	void setAngle(float a)
	{
		ang = a;
	}
	operator float() { return ang; }
	Angle operator*(Angle a){return Angle(ang*a);}
	Angle operator/(Angle a){return Angle(ang/a);}
	Angle operator+(Angle a){return Angle(ang+a);}
	Angle operator-(Angle a){return Angle(ang-a);}
};

Angle atan2A(float X,float Y)
{
	return Angle(atan2(X,Y)*180/PI);
}
Angle asinA(float X)
{
	return Angle(asin(X)*180/PI);
}
Angle acosA(float X)
{
	return Angle(acos(X)*180/PI);
}