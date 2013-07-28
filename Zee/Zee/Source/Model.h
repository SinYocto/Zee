#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include <list>

// #���
// ÿ��Model�������һ��mesh�б�, ͬʱModel��mesh����Object����, ��Object�㼶��ÿ��mesh����Model���Ӷ���
// ����һ��meshҪ���뵽mesh�б�Ҫ��Object�㼶����ΪModel���Ӷ���

// #��Դ����
// Model��new������mesh����, ����ʱ����mesh��delete����

enum ModelFileFormat
{
	MODEL_OBJ
};

class Camera;

class Model : public Object
{
public:
	Model()
	{

	}

	// �˹��캯��ʹ��һ��geo��material����������һ������һ��SubMesh��Model����
	Model(Object* parent, Geometry* geo, Material* material)
	{
		Mesh* mesh = new Mesh(this, geo, material);
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
			// subMeshes���Ѵ��ڴ˶���ָ��
			return;
		}
		else
		{
			mesh->SetParent(this);
			mSubMeshes.push_back(mesh);
		}
	}

	void LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format);

	void Draw(Camera* camera);



private:
	std::list<Mesh*> mSubMeshes;
};


#endif