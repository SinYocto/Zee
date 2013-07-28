#include "Model.h"
#include "Camera.h"

void Model::LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format)
{

}

void Model::Draw(Camera* camera)
{
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		(*iter)->Draw(camera);
	}
}