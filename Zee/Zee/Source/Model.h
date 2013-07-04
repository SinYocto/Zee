#ifndef MODEL_H
#define MODEL_H

#include "SubModel.h"
#include <list>

// #���
// ÿ��Model�������һ��SubModel�б�, ͬʱModel��SubModel����Object����, ��Object�㼶��ÿ��SubModel����Model���Ӷ���
// ����һ��SubModelҪ���뵽SubModel�б�Ҫ��Object�㼶����ΪModel���Ӷ���

// #��Դ����
// Model��new������SubModel����, ����ʱ����SubModel��delete����

enum ModelFileFormat
{
	MODEL_OBJ
};

class Camera;

class Model : public Object
{
public:
	// �˹��캯��ʹ��һ��mesh��material����������һ������һ��SubModel��Model����
	Model(Object* _parent = NULL, Mesh* _mesh = NULL, Material* _material = NULL)
	{
		SubModel* subModel = new SubModel(this, _mesh, _material);
		addSubModel(subModel);
	}

	~Model()
	{
		for(std::list<SubModel*>::iterator iter = subModels.begin(); iter != subModels.end(); ++iter)
		{
			SAFE_DELETE(*iter);
		}
	}

	void LoadModelDataFromFile(char* filename, ModelFileFormat format);

	void Draw(Camera* camera);

private:
	void addSubModel(SubModel* subModel)
	{
		_Assert(NULL != subModel);

		if(std::find(subModels.begin(), subModels.end(), subModel) != subModels.end())
		{
			// subModels���Ѵ��ڴ˶���ָ��
			return;
		}
		else
		{
			subModel->SetParent(this);
			subModels.push_back(subModel);
		}
	}

private:
	std::list<SubModel*> subModels;
};


#endif