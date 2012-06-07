
template <class T>
class Vector2
{
public:
	T x,y;

	Vector2(): x(), y() {}
	Vector2(T X, T Y): x(X), y(Y) {}

	template<class U> Vector2(Vector2<U> u): x(u.x), y(u.y) {}

	T& operator[](int index)
	{
		if(index==0)	return x;
		if(index==1)	return y;
		debugBreak();
		throw out_of_range(string("attempted to access element ") + lexical_cast<string>(index) + "of a 2D vector. Index should be either 0 or 1.");
	}
	T operator[](int index) const
	{
		if(index==0)	return x;
		if(index==1)	return y;
		debugBreak();
		throw out_of_range(string("attempted to access element ") + lexical_cast<string>(index) + "of a 2D vector. Index should be either 0 or 1.");
	}
	Vector2 operator*(T scale) const{return Vector2(x * scale, y * scale);}
	Vector2 operator/(T scale) const{return Vector2(x / scale, y / scale);}
	Vector2 operator+(const Vector2& other) const{return Vector2(x + other.x, y + other.y);}
	Vector2 operator-(const Vector2& other) const{return Vector2(x - other.x, y - other.y);}
	Vector2 operator-() const{return Vector2(-x, -y);}

	Vector2& operator*=(T scale){	return *this = (*this)*scale;}
	Vector2& operator/=(T scale){	return *this = (*this)/scale;}
	Vector2& operator+=(const Vector2 &other){return *this = (*this)+other;}
	Vector2& operator-=(const Vector2 &other){return *this = (*this)-other;}

	bool operator!=(const Vector2& other) const{return !equal(other);}
	bool operator==(const Vector2& other) const{return equal(other);}

	T magnitude() const
	{
		return sqrt(x*x + y*y);
	}
	T magnitudeSquared() const
	{
		return x*x + y*y;
	}
	Vector2 normalize() const
	{
		T m = x * x + y * y;
		if(m > 0)
		{
			m = sqrt(m);
			return Vector2(x / m, y / m);
		}
		else
		{
			return *this;
		}
	}
	T dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}
	Vector2& set(T X, T Y)
	{
		x=X;
		y=Y;
		return *this;
	}
	bool equal(const Vector2& v, T maxDifference=0.01) const
	{
		return (abs(x-v.x) <= maxDifference && abs(y-v.y) <= maxDifference);
	}
	T distance(const Vector2& v) const
	{
		return sqrt((x-v.x)*(x-v.x)+(y-v.y)*(y-v.y));
	}
	T distanceSquared(const Vector2& v)const
	{
		return (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y);
	}
	template<class U> bool operator<(Vector2<U> v) const //less than and greater than operators are just for sorting in containers
	{
		return (x != v.x) ? x < v.x : y < v.y;
	}
	template<class U> bool operator>(Vector2<U> v) const
	{
		return (x != v.x) ? x > v.x : y > v.y;
	}
};
typedef Vector2<float>			Vec2f;
typedef Vector2<double>			Vec2d;
typedef Vector2<int>			Vec2i;
typedef Vector2<unsigned int>	Vec2u;

template <class T, class U>
Vector2<T> operator*(const Vector2<T> v, U scale)
{
	return Vector2<T>(v.x * scale, v.y * scale);
}

template <class T> Vector2<T> lerp(const Vector2<T>& v1, const Vector2<T>& v2, T t)
{
	if (t <= (T)0.0)	return v1;
	if (t >= (T)1.0)	return v2;
	return (v1 * ((T)1.0 - t) + v2 * t).normalize();
};

template <class T>
class Vector3
{
public:
	T x,y,z;
	Vector3(): x(), y(), z() {}
	Vector3(T X, T Y, T Z): x(X), y(Y), z(Z) {}
	template<class U> Vector3(const Vector3<U>& u):x(u.x), y(u.y), z(u.z) {}

	T &operator[](int index)
	{
		if(index==0)	return x;
		if(index==1)	return y;
		if(index==2)	return z;
		debugBreak();
		throw out_of_range(string("attempted to access element ") + lexical_cast<string>(index) + "of a 3D vector. Index should be either 0, 1, or 2.");
	}
	T operator[](int index) const
	{
		if(index==0)	return x;
		if(index==1)	return y;
		if(index==2)	return z;
		debugBreak();
		throw out_of_range(string("attempted to access element ") + lexical_cast<string>(index) + "of a 3D vector. Index should be either 0, 1, or 2.");
	}


