
double GetTime();

class GameTime
{
private:
	double timeSpeed;
	__int64 pauseTime;
	bool paused;


	__int64 startTime;
	__int64 ticksPerSecond;

	__int64 totalTicks()
	{
		__int64 ticks;
		if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
			ticks = (__int64)GetTickCount();
		return ticks;
	}
public:
	GameTime(): timeSpeed(1.0), paused(false)
	{
		if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
			ticksPerSecond = 1000;
		startTime=totalTicks();
	}
	double getTime()
	{
		if(paused)	return 1000.0*pauseTime/ticksPerSecond;
		return 1000.0*(totalTicks()-startTime)*timeSpeed/ticksPerSecond;
	}
	double operator() ()
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
	bool isPaused()
	{
		return paused;
	}
};
extern GameTime gameTime;