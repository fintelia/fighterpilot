
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
		set(	1.0 - 2.0*q.y*q.y - 2.0*q.z*q.z,		2.0*q.x*q.y + 2.0*q.z*w,				2.0*q.x*q.z + 2.0*q.y*w,
				2.0*q.x*q.y - 2.0*q.z*w,				1.0 - 2.0*q.x*q.x - 2.0*q.z*q.z,		2.0*q.y*q.z + 2.0*q.x*w,
				2.0*q.x*q.z + 2.0*q.y*w,				2.0*q.y*q.z - 2.0*q.x*w,				1.0 - 2.0*q.x*q.x - 2.0*q.y*q.y);
	}
	Matrix3(T* p)
	{
		memcpy(v,p,9*sizeof(T));
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
	T determinant()
	{
		return v[0]*v[4]*v[8] - v[0]*v[5]*v[7] - v[1]*v[3]*v[8] + v[2]*v[3]*v[7] + v[1]*v[5]*v[6] - v[2]*v[4]*v[6];
	}
};

template <class T>
class matrix4x4
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
	matrix4x4()
	{
		set(	1.0,	0.0,	0.0,	0.0,
				0.0,	1.0,	0.0,	0.0,
				0.0,	0.0,	1.0,	0.0,
				0.0,	0.0,	0.0,	1.0);
	}
	matrix4x4(	T v0, T v4, T v8,  T v12,
				T v1, T v5, T v9,  T v13,
				T v2, T v6, T v10, T v14,
				T v3, T v7, T v11, T v15)
	{
		set( v0, v4, v8,  v12,
			 v1, v5, v9,  v13,
			 v2, v6, v10, v14,
			 v3, v7, v11, v15);
	}
	matrix4x4(Quaternion<T> q)
	{
		set(	1.0 - 2.0*q.y*q.y - 2.0*q.z*q.z,		2.0*q.x*q.y + 2.0*q.z*w,				2.0*q.x*q.z + 2.0*q.y*w,			0.0,
				2.0*q.x*q.y - 2.0*q.z*w,				1.0 - 2.0*q.x*q.x - 2.0*q.z*q.z,		2.0*q.y*q.z + 2.0*q.x*w,			0.0,
				2.0*q.x*q.z + 2.0*q.y*w,				2.0*q.y*q.z - 2.0*q.x*w,				1.0 - 2.0*q.x*q.x - 2.0*q.y*q.y		0.0,
				0.0,									0.0,									0.0,								1.0);
	}
	matrix4x4(Quaternion<T> q, Vector3<T> t)
	{
		set(	1.0 - 2.0*q.y*q.y - 2.0*q.z*q.z,		2.0*q.x*q.y + 2.0*q.z*w,				2.0*q.x*q.z + 2.0*q.y*w,			0.0,
				2.0*q.x*q.y - 2.0*q.z*w,				1.0 - 2.0*q.x*q.x - 2.0*q.z*q.z,		2.0*q.y*q.z + 2.0*q.x*w,			0.0,
				2.0*q.x*q.z + 2.0*q.y*w,				2.0*q.y*q.z - 2.0*q.x*w,				1.0 - 2.0*q.x*q.x - 2.0*q.y*q.y		0.0,
				t.x,									t.y,									t.z,								1.0);
	}
	matrix4x4(Quaternion<T> q, Vector3<T> t, T s)
	{
		set(	s*(1.0 - 2.0*q.y*q.y - 2.0*q.z*q.z),	s*(2.0*q.x*q.y + 2.0*q.z*w),			s*(2.0*q.x*q.z + 2.0*q.y*w),			0.0,
				s*(2.0*q.x*q.y - 2.0*q.z*w),			s*(1.0 - 2.0*q.x*q.x - 2.0*q.z*q.z),	s*(2.0*q.y*q.z + 2.0*q.x*w),			0.0,
				s*(2.0*q.x*q.z + 2.0*q.y*w),			s*(2.0*q.y*q.z - 2.0*q.x*w),			s*(1.0 - 2.0*q.x*q.x - 2.0*q.y*q.y)		0.0,
				t.x,									t.y,									t.z,									1.0);
	}
	matrix4x4(Quaternion<T> q, Vector3<T> t, Vector3<T> s)
	{
		set(	s.x*(1.0 - 2.0*q.y*q.y - 2.0*q.z*q.z),	s.y*(2.0*q.x*q.y + 2.0*q.z*w),			s.z*(2.0*q.x*q.z + 2.0*q.y*w),			0.0,
				s.x*(2.0*q.x*q.y - 2.0*q.z*w),			s.y*(1.0 - 2.0*q.x*q.x - 2.0*q.z*q.z),	s.z*(2.0*q.y*q.z + 2.0*q.x*w),			0.0,
				s.x*(2.0*q.x*q.z + 2.0*q.y*w),			s.y*(2.0*q.y*q.z - 2.0*q.x*w),			s.z*(1.0 - 2.0*q.x*q.x - 2.0*q.y*q.y)	0.0,
				t.x,									t.y,									t.z,									1.0);
	}
	template <class U>
	matrix4x4(matrix4x4<U> u)
	{
		v[0]=(T)u.v[0];		v[1]=(T)u.v[1];		v[2]=(T)u.v[2];		v[3]=(T)u.v[3];
		v[4]=(T)u.v[4];		v[5]=(T)u.v[5];		v[6]=(T)u.v[6];		v[7]=(T)u.v[7];
		v[8]=(T)u.v[8];		v[9]=(T)u.v[9];		v[10]=(T)u.v[10];	v[11]=(T)u.v[11];
		v[12]=(T)u.v[12];	v[13]=(T)u.v[13];	v[14]=(T)u.v[14];	v[15]=(T)u.v[15];
	}
	matrix4x4(T* p)
	{
		memcpy(v,p,16*sizeof(T));
	}
	T* ptr()
	{
		return v;
	}
	Vector3<T>& translation()
	{
		return *((Vector3<T>*)(v+13));
	}
	matrix4x4 operator* (matrix4x4 o)
	{
		return matrix4x4(	v[0]*o.v[0] + v[4]*o.v[1] + v[8]*o.v[2] + v[12]*o.v[3],
							v[0]*o.v[4] + v[4]*o.v[5] + v[8]*o.v[6] + v[12]*o.v[7],
							v[0]*o.v[8] + v[4]*o.v[9] + v[8]*o.v[10] + v[12]*o.v[11],
							v[0]*o.v[12] + v[4]*o.v[13] + v[8]*o.v[14] + v[12]*o.v[5],

							v[1]*o.v[0] + v[5]*o.v[1] + v[9]*o.v[2] + v[13]*o.v[3],
							v[1]*o.v[4] + v[5]*o.v[5] + v[9]*o.v[6] + v[13]*o.v[7],
							v[1]*o.v[8] + v[5]*o.v[9] + v[9]*o.v[10] + v[13]*o.v[11],
							v[1]*o.v[12] + v[5]*o.v[13] + v[9]*o.v[14] + v[13]*o.v[5],

							v[2]*o.v[0] + v[6]*o.v[1] + v[10]*o.v[2] + v[14]*o.v[3],
							v[2]*o.v[4] + v[6]*o.v[5] + v[10]*o.v[6] + v[14]*o.v[7],
							v[2]*o.v[8] + v[6]*o.v[9] + v[10]*o.v[10] + v[14]*o.v[11],
							v[2]*o.v[12] + v[6]*o.v[13] + v[10]*o.v[14] + v[14]*o.v[5],

							v[3]*o.v[0] + v[7]*o.v[1] + v[11]*o.v[2] + v[15]*o.v[3],
							v[3]*o.v[4] + v[7]*o.v[5] + v[11]*o.v[6] + v[15]*o.v[7],
							v[3]*o.v[8] + v[7]*o.v[9] + v[11]*o.v[10] + v[15]*o.v[11],
							v[3]*o.v[12] + v[7]*o.v[13] + v[11]*o.v[14] + v[15]*o.v[5]);

	}
	Vector3<T> operator* (Vector3<T> o)
	{
		T invW = 1.0 / (v[3]*o.x + v[7]*o.y + v[11]*o.z + v[15]);

		return Vector3<T>(	(v[0]*o.x + v[4]*o.y + v[8]*o.z + v[12])*invW,
							(v[1]*o.x + v[5]*o.y + v[9]*o.z + v[13])*invW,
							(v[2]*o.x + v[6]*o.y + v[10]*o.z + v[14])*invW);

	}
	matrix4x4 operator* (T o)
	{
		return matrix4x4(	v[0]*o,		v[4]*o,		v[8]*o,		v[12]*o,
							v[1]*o,		v[5]*o,		v[9]*o,		v[13]*o,
							v[2]*o,		v[6]*o,		v[10]*o,	v[14]*o,
							v[3]*o,		v[7]*o,		v[11]*o,	v[15]*o);
	}
	T operator[] (int i)
	{
		if(i >= 0 && i <= 15)
			return v[i];

		return 0;
	}
	matrix4x4 transpose()
	{
		return matrix4x4(	v[0],	v[1],	v[2],	v[3],
							v[4],	v[5],	v[6],	v[7],
							v[8],	v[9],	v[10],	v[11],
							v[12],	v[13],	v[14],	v[15]);
	}

	T minor(unsigned char element)
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
	T cofactor(unsigned char element)
	{
		return minor(element) * (1.0 - 2.0 * (((element%4) + (element/4))%2));
	}
	T determinant()
	{
		return cofactor(0) + cofactor(1) + cofactor(2) + cofactor(3);
	}
	bool inverse(matrix4x4& output)
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
};
