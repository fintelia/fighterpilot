

//#include <cstdlib>
#include "engine.h"

//float randf(float min,float max)
//{
//	return rand()*(max-min)/RAND_MAX+min;
//}
//float randf(float max)
//{
//	return randf(0.0,max);
//}
//float randf()
//{
//	return randf(0.0,1.0);
//}
//
//double randd(double min,double max)
//{
//	return rand()*(max-min)/RAND_MAX+min;
//}
//double randd(double max)
//{
//	return randf(0.0,max);
//}
//double randd()
//{
//	return randf(0.0,1.0);
//}
//
//int randi(int min, int max)
//{
//	return rand()%(max-min)+min;
//}
//int randi(int max)
//{
//	return randi(0, max);
//}
#ifndef VISUAL_STUDIO
namespace Rand{
    template<class T> T random(T min, T max)
    {
        static uniform_real_distribution<double> dist(0.0, 1.0);

		return min + dist(randomGen) * max;
    }
////////////////////////////////////////////////////////////////////////
    template<class T> T random(T max)
    {
        return random<T>((T)0, max);
    }
////////////////////////////////////////////////////////////////////////
    template<class T> T random()
    {
		static uniform_real_distribution<T> dist;
		return dist(randomGen);
    }
	template<> float random<float>()
	{
		static uniform_real_distribution<float> dist(0.0f,1.0f);
		return dist(randomGen);
	}
	template <>	Vec3f random<Vec3f> ()
	{
		float t = random<float>() * 2.0f*PI;
		float z = random<float>() * 2.0f - 1.0f;
		return Vec3f(z*cos(t),z*sin(t),(z < 0.0f) ? -sqrt(1.0f-z*z) : sqrt(1.0f-z*z));
	}
	template <>	Vec3d random<Vec3d> ()
	{
		double t = random<double>() * 2.0*PI;
		double z = random<double>();
		return Vec3d(cos(t),sin(t),sqrt(1.0-z*z));
	}
	template <>	Vector2<float> random<Vector2<float>> ()
	{
		float t = random<float>() * 2.0f*PI;
		return Vector2<float>(cos(t),sin(t));
	}
	template <>	Vector2<double> random<Vector2<double>> ()
	{
		double t = random<double>() * 2.0*PI;
		return Vector2<double>(cos(t),sin(t));
	}
}
#endif
minstd_rand randomGen;
