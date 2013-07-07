#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "Mesh.h"
#include <list>

class MeshManager
{
public:
	static void AddMesh(Mesh* mesh);
	static void DeleteAll();
	static void GetMesh(const char* name, Mesh** mesh);

	static void OnLostDevice();
	static void OnResetDevice();

private:
	static std::list<Mesh*> resourceList;
	static DWORD curIDIndex;
};


#endif