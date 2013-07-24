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
	Model(Object* _parent, Geometry* _geo, Material* _material)
	{
		Mesh* mesh = new Mesh(this, _geo, _material);
		AddSubMesh(mesh);
	}

	~Model()
	{
		for(std::list<Mesh*>::iterator iter = subMeshes.begin(); iter != subMeshes.end(); ++iter)
		{
			SAFE_DELETE(*iter);
		}
	}
	void AddSubMesh(Mesh* mesh)
	{
		_Assert(NULL != mesh);

		if(std::find(subMeshes.begin(), subMeshes.end(), mesh) != subMeshes.end())
		{
			// subMeshes���Ѵ��ڴ˶���ָ��
			return;
		}
		else
		{
			mesh->SetParent(this);
			subMeshes.push_back(mesh);
		}
	}

	void LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format);

	void Draw(Camera* camera);



private:
	std::list<Mesh*> subMeshes;
};


#endif