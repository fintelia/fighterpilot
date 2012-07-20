
double GetTime();

const extern double UPDATE_LENGTH;

class GameTime
{
private:
	//---------interpolate speed---------
	long long sReal, eReal;
	double sGame;
	double sSpeed, eSpeed;
	bool changingSpeed;
	//-----------------------------------


	double timeSpeed;//time speed in    ms(gametime) / ms(realtime)
	int paused;// =0 if time is paused; >0 if time is not paused

	long long lReal;//ticks at last frame
	double lGame;//time in ms at last frame

	long long cReal;//ticks at current frame
	double cGame;//ticks in ms at current frame


	long long ticksPerSecond;
	double trueGameTime() const;


	bool updateStage;
	unsigned long numUpdates;

public:
	GameTime();
	void reset();
	void nextUpdate();
	void nextFrame();

	void changeSpeed(double speed, double changeRate = 0.0);//change rate in ms^2(gametime) / ms(realtime)

	void pause();
	void unpause();
	void setPaused(bool b);
	bool isPaused() const;
	double length() const;
	double lastTime() const;
	double time() const;
	double operator() () const;
	bool needsUpdate() const;
	double interpolate() const;
	double getSpeed() const;
};
