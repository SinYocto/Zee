#include "OBJParser.h"
#include "Model.h"
#include "YFile.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include <stdio.h>
#include <string.h>
#include <set>
#include "Time.h"

DWORD OBJParser::dataContentType = 0;

std::vector<Vector3> OBJParser::positionData;
std::vector<Vector2> OBJParser::uvData;
std::vector<Vector3> OBJParser::normalData;

std::vector<Mesh*> OBJParser::meshList;
std::vector<Material*> OBJParser::materialList;

Model* OBJParser::resultModel = NULL;

void OBJParser::clear()
{
	positionData.clear();
	uvData.clear();
	normalData.clear();

	meshList.clear();
	materialList.clear();
	resultModel = NULL;
}

bool OBJParser::Parse(const wchar_t* filePath, Model** result)
{
	bool isSucceed = false;

	{
		Assert(NULL != filePath);
		Assert(NULL != result);
		Assert(*result == NULL);

		PerformanceTimer::Begin(L"parsing OBJ file");

		clear();

		YFile* file = YFile::Open(filePath, YFile::READ);
		Assert(NULL != file);

		wchar_t lineContent[MAX_STR_LEN];
		while(file->ReadLine(lineContent, MAX_STR_LEN) != NULL)
			parseLine(file, lineContent);

		file->Close();

		PerformanceTimer::End();

		PerformanceTimer::Begin(L"building result model geometry");

		// 为获取得到的mesh数据计算tbn并创建vb,ib
		for(size_t i = 0; i < meshList.size(); ++i)
		{
			Mesh* mesh = meshList[i];

			if(((dataContentType & UV_DATA) == 0))
			{
				mesh->CalculateNormals();
				mesh->BuildGeometry(XYZ_N);
			}
			else
			{
				mesh->CalculateTBN();
				mesh->BuildGeometry(XYZ_UV_TBN);
			}
		}

		PerformanceTimer::End();

		*result = resultModel;
		clear();
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

void OBJParser::parseLine(YFile* file, const wchar_t* lineContent)
{
	Assert(NULL != file);
	Assert(NULL != lineContent);

	OBJ_SPECIFIER specifier = OBJ_OTEHR;
	getOBJSpecifier(lineContent, &specifier);

	switch(specifier)
	{
	case OBJ_COMMENT:
		break;
	case MTLLIB:
		{
			wchar_t mtlFilePath[MAX_STR_LEN];
			YString::Scan(lineContent, L"%*s %s", mtlFilePath);

			YString::GetFullPath(mtlFilePath, _countof(mtlFilePath));		// TODO:改成相对obj文件的路径
			parseMtl(mtlFilePath);

			break;
		}
	case VERT_POS:
		{
			Vector3 pos;
			YString::Scan(lineContent, L"%*c %f %f %f", &pos.x, &pos.y, &pos.z);

			positionData.push_back(pos);
			break;
		}
	case VERT_UV:
		{
			Vector2 uv;
			YString::Scan(lineContent, L"%*s %f %f", &uv.x, &uv.y);

			uvData.push_back(uv);
			break;
		}
	case VERT_NORMAL:
		{
			Vector3 normal;
			YString::Scan(lineContent, L"%*s %f %f %f", &normal.x, &normal.y, &normal.z);

			normalData.push_back(normal);
			break;
		}
	case GROUP:
		{
			break;
		}
	case MESH_MTL:
		{
			determineDataContentType();

			std::vector<std::wstring> blockContent;
			if(file->ReadBlock(&blockContent, L"usemtl", NULL, lineContent) != 0)
				parseTrianglesBlock(blockContent);

			while(file->ReadBlock(&blockContent, L"usemtl") != 0)
			{
				parseTrianglesBlock(blockContent);
			}

			break;
		}
	case OBJ_OTEHR:
		{
			break;
		}
	}

Exit:
	return;
}

void OBJParser::parseTrianglesBlock(const std::vector<std::wstring>& blockContent)
{
	Mesh* mesh = NULL;

	std::map<int, int> posIndexMap;
	std::map<int, int> uvIndexMap;
	std::map<int, int> normalIndexMap;

	//typedef void (*ParseFunc)(const wchar_t*, Mesh**, std::map<int, int>&, std::map<int, int>&, std::map<int, int>&);
	//ParseFunc parseFunc = NULL;

	Assert((dataContentType & POS_DATA) != 0);

	//if(((dataContentType & UV_DATA) == 0) && ((dataContentType & NORMAL_DATA) == 0))
	//{
	//	parseFunc = parseTrianglesBlockLinePos;
	//}
	//else if(((dataContentType & UV_DATA) == 0) && ((dataContentType & NORMAL_DATA) != 0))
	//{
	//	parseFunc = parseTrianglesBlockLinePosNormal;
	//}
	//else if(((dataContentType & UV_DATA) != 0) && ((dataContentType & NORMAL_DATA) == 0))
	//{
	//	parseFunc = parseTrianglesBlockLinePosUV;
	//}
	//else if(((dataContentType & UV_DATA) != 0) && ((dataContentType & NORMAL_DATA) != 0))
	//{
	//	parseFunc = parseTrianglesBlockLinePosUVNormal;
	//}

	//Assert(NULL != parseFunc);
	for(size_t i = 0; i < blockContent.size(); ++i)
	{
		parseTrianglesBlockLine(blockContent[i].c_str(), &mesh, posIndexMap, uvIndexMap, normalIndexMap);
	}

Exit:
	return;
}


void OBJParser::parseTrianglesBlockLine(const wchar_t* lineContent, Mesh** curMesh, std::map<int, int>& posIndexMap,
								std::map<int, int>& uvIndexMap, std::map<int, int>& normalIndexMap)
{
	Assert(NULL != lineContent);
	{
		OBJ_SPECIFIER specifier = OBJ_OTEHR;
		getOBJSpecifier(lineContent, &specifier);

		switch(specifier)
		{
		case MESH_MTL:
			{
				Material* material = NULL;
				{
					wchar_t mtlName[MAX_STR_LEN];
					YString::Scan(lineContent, L"%*s %s", mtlName);

					getMaterial(mtlName, &material);
					Assert(NULL != material);
				}

				// 为使用mtlName材质的triangleList创建一个mesh, 此mtl和mesh共同构成一个subModel
				SubModel* subModel = NULL;
				{
					Assert((*curMesh) == NULL);
					(*curMesh) = new Mesh(L"mesh");				// TODO:给个按序号增加的默认名?
					MeshManager::AddMesh((*curMesh));

					meshList.push_back((*curMesh));

					if(resultModel == NULL)
						resultModel = new Model();
					Assert(NULL != resultModel);

					subModel = new SubModel(NULL, (*curMesh), material);
					resultModel->AddSubModel(subModel);
				}

				break;
			}
		case FACE:
			{
				int posIndex[3] = { -1, -1, -1 };
				int uvIndex[3] = { -1, -1, -1 };
				int normalIndex[3] = { -1, -1, -1 };

				if(((dataContentType & UV_DATA) == 0) && ((dataContentType & NORMAL_DATA) == 0))
				{
					YString::Scan(lineContent, L"%*c %d %d %d", 
						&posIndex[0], &posIndex[1], &posIndex[2]);
				}
				else if(((dataContentType & UV_DATA) == 0) && ((dataContentType & NORMAL_DATA) != 0))
				{
					YString::Scan(lineContent, L"%*c %d//%d %d//%d %d//%d",
						&posIndex[0], &normalIndex[0],
						&posIndex[1], &normalIndex[1],
						&posIndex[2], &normalIndex[2]);
				}
				else if(((dataContentType & UV_DATA) != 0) && ((dataContentType & NORMAL_DATA) == 0))
				{
					YString::Scan(lineContent, L"%*c %d/%d %d/%d %d/%d", 
						&posIndex[0], &uvIndex[0], 
						&posIndex[1], &uvIndex[1], 
						&posIndex[2], &uvIndex[2]);
				}
				else if(((dataContentType & UV_DATA) != 0) && ((dataContentType & NORMAL_DATA) != 0))
				{
					YString::Scan(lineContent, L"%*c %d/%d/%d %d/%d/%d %d/%d/%d",		// TODO:face可能是三角面, 也可能是四边面
						&posIndex[0], &uvIndex[0], &normalIndex[0], 
						&posIndex[1], &uvIndex[1], &normalIndex[1], 
						&posIndex[2], &uvIndex[2], &normalIndex[2]);
				}

				Mesh::Triangle tri;
				for(int i = 0; i < 3; ++i)
				{			
					// OBJ文件是从1开始的, 减1变成从0开始
					Assert((posIndex[i] -= 1) >= 0);

					int curMeshPosIndex = -1;
					int curMeshUVIndex = -1;
					int curMeshNormalIndex = -1;
					int curVertIndex = -1;

					bool posIndexExist = posIndexMap.find(posIndex[i]) != posIndexMap.end();

					if(posIndexExist)
					{
						curMeshPosIndex = posIndexMap[posIndex[i]];
					}
					else
					{
						curMeshPosIndex = (*curMesh)->positionData.size();
						(*curMesh)->positionData.push_back(positionData[posIndex[i]]);
						posIndexMap[posIndex[i]] = curMeshPosIndex;
					}

					if((dataContentType & UV_DATA) != 0)
					{
						Assert((uvIndex[i] -= 1) >= 0);

						bool uvIndexExist = uvIndexMap.find(uvIndex[i]) != uvIndexMap.end();

						if(uvIndexExist)
						{
							curMeshUVIndex = uvIndexMap[uvIndex[i]];
						}
						else
						{
							curMeshUVIndex = (*curMesh)->uvData.size();
							(*curMesh)->uvData.push_back(uvData[uvIndex[i]]);
							uvIndexMap[uvIndex[i]] = curMeshUVIndex;
						}
					}

					if((dataContentType & NORMAL_DATA) != 0)
					{
						Assert((normalIndex[i] -= 1) >= 0);

						bool normalIndexExist = uvIndexMap.find(uvIndex[i]) != uvIndexMap.end();

						if(normalIndexExist)
						{
							curMeshNormalIndex = normalIndexMap[normalIndex[i]];
						}
						else
						{
							curMeshNormalIndex = (*curMesh)->normalData.size();
							(*curMesh)->normalData.push_back(normalData[normalIndex[i]]);
							normalIndexMap[normalIndex[i]] = curMeshNormalIndex;
						}
					}

					// 查找tri的vert是否是已经加入到verts中的重复vert
					bool isVertExist = false;
					for(size_t k = 0; k < (*curMesh)->verts.size(); ++k)
					{
						Mesh::Vert& vert = (*curMesh)->verts[k];

						if(vert.posIndex != curMeshPosIndex)
							continue;

						if((dataContentType & UV_DATA) != 0 && vert.uvIndex != curMeshUVIndex)
							continue;

						if((dataContentType & NORMAL_DATA) != 0 && vert.normalIndex != curMeshNormalIndex)
							continue;

						isVertExist = true;
						curVertIndex = k;
						break;
					}

					if(!isVertExist)
					{
						curVertIndex = (*curMesh)->verts.size();

						Mesh::Vert vert(curMeshPosIndex, curMeshUVIndex, curMeshNormalIndex);
						(*curMesh)->verts.push_back(vert);
					}

					tri.vertexIndex[i] = curVertIndex;
				}

				(*curMesh)->triangleList.push_back(tri);

				break;
			}
		case OBJ_OTEHR:
			{
				break;
			}
		}
	}

Exit:
	return;
}

void OBJParser::parseMtl(const wchar_t* mtlFilePath)
{
	YFile* file = YFile::Open(mtlFilePath, YFile::READ);
	Assert(NULL != file);

	{
		std::vector<std::wstring> blockContent;
		while(file->ReadBlock(&blockContent, L"newmtl") != 0)
		{
			parseMtlBlock(blockContent);
		}

		file->Close();
	}

Exit:
	return;
}

void OBJParser::parseMtlBlock(const std::vector<std::wstring>& blockContent)
{
	Material* material = NULL;
	MaterialContent materialContent;

	for(size_t i = 0; i < blockContent.size(); ++i)
	{
		parseMtlLine(blockContent[i].c_str(), &material, &materialContent);
	} 

	if(YString::isEmpty(materialContent.bumpTexFilePath))		// 暂时obj文件中得到的material使用这两种shader
	{
		material->SetShader(Specular);
	}
	else
	{
		material->SetShader(BumpSpecular);
		material->shader->SetNormalTex(materialContent.bumpTexFilePath);
	}

	material->shader->SetAmbientColor(materialContent.ambientColor);
	material->shader->SetDiffuseColor(materialContent.diffuseColor);
	material->shader->SetSpecularColor(materialContent.specColor);
	material->shader->SetSpecGloss(materialContent.gloss); 

	if(!YString::isEmpty(materialContent.diffuseTexFilePath))		// TODO:考虑没有diffuse贴图的情况
		material->shader->SetColorTex(materialContent.diffuseTexFilePath);

	materialList.push_back(material);
}

void OBJParser::parseMtlLine(const wchar_t* lineContent, Material** curMaterial, MaterialContent* materialContent)
{
	Assert(NULL != lineContent);

	MTL_SPECIFIER specifier = MTL_OTHER;
	getMTLSpecifier(lineContent, &specifier);

	switch(specifier)
	{
	case MTL_COMMENT:
		{
			break;
		}
	case NEW_MTL:
		{
			wchar_t mtlName[MAX_STR_LEN];
			YString::Scan(lineContent, L"%*s %s", mtlName);

			Assert(*curMaterial == NULL);

			*curMaterial = new Material(mtlName);
			MaterialManager::AddMaterial(*curMaterial);			// TODO:改到Material的构造函数中	

			break;
		}
	case AMBIENT_COLOR:
		{
			D3DXCOLOR& ambientColor = materialContent->ambientColor;
			ambientColor.a = 1.0f;

			YString::Scan(lineContent, L"%*s %f %f %f", &ambientColor.r, &ambientColor.g, &ambientColor.b);
			break;
		}
	case DIFFUSE_COLOR:
		{
			D3DXCOLOR& diffuseColor = materialContent->diffuseColor;
			diffuseColor.a = 1.0f;

			YString::Scan(lineContent, L"%*s %f %f %f", &diffuseColor.r, &diffuseColor.g, &diffuseColor.b);
			break;
		}
	case SPEC_COLOR:
		{
			D3DXCOLOR& specColor = materialContent->specColor;
			specColor.a = 1.0f;

			YString::Scan(lineContent, L"%*s %f %f %f", &specColor.r, &specColor.g, &specColor.b);
			break;
		}
	case SPEC_GLOSS:
		{
			YString::Scan(lineContent, L"%*s %f", &materialContent->gloss);
			break;
		}
	case AMBIENT_TEX:
		{
			YString::Scan(lineContent, L"%*s %s", materialContent->ambientTexFilePath);		// TODO:如果map_Ka后面没内容, 会读到什么?
			break;
		}
	case DIFFUSE_TEX:
		{
			YString::Scan(lineContent, L"%*s %s", materialContent->diffuseTexFilePath);
			break;
		}
	case SPEC_TEX:
		{
			YString::Scan(lineContent, L"%*s %s", materialContent->specTexFilePath);
			break;
		}
	case BUMP_TEX:
		{
			YString::Scan(lineContent, L"%*s %s", materialContent->bumpTexFilePath);
			break;
		}
	case MTL_OTHER:
		{
			break;
		}
	}

Exit:
	return;
}

bool OBJParser::getMaterial(const wchar_t* mtlName, Material** material)
{
	bool isSucceed = false;

	Assert(NULL != material);
	*material = NULL;

	for(size_t i = 0; i < materialList.size(); ++i)
	{
		Material* curMaterial = materialList[i];

		if(YString::Compare(curMaterial->GetName(), mtlName) == 0)
			*material = curMaterial;
	}

	Assert(NULL != *material);

	isSucceed = true;
Exit:
	return isSucceed;
}

bool OBJParser::getOBJSpecifier(const wchar_t* lineContent, OBJ_SPECIFIER* specifier)
{
	bool isSucceed = false;
	Assert(NULL != lineContent);
	Assert(NULL != specifier);

	wchar_t lineSpecifier[MAX_STR_LEN];
	YString::GetSpecifier(lineSpecifier, _countof(lineSpecifier), lineContent);

	if(YString::Compare(lineSpecifier, L"#") == 0)
	{
		*specifier = OBJ_COMMENT;
	}
	else if(YString::Compare(lineSpecifier, L"mtllib") == 0)
	{
		*specifier = MTLLIB;
	}
	else if(YString::Compare(lineSpecifier, L"v") == 0)
	{
		*specifier = VERT_POS;
	}
	else if(YString::Compare(lineSpecifier, L"vt") == 0)
	{
		*specifier = VERT_UV;
	}
	else if(YString::Compare(lineSpecifier, L"vn") == 0)
	{
		*specifier = VERT_NORMAL;
	}
	else if(YString::Compare(lineSpecifier, L"g") == 0)
	{
		*specifier = GROUP;
	}
	else if(YString::Compare(lineSpecifier, L"usemtl") == 0)
	{
		*specifier = MESH_MTL;
	}
	else if(YString::Compare(lineSpecifier, L"f") == 0)
	{
		*specifier = FACE;
	}
	else
	{
		*specifier = OBJ_OTEHR;
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool OBJParser::getMTLSpecifier(const wchar_t* lineContent, MTL_SPECIFIER* specifier)
{
	bool isSucceed = false;
	Assert(NULL != lineContent);
	Assert(NULL != specifier);

	wchar_t lineSpecifier[MAX_STR_LEN];
	YString::GetSpecifier(lineSpecifier, _countof(lineSpecifier), lineContent);

	if(YString::Compare(lineSpecifier, L"#") == 0)
	{
		*specifier = MTL_COMMENT;
	}
	else if(YString::Compare(lineSpecifier, L"newmtl") == 0)
	{
		*specifier = NEW_MTL;
	}
	else if(YString::Compare(lineSpecifier, L"Ka") == 0)
	{
		*specifier = AMBIENT_COLOR;
	}
	else if(YString::Compare(lineSpecifier, L"Kd") == 0)
	{
		*specifier = DIFFUSE_COLOR;
	}
	else if(YString::Compare(lineSpecifier, L"Ks") == 0)
	{
		*specifier = SPEC_COLOR;
	}
	else if(YString::Compare(lineSpecifier, L"Ns") == 0)
	{
		*specifier = SPEC_GLOSS;
	}
	else if(YString::Compare(lineSpecifier, L"map_Ka") == 0)
	{
		*specifier = AMBIENT_TEX;
	}
	else if(YString::Compare(lineSpecifier, L"map_Kd") == 0)
	{
		*specifier = DIFFUSE_TEX;
	}
	else if(YString::Compare(lineSpecifier, L"map_Ks") == 0)
	{
		*specifier = SPEC_TEX;
	}
	else if(YString::Compare(lineSpecifier, L"map_Kb") == 0)
	{
		*specifier = BUMP_TEX;
	}
	else
	{
		*specifier = MTL_OTHER;
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

void OBJParser::determineDataContentType()
{
	if(!(positionData.empty()))
		dataContentType |= POS_DATA;

	if(!(uvData.empty()))
		dataContentType |= UV_DATA;

	if(!(normalData.empty()))
		dataContentType |= NORMAL_DATA;
}
