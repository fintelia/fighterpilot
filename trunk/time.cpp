

#include <cmath>
#include <windows.h>

#include "debugBreak.h"
#include "time.h"

double GetTime()
{
	static __int64 ticksPerSecond=-1;
	if(ticksPerSecond==-1)
	{
		if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
			ticksPerSecond = 1000;
	}
	__int64 ticks;

	// This is the number of clock ticks since start
	if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
		ticks = (__int64)GetTickCount();

	// Divide by frequency to get the time in ms(__int64)
	double d=static_cast<double>( (ticks*1000.0) )/ticksPerSecond;
	return d;
}
__int64 GameTime::totalTicks() const
{
	__int64 ticks;
	if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
		ticks = (__int64)GetTickCount();
	return ticks;
}
GameTime::GameTime(): sReal(0), eReal(0), sGame(0), sSpeed(0), eSpeed(0), changingSpeed(false), timeSpeed(1.0), paused(false), lReal(0), lGame(0.0), cReal(0), cGame(0.0), ticksPerSecond(1000),  updateStage(false), numUpdates(0)
{
	if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
		ticksPerSecond = 1000;

	lReal = totalTicks();
	cReal = lReal;
}
void GameTime::reset()
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