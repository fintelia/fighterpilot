
extern minstd_rand randomGen;

template<class T> T random(T min, T max)
{
    static uniform_real_distribution<double> dist(0.0, 1.0);

	return min + dist(randomGen) * max;
}
////////////////////////////////////////////////////////////////////
template<class T> T random(T max)
{
    return random<T>((T)0, max);
}
////////////////////////////////////////////////////////////////////
template<class T> T random()
{
	static uniform_real_distribution<T> dist;
	return dist(randomGen);
}
template<class T> Vector3<T> random3()
{
	T t = random<T>() * 2.0*PI;
	T z = random<T>() * 2.0 - 1.0;
	return Vector3<T>(z*cos(t),z*sin(t),(z < 0.0) ? -sqrt(1.0-z*z) : sqrt(1.0-z*z));
}
template<class T> Vector2<T> random2()
{
	T t = random<T>() * 2.0*PI;
	return Vector2<T>(cos(t),sin(t));
}