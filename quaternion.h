
template <typename T>
class Quaternion
{
public:
	T x, y, z, w;
	Quaternion(X, Y, Z, W): x(X), y(Y), z(Z), w(W) 
	{

	}
	Quaternion(const Vector3<T>& vec, Angle theta): w(cos(ang/2)) 
	{
		Vector3<t> vn = vec.normalize() * sin(theta/2);

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
	void normalize()
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
	}
	Quaternion conjugate()
	{
		return Quaternion(-x, -y, -z, w);
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
		Quaternion<T> vecQuat(vec.x,vec.y,vec.z,0.0);
		vecQuat.normalize();

		Vector3<T> vn(vec);
		vn.normalize();
 
		Quaternion<T> vecQuat(vn.x,vn.y,vn.z,0.0), resQuat;
 
		resQuat = vecQuat * getConjugate();
		resQuat = *this * resQuat;
 
		return (Vector3<T>(resQuat.x, resQuat.y, resQuat.z));
	}
};

typedef Quat4f Quaternion<float>;
typedef Quat4d Quaternion<double>;
