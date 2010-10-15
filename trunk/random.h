
typedef linear_congruential<unsigned long, 48271, 0, 2147483647>
	minstd_rand;


float randf(float min,float max);
float randf(float max);
float randf();

double randd(double min,double max);
double randd(double max);
double randd();

int randi(int min, int max);
int randi(int max);

template <class T>
T rand(T min, T max)
{
	return 0;
}

template <class T>
T rand(T max)
{
	return 0;
}
template <class T>
T rand()
{
	return 0;
}
class Rand
{
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
		tr1::uniform_int<T> dist(min,max);
		return dist(gen);
	}
	template <>
	float operator() <float> (float min, float max)
	{
		tr1::uniform_real<float> dist(min,max);
		return dist(gen);
	}
	template <>
	double operator() <double> (double min, double max)
	{
		tr1::uniform_real<double> dist(min,max);
		return dist(gen);
	}
//////////////////////////////////MAX/////////////////////////////////////////////
	template <class T>
	T operator() (T max)
	{
		tr1::uniform_int<T> dist(0,max);
		return dist(gen);
	}
	template <>
	float operator() <float> (float max)
	{
		tr1::uniform_real<float> dist(0.0f,max);
		return dist(gen);
	}
	template <>
	double operator() <double> (double max)
	{
		tr1::uniform_real<double> dist(0.0,max);
		return dist(gen);
	}
///////////////////////////////NO ARGS////////////////////////////////////////////
	template <class T>
	T operator() ()
	{
		tr1::uniform_real<T> dist(0.0,1.0);
		return dist(gen);
	}
	template <>
	Vec3f operator()<Vec3f> ()
	{
		tr1::uniform_real<float> dist(0.0f,1.0f);
		return Vec3f(dist(gen),dist(gen),dist(gen));;
	}
	template <>
	Vec3d operator()<Vec3d> ()
	{
		tr1::uniform_real<double> dist(0.0,1.0);
		return Vec3d(dist(gen),dist(gen),dist(gen));;
	}
	template <>
	Vector2<float> operator()<Vector2<float>> ()
	{
		tr1::uniform_real<float> dist(0.0f,1.0f);
		return Vector2<float>(dist(gen),dist(gen));;
	}
	template <>
	Vector2<double> operator()<Vector2<double>> ()
	{
		tr1::uniform_real<double> dist(0.0,1.0);
		return Vector2<double>(dist(gen),dist(gen));;
	}
private:
	tr1::minstd_rand gen;

	Rand()
	{
		gen.seed((unsigned int)time(NULL));
	}
};