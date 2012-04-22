
class Ephemeris
{
private:
	Ephemeris();
	~Ephemeris();

	double date; //in days since January 1, 2000
	Angle latitude; //positive = north
	Angle longitude; //positive = east

public:
	static Ephemeris& getInstance()
	{
		static Ephemeris* pInstance = new Ephemeris();
		return *pInstance;
	}

	void setPosition(Angle Latitude, Angle Longitude);
	void setTime(double hours, int month, int day, int year);

	Vec3f getSunDirection();
};

extern Ephemeris& ephemeris;