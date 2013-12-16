#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include "SceneNode.h"
#include "Model.h"

class ModelNode : public SceneNode
{
public:
	ModelNode(const wchar_t* name, SceneNode* parent, Model* model);
	ModelNode(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material);

	~ModelNode()
	{
		SAFE_DROP(mModel);
	}

	virtual void Draw(Camera* camera);

	Model* GetModel();

private:
	//virtual void updateAABBox();

private:
	Model* mModel;
};

#endif