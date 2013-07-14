#ifndef MODEL_H
#define MODEL_H

#include "SubModel.h"
#include <list>

// #设计
// 每个Model对象包含一个SubModel列表, 同时Model和SubModel都是Object对象, 在Object层级上每个SubModel都是Model的子对象
// 增加一个SubModel要加入到SubModel列表还要在Object层级设置为Model的子对象

// #资源管理
// Model会new出若干SubModel对象, 析构时调用SubModel的delete即可

enum ModelFileFormat
{
	MODEL_OBJ
};

class Camera;

class Model : public Object
{
public:
	// 此构造函数使用一个mesh和material对象来构造一个包含一个SubModel的Model对象
	Model()
	{

	}

	Model(Object* _parent, Mesh* _mesh, Material* _material)
	{
		SubModel* subModel = new SubModel(this, _mesh, _material);
		AddSubModel(subModel);
	}

	~Model()
	{
		for(std::list<SubModel*>::iterator iter = subModels.begin(); iter != subModels.end(); ++iter)
		{
			SAFE_DELETE(*iter);
		}
	}
	void AddSubModel(SubModel* subModel)
	{
		_Assert(NULL != subModel);

		if(std::find(subModels.begin(), subModels.end(), subModel) != subModels.end())
		{
			// subModels中已存在此对象指针
			return;
		}
		else
		{
			subModel->SetParent(this);
			subModels.push_back(subModel);
		}
	}

	void LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format);

	void Draw(Camera* camera);



private:
	std::list<SubModel*> subModels;
};


#endif