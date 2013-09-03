#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "SceneNode.h"
#include "Model.h"

class ModelNode : public SceneNode
{
public:
	ModelNode(const wchar_t* name, SceneNode* parent, Model* model)
		:SceneNode(name, parent)
	{
		mType = SCENE_NODE_MODEL;
		mModel = model;
		mModel->Grab();
	}

	ModelNode(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material)
		:SceneNode(name, parent)
	{
		mType = SCENE_NODE_MODEL;
		mModel = new Model(geo, material);
	}

	~ModelNode()
	{
		SAFE_DROP(mModel);
	}

	virtual void Draw(Camera* camera);

private:
	virtual void calCurrentAABBox();

private:
	Model* mModel;
};

#endif