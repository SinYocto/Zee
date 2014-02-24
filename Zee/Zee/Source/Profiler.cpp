#include "Profiler.h"

int Profiler::numDrawCalls = 0;
int Profiler::numVerts = 0;
int Profiler::numTris = 0;


void Profiler::AddDrawCalls(int count /*= 1*/)
{
	numDrawCalls += count;
}

void Profiler::AddNumVerts(int count)
{
	numVerts += count;
}

void Profiler::AddNumTris(int count)
{
	numTris += count;
}

int Profiler::GetNumDrawCalls()
{
	return numDrawCalls;
}

int Profiler::GetNumVerts()
{
	return numVerts;
}

int Profiler::GetNumTris()
{
	return numTris;
}

void Profiler::FrameUpdate()
{
	numDrawCalls = 0;
	numVerts = 0;
	numTris = 0;
}


