
extern const double PI;
class Angle
{
private:
	double ang;
public:
	Angle(double a): ang(a)
	{
		normalize();
	}
	Angle(): ang(0)
	{

	}
	void normalize()
	{
		while(ang>2.0*PI)
			ang-=2.0*PI;
		while(ang<0.0)
			ang+=2.0*PI;
	}
	double getAngle()
	{
		return ang;
	}
	void setAngle(double a)
	{
		ang = a;
	}
	double degrees()
	{
		return ang*180.0/PI;
	}
	double degrees_plusMinus180()
	{
		return ang < PI ? ang*180.0/PI : ang*180.0/PI - 360;
	}
	double radians_plusMinusPI()
	{
		return ang < PI ? ang : ang - 2.0*PI;
	}
	bool inRange(Angle low,Angle high, bool inclusive = true)
	{
		if(inclusive)
		{
			if(low.ang == high.ang)	return ang == low.ang;
			if(low.ang<high.ang)	return (ang>=low.ang && ang<=high.ang);
									return !(ang>=high.ang && ang<=low.ang);
		}
		else
		{
			if(low.ang<high.ang)	return (ang>low.ang && ang<high.ang);
									return !(ang>high.ang && ang<low.ang);
		}

	}
	bool inRange(double low,double high, bool inclusive = true)
	{
		while(low>2.0*PI)
			low-=2.0*PI;
		while(low<0.0)
			low+=2.0*PI;

		while(high>2.0*PI)
			high-=2.0*PI;
		while(high<0.0)
			high+=2.0*PI;

		if(inclusive)
		{
			if(low == high)	return ang == low;
			if(low<high)	return (ang>=low && ang<=high);
							return !(ang>=high && ang<=low);
		}
		else
		{
			if(low<high)	return (ang>low && ang<high);
							return !(ang>high && ang<low);
		}
	}
	void clampedAdd(double amount, double low,double high)
	{
		if(low>high)
			return;

		if(amount > 0)
		{
			ang+=amount;
			if(ang > high)
				ang=high;
		}
		else
		{
			ang+=amount;
			if(ang < low)
				ang=low;
		}
		normalize();
	}
	operator double() { return ang; }

	Angle operator*(Angle a){return Angle(ang*a.ang);}
	Angle operator/(Angle a){return Angle(ang/a.ang);}
	Angle operator+(Angle a){return Angle(ang+a.ang);}
	Angle operator-(Angle a){return Angle(ang-a.ang);}

	Angle operator*(double a){return Angle(ang*a);}
	Angle operator/(double a){return Angle(ang/a);}
	Angle operator+(double a){return Angle(ang+a);}
	Angle operator-(double a){return Angle(ang-a);}

	Angle operator*(float a){return Angle(ang*a);}
	Angle operator/(float a){return Angle(ang/a);}
	Angle operator+(float a){return Angle(ang+a);}
	Angle operator-(float a){return Angle(ang-a);}

	Angle operator*(int a){return Angle(ang*a);}
	Angle operator/(int a){return Angle(ang/a);}
	Angle operator+(int a){return Angle(ang+a);}
	Angle operator-(int a){return Angle(ang-a);}

	Angle& operator*=(const Angle& a){ang*=a.ang; normalize(); return *this;}
	Angle& operator/=(const Angle& a){ang/=a.ang; normalize(); return *this;}
	Angle& operator+=(const Angle& a){ang+=a.ang; normalize(); return *this;}
	Angle& operator-=(const Angle& a){ang-=a.ang; normalize(); return *this;}

	Angle& operator*=(const double& a){ang*=a; normalize(); return *this;}
	Angle& operator/=(const double& a){ang/=a; normalize(); return *this;}
	Angle& operator+=(const double& a){ang+=a; normalize(); return *this;}
	Angle& operator-=(const double& a){ang-=a; normalize(); return *this;}

	Angle& operator*=(const float& a){ang*=a; normalize(); return *this;}
	Angle& operator/=(const float& a){ang/=a; normalize(); return *this;}
	Angle& operator+=(const float& a){ang+=a; normalize(); return *this;}
	Angle& operator-=(const float& a){ang-=a; normalize(); return *this;}

	Angle& operator*=(const int& a){ang*=a; normalize(); return *this;}
	Angle& operator/=(const int& a){ang/=a; normalize(); return *this;}
	Angle& operator+=(const int& a){ang+=a; normalize(); return *this;}
	Angle& operator-=(const int& a){ang-=a; normalize(); return *this;}


};
Angle atan2A(double opp,double adj);
Angle asinA(double X);
Angle acosA(double X);
Angle lerp(Angle a1,Angle a2,double l);

template <class U,class V>
Angle clamp(Angle angle, U v1, V v2)
{
	if(angle.inRange(v1,v2))
		return angle;

	Angle low=v1;
	Angle high=v2;

	Angle center;
	if(high.getAngle() > low.getAngle())	center = (high.getAngle() + low.getAngle())/2;
	else									center = (high.getAngle()-PI*2 + low.getAngle())/2;

	Angle opp = center + PI;

	if(angle.inRange(high,opp))
		return high;

	return low;

}
