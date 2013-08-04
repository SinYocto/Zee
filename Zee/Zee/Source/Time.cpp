#include"Time.h"
#include "YString.h"

Timer Time::timer;
float Time::deltaTime;		// TODO:是否一定要在外面定义?
//LONGLONG Time::ticksPerSecond;
//LONGLONG Time::lastTick;

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
		Log(temp, performanceList[curPerformanceIndex].desc);
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
		Log(temp, performanceList[curPerformanceIndex].desc, performanceList[curPerformanceIndex].timeUsed);
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
