#include "MeshManager.h"

std::list<Mesh*> MeshManager::resourceList;
DWORD MeshManager::curIDIndex = 0;

void MeshManager::AddMesh(Mesh* mesh)
{
	_Assert(NULL != mesh);

	mesh->SetID(curIDIndex++);			// QUESTION:ID一直加不会溢出吧
	resourceList.push_back(mesh);
}

void MeshManager::DeleteAll()
{
	for(std::list<Mesh*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// 这里使用了drop方法而不是delete, 当有Model持有mesh对象时, 虽然mesh对象不在MeshManager中管理了
								// 但Model中仍能正常持有
	}

	resourceList.clear();
}

void MeshManager::GetMesh(const wchar_t* name, Mesh** mesh)
{
	_Assert(NULL != mesh);

	*mesh = NULL;
	for(std::list<Mesh*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		Mesh* curMesh = *iter;
		if(YString::Compare(curMesh->GetName(), name) == 0)
		{
			*mesh = curMesh;
			break;
		}
	}
}

void MeshManager::OnLostDevice()
{
	for(std::list<Mesh*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		(*iter)->OnLostDevice();
	}
}

void MeshManager::OnResetDevice()
{
	for(std::list<Mesh*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		(*iter)->OnResetDevice();
	}
}
