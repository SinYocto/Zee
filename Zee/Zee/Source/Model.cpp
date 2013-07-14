#include "Model.h"
#include "Camera.h"

void Model::LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format)
{

}

void Model::Draw(Camera* camera)
{
	for(std::list<SubModel*>::iterator iter = subModels.begin(); iter != subModels.end(); ++iter)
	{
		(*iter)->Draw(camera);
	}
}