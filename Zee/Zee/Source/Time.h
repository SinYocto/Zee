#ifndef	TIME_H
#define TIME_H

#include "Utility.h"
#include "YString.h"

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

class PerformanceTimer
{
private:
	//struct FuncPerformance
	//{
	//	FuncPerformance(const wchar_t* _funcName)
	//		:runTime(0)
	//	{
	//		YString::Copy(funcName, _countof(funcName), _funcName);
	//	}

	//	wchar_t funcName[MAX_STR_LEN];
	//	float runTime;

	//	Timer timer;
	//};

	struct Performance
	{
		Performance(const wchar_t* descStr)
			:timeUsed(0)
		{
			YString::Copy(desc, _countof(desc), descStr);
		}

		wchar_t desc[MAX_STR_LEN];
		float timeUsed;

		//std::vector<FuncPerformance> funcPerformanceList;
	};

public:
	static void Begin(const wchar_t* desc);
	static void End();

	//static void BeginObserveFunc(const wchar_t* funcName);
	//static void EndObserveFunc();

private:
	static Timer timer;
	static std::vector<Performance> performanceList;
	static int curPerformanceIndex;
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