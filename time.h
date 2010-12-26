
double GetTime();

class GameTime
{
private:
	double timeSpeed;
	__int64 pauseTime;
	bool paused;


	__int64 startTime;
	__int64 ticksPerSecond;

	__int64 totalTicks() const
	{
		__int64 ticks;
		if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
			ticks = (__int64)GetTickCount();
		return ticks;
	}
public:
	void reset()
	{
		startTime=totalTicks();
		timeSpeed=1.0;
		paused=false;
	}
	GameTime()
	{
		if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
			ticksPerSecond = 1000;
		reset();
	}
	double getTime() const
	{
		if(paused)	return 1000.0*pauseTime/ticksPerSecond;
		return 1000.0*(totalTicks()-startTime)*timeSpeed/ticksPerSecond;
	}
	double operator() () const
	{
		return getTime();
	}
	void setSpeed(double speed)
	{
		__int64 currentTime=totalTicks();
		startTime=currentTime-(currentTime-startTime)*speed/timeSpeed;
		timeSpeed=speed;
	}
	void pause()
	{
		if(!paused)
		{
			paused=true;
			pauseTime=totalTicks()-startTime;
		}
	}
	void unpause()
	{
		if(paused)
		{
			paused=false;
			__int64 currentTime=totalTicks();
			startTime+=currentTime-(pauseTime+startTime);
		}
	}
	bool isPaused() const
	{
		return paused;
	}
	void addTime(double ms)
	{
		startTime -= ms / 1000.0 * ticksPerSecond;
		__int64 t = totalTicks();
		if(startTime > t)	startTime = t;
	}
};