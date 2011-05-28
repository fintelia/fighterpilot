
typedef linear_congruential<unsigned long, 48271, 0, 2147483647>
	minstd_rand;

//float randf(float min,float max);
//float randf(float max);
//float randf();
//
//double randd(double min,double max);
//double randd(double max);
//double randd();
//
//int randi(int min, int max);
//int randi(int max);
//
//template <class T>
//T rand(T min, T max)
//{
//	return 0;
//}
//
//template <class T>
//T rand(T max)
//{
//	return 0;
//}
//template <class T>
//T rand()
//{
//	return 0;
//}
class Rand
{
private:
	template <class T>
	T gRand()
	{
		static_assert(tr1::is_integral::value,"Rand::gRand() called with invalid type");
		static bool isUnsigned = tr1::is_unsigned<T>::value;
		static tr1::uniform_real<T> dist(isUnsigned ? ((T)0) : ~((T)0), isUnsigned ? ~((T)0) : -(~((T)0)));
		return dist(gen);
	}
	template<> float gRand<float>() 
	{
		static tr1::uniform_real<float> dist(0.0f,1.0f);
		return dist(gen);
	}
	template<> double gRand<double>() 
	{
		static tr1::uniform_real<double> dist(0.0,1.0);
		return dist(gen);
	}
public:
	static Rand& getInstance()
	{
		static Rand* pInstance = new Rand();
		return *pInstance;
	}
////////////////////////////////MIN/MAX///////////////////////////////////////////
	template <class T>
	T operator() (T min, T max)
	{
		return min + (max - min) * gRand<double>();
	}
	template <>
	float operator() <float> (float min, float max)
	{
		return min + (max - min) * gRand<float>();
	}
//////////////////////////////////MAX/////////////////////////////////////////////
	template <class T>
	T operator() (T max)
	{
		return operator()((T)0,max);
	}
///////////////////////////////NO ARGS////////////////////////////////////////////
	template <class T>
	T operator() ()
	{
		return gRand<T>();
	}
	template <>
	Vec3f operator()<Vec3f> ()
	{
		float t = gRand<float>() * 2.0f*PI;
		float z = gRand<float>();
		return Vec3f(cos(t),sin(t),sqrt(1.0f-z*z));
	}
	template <>
	Vec3d operator()<Vec3d> ()
	{
		double t = gRand<double>() * 2.0*PI;
		double z = gRand<double>();
		return Vec3d(cos(t),sin(t),sqrt(1.0-z*z));
	}
	template <>
	Vector2<float> operator()<Vector2<float>> ()
	{
		float t = gRand<float>() * 2.0f*PI;
		return Vector2<float>(cos(t),sin(t));
	}
	template <>
	Vector2<double> operator()<Vector2<double>> ()
	{
		double t = gRand<double>() * 2.0*PI;
		return Vector2<double>(cos(t),sin(t));
	}
private:
	tr1::minstd_rand gen;

	Rand()
	{
		gen.seed((unsigned int)time(NULL));
	}
};
extern Rand& randomGen;

template<class T> T random()
{
	return randomGen.operator()<T>();
}
template<class T> T random(T max)
{
	return randomGen.operator()<T>(max);
}

template<class T> T random(T min, T max)
{
	return randomGen.operator()<T>(min,max);
}