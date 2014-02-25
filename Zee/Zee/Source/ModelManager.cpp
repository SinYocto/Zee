#include "ModelManager.h"
#include "Engine.h"

ModelManager::ModelManager()
:defaultCube(NULL)
{

}

void ModelManager::AddModel(Model* model)
{
	_Assert(NULL != model);

	modelList.push_back(model);
}

void ModelManager::Destroy()
{
	for(std::list<Model*>::iterator iter = modelList.begin(); iter != modelList.end(); ++iter)
	{
		SAFE_DROP(*iter);		
	}

	modelList.clear();
}

std::list<Model*> ModelManager::GetModelList()
{
	return modelList;
}

void ModelManager::Init()
{
	defaultCube = new Model(L"cube", L"Assets/Models/defaultCube.model");		// �ᱻ���뵽modeList, ���Բ���Ҫר��delete
}

Model* ModelManager::GetDefaultCube()
{
	_Assert(NULL != defaultCube);
	return defaultCube;
}
