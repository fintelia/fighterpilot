
template <class T>
class Matrix2
{
	public:

	T v[4];
	void set(	T v0, T v2,
				T v1, T v3)
	{
		v[0]=v0;	v[2]=v2;
		v[1]=v1;	v[3]=v3;
	}
	Matrix2()
	{
		set(	1.0,	0.0,
				0.0,	1.0);
	}
	Matrix2(	T v0, T v2,
				T v1, T v3)
	{
		set( v0, v2,
			 v1, v3);
	}
	Matrix2(T* p)
	{
		memcpy(v,p,4*sizeof(T));
	}
	T* ptr()
	{
		return v;
	}
	Matrix2 transpose()
	{
		return Matrix3(	v[0],	v[1],
						v[2],	v[3]);
	}
	T determinant()
	{
		return v[0]*v[3] - v[1]*v[2];
	}
};

template <class T>
class Matrix3
{
	public:

	T v[9];
	void set(	T v0, T v3, T v6,
				T v1, T v4, T v7,
				T v2, T v5, T v8)
	{
		v[0]=v0;	v[3]=v3;	v[6]=v6;
		v[1]=v1;	v[4]=v4;	v[7]=v7;
		v[2]=v2;	v[5]=v5;	v[8]=v8;
	}
	Matrix3()
	{
		set(	1.0,	0.0,	0.0,
				0.0,	1.0,	0.0,
				0.0,	0.0,	1.0);
	}
	Matrix3(	T v0, T v3, T v6,
				T v1, T v4, T v7,
				T v2, T v5, T v8)
	{
		set( v0, v3, v6,
			 v1, v4, v7,
			 v2, v5, v8);
	}
	Matrix3(Quaternion<T> q)
	{
		set(	1.0 - 2.0*q.y*q.y - 2.0*q.z*q.z,		2.0*q.x*q.y - 2.0*q.z*q.w,				2.0*q.x*q.z + 2.0*q.y*q.w,
				2.0*q.x*q.y + 2.0*q.z*q.w,				1.0 - 2.0*q.x*q.x - 2.0*q.z*q.z,		2.0*q.y*q.z - 2.0*q.x*q.w,
				2.0*q.x*q.z - 2.0*q.y*q.w,				2.0*q.y*q.z + 2.0*q.x*q.w,				1.0 - 2.0*q.x*q.x - 2.0*q.y*q.y);
	}
	Matrix3(T* p)
	{
		memcpy(v,p,9*sizeof(T));
	}
	const T* ptr() const
	{
		return &v[0];
	}
	T* ptr()
	{
		return v;
	}
	Matrix3 transpose()
	{
		return Matrix3(	v[0],	v[1],	v[2],
						v[3],	v[4],	v[5],
						v[6],	v[7],	v[8]);
	}
	T determinant() const
	{
		return v[0]*v[4]*v[8] - v[0]*v[5]*v[7] - v[1]*v[3]*v[8] + v[2]*v[3]*v[7] + v[1]*v[5]*v[6] - v[2]*v[4]*v[6];
	}
};

template <class T>
class Matrix4
{
public:

	T v[16];

