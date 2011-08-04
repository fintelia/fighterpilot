
#include "cmath"
#include "angle.h"

//const double PI = 3.14159265358979323846264338327950288419;
Angle atan2A(double opp,double adj)
{
	return Angle(atan2(opp,adj));
}
Angle asinA(double X)
{
	return Angle(asin(X));
}
Angle acosA(double X)
{
	return Angle(acos(X));
}
Angle lerp(Angle a1,Angle a2,double l)
{
	if(abs(a2.getAngle()-a1.getAngle())<=PI) 
		return a1*l+a2*(1.0-l);
	else if(a1<a2)
		return a1*l+a2*(1.0-l)+2.0*PI*l;
	return a1*l+a2*(1.0-l)+2.0*PI*(1.0-l);
}