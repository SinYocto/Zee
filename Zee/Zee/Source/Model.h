#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"

// #设计
// 每个Model对象包含一个mesh列表, 同时Model和mesh都是Object对象, 在Object层级上每个mesh都是Model的子对象
// 增加一个mesh要加入到mesh列表还要在Object层级设置为Model的子对象

// #资源管理
// Model会new出若干mesh对象, 析构时调用mesh的delete即可

enum ModelFileFormat
{
	MODEL_OBJ
};

class Camera;

class Model : public SceneNode
{
public:
	Model(const wchar_t* name, SceneNode* parent)
		:SceneNode(name, parent)
	{
		
	}

	// 此构造函数使用一个geo和material对象来构造一个包含一个SubMesh的Model对象
	Model(const wchar_t* name, SceneNode* parent, Geometry* geo, Material* material)
		:SceneNode(name, parent)
	{
		Mesh* mesh = new Mesh(L"mesh", this, geo, material);
		AddSubMesh(mesh);
	}

	~Model()
	{
		for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
		{
			SAFE_DELETE(*iter);
		}
	}

	void AddSubMesh(Mesh* mesh)
	{
		_Assert(NULL != mesh);

		if(std::find(mSubMeshes.begin(), mSubMeshes.end(), mesh) != mSubMeshes.end())
		{
			// subMeshes中已存在此对象指针
			return;
		}
		else
		{
			mesh->SetParent(this);
			mSubMeshes.push_back(mesh);

			mAABBox = AABBox::CombineBBox(mAABBox, mesh->GetAABBox());
		}
	}

	void LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format);

	virtual void Draw(Camera* camera);

private:
	virtual void calCurrentAABBox();

private:
	std::list<Mesh*> mSubMeshes;
};


#endif