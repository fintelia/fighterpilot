
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
	float d = date;// - (longitude.degrees()/15.0)/24; //adjust for local time

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

//	float phi = 0;//Dec - latitude;
//	float theta = (date-floor(date))*2.0*PI + PI;//RA - longitude;
//	return Vec3f(cos(phi)*sin(theta), cos(theta), sin(phi)*sin(theta)); //SHOULD BE: theta = angle from sun to zenith, phi = angle from south axis (positive is towards east)


	//See: Practical Astronomy With Your Calculator (Third Edition) Pages 40 and 86
	Angle epsilon = 280.4659 * PI/180;
	Angle w = 282.940472178 * PI/180;
	float e = 0.016709114;

	Angle N = 2*PI * d/365.242191;
	Angle M = N + epsilon - w;
	Angle lamda = M + 2.0*e*sin(M) + w;
	Angle beta = 0.0; 

	Angle alpha = atan2(sin(lamda)*cos(23.439292*PI/180) - tan(beta)*sin(23.439292*PI/180), cos(lamda));
	Angle delta = asin(sin(beta)*cos(23.439292*PI/180) + cos(beta)*sin(23.439292*PI/180)*sin(lamda));

	float UT = (d - floor(d)) - floor(longitude.degrees()/15.0)/24; //in decimal days
	UT = UT - floor(UT);
	float T = (floor(d) - 0.5) / 36525.0;
	float T0 = 6.697374558 + 2400.051335*T + 0.000025862*T*T;
	float GST = UT * 1.002737909 + T0/24.0; //also in decimal days
	GST = GST - floor(GST);

	float LST = GST + (longitude.degrees()/15.0)/24; //still in decimal days
	LST = LST - floor(LST);
	float H = (LST - alpha/(2.0*PI)) * 2.0*PI;

	Angle a = asin(sin(delta)*sin(latitude)+cos(delta)*cos(latitude)*cos(H)); //altitude
	Angle A = asin(-sin(H)*cos(delta)/cos(a)); //azimuth (N = 0 degrees, E = 90 degrees,...)
	//float A = acos((sin(delta) - sin(latitude)*sin(alpha)) / (cos(latitude)*sin(alpha)));

	Profiler.setOutput("a", a.degrees());
	Profiler.setOutput("A", A.degrees());
	return Vec3f(cos(A)*sin(a), cos(a), sin(A)*sin(a));
}
