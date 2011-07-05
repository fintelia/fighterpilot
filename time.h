
double GetTime();

class GameTime
{
private:

	__int64 sReal, eReal;
	double sGame;
	double sSpeed, eSpeed;
	bool changingSpeed;


	double timeSpeed;
	bool paused;

	__int64 lReal;
	double lGame;

	__int64 cReal;
	double cGame;


	__int64 ticksPerSecond;
	__int64 totalTicks() const
	{
		__int64 ticks;
		if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
			ticks = (__int64)GetTickCount();
		return ticks;
	}
public:
	GameTime(): changingSpeed(false), timeSpeed(1.0), paused(false), lGame(0.0), cGame(0.0)
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
	}
	void nextFrame()
	{
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

	void ChangeSpeed(double speed, double changeRate)//change rate in ms^2(gametime) / ms(realtime)
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
		return cGame-lGame;
	}
	double lastTime() const
	{
		return lGame;
	}
	double time() const
	{
		return cGame;
	}
	double operator() () const
	{
		return cGame;
	}
};