
#include "engine.h"


Ephemeris::Ephemeris(): date(0.0), latitude(0.0), longitude(0.0)
{

}

Ephemeris::~Ephemeris()
{

}

void Ephemeris::setPosition(Angle Latitude, Angle Longitude)
{
	latitude = Latitude;
	longitude = Longitude;
}

void Ephemeris::setTime(double hours, int month, int day, int year)
{
	date = 367*year - 7 * ( year + (month+9)/12 ) / 4 + 275*month/9 + day - 730530; //use integer math to find the number of days
	date += hours/24.0; //now add fractal number of days using floating point math
}


Vec3f Ephemeris::getSunDirection() //see: http://stjarnhimlen.se/comp/ppcomp.html
{
	float d = date - (longitude.degrees()/15.0)/24; //adjust for local time
	//float N = 0.0;
	//float i = 0.0;
	//float w = (282.9404 + 4.70935e-5 * d) * PI/180;
	//float a = 1.000000; //in AU
	//float e = 0.016709 - 1.151e-9 * d;
	//float ecl = (23.4393 - 3.563E-7 * d) * PI/180;
	//float M = (356.0470 + 0.9856002585 * d) * PI/180;
	
	//float E = M + e * sin(M) * (1.0 + e * cos(M));
	//float xv = cos(E) - e;
	//float yv = sqrt(1.0 - e*e) * sin(E);
	
	//float v = atan2( yv, xv );
	//float r = sqrt( xv*xv + yv*yv );
	
	//float lonsun = v + w;
	//float xs = r * cos(lonsun);
	//float ys = r * sin(lonsun);
	
	//float xe = xs;
    //float ye = ys * cos(ecl);
    //float ze = ys * sin(ecl);
	
	//Angle RA  = atan2A( ye, xe );
	//Angle Dec = atan2A( ze, sqrt(xe*xe+ye*ye) ) /*+ (d-floor(d))*2.0*PI*/;

	//Profiler.setOutput("RA", RA.degrees());
	//Profiler.setOutput("Dec", Dec.degrees());

	float phi = 0;//Dec - latitude;
	float theta = (date-floor(date))*2.0*PI + PI;//RA - longitude;
	return Vec3f(cos(phi)*sin(theta), cos(theta), sin(phi)*sin(theta)); //SHOULD BE: theta = angle from sun to zenith, phi = angle from south axis (positive is towards east)
}