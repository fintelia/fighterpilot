
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
/*
class Rand
{
private:
	template <class T>
	T gRand()
	{
		static_assert(is_integral::value,"Rand::gRand() called with invalid type");
		static bool isUnsigned = is_unsigned<T>::value;
		static uniform_real<T> dist(isUnsigned ? ((T)0) : ~((T)0), isUnsigned ? ~((T)0) : -(~((T)0)));
		return dist(gen);
	}
	template<> float gRand<float>()
	{
		static uniform_real<float> dist(0.0f,1.0f);
		return dist(gen);
	}
	template<> double gRand<double>()
	{
		static uniform_real<double> dist(0.0,1.0);
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
		float z = gRand<float>() * 2.0f - 1.0f;
		return Vec3f(z*cos(t),z*sin(t),(z < 0.0f) ? -sqrt(1.0f-z*z) : sqrt(1.0f-z*z));
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
	minstd_rand gen;

	Rand()
	{
		gen.seed((unsigned int)time(NULL));
	}
};*/

#include <random>

extern minstd_rand randomGen;


#ifdef VISUAL_STUDIO
	struct Rand{
    template<class T> static T random(T min, T max)
    {
        static uniform_real_distribution<double> dist(0.0, 1.0);

		return min + dist(randomGen) * (max-min);
    }
////////////////////////////////////////////////////////////////////////
    template<class T> static T random(T max)
    {
        return random<T>((T)0, max);
    }
////////////////////////////////////////////////////////////////////////
    template<class T> static T random()
    {
		static uniform_real_distribution<T> dist;
		return dist(randomGen);
    }
	template<> static float random<float>()
	{
		static uniform_real_distribution<float> dist(0.0f,1.0f);
		return dist(randomGen);
	}
	template <>	static Vec3f random<Vec3f> ()
	{
		float t = random<float>() * 2.0f*PI;
		float z = random<float>() * 2.0f - 1.0f;
		return Vec3f(z*cos(t),z*sin(t),(z < 0.0f) ? -sqrt(1.0f-z*z) : sqrt(1.0f-z*z));
	}
	template <>	static Vec3d random<Vec3d> ()
	{
		double t = random<double>() * 2.0*PI;
		double z = random<double>();
		return Vec3d(cos(t),sin(t),sqrt(1.0-z*z));
	}
	template <>	static Vector2<float> random<Vector2<float>> ()
	{
		float t = random<float>() * 2.0f*PI;
		return Vector2<float>(cos(t),sin(t));
	}
	template <>	static Vector2<double> random<Vector2<double>> ()
	{
		double t = random<double>() * 2.0*PI;
		return Vector2<double>(cos(t),sin(t));
	}
	};
	template<class T> T random(T min, T max){return Rand::random<T>(min,max);}
	template<class T> T random(T max){return Rand::random<T>(max);}
	template<class T> T random(){return Rand::random<T>();}
#else
namespace Rand{
    template<class T> T random(T min, T max);
////////////////////////////////////////////////////////////////////////
    template<class T> T random(T max);
////////////////////////////////////////////////////////////////////////
    template<class T> T random();
	template<> float random<float>();
	template <>	Vec3f random<Vec3f> ();
	template <>	Vec3d random<Vec3d> ();
	template <>	Vector2<float> random<Vector2<float>> ();
	template <>	Vector2<double> random<Vector2<double>> ();
}
using namespace Rand;
#endif
