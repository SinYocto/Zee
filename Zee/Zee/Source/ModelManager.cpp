#include "ModelManager.h"
#include "Engine.h"

ModelManager::ModelManager()
{

}

void ModelManager::AddModel(Model* model)
{
	_Assert(NULL != model);

	modelList.push_back(model);
	model->Grab();
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
