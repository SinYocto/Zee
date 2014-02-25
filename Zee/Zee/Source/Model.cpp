#include "Model.h"
#include "Camera.h"
#include "DebugDrawer.h"
#include "Engine.h"

Model::Model(const wchar_t* name, Geometry* geo, Material* material)
{
	YString::Copy(mName, _countof(mName), name);

	Mesh* mesh = New Mesh(name, geo, material);
	AddSubMesh(mesh);
	SAFE_DROP(mesh);

	gEngine->GetModelManager()->AddModel(this);
}

Model::Model(const wchar_t* name, const wchar_t* filePath)
{
	YString::Copy(mName, _countof(mName), name);
	buildFromFile(filePath);

	gEngine->GetModelManager()->AddModel(this);
}

Model::Model(const wchar_t* name)
{
	YString::Copy(mName, _countof(mName), name);

	gEngine->GetModelManager()->AddModel(this);
}

Model::Model(const wchar_t* name, Model* model)
{
	YString::Copy(mName, _countof(mName), name);

	std::list<Mesh*> meshList = model->GetSubMeshList();
	for(std::list<Mesh*>::iterator iter = meshList.begin(); iter != meshList.end(); ++iter)
	{
		Mesh* mesh = new Mesh(L"mesh", *iter);

		AddSubMesh(mesh);
		SAFE_DROP(mesh);
	}

	gEngine->GetModelManager()->AddModel(this);
}

Model::~Model()
{
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		SAFE_DROP(*iter);
	}
}

void Model::LoadModelDataFromFile(wchar_t* filename, ModelFileFormat format)
{

}

void Model::AddSubMesh(Mesh* mesh)
{
	_Assert(NULL != mesh);

	if(std::find(mSubMeshes.begin(), mSubMeshes.end(), mesh) != mSubMeshes.end())
	{
		return;
	}
	else
	{
		mesh->Grab();
		mSubMeshes.push_back(mesh);
	}
}

void Model::Draw(const D3DXMATRIX& matWorld, Camera* camera, bool isSolid)
{
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		(*iter)->Draw(matWorld, camera, isSolid);
	}
}

void Model::CalcDynamicAABBox(const D3DXMATRIX& matWorld, AABBox* box)
{
	_Assert(NULL != box);
	AABBox resultBox = AABBox::Invalid;

	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		Mesh* mesh = *iter;
		AABBox subMeshBBox = AABBox::Invalid;

		mesh->GetGeometry()->CalcDynamicAABBox(matWorld, &subMeshBBox);
		resultBox = AABBox::CombineBBox(resultBox, subMeshBBox);
	}

	*box = resultBox;
}

std::list<Mesh*> Model::GetSubMeshList()
{
	return mSubMeshes;
}

wchar_t* Model::GetName()
{
	return mName;
}

void Model::DrawUseMtl(const D3DXMATRIX& matWorld, Camera* camera, Material* mtl)
{
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter)
	{
		(*iter)->DrawUseMtl(matWorld, camera, mtl);
	}
}

void Model::SaveToFile(const wchar_t* dirPath)
{
	wchar_t filePath[MAX_PATH_LEN];
	YString::Copy(filePath, _countof(filePath), dirPath);
	YString::Concat(filePath, _countof(filePath), L"/");
	YString::Concat(filePath, _countof(filePath), mName, L".model");

	if(YFile::Exist(filePath))
	{
		Log(L"warning: file already exists. Model::SaveToFile(%s)\n", filePath);
		return;
	}

	// TODO: dirPath²»´æÔÚ

	YFile* file = YFile::Open(filePath, YFile::WRITE);

	file->WriteLine(L"numSubMeshes %d", mSubMeshes.size());

	int i = 0;
	for(std::list<Mesh*>::iterator iter = mSubMeshes.begin(); iter != mSubMeshes.end(); ++iter, ++i)
	{
		Mesh* mesh = *iter;

		wchar_t geoFilePath[MAX_PATH_LEN];
		wchar_t mtlFilePath[MAX_PATH_LEN];
		YString::Copy(geoFilePath, _countof(geoFilePath), mesh->GetGeometry()->GetFilePath());
		YString::Copy(mtlFilePath, _countof(mtlFilePath), mesh->GetMaterial()->GetFilePath());
		_Assert(!YString::isEmpty(geoFilePath));
		_Assert(!YString::isEmpty(mtlFilePath));

		file->WriteLine(L"mesh%d %s", i, mesh->GetName());
		file->WriteLine(L"geo%d %s", i, geoFilePath);
		file->WriteLine(L"mtl%d %s", i, mtlFilePath);
	}

	file->Close();
}

void Model::buildFromFile(const wchar_t* filePath)
{
	_Assert(mSubMeshes.size() == 0);

	wchar_t suffix[MAX_STR_LEN];
	YString::GetFileSuffix(suffix, _countof(suffix), filePath);
	_Assert(YString::Compare(suffix, L"model", true) == 0);

	YFile* file = YFile::Open(filePath, YFile::READ);

	wchar_t lineContent[MAX_STR_LEN];
	_Assert(file->ReadLine(lineContent, MAX_STR_LEN));

	int numSubMeshes = 0;
	YString::Scan(lineContent, L"%*s %d", &numSubMeshes);

	for(int i = 0; i < numSubMeshes; ++i)
	{
		wchar_t subMeshName[MAX_STR_LEN];
		wchar_t geoFilePath[MAX_PATH_LEN];
		wchar_t mtlFilePath[MAX_PATH_LEN];

		_Assert(file->ReadLine(lineContent, MAX_STR_LEN));
		YString::Scan(lineContent, L"%*s %s", &subMeshName);

		_Assert(file->ReadLine(lineContent, MAX_STR_LEN));
		YString::Scan(lineContent, L"%*s %s", &geoFilePath);

		_Assert(file->ReadLine(lineContent, MAX_STR_LEN));
		YString::Scan(lineContent, L"%*s %s", &mtlFilePath);

		GeometryManager* geoMgr = gEngine->GetGeometryManager();
		MaterialManager* mtlMgr = gEngine->GetMaterialManager();

		Mesh* mesh = New Mesh(subMeshName, geoMgr->GetOrCreateGeometry(geoFilePath), mtlMgr->GetOrCreateMaterial(mtlFilePath));
		AddSubMesh(mesh);
		SAFE_DROP(mesh);
	}

	file->Close();
}
