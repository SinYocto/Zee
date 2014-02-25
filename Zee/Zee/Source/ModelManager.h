#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include "Model.h"

class ModelManager
{
public:
	ModelManager();

	void Init();
	void Destroy();

	void AddModel(Model* model);

	std::list<Model*> GetModelList();
	Model* GetDefaultCube();

private:
	std::list<Model*> modelList;
	Model* defaultCube;
};


#endif