#include"Time.h"
#include "YString.h"

Timer PerformanceTimer::timer;
std::vector<PerformanceTimer::Performance> PerformanceTimer::performanceList;
int PerformanceTimer::curPerformanceIndex = -1;

void PerformanceTimer::Begin(const wchar_t* desc)
{
	Performance newPerformance(desc);
	performanceList.push_back(newPerformance);

	float intervalTime = 0;
	if(++curPerformanceIndex != 0)
	{
		intervalTime = timer.GetElapsedTime();
	}
 
	timer.Reset();

	{
		wchar_t temp[MAX_STR_LEN];
		YString::Empty(temp);
		for(int i = 0; i < curPerformanceIndex; ++i)
		{
			YString::Concat(temp, _countof(temp), L"--");
		}

		YString::Concat(temp, _countof(temp), L"start %s\n");	// log消息加上前面的"--"层级符
		ConsolePrint(temp, performanceList[curPerformanceIndex].desc);
	}

	for(int i = 0; i < curPerformanceIndex; ++i)
		performanceList[i].timeUsed += intervalTime;
}

void PerformanceTimer::End()
{
	_Assert(curPerformanceIndex >= 0);

	float intervalTime = timer.GetElapsedTime();
	timer.Reset();

	for(int i = 0; i <= curPerformanceIndex; ++i)
		performanceList[i].timeUsed += intervalTime;

	{
		wchar_t temp[MAX_STR_LEN];
		YString::Empty(temp);
		for(int i = 0; i < curPerformanceIndex; ++i)
		{
			YString::Concat(temp, _countof(temp), L"--");
		}

		YString::Concat(temp, _countof(temp), L"finish %s. Time used(%fs)\n");	// log消息加上前面的"--"层级符
		ConsolePrint(temp, performanceList[curPerformanceIndex].desc, performanceList[curPerformanceIndex].timeUsed);
	}

	performanceList.pop_back();

	curPerformanceIndex--;
}

//void PerformanceTimer::BeginObserveFunc(const wchar_t* funcName)
//{
//	
//}
//
//void PerformanceTimer::EndObserveFunc()
//{
//
//}

float FrameTimer::GetDeltaTime()
{
	return deltaTime;
}

int FrameTimer::GetFPS()
{
	static int fps = 0;
	static int fpsAccumulator = 0;
	static float elapseTime = 0;

	if(elapseTime > 1.0f)
	{
		fps = fpsAccumulator;

		fpsAccumulator = 0;
		elapseTime = 0;
	}

	elapseTime += deltaTime;
	fpsAccumulator++;

	return fps;
}
