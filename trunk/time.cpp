

#include <cmath>

#if defined __WIN32
	#include <windows.h>
#elif defined __linux__
	#include <time.h>
#else
	#error OS not supported by time.cpp
#endif

#include "debugBreak.h"
#include "time.h"




#if defined __WIN32
	long long getTotalTicksPerSecond()
	{
		long long ticksPerSecond;
		if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
			ticksPerSecond = 1000;
		return ticksPerSecond;
	}
	long long totalTicks()
	{
		long long ticks;
		if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
			ticks = (long long)GetTickCount();
		return ticks;
	}
#elif defined __linux__
	long long getTotalTicksPerSecond()
	{
		return 1000000000;
	}
	long long totalTicks()
	{
		timespec t;
#ifdef _POSIX_CPUTIME
		if(!clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t))
#else
		if(!clock_gettime(CLOCK_REALTIME, &t))
#endif
		{
			return static_cast<long long>(t.tv_sec) * 1000000000 + t.tv_nsec;
		}
		else
		{
			return -1;
		}
	}
#endif

double GetTime()
{
	static long long ticksPerSecond=-1;
	if(ticksPerSecond==-1)
	{
		ticksPerSecond = getTotalTicksPerSecond();
	}


	// Divide by frequency to get the time in ms (long long)
	return static_cast<double>( (totalTicks()*1000.0) )/ticksPerSecond;
}
double GameTime::trueGameTime() const
{
	long long real = totalTicks();
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
GameTime::GameTime(): sReal(0), eReal(0), sGame(0), sSpeed(0), eSpeed(0), changingSpeed(false), timeSpeed(1.0), paused(0), lReal(0), lGame(0.0), cReal(0), cGame(0.0), ticksPerSecond(1000),  updateStage(false), numUpdates(0)
{
	ticksPerSecond = getTotalTicksPerSecond();
	lReal = totalTicks();
	cReal = lReal;
}
void GameTime::reset()
{
	ticksPerSecond = getTotalTicksPerSecond();
	lReal = cReal = totalTicks();
	lGame = cGame = 0.0;
	timeSpeed=1.0;
	paused = false;
	changingSpeed = false;
	updateStage = false;
	numUpdates = 0;
}
void GameTime::nextUpdate()
{
	updateStage = true;
	numUpdates++;
}
void GameTime::nextFrame()
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
void GameTime::changeSpeed(double speed, double changeRate)//change rate in ms^2(gametime) / ms(realtime)
{
	if(speed <= 0.0)
	{
		debugBreak();
		return;
	}
	//else if(abs(speed - timeSpeed) < 0.001 || changingSpeed)
	//{
	//	return;
	//}
	//if(changeRate == 0.0)
	//{
		timeSpeed = speed;
	//}
	//else
	//{
	//	sReal = cReal;
	//	sGame = cGame;
	//	sSpeed = timeSpeed;
	//	eSpeed = speed;
	//	double d = (eSpeed - sSpeed)*ticksPerSecond;
	//	eReal = sReal + (long long)abs(d/changeRate);
	//	changingSpeed = true;
	//}
}
void GameTime::pause()
{
	paused++;
}
void GameTime::unpause()
{
	paused--;
	if(paused < 0)
		paused = 0;
}
void GameTime::setPaused(bool b)
{
	paused += b ? 1 : -1;
	if(paused < 0)
		paused = 0;
}
bool GameTime::isPaused() const
{
	return paused > 0;
}
double GameTime::length() const
{
	if(updateStage)
		return UPDATE_LENGTH;
	else
		return cGame-lGame;
}
double GameTime::lastTime() const
{
	if(updateStage)
		return UPDATE_LENGTH * numUpdates - UPDATE_LENGTH;
	else
		return lGame;
}
double GameTime::time() const
{
	if(updateStage)
		return UPDATE_LENGTH * numUpdates;
	else
		return cGame;
}
double GameTime::operator() () const
{
	return time();
}
bool GameTime::needsUpdate() const
{
	return trueGameTime() > UPDATE_LENGTH * numUpdates;
}
double GameTime::interpolate() const
{
	return 1.0 - (((UPDATE_LENGTH * numUpdates - cGame) / UPDATE_LENGTH < 1.0) ? ((UPDATE_LENGTH * numUpdates - cGame) / UPDATE_LENGTH) : 1.0);
}
double GameTime::getSpeed() const
{
	if(changingSpeed)
	{
		return sSpeed + (eSpeed - sSpeed) * (cReal - sReal) / (eReal - sReal);
	}
	else
	{
		return timeSpeed;
	}
}