	Vector3 operator/(T scale) const{				return Vector3(x / scale, y / scale, z / scale);		}
	Vector3 operator+(const Vector3 &other) const{	return Vector3(x + other.x, y + other.y, z + other.z);	}
	Vector3 operator-(const Vector3 &other) const{	return Vector3(x - other.x, y - other.y, z - other.z);	}
	Vector3 operator-() const{						return Vector3(-x, -y, -z);								}

	Vector3& operator*=(T scale){return *this = (*this)*scale;}
	Vector3& operator/=(T scale){return *this = (*this)/scale;}
	Vector3& operator+=(const Vector3& other){return *this = (*this)+other;}
	Vector3& operator-=(const Vector3& other){return *this = (*this)-other;}

	bool operator!=(const Vector3& other) const{return !equal(other);}
	bool operator==(const Vector3& other) const{return equal(other);}

	T magnitude() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	T magnitudeSquared() const
	{
		return x*x + y*y + z*z;
	}
	Vector3 normalize() const
	{
		T m = x * x + y * y + z * z;
		if(m > 0)
		{
			m = sqrt(m);
			return Vector3(x / m, y / m, z / m);
		}
		else
		{
			return *this;
		}
	}
	T dot(const Vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}
	Vector3 cross(const Vector3& other) const
	{
		return Vector3(y * other.z - z * other.y,	 z * other.x - x * other.z,	 x * other.y - y * other.x);
	}
	Vector3& set(T X, T Y, T Z)
	{
		x=X;
		y=Y;
		z=Z;
		return *this;
	}
	bool equal(const Vector3& v,T maxDifference=0.01) const
	{
		return (abs(x-v.x) <= maxDifference && abs(y-v.y) <= maxDifference && abs(z-v.z) <= maxDifference);
	}
	T distance(const Vector3& v) const
	{
		return sqrt((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z));
	}
	T distanceSquared(const Vector3& v) const
	{
		return (v.x-x)*(v.x-x)+(v.y-y)*(v.y-y)+(v.z-z)*(v.z-z);
	}
};
typedef Vector3<float>		Vec3f;
typedef Vector3<double>		Vec3d;

template <class T, class U>
Vector3<T> operator*(const Vector3<T>& v, U scale)
{
	return Vector3<T>(v.x * scale, v.y * scale, v.z * scale);
}

template <class T> Vector3<T> lerp(const Vector3<T>& v1, const Vector3<T>& v2, T t)
{
	if (t <= (T)0.0)	return v1;
	if (t >= (T)1.0)	return v2;
	return (v1 * ((T)1.0 - t) + v2 * t);
};

template<class T>
std::ostream& operator<<(std::ostream& s, const Vector3<T>& v)
{
    return s << "(" << v.x << "," << v.y << "," << v.z << ")";
}

template<class T>
std::istream& operator>>(std::istream& s, Vector3<T>& v)
{
	char c[64];
	s.width(64);
	s >> c;

	v.x=0;
	v.y=0;
	v.z=0;

	double p=0.1;
	double sign = 1.0;
	bool decimalPoint = false;
	bool started = false;

	T* val = &v.x;
	int componentNum=0;
	int n=0;

	while(n < 64 && c[n] != 0)
	{
		if(!started && c[n] == '-')
		{
			sign = -1.0;
		}
		else if(c[n] >= '0' && c[n] <= '9')
		{
			if(started && decimalPoint)
			{
				*val += sign * p * (c[n] - '0');	//add the current digit to the proper decimal place
				p *= 0.1;							//move one place to the right
			}
			else
			{
				started = true;								//started may or may not already have been set to true
				*val = (*val) * 10.0 + sign * (c[n] - '0');	//shift the contents of the current value one place to the left and add the current digit
			}
		}
		else if(started && !decimalPoint && c[n] == '.')
		{
			decimalPoint = true;	//if we have not hit a decimal point yet, start the decimal part (otherwise move on to the next component)
		}
		else if(started)
		{
			val++;					//move on to the next component
			p=0.1;					//reset p
			sign = 1.0;				//reset sign
			started = false;		//reset started
			decimalPoint = false;	//reset decimalPoint
			if(++componentNum > 2)	//make sure that we have not finished writing yet
			{
				break;
			}
		}
		n++;
	}
    return s;
}

