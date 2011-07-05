
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
	matrix4x4(T* ptr)
	{
		memcpy(v,ptr,16*sizeof(T));
	}
	T* ptr()
	{
		return v;
	}
	Vector3<T>& translation()
	{
		return *((Vector3<T>*)(v+13));
	}
};