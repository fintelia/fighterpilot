
double GetTime();

const extern double UPDATE_LENGTH;

class GameTime
{
private:
	//---------interpolate speed---------
	__int64 sReal, eReal;
	double sGame;
	double sSpeed, eSpeed;
	bool changingSpeed;
	//-----------------------------------


	double timeSpeed;//time speed in    ms(gametime) / ms(realtime)
	bool paused;//wether time is paused

	__int64 lReal;//ticks at last frame 
	double lGame;//time in ms at last frame

	__int64 cReal;//ticks at current frame 
	double cGame;//ticks in ms at current frame


	__int64 ticksPerSecond;
	__int64 totalTicks() const
	{
		__int64 ticks;
		if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
			ticks = (__int64)GetTickCount();
		return ticks;
	}
	double trueGameTime() const
	{
		__int64 real = totalTicks();
		
		if(changingSpeed)
		{
			if(paused)
			{
				return cGame;
			}
			else
			{
				double t = 1000.0*(real-sReal)/ticksPerSecond;
				if(real < eReal)
				{
					return sGame + t*sSpeed - (t*t/2-t) * (sSpeed-eSpeed)/(1000.0*(eReal-sReal)/ticksPerSecond);
				}
				else
				{
					return sGame + t*sSpeed - (t*t/2-t) * (sSpeed-eSpeed)/(1000.0*(eReal-sReal)/ticksPerSecond) + eSpeed*(1000.0*(real - eReal)/ticksPerSecond);
				}
			}
		}
		else
		{
			if(paused)
			{
				return cGame;
			}
			else
			{
				return cGame + timeSpeed*(1000*(real - cReal)/ticksPerSecond);
			}		
		}
	}


	bool updateStage;
	unsigned long numUpdates;

public:
	GameTime(): changingSpeed(false), timeSpeed(1.0), paused(false), lGame(0.0), cGame(0.0), updateStage(false), numUpdates(0)
	{
		if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
			ticksPerSecond = 1000;

		lReal = totalTicks();
		cReal = lReal;
	}
	void reset()
	{
		if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
			ticksPerSecond = 1000;

		lReal = cReal = totalTicks();
		lGame = cGame = 0.0;
		timeSpeed=1.0;
		paused = false;
		changingSpeed = false;
		updateStage = false;
		numUpdates = 0;
	}
	void nextUpdate()
	{
		updateStage = true;
		numUpdates++;
	}
	void nextFrame()
	{
		updateStage = false;

		lReal = cReal;
		lGame = cGame;

		cReal = totalTicks();
		
		if(changingSpeed)
		{
			if(paused)
			{
				sReal += cReal - lReal;
				eReal += cReal - lReal;
			}
			else
			{
				double t = 1000.0*(cReal-sReal)/ticksPerSecond;
				if(cReal < eReal)
				{
					cGame = sGame + t*sSpeed - (t*t/2-t) * (sSpeed-eSpeed)/(1000.0*(eReal-sReal)/ticksPerSecond);
				}
				else
				{
					timeSpeed = eSpeed;
					changingSpeed = false;
					cGame = sGame + t*sSpeed - (t*t/2-t) * (sSpeed-eSpeed)/(1000.0*(eReal-sReal)/ticksPerSecond) + timeSpeed*(1000.0*(cReal - eReal)/ticksPerSecond);
				}
			}
		}
		else
		{
			if(paused)
			{
				cGame = lGame;
			}
			else
			{
				cGame = lGame + timeSpeed*(1000*(cReal - lReal)/ticksPerSecond);
			}		
		}
	}

	void changeSpeed(double speed, double changeRate = 0.0)//change rate in ms^2(gametime) / ms(realtime)
	{
		if(speed <= 0.0)
		{
			debugBreak();
			return;
		}

		if(changeRate == 0.0)
		{
			timeSpeed = speed;
		}
		else
		{
			sReal = cReal;
			sGame = cGame;
			sSpeed = timeSpeed;

			eSpeed = speed;
			double d = (eSpeed - sSpeed)*ticksPerSecond;
			eReal = sReal + (__int64)abs(d/changeRate);
			changingSpeed = true;
		}
	}
	void pause()
	{
		paused = true;
	}
	void unpause()
	{
		paused = false;
	}
	void setPaused(bool b)
	{
		paused = b;
	}

	bool isPaused() const
	{
		return paused;
	}

	double length() const
	{
		if(updateStage)
			return UPDATE_LENGTH;
		else
			return cGame-lGame;
	}
	double lastTime() const
	{
		if(updateStage)
			return UPDATE_LENGTH * numUpdates - UPDATE_LENGTH;
		else
			return lGame;
	}
	double time() const
	{
		if(updateStage)
			return UPDATE_LENGTH * numUpdates;
		else
			return cGame;
	}
	double operator() () const
	{
		return time();
	}
	bool needsUpdate()
	{
		return trueGameTime() > UPDATE_LENGTH * numUpdates;
	}
	double interpolate()
	{
		return min((UPDATE_LENGTH * numUpdates - cGame) / UPDATE_LENGTH, 1.0);
	}
};