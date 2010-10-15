#include "windows.h"
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