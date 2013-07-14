#include "MeshManager.h"

std::list<Mesh*> MeshManager::resourceList;
DWORD MeshManager::curIDIndex = 0;

void MeshManager::AddMesh(Mesh* mesh)
{
	_Assert(NULL != mesh);

	mesh->SetID(curIDIndex++);			// QUESTION:IDһֱ�Ӳ��������
	resourceList.push_back(mesh);
}

void MeshManager::DeleteAll()
{
	for(std::list<Mesh*>::iterator iter = resourceList.begin(); iter != resourceList.end(); ++iter)
	{
		SAFE_DROP(*iter);		// ����ʹ����drop����������delete, ����Model����mesh����ʱ, ��Ȼmesh������MeshManager�й�����
								// ��Model��������������
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