	void set(	T v0, T v4, T v8,  T v12,
				T v1, T v5, T v9,  T v13,
				T v2, T v6, T v10, T v14,
				T v3, T v7, T v11, T v15)
	{
		v[0]=v0;	v[1]=v1;	v[2]=v2;	v[3]=v3;
		v[4]=v4;	v[5]=v5;	v[6]=v6;	v[7]=v7;
		v[8]=v8;	v[9]=v9;	v[10]=v10;	v[11]=v11;
		v[12]=v12;	v[13]=v13;	v[14]=v14;	v[15]=v15;
	}
	Matrix4()
	{
		set(	1.0,	0.0,	0.0,	0.0,
				0.0,	1.0,	0.0,	0.0,
				0.0,	0.0,	1.0,	0.0,
				0.0,	0.0,	0.0,	1.0);
	}
	Matrix4(	T v0, T v4, T v8,  T v12,
				T v1, T v5, T v9,  T v13,
				T v2, T v6, T v10, T v14,
				T v3, T v7, T v11, T v15)
	{
		set( v0, v4, v8,  v12,
			 v1, v5, v9,  v13,
			 v2, v6, v10, v14,
			 v3, v7, v11, v15);
	}
	Matrix4(Quaternion<T> rotation)
	{
		set(	1.0 - 2.0*rotation.y*rotation.y - 2.0*rotation.z*rotation.z,		2.0*rotation.x*rotation.y - 2.0*rotation.z*rotation.w,				2.0*rotation.x*rotation.z + 2.0*rotation.y*rotation.w,			0.0,
				2.0*rotation.x*rotation.y + 2.0*rotation.z*rotation.w,				1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.z*rotation.z,		2.0*rotation.y*rotation.z - 2.0*rotation.x*rotation.w,			0.0,
				2.0*rotation.x*rotation.z - 2.0*rotation.y*rotation.w,				2.0*rotation.y*rotation.z + 2.0*rotation.x*rotation.w,				1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.y*rotation.y,	0.0,
				0.0,																0.0,																0.0,															1.0);
	}
	Matrix4(Vector3<T> translation)
	{
		set(1.0, 0.0, 0.0, translation.x,
			0.0, 1.0, 0.0, translation.y,
			0.0, 0.0, 1.0, translation.z,
			0.0, 0.0, 0.0, 1.0);
	}
	Matrix4(Quaternion<T> rotation, Vector3<T> translation)
	{
		set(	1.0 - 2.0*rotation.y*rotation.y - 2.0*rotation.z*rotation.z,		2.0*rotation.x*rotation.y - 2.0*rotation.z*rotation.w,				2.0*rotation.x*rotation.z + 2.0*rotation.y*rotation.w,			translation.x,
				2.0*rotation.x*rotation.y + 2.0*rotation.z*rotation.w,				1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.z*rotation.z,		2.0*rotation.y*rotation.z - 2.0*rotation.x*rotation.w,			translation.y,
				2.0*rotation.x*rotation.z - 2.0*rotation.y*rotation.w,				2.0*rotation.y*rotation.z + 2.0*rotation.x*rotation.w,				1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.y*rotation.y,	translation.z,
				0.0,																0.0,																0.0,															1.0);
	}
	Matrix4(Quaternion<T> rotation, Vector3<T> translation, T scale)
	{
		set(	scale*(1.0 - 2.0*rotation.y*rotation.y - 2.0*rotation.z*rotation.z),	scale*(2.0*rotation.x*rotation.y - 2.0*rotation.z*rotation.w),			scale*(2.0*rotation.x*rotation.z + 2.0*rotation.y*rotation.w),		translation.x,
				scale*(2.0*rotation.x*rotation.y + 2.0*rotation.z*rotation.w),			scale*(1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.z*rotation.z),	scale*(2.0*rotation.y*rotation.z - 2.0*rotation.x*rotation.w),		translation.y,
				scale*(2.0*rotation.x*rotation.z - 2.0*rotation.y*rotation.w),			scale*(2.0*rotation.y*rotation.z + 2.0*rotation.x*rotation.w),			scale*(1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.y*rotation.y),translation.z,
				0.0,																	0.0,																	0.0,																1.0);
	}
	Matrix4(Quaternion<T> rotation, Vector3<T> translation, Vector3<T> scale)
	{
		set(	scale.x*(1.0 - 2.0*rotation.y*rotation.y - 2.0*rotation.z*rotation.z),	scale.y*(2.0*rotation.x*rotation.y - 2.0*rotation.z*rotation.w),		scale.z*(2.0*rotation.x*rotation.z + 2.0*rotation.y*rotation.w),		translation.x,
				scale.x*(2.0*rotation.x*rotation.y + 2.0*rotation.z*rotation.w),		scale.y*(1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.z*rotation.z),	scale.z*(2.0*rotation.y*rotation.z - 2.0*rotation.x*rotation.w),		translation.y,
				scale.x*(2.0*rotation.x*rotation.z - 2.0*rotation.y*rotation.w),		scale.y*(2.0*rotation.y*rotation.z + 2.0*rotation.x*rotation.w),		scale.z*(1.0 - 2.0*rotation.x*rotation.x - 2.0*rotation.y*rotation.y),	translation.z,
				0.0,																	0.0,																	0.0,																	1.0);
	}
	template <class U>
	Matrix4(Matrix4<U> u)
	{
		v[0]=(T)u.v[0];		v[1]=(T)u.v[1];		v[2]=(T)u.v[2];		v[3]=(T)u.v[3];
		v[4]=(T)u.v[4];		v[5]=(T)u.v[5];		v[6]=(T)u.v[6];		v[7]=(T)u.v[7];
		v[8]=(T)u.v[8];		v[9]=(T)u.v[9];		v[10]=(T)u.v[10];	v[11]=(T)u.v[11];
		v[12]=(T)u.v[12];	v[13]=(T)u.v[13];	v[14]=(T)u.v[14];	v[15]=(T)u.v[15];
	}
	Matrix4(T* p)
	{
		memcpy(v,p,16*sizeof(T));
	}
	const T* ptr() const
	{
		return &v[0];
	}
	T* ptr()
	{
		return v;
	}
	Vector3<T>& translation() const
	{
		return *((Vector3<T>*)(v+13));
	}
	Matrix4 operator* (Matrix4 o) const
	{
		return Matrix4(	v[0]*o.v[0] + v[4]*o.v[1] + v[8]*o.v[2] + v[12]*o.v[3],				v[0]*o.v[4] + v[4]*o.v[5] + v[8]*o.v[6] + v[12]*o.v[7],				v[0]*o.v[8] + v[4]*o.v[9] + v[8]*o.v[10] + v[12]*o.v[11],			v[0]*o.v[12] + v[4]*o.v[13] + v[8]*o.v[14] + v[12]*o.v[15],
						v[1]*o.v[0] + v[5]*o.v[1] + v[9]*o.v[2] + v[13]*o.v[3],				v[1]*o.v[4] + v[5]*o.v[5] + v[9]*o.v[6] + v[13]*o.v[7],				v[1]*o.v[8] + v[5]*o.v[9] + v[9]*o.v[10] + v[13]*o.v[11],			v[1]*o.v[12] + v[5]*o.v[13] + v[9]*o.v[14] + v[13]*o.v[15],
						v[2]*o.v[0] + v[6]*o.v[1] + v[10]*o.v[2] + v[14]*o.v[3],			v[2]*o.v[4] + v[6]*o.v[5] + v[10]*o.v[6] + v[14]*o.v[7],			v[2]*o.v[8] + v[6]*o.v[9] + v[10]*o.v[10] + v[14]*o.v[11],			v[2]*o.v[12] + v[6]*o.v[13] + v[10]*o.v[14] + v[14]*o.v[15],
						v[3]*o.v[0] + v[7]*o.v[1] + v[11]*o.v[2] + v[15]*o.v[3],			v[3]*o.v[4] + v[7]*o.v[5] + v[11]*o.v[6] + v[15]*o.v[7],			v[3]*o.v[8] + v[7]*o.v[9] + v[11]*o.v[10] + v[15]*o.v[11],			v[3]*o.v[12] + v[7]*o.v[13] + v[11]*o.v[14] + v[15]*o.v[15]);

	}
	Vector3<T> operator* (Vector3<T> o) const
	{
		T invW = 1.0 / (v[3]*o.x + v[7]*o.y + v[11]*o.z + v[15]);

		return Vector3<T>(	(v[0]*o.x + v[4]*o.y + v[8]*o.z + v[12])*invW,
							(v[1]*o.x + v[5]*o.y + v[9]*o.z + v[13])*invW,
							(v[2]*o.x + v[6]*o.y + v[10]*o.z + v[14])*invW);

	}
	Matrix4 operator* (T o) const
	{
		return Matrix4(	v[0]*o,		v[4]*o,		v[8]*o,		v[12]*o,
						v[1]*o,		v[5]*o,		v[9]*o,		v[13]*o,
						v[2]*o,		v[6]*o,		v[10]*o,	v[14]*o,
						v[3]*o,		v[7]*o,		v[11]*o,	v[15]*o);
	}
	T operator[] (int i) const
	{
		if(i >= 0 && i <= 15)
			return v[i];

		return 0;
	}
	Matrix4 transpose() 
	{
		return Matrix4(	v[0],	v[1],	v[2],	v[3],
						v[4],	v[5],	v[6],	v[7],
						v[8],	v[9],	v[10],	v[11],
						v[12],	v[13],	v[14],	v[15]);
	}

