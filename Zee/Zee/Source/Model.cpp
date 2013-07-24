#include "Model.h"
#include "Camera.h"

void Model::LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format)
{

}

void Model::Draw(Camera* camera)
{
	for(std::list<Mesh*>::iterator iter = subMeshes.begin(); iter != subMeshes.end(); ++iter)
	{
		(*iter)->Draw(camera);
	}
}