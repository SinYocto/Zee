#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include "Model.h"

class ModelManager
{
public:
	ModelManager();

	void Destroy();

	void AddModel(Model* model);

	std::list<Model*> GetModelList();

private:
	std::list<Model*> modelList;
};


#endif