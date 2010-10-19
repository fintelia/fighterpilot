
template <typename T>
class Quaternion
{
public:
	T x, y, z, w;
	Quaternion(): x(0), y(0), z(0), w(1.0) 
	{

	}
	Quaternion(T X, T Y, T Z, T W): x(X), y(Y), z(Z), w(W) 
	{

	}
	Quaternion(const Vector3<T>& vec, Angle theta): w(cos(theta/2)) 
	{
		Vector3<T> vn = vec.normalize() * sin(theta/2);

		x = vn.x;
		y = vn.y;
		z = vn.z;
	}
	Quaternion(Angle yaw, Angle pitch, Angle roll)
	{
		yaw		/= 2.0;
		pitch	/= 2.0;
		roll	/= 2.0;

 		T siny = sin(yaw);
		T sinp = sin(pitch);
		T sinr = sin(roll);
		T cosy = cos(yaw);
		T cosp = cos(pitch);
		T cosr = cos(roll);
 
		this->x = sinr * cosp * cosy - cosr * sinp * siny;
		this->y = cosr * sinp * cosy + sinr * cosp * siny;
		this->z = cosr * cosp * siny - sinr * sinp * cosy;
		this->w = cosr * cosp * cosy + sinr * sinp * siny;
 
		normalize();
	}
	Quaternion normalize()
	{
		// Don't normalize if we don't have to
		T mag2 = w * w + x * x + y * y + z * z;
		if (  mag2!=0.0 && (fabs(mag2 - 1.0f) > 0.00001)) {
			T mag = sqrt(mag2);
			w /= mag;
			x /= mag;
			y /= mag;
			z /= mag;
		}
		return *this;
	}
	Quaternion conjugate() const
	{
		return Quaternion<T>(-x, -y, -z, w);
	}
	template <typename U>
	Quaternion<T> operator* (const Quaternion<U> &rq) const
	{
		// the constructor takes its arguments as (x, y, z, w)
		return Quaternion(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
						  w * rq.y + y * rq.w + z * rq.x - x * rq.z,
						  w * rq.z + z * rq.w + x * rq.y - y * rq.x,
						  w * rq.w - x * rq.x - y * rq.y - z * rq.z);
	}
	Vec3f operator* (const Vector3<T> &vec) const
	{
		//Quaternion<T> vecQuat(vec.x,vec.y,vec.z,0.0);
		//vecQuat.normalize();

		Vector3<T> vn(vec);
		vn.normalize();
 
		Quaternion<T> vecQuat(vn.x,vn.y,vn.z,0.0), resQuat;
 
		resQuat = vecQuat * this->conjugate();
		resQuat = *this * resQuat;
 
		return (Vector3<T>(resQuat.x, resQuat.y, resQuat.z));
	}
	Quaternion lerp(const Quaternion& q2, T t)
	{
		if (t <= (T)0.0)
			return *this;
		if (t >= (T)1.0)
			return q2;

		return ((*this) * ((T)1.0 - t)+q2 * t).normalize();
	}
	Quaternion<T> slerp(const Quaternion<T>& q2, T t)
	{
		if (t <= (T)0.0)
			return *this;
		if (t >= (T)1.0)
			return q2;
		
		Quaternion<T> q3 = q2;
		T c = dot(q3);

		if (c < (T)0.0)
		{
			q3 = -q3;
			c = -c;
		}
	
		if (c > (T)0.999)
			return lerp(q3, t); // Lerp() = q1 + t * (q2 - q1)
		
		Angle a = acosA(c); // ACos() clamps input to [-1, 1]
		return  ((*this) *sin(a * (1.0 - t)) + q3 * sin(a * t)) / sin(a);
	}
	template <typename U>
	T dot(const Quaternion<U> &q)
	{
		return x*q.x+y*q.y+z*q.z+w*q.w;
	}
	Quaternion operator*(T scale) const
	{
		return Quaternion<T>(x*scale,y*scale,z*scale,w*scale);
	}
	Quaternion operator/(T scale) const
	{
		return Quaternion<T>(x/scale,y/scale,z/scale,w/scale);
	}
	Quaternion operator+(const Quaternion &other) const
	{
		return Quaternion<T>(x+other.x,y+other.y,z+other.z,w+other.w);
	}
	Quaternion operator-(const Quaternion &other) const
	{
		return Quaternion<T>(x-other.x,y-other.y,z-other.z,w-other.w);
	}
	Quaternion operator-() const
	{
		return Quaternion<T>(-x,-y,-z,-w);
	}
};

typedef Quaternion<float> Quat4f;
typedef Quaternion<double> Quat4d;