	T minor(unsigned char element) const
	{
		static Matrix3<T> m;
		unsigned char n=0;
		for(unsigned char i=0;i<16;i++)
		{
			if((i/4 != element/4) && (i%4 != element%4))
			{
				m.v[n++] = v[i];
			}
		}
		return m.determinant();
	}
	T cofactor(unsigned char element) const
	{
		return minor(element) * (1.0 - 2.0 * (((element%4) + (element/4))%2));
	}
	T determinant() const
	{
		return cofactor(0) + cofactor(1) + cofactor(2) + cofactor(3);
	}
	bool inverse(Matrix4& output) const
	{
		for(unsigned char i=0; i<16; i++)
			output.v[i] = cofactor(i);

		output = output.transpose();
		T det = v[0] * output.v[0] +
				v[4] * output.v[1] +
				v[8] * output.v[2] +
				v[12] *output.v[3];

		if(det == 0.0)
			return false;

		output = output * (1.0 / det);
		return true;
	}
	Matrix4& operator+=(const Matrix4& other){return *this = (*this)+other;}
	Matrix4& operator-=(const Matrix4& other){return *this = (*this)-other;}
	Matrix4& operator*=(const Matrix4& other){return *this = (*this)*other;}
	Matrix4& operator/=(const Matrix4& other){return *this = (*this)/other;}
};

typedef Matrix4<double> Mat4d;
typedef Matrix3<double> Mat3d;
typedef Matrix2<double> Mat2d;

typedef Matrix4<float> Mat4f;
typedef Matrix3<float> Mat3f;
typedef Matrix2<float> Mat2f;