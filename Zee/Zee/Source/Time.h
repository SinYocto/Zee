#ifndef	TIME_H
#define TIME_H

#include "Utility.h"
#include<windows.h>

class Timer
{
public:
	Timer()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		ticksPerSecond = frequency.QuadPart;
	}

	void Reset()
	{
		getCurrentTick(&startTick);
	}

	float GetElapsedTime()
	{
		LONGLONG curTick;
		getCurrentTick(&curTick);

		return (curTick - startTick) / (float)ticksPerSecond;
	}

private:
	void getCurrentTick(LONGLONG* curTick)
	{
		_Assert(NULL != curTick);

		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);

		*curTick = tick.QuadPart;
	}

private:
	LONGLONG ticksPerSecond;
	LONGLONG startTick;
};

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