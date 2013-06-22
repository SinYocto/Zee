#ifndef	TIME_H
#define TIME_H

#include<windows.h>

class Time
{
public:
	Time() {}

	static void Start()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		ticksPerSecond = frequency.QuadPart;

		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);
		lastTick = tick.QuadPart;
	}

	static void Tick()
	{
		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);

		LONGLONG curTick = tick.QuadPart;
		deltaTime = (curTick - lastTick) / (float)ticksPerSecond;

		lastTick = curTick;
	}

public:
	static float deltaTime;

private:
	static LONGLONG ticksPerSecond;
	static LONGLONG lastTick;
};

#endif