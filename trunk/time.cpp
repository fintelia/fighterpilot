
#include "engine.h"

//#include <cmath>
//
//#ifdef _DEBUG
//#include <iostream>
//#endif

#if defined(_WIN32)
	#define NOMINMAX
	#include <windows.h>
#elif defined(__linux__)
	#include <time.h>
#else
	#error OS not supported by time.cpp
#endif

//#include "debugBreak.h"
//#include "time.h"




#if defined(_WIN32)
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
#elif defined(__linux__)
	long long getTotalTicksPerSecond()
	{
		return 1000000000;
	}
	long long totalTicks()
	{
		timespec t;
		if(!clock_gettime(CLOCK_MONOTONIC, &t))
		{
			return static_cast<long long>(t.tv_sec) * 1000000000 + t.tv_nsec;
		}
		else
		{
#ifdef _DEBUG
			std::cout << "clock_gettime failed" << std::endl;
#endif
			return -1;
		}
	}
#endif

double GetTime()
{
	static long long ticksPerSecond = getTotalTicksPerSecond();
	return static_cast<double>(totalTicks()) * 1000.0 / ticksPerSecond;
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
			double t = 1000.0*(real-cReal)/ticksPerSecond;
			if(t <= changeTimeLeft)
			{
				return cGame + timeSpeed * t + (finalTimeSpeed-timeSpeed) * t*t / changeTimeLeft;
			}
			else
			{
				return cGame + timeSpeed * changeTimeLeft + (finalTimeSpeed-timeSpeed) * changeTimeLeft + finalTimeSpeed*(changeTimeLeft-t);
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
GameTime::GameTime(): timeSpeed(1.0), paused(0), lReal(0), lGame(0.0), cReal(0), cGame(0.0), changingSpeed(false), changeTimeLeft(0.0), finalTimeSpeed(1.0), ticksPerSecond(1000), updateStage(false), cUpdateTime(0), lUpdateTime(0), updateLength(8.33333)
{
	ticksPerSecond = getTotalTicksPerSecond();
	lReal = cReal = totalTicks();
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
	lUpdateTime = 0;
	cUpdateTime = 0;
}
void GameTime::nextUpdate()
{
	updateStage = true;
	lUpdateTime = cUpdateTime;
	cUpdateTime += updateLength;
}
void GameTime::nextFrame()
{
	updateStage = false;
	lReal = cReal;
	lGame = cGame;

	cReal = totalTicks();
	if(changingSpeed)
	{
		if(!paused)
		{
			double t = static_cast<float>(1000.0*(cReal - lReal))/ticksPerSecond;
			if(t <= changeTimeLeft)
			{
				cGame += timeSpeed * t + (finalTimeSpeed-timeSpeed) * t*t / changeTimeLeft;
				timeSpeed += (finalTimeSpeed-timeSpeed) * t / changeTimeLeft;
				changeTimeLeft -= t;
			}
			else
			{
				cGame += timeSpeed * changeTimeLeft + (finalTimeSpeed-timeSpeed) * changeTimeLeft + finalTimeSpeed*(changeTimeLeft-t);
				timeSpeed = finalTimeSpeed;
				changeTimeLeft = 0.0;
				changingSpeed = false;
			}
		}
	}
	else
	{
		if(!paused)
		{
			cGame += timeSpeed*(1000*(cReal - lReal)/ticksPerSecond);
		}
	}
	if(cGame > cUpdateTime)
	{
		cGame = cUpdateTime;
	}
}
void GameTime::changeSpeed(double speed, double changeTime)//change rate in ms^2(gametime) / ms(realtime)
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
	//	timeSpeed = speed;
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
	if(changeTime <= 0)
	{
		timeSpeed = speed;
	}
	else
	{
		changingSpeed = true;
		changeTimeLeft = changeTime;
		finalTimeSpeed = speed;
	}
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
		return cUpdateTime-lUpdateTime;
	else
		return cGame-lGame;
}
double GameTime::lastTime() const
{
	if(updateStage)
		return lUpdateTime;
	else
		return lGame;
}
double GameTime::time() const
{
	if(updateStage)
		return cUpdateTime;
	else
		return cGame;
}
double GameTime::operator() () const
{
	return time();
}
bool GameTime::needsUpdate() const
{
	return trueGameTime() > cUpdateTime;
}
double GameTime::interpolate() const
{
	double i = 1.0 - (cUpdateTime - cGame) / (cUpdateTime - lUpdateTime);
	if(i >= 0.0 && i <= 1.0)
		return i;
	else if(i > 1.0)
		return 1.0;
	else
		return 0.0;
}
double GameTime::getSpeed() const
{
	return timeSpeed;
}
double GameTime::getUpdateLength() const
{
	return updateLength;
}
void GameTime::setUpdateLength(double length)
{
	updateLength = length;
}