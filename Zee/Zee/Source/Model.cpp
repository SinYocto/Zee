#include "Model.h"
#include "Camera.h"
#include "DebugDrawer.h"

void Model::LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format)
{

}

void Model::Draw(Camera* camera)
{
	if(mAttribute.drawBBox && mAABBox.isValid())
	{
		DebugDrawer::DrawAABBox(mAABBox, 0xffff0000, camera);
	}
}

void Model::calCurrentAABBox()
{
	mAABBox = AABBox::Invalid;

	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		Mesh* mesh = *iter;
		mAABBox = AABBox::CombineBBox(mAABBox, mesh->GetAABBox());
	}
}
