#ifndef PROFILER_H
#define PROFILER_H

class Profiler
{
public:
	static void AddDrawCalls(int count = 1);
	static void AddNumVerts(int count);
	static void AddNumTris(int count);

	static void FrameUpdate();

	static int GetNumDrawCalls();
	static int GetNumVerts();
	static int GetNumTris();

private:
	static int numDrawCalls;
	static int numVerts;
	static int numTris;
};

#endif