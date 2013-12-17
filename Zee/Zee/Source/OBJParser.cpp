#include "OBJParser.h"
#include "Model.h"
#include "Engine.h"
#include "SceneManager.h"

DWORD OBJParser::mDataContentType = 0;

std::vector<Vector3> OBJParser::mPosData;
std::vector<Vector2> OBJParser::uvData;
std::vector<Vector3> OBJParser::normalData;

std::vector<Geometry*> OBJParser::mGeoList;
std::vector<Material*> OBJParser::mMtlList;

Model* OBJParser::mResultModel = NULL;

wchar_t OBJParser::mObjName[MAX_STR_LEN];
wchar_t OBJParser::mCurSpecifierStr[MAX_STR_LEN];
OBJParser::OBJ_SPECIFIER OBJParser::mCurSpecifier = OBJParser::OBJ_OTEHR;

void OBJParser::clear()
{
	mPosData.clear();
	uvData.clear();
	normalData.clear();

	mGeoList.clear();
	mMtlList.clear();
	mResultModel = NULL;

	YString::Empty(mCurSpecifierStr);
}

bool OBJParser::Parse(const wchar_t* filePath, Model** result)
{
	bool isSucceed = false;

	{
		Assert(NULL != filePath);
		Assert(NULL != result);
		Assert(*result == NULL);

		YString::GetFileName(mObjName, _countof(mObjName), filePath, false);

		PerformanceTimer::Begin(L"parsing OBJ file");

		clear();

		YFile* file = YFile::Open(filePath, YFile::READ);
		Assert(NULL != file);

		wchar_t lineContent[MAX_STR_LEN];
		while(file->ReadLine(lineContent, MAX_STR_LEN) != NULL)
		{
			if(parseLine(file, lineContent))
				break;
		}

		file->Close();



		PerformanceTimer::End();

		PerformanceTimer::Begin(L"building result model geometry");

		// 为获取得到的geo数据计算tbn并创建vb,ib
		for(size_t i = 0; i < mGeoList.size(); ++i)
		{
			Geometry* geo = mGeoList[i];

			if(((mDataContentType & UV_DATA) == 0))
			{
				geo->CalculateNormals();
				geo->BuildGeometry(XYZ_N);
			}
			else
			{
				geo->CalculateTBN();
				geo->BuildGeometry(XYZ_UV_TBN);
			}

			geo->SaveToFile(L"Assets/Geometries");
			gEngine->GetGeometryManager()->AddGeometry(geo);
		}

		PerformanceTimer::End();

		*result = mResultModel;
		clear();
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool OBJParser::parseLine(YFile* file, const wchar_t* lineContent)
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
			wchar_t lineStrs[3][MAX_STR_LEN];
			YString::Scan(lineContent, L"%*c %s %s %s", &lineStrs[0], &lineStrs[1], &lineStrs[2]);
			mPosData.push_back(Vector3(_wtof(lineStrs[0]), _wtof(lineStrs[1]), _wtof(lineStrs[2])));
			break;
		}
	case VERT_UV:
		{
			wchar_t lineStrs[2][MAX_STR_LEN];
			YString::Scan(lineContent, L"%*c %s %s", &lineStrs[0], &lineStrs[1]);
			uvData.push_back(Vector2(_wtof(lineStrs[0]), _wtof(lineStrs[1])));
			break;
		}
	case VERT_NORMAL: 
		{
			wchar_t lineStrs[3][MAX_STR_LEN];
			YString::Scan(lineContent, L"%*c %s %s %s", &lineStrs[0], &lineStrs[1], &lineStrs[2]);
			normalData.push_back(Vector3(_wtof(lineStrs[0]), _wtof(lineStrs[1]), _wtof(lineStrs[2])));
			break;
		}
	case GROUP:
		{
			break;
		}
	case MESH_MTL:
		{
			//return true;
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
	return false;
}

void OBJParser::parseTrianglesBlock(const std::vector<std::wstring>& blockContent)
{
	Geometry* geo = NULL;

	std::map<int, int> posIndexMap;		// obj文件中face的pos索引值对应geo中posdata的索引值
	std::map<int, int> uvIndexMap;
	std::map<int, int> normalIndexMap;
	std::map<Vector3, int, Vector3::Comparer> vertIndexMap;

	Assert((mDataContentType & POS_DATA) != 0);

	for(size_t i = 0; i < blockContent.size(); ++i)
	{
		parseTrianglesBlockLine(blockContent[i].c_str(), &geo, posIndexMap, uvIndexMap, normalIndexMap, vertIndexMap);
	}

Exit:
	return;
}


void OBJParser::parseTrianglesBlockLine(const wchar_t* lineContent, Geometry** curGeo, std::map<int, int>& posIndexMap,
								std::map<int, int>& uvIndexMap, std::map<int, int>& normalIndexMap, 
								std::map<Vector3, int, Vector3::Comparer>& vertIndexMap)
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

				// 为使用mtlName材质的triangleList创建一个geo, 此mtl和geo共同构成一个subMesh
				Mesh* subMesh = NULL;
				{
					Assert((*curGeo) == NULL);

					wchar_t geoName[MAX_STR_LEN];
					wchar_t meshName[MAX_STR_LEN];
					YString::Format(geoName, L"%s_geo_%d", mObjName, mGeoList.size());
					YString::Format(meshName, L"%s_mesh_%d", mObjName, mGeoList.size());

					(*curGeo) = New Geometry(geoName);
					mGeoList.push_back((*curGeo));

					if(mResultModel == NULL)
					{
						mResultModel = New Model(mObjName, (*curGeo), material);
					}
					else
					{
						subMesh = New Mesh(L"mesh", (*curGeo), material);
						mResultModel->AddSubMesh(subMesh);
						SAFE_DROP(subMesh);
					}
				}

				break;
			}
		case FACE:
			{
				int posIndex[4] = { -1, -1, -1, -1 };
				int uvIndex[4] = { -1, -1, -1, -1 };
				int normalIndex[4] = { -1, -1, -1, -1 };

				if(((mDataContentType & UV_DATA) == 0) && ((mDataContentType & NORMAL_DATA) == 0))
				{
					YString::Scan(lineContent, L"%*c %d %d %d %d", 
						&posIndex[0], &posIndex[1], &posIndex[2], &posIndex[3]);
				}
				else if(((mDataContentType & UV_DATA) == 0) && ((mDataContentType & NORMAL_DATA) != 0))
				{
					YString::Scan(lineContent, L"%*c %d//%d %d//%d %d//%d %d//%d",
						&posIndex[0], &normalIndex[0],
						&posIndex[1], &normalIndex[1],
						&posIndex[2], &normalIndex[2],
						&posIndex[3], &normalIndex[3]);
				}
				else if(((mDataContentType & UV_DATA) != 0) && ((mDataContentType & NORMAL_DATA) == 0))
				{
					YString::Scan(lineContent, L"%*c %d/%d %d/%d %d/%d %d/%d", 
						&posIndex[0], &uvIndex[0], 
						&posIndex[1], &uvIndex[1], 
						&posIndex[2], &uvIndex[2], 
						&posIndex[3], &uvIndex[3]);
				}
				else if(((mDataContentType & UV_DATA) != 0) && ((mDataContentType & NORMAL_DATA) != 0))
				{
					YString::Scan(lineContent, L"%*c %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",		// TODO:face可能是三角面, 也可能是四边面
						&posIndex[0], &uvIndex[0], &normalIndex[0], 
						&posIndex[1], &uvIndex[1], &normalIndex[1], 
						&posIndex[2], &uvIndex[2], &normalIndex[2], 
						&posIndex[3], &uvIndex[3], &normalIndex[3]);
				}

				int vertIndex[4] = { -1, -1, -1, -1 };
				Vector3 vert[4];
				vert[0] = Vector3(posIndex[0] - 1, uvIndex[0] - 1, normalIndex[0] - 1);
				vert[1] = Vector3(posIndex[1] - 1, uvIndex[1] - 1, normalIndex[1] - 1);
				vert[2] = Vector3(posIndex[2] - 1, uvIndex[2] - 1, normalIndex[2] - 1);
				vert[3] = Vector3(posIndex[3] - 1, uvIndex[3] - 1, normalIndex[3] - 1);

				for(int i = 0; i < 4; ++i)
				{			
					if(i == 3 && posIndex[i] == -1)		// 三角面
						break;

					// OBJ文件是从1开始的, 减1变成从0开始
					Assert((posIndex[i] -= 1) >= 0);

					int curGeoPosIndex = -1;
					int curGeoUVIndex = -1;
					int curGeoNormalIndex = -1;
					//int curVertIndex = -1;
					int& curVertIndex = vertIndex[i];

					bool posIndexExist = posIndexMap.find(posIndex[i]) != posIndexMap.end();

					if(posIndexExist)
					{
						curGeoPosIndex = posIndexMap[posIndex[i]];
					}
					else
					{
						curGeoPosIndex = (*curGeo)->mGeoData.posData.size();
						(*curGeo)->mGeoData.posData.push_back(mPosData[posIndex[i]]);
						posIndexMap[posIndex[i]] = curGeoPosIndex;
					}

					if((mDataContentType & UV_DATA) != 0)
					{
						Assert((uvIndex[i] -= 1) >= 0);

						bool uvIndexExist = uvIndexMap.find(uvIndex[i]) != uvIndexMap.end();

						if(uvIndexExist)
						{
							curGeoUVIndex = uvIndexMap[uvIndex[i]];
						}
						else
						{
							curGeoUVIndex = (*curGeo)->mGeoData.uvData.size();
							(*curGeo)->mGeoData.uvData.push_back(uvData[uvIndex[i]]);
							uvIndexMap[uvIndex[i]] = curGeoUVIndex;
						}
					}

					if((mDataContentType & NORMAL_DATA) != 0)
					{
						Assert((normalIndex[i] -= 1) >= 0);

						bool normalIndexExist = uvIndexMap.find(uvIndex[i]) != uvIndexMap.end();

						if(normalIndexExist)
						{
							curGeoNormalIndex = normalIndexMap[normalIndex[i]];
						}
						else
						{
							curGeoNormalIndex = (*curGeo)->mGeoData.normalData.size();
							(*curGeo)->mGeoData.normalData.push_back(normalData[normalIndex[i]]);
							normalIndexMap[normalIndex[i]] = curGeoNormalIndex;
						}
					}

					bool isVertExist = vertIndexMap.find(vert[i]) != vertIndexMap.end();
					if(isVertExist)
					{
						curVertIndex = vertIndexMap[vert[i]];
					}
					else
					{					
						curVertIndex = (*curGeo)->mGeoData.verts.size();

						Vert newVert(curGeoPosIndex, curGeoUVIndex, curGeoNormalIndex);
						(*curGeo)->mGeoData.verts.push_back(newVert);
						vertIndexMap[vert[i]] = curVertIndex;
					}
				}

				Triangle tri1;
				tri1.vertexIndex[0] = vertIndex[0];
				tri1.vertexIndex[1] = vertIndex[1];
				tri1.vertexIndex[2] = vertIndex[2];
				(*curGeo)->mGeoData.tris.push_back(tri1);

				if(vertIndex[3] != -1)
				{
					Triangle tri2;
					tri2.vertexIndex[0] = vertIndex[0];
					tri2.vertexIndex[1] = vertIndex[2];
					tri2.vertexIndex[2] = vertIndex[3];
					(*curGeo)->mGeoData.tris.push_back(tri2);
				}

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
		material->mShader->SetNormalTex(materialContent.bumpTexFilePath);
	}

	material->mShader->SetAmbientColor(materialContent.ambientColor);
	material->mShader->SetDiffuseColor(materialContent.diffuseColor);
	material->mShader->SetSpecularColor(materialContent.specColor);
	material->mShader->SetSpecGloss(materialContent.gloss); 

	if(!YString::isEmpty(materialContent.diffuseTexFilePath))		// TODO:考虑没有diffuse贴图的情况
		material->mShader->SetColorTex(materialContent.diffuseTexFilePath);

	material->SaveToFile(L"Assets/Materials");
	gEngine->GetMaterialManager()->AddMaterial(material);

	mMtlList.push_back(material);
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

			*curMaterial = New Material(mtlName);
			//gEngine->GetMaterialManager()->AddMaterial(*curMaterial);			// TODO:改到Material的构造函数中	
			//SAFE_DROP(*curMaterial);
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

	for(size_t i = 0; i < mMtlList.size(); ++i)
	{
		Material* curMaterial = mMtlList[i];

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

	if(YString::Compare(lineSpecifier, mCurSpecifierStr) == 0)
	{
		*specifier = mCurSpecifier;
		return true;
	}

	if(YString::Compare(lineSpecifier, L"#") == 0)
	{
		*specifier = OBJ_COMMENT;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"#");
	}
	else if(YString::Compare(lineSpecifier, L"mtllib") == 0)
	{
		*specifier = MTLLIB;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"mtllib");
	}
	else if(YString::Compare(lineSpecifier, L"v") == 0)
	{
		*specifier = VERT_POS;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"v");
	}
	else if(YString::Compare(lineSpecifier, L"vt") == 0)
	{
		*specifier = VERT_UV;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"vt");
	}
	else if(YString::Compare(lineSpecifier, L"vn") == 0)
	{
		*specifier = VERT_NORMAL;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"vn");
	}
	else if(YString::Compare(lineSpecifier, L"g") == 0)
	{
		*specifier = GROUP;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"g");
	}
	else if(YString::Compare(lineSpecifier, L"usemtl") == 0)
	{
		*specifier = MESH_MTL;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"usemtl");
	}
	else if(YString::Compare(lineSpecifier, L"f") == 0)
	{
		*specifier = FACE;
		mCurSpecifier = *specifier;
		YString::Copy(mCurSpecifierStr, _countof(mCurSpecifierStr), L"f");
	}
	else
	{
		*specifier = OBJ_OTEHR;
		mCurSpecifier = *specifier;
		YString::Empty(mCurSpecifierStr);
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
	if(!(mPosData.empty()))
		mDataContentType |= POS_DATA;

	if(!(uvData.empty()))
		mDataContentType |= UV_DATA;

	if(!(normalData.empty()))
		mDataContentType |= NORMAL_DATA;
}
