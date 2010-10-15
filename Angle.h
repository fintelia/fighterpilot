
extern const float PI;
class Angle
{
private:
	double ang;
public:
	Angle(double a)
	{
		ang=a;
		normalize();
	}
	Angle()
	{
		ang=0;
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
	
	bool inRange(Angle low,Angle high)
	{
		if(low.ang<high.ang)
		{
			return (ang>low.ang && ang<high.ang);
		}
		else
		{
			return !(ang>low.ang && ang<high.ang);
		}
	}
	bool inRange(double low,double high)
	{
		if(low<high)
		{
			return (ang>low && ang<high);
		}
		else
		{
			return !(ang>high && ang<low);
		}
	}
	operator double() { return ang; }

	Angle operator*(Angle a){return Angle(ang*a.ang);}
	Angle operator/(Angle a){return Angle(ang/a.ang);}
	Angle operator+(Angle a){return Angle(ang+a.ang);}
	Angle operator-(Angle a){return Angle(ang-a.ang);}

	Angle operator*(double a){return Angle(ang*(float)a);}
	Angle operator/(double a){return Angle(ang/(float)a);}
	Angle operator+(double a){return Angle(ang+(float)a);}
	Angle operator-(double a){return Angle(ang-(float)a);}

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

	Angle& operator*=(const double& a){ang*=(float)a; normalize(); return *this;}
	Angle& operator/=(const double& a){ang/=(float)a; normalize(); return *this;}
	Angle& operator+=(const double& a){ang+=(float)a; normalize(); return *this;}
	Angle& operator-=(const double& a){ang-=(float)a; normalize(); return *this;}

	Angle& operator*=(const float& a){ang*=a; normalize(); return *this;}
	Angle& operator/=(const float& a){ang/=a; normalize(); return *this;}
	Angle& operator+=(const float& a){ang+=a; normalize(); return *this;}
	Angle& operator-=(const float& a){ang-=a; normalize(); return *this;}

	Angle& operator*=(const int& a){ang*=a; normalize(); return *this;}
	Angle& operator/=(const int& a){ang/=a; normalize(); return *this;}
	Angle& operator+=(const int& a){ang+=a; normalize(); return *this;}
	Angle& operator-=(const int& a){ang-=a; normalize(); return *this;}


};
Angle atan2A(double X,double Y);
Angle asinA(double X);
Angle acosA(double X);
Angle lerp(Angle a1,Angle a2,double l);