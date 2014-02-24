#include "Geometry.h"
#include "Bound.h"
#include "Engine.h"
#include "IDAllocator.h"

Geometry::Geometry(const wchar_t* name, const wchar_t* filePath /*= NULL*/) 
:mVertexBuffer(NULL)
,mIndexBuffer(NULL)
,mVertexDecl(NULL)
,mVertexType(VERTEX_TYPE_INVALID)
,mID(IDAllocator::ID_ANY)
{
	YString::Empty(mFilePath);
	YString::Copy(mName, _countof(mName), name);

	if(NULL != filePath)
	{
		LoadDataFromFile(filePath);
		YString::Copy(mFilePath, _countof(mFilePath), filePath);

		if(hasUVData())
		{
			if(hasTBNData())
			{
				BuildGeometry(XYZ_UV_TBN);
			}
			else
			{
				_Assert(hasNormalData());
				BuildGeometry(XYZ_UV_N);
			}
		}
		else
		{
			_Assert(hasNormalData());
			BuildGeometry(XYZ_N);
		}
	}
}

void Geometry::createVertexBuffer(void* vertexData)
{
	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &mVertexBuffer, vertexData, mGeoData.verts.size(), mVertexType);
}

void Geometry::createIndexBuffer()
{
	int numIndices = 3 * mGeoData.tris.size();

	DWORD* indexData = New DWORD[numIndices];
	for(size_t triIndex = 0; triIndex < mGeoData.tris.size(); ++triIndex)
	{
		indexData[3*triIndex + 0] = mGeoData.tris[triIndex].vertexIndex[0];
		indexData[3*triIndex + 1] = mGeoData.tris[triIndex].vertexIndex[1];
		indexData[3*triIndex + 2] = mGeoData.tris[triIndex].vertexIndex[2];
	}

	CreateIB(gEngine->GetDriver()->GetD3DDevice(), &mIndexBuffer, indexData, numIndices);

	delete[] indexData;
}

// BuildGeometry之前先要获取相应VertexType所需的顶点数据
void Geometry::BuildGeometry(VertexType type)
{
	if(type == VERTEX_TYPE_INVALID)
		return;

	mVertexType = type;

	void* vertexData = NULL;
	bool needToDeleteVertexData = false;

	int numVertices = (int)mGeoData.verts.size();

	switch(mVertexType)
	{
		case XYZ:
			{
				vertexData = New Vertex[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					Vertex vert = Vertex(mGeoData.posData[mGeoData.verts[i].posIndex].x, 
						mGeoData.posData[mGeoData.verts[i].posIndex].y, 
						mGeoData.posData[mGeoData.verts[i].posIndex].z);

					((Vertex*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV:
			{
				vertexData = New VertexUV[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexUV vert = VertexUV(mGeoData.posData[mGeoData.verts[i].posIndex].x, 
						mGeoData.posData[mGeoData.verts[i].posIndex].y, 
						mGeoData.posData[mGeoData.verts[i].posIndex].z, 
						mGeoData.uvData[mGeoData.verts[i].uvIndex].x, 
						mGeoData.uvData[mGeoData.verts[i].uvIndex].y);

					((VertexUV*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_N:
			{
				vertexData = New VertexN[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexN vert = VertexN(mGeoData.posData[mGeoData.verts[i].posIndex].x, 
						mGeoData.posData[mGeoData.verts[i].posIndex].y, 
						mGeoData.posData[mGeoData.verts[i].posIndex].z, 
						mGeoData.normalData[mGeoData.verts[i].normalIndex].x, 
						mGeoData.normalData[mGeoData.verts[i].normalIndex].y,
						mGeoData.normalData[mGeoData.verts[i].normalIndex].z);

					((VertexN*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV_N:
			{
				vertexData = New VertexUVN[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexUVN vert = VertexUVN(mGeoData.posData[mGeoData.verts[i].posIndex].x, 
						mGeoData.posData[mGeoData.verts[i].posIndex].y, 
						mGeoData.posData[mGeoData.verts[i].posIndex].z,
						mGeoData.normalData[mGeoData.verts[i].normalIndex].x, 
						mGeoData.normalData[mGeoData.verts[i].normalIndex].y,
						mGeoData.normalData[mGeoData.verts[i].normalIndex].z, 
						mGeoData.uvData[mGeoData.verts[i].uvIndex].x, 
						mGeoData.uvData[mGeoData.verts[i].uvIndex].y);

					((VertexUVN*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV_TBN:
			{
				vertexData = New VertexUVTBN[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexUVTBN vert = VertexUVTBN(mGeoData.posData[mGeoData.verts[i].posIndex].x, 
						mGeoData.posData[mGeoData.verts[i].posIndex].y, 
						mGeoData.posData[mGeoData.verts[i].posIndex].z, 
						mGeoData.uvData[mGeoData.verts[i].uvIndex].x, 
						mGeoData.uvData[mGeoData.verts[i].uvIndex].y,
						mGeoData.tangentData[mGeoData.verts[i].tangentIndex].x, 
						mGeoData.tangentData[mGeoData.verts[i].tangentIndex].y,
						mGeoData.tangentData[mGeoData.verts[i].tangentIndex].z,
						mGeoData.bitangentData[mGeoData.verts[i].bitangentIndex].x, 
						mGeoData.bitangentData[mGeoData.verts[i].bitangentIndex].y,
						mGeoData.bitangentData[mGeoData.verts[i].bitangentIndex].z,
						mGeoData.normalData[mGeoData.verts[i].normalIndex].x, 
						mGeoData.normalData[mGeoData.verts[i].normalIndex].y,
						mGeoData.normalData[mGeoData.verts[i].normalIndex].z);

					((VertexUVTBN*)vertexData)[i] = vert;
				}
				break;
			}
		default:
			_Assert(false);
	}

	createVertexBuffer(vertexData);
	createIndexBuffer();

	createVertexDeclaration();

	calculateAABBox();		// TODO: 设备恢复时要调用BuildGeometry方法, 所以其他函数尽量别放在这里面

	if(needToDeleteVertexData)
	{
		delete[] vertexData;
	}
}

void Geometry::createVertexDeclaration()
{
	switch(mVertexType)
	{
	case XYZ:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},      
				D3DDECL_END()
			};

			if(mVertexDecl)
				SAFE_RELEASE(mVertexDecl);

			gEngine->GetDriver()->GetD3DDevice()->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
			break;
		}
	case XYZ_UV:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},      
				D3DDECL_END()
			};

			if(mVertexDecl)
				SAFE_RELEASE(mVertexDecl);

			gEngine->GetDriver()->GetD3DDevice()->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
			break;
		}
	case XYZ_N:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},   
				{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},        
				D3DDECL_END()
			};

			if(mVertexDecl)
				SAFE_RELEASE(mVertexDecl);

			gEngine->GetDriver()->GetD3DDevice()->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
			break;
		}
	case XYZ_UV_N:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},    
				{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},      
				D3DDECL_END()
			};

			if(mVertexDecl)
				SAFE_RELEASE(mVertexDecl);

			gEngine->GetDriver()->GetD3DDevice()->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
			break;
		}
	case XYZ_UV_TBN:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},    
				{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},     
				{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},     
				{0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},      
				D3DDECL_END()
			};

			if(mVertexDecl)
				SAFE_RELEASE(mVertexDecl);

			gEngine->GetDriver()->GetD3DDevice()->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
			break;
		}
	}
}

void Geometry::SetVertexDeclaration()
{
	gEngine->GetDriver()->GetD3DDevice()->SetVertexDeclaration(mVertexDecl);
}

void Geometry::Draw()
{
	gEngine->GetDriver()->GetD3DDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mGeoData.verts.size(), 0, mGeoData.tris.size());

	Profiler::AddDrawCalls();
	Profiler::AddNumVerts((int)mGeoData.verts.size());
	Profiler::AddNumTris((int)mGeoData.tris.size());
}

void Geometry::SetVertexStream()
{	
	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();
	d3dDevice->SetStreamSource(0, mVertexBuffer, 0, SizeofVertex(mVertexType));
	d3dDevice->SetIndices(mIndexBuffer);
}

void Geometry::processSmoothNormal(const Vector3& curPos, const TriIDList& overAllTriGroup)
{
	TriangleSmoothGroupMap triangleSmoothGroupMap;		// 三角面属于哪个smoothgroup
	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		triangleSmoothGroupMap[overAllTriGroup[i]] = NO_GROUP;
	}

	std::vector<TriIDList> triSmoothGroups;		// 包含这个顶点的面可分为几个smoothgroup

	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		int curTriID = overAllTriGroup[i];
		buildNormalSmoothGroup(curPos, overAllTriGroup, curTriID, triangleSmoothGroupMap, triSmoothGroups);
	}

	for(size_t i = 0; i < triSmoothGroups.size(); ++i)		// 对于每个smoothgroup, 计算normal并更新顶点数据
	{
		TriIDList& curGroup = triSmoothGroups[i];

		Vector3 groupNormal = Vector3::Zero;
		for(size_t k = 0; k < curGroup.size(); ++k)
		{
			const Triangle& tri = mGeoData.tris[curGroup[k]];

			Vector3 triNormal;
			calculateTriangleNormalWeighted(tri, curPos, &triNormal);	// tri张角加成
			//calculateTriangleNormal(tri, &triNormal);

			groupNormal += triNormal;
		}

		groupNormal.Normalize();

		mGeoData.normalData.push_back(groupNormal);		// normal加入到normaldata数据中

		int newVertIndex = INVALID_INDEX;
		for(size_t i = 0; i < curGroup.size(); ++i)
		{
			Triangle& curTri = mGeoData.tris[curGroup[i]];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(mGeoData.posData[mGeoData.verts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(mGeoData.verts[vertIx].normalIndex == INVALID_INDEX)
			{
				mGeoData.verts[vertIx].normalIndex = mGeoData.normalData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(mGeoData.normalData[mGeoData.verts[vertIx].normalIndex], groupNormal, 0.01f))
				{
					if(newVertIndex != INVALID_INDEX)
					{
						curTri.vertexIndex[indexInTri] = newVertIndex;
					}
					else
					{
						Vert newVert = mGeoData.verts[vertIx];
						newVert.normalIndex = mGeoData.normalData.size() - 1;

						mGeoData.verts.push_back(newVert);
						curTri.vertexIndex[indexInTri] = mGeoData.verts.size() - 1;

						newVertIndex = mGeoData.verts.size() - 1;
					}
				}
			}
		}
	}
}


void Geometry::processSmoothTangent(const Vector3& curPos, const TriIDList& overAllTriGroup)
{
	TriangleSmoothGroupMap triangleSmoothGroupMap;		// 三角面属于哪个smoothgroup
	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		triangleSmoothGroupMap[overAllTriGroup[i]] = NO_GROUP;
	}

	std::vector<TriIDList> triSmoothGroups;		// 包含这个顶点的面可分为几个smoothgroup

	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		int curTriID = overAllTriGroup[i];
		buildTangentSmoothGroup(curPos, overAllTriGroup, curTriID, triangleSmoothGroupMap, triSmoothGroups);
	}

	for(size_t i = 0; i < triSmoothGroups.size(); ++i)		// 对于每个smoothgroup, 计算normal并更新顶点数据
	{
		TriIDList& curGroup = triSmoothGroups[i];

		Vector3 groupTangent = Vector3::Zero;
		for(size_t k = 0; k < curGroup.size(); ++k)
		{
			const Triangle& tri = mGeoData.tris[curGroup[k]];

			Vector3 triTangent;
			calculateTriangleTangent(tri, &triTangent);

			groupTangent += triTangent;		// TODO:面积加成 
		}

		groupTangent.Normalize();

		mGeoData.tangentData.push_back(groupTangent);		// normal加入到normaldata数据中

		int newVertIndex = INVALID_INDEX;
		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = mGeoData.tris[curGroup[triIx]];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(mGeoData.posData[mGeoData.verts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(mGeoData.verts[vertIx].tangentIndex == INVALID_INDEX)
			{
				mGeoData.verts[vertIx].tangentIndex = mGeoData.tangentData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(mGeoData.tangentData[mGeoData.verts[vertIx].tangentIndex], groupTangent, 0.01f))
				{
					if(newVertIndex != INVALID_INDEX)
					{
						curTri.vertexIndex[indexInTri] = newVertIndex;
					}
					else
					{
						Vert newVert = mGeoData.verts[vertIx];
						newVert.tangentIndex = mGeoData.tangentData.size() - 1;

						mGeoData.verts.push_back(newVert);
						curTri.vertexIndex[indexInTri] = mGeoData.verts.size() - 1;

						newVertIndex = mGeoData.verts.size() - 1;
					}
				}
			}
		}
	}
}


void Geometry::processSmoothBitangent(const Vector3& curPos, const TriIDList& overAllTriGroup)
{
	TriangleSmoothGroupMap triangleSmoothGroupMap;		// 三角面属于哪个smoothgroup
	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		triangleSmoothGroupMap[overAllTriGroup[i]] = NO_GROUP;
	}

	std::vector<TriIDList> triSmoothGroups;		// 包含这个顶点的面可分为几个smoothgroup

	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		int curTriID = overAllTriGroup[i];
		buildBitangentSmoothGroup(curPos, overAllTriGroup, curTriID, triangleSmoothGroupMap, triSmoothGroups);
	}

	int newVertIndex = INVALID_INDEX;
	for(size_t i = 0; i < triSmoothGroups.size(); ++i)		// 对于每个smoothgroup, 计算normal并更新顶点数据
	{
		TriIDList& curGroup = triSmoothGroups[i];

		Vector3 groupBitangent = Vector3::Zero;
		for(size_t k = 0; k < curGroup.size(); ++k)
		{
			const Triangle& tri = mGeoData.tris[curGroup[k]];

			Vector3 triBitangent;
			calculateTriangleBitanget(tri, &triBitangent);

			groupBitangent += triBitangent;		// TODO:面积加成 
		}

		groupBitangent.Normalize();

		mGeoData.bitangentData.push_back(groupBitangent);		// normal加入到normaldata数据中

		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = mGeoData.tris[curGroup[triIx]];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(mGeoData.posData[mGeoData.verts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(mGeoData.verts[vertIx].bitangentIndex == INVALID_INDEX)
			{
				mGeoData.verts[vertIx].bitangentIndex = mGeoData.bitangentData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(mGeoData.bitangentData[mGeoData.verts[vertIx].bitangentIndex], groupBitangent, 0.01f))
				{
					if(newVertIndex != INVALID_INDEX)
					{
						curTri.vertexIndex[indexInTri] = newVertIndex;
					}
					else
					{
						Vert newVert = mGeoData.verts[vertIx];
						newVert.bitangentIndex = mGeoData.bitangentData.size() - 1;

						mGeoData.verts.push_back(newVert);
						curTri.vertexIndex[indexInTri] = mGeoData.verts.size() - 1;

						newVertIndex = mGeoData.verts.size() - 1;
					}
				}
			}
		}
	}
}

void Geometry::calculateTBN(bool calculateNormal, bool calculateTangent, bool calculateBitangent)
{
	if(calculateNormal)
		mGeoData.normalData.clear();

	if(calculateTangent)
		mGeoData.tangentData.clear();

	if(calculateBitangent)
		mGeoData.bitangentData.clear();

	for(size_t i = 0; i < mGeoData.verts.size(); ++i)
	{
		if(calculateNormal)
			mGeoData.verts[i].normalIndex = INVALID_INDEX;

		if(calculateTangent)
			mGeoData.verts[i].tangentIndex = INVALID_INDEX;

		if(calculateBitangent)
			mGeoData.verts[i].bitangentIndex = INVALID_INDEX;
	}

	if(!calculateNormal && !calculateTangent && !calculateBitangent)
		return;

	VertexTrianglesMap vertexTrianglesMap;

	// get vertexTrianglesMap
	for(size_t triIndex = 0; triIndex < mGeoData.tris.size(); ++triIndex)
	{
		int curTriID = triIndex;
		Triangle& curTri = mGeoData.tris[curTriID];		// each triangle

		for(int i = 0; i < 3; ++i)
		{
			Vector3 curPos = mGeoData.posData[mGeoData.verts[curTri.vertexIndex[i]].posIndex];	// each vertPos of curTri

			VertexTrianglesMap::iterator iter = vertexTrianglesMap.find(curPos);
			if(iter != vertexTrianglesMap.end())
			{
				iter->second.push_back(curTriID);
			}
			else
			{
				TriIDList triList;
				triList.push_back(curTriID);

				vertexTrianglesMap[curPos] = triList;
			}
		}
	}

	for(VertexTrianglesMap::iterator iter = vertexTrianglesMap.begin(); iter != vertexTrianglesMap.end(); ++iter)
	{
		Vector3 curPos = iter->first;		// for each unique position

		TriIDList& overAllTriGroup = iter->second;		

		if(calculateNormal)
			processSmoothNormal(curPos, overAllTriGroup);

		if(calculateTangent)
			processSmoothTangent(curPos, overAllTriGroup);

		if(calculateBitangent)
			processSmoothBitangent(curPos, overAllTriGroup);

		//// 对于存在孤立的点的特殊情况(不会被任何smoothgroup处理, 所以没计算tbn), 暂时设置tbn索引都为0
		//for(std::vector<Vert>::iterator iter = mVerts.begin(); iter != mVerts.end(); ++iter)
		//{
		//	Vert& vert = *iter;
		//	if(calculateNormal && vert.normalIndex == INVALID_INDEX)
		//		vert.normalIndex = 0;

		//	if(calculateTangent && vert.tangentIndex == INVALID_INDEX)
		//		vert.tangentIndex = 0;

		//	if(calculateBitangent && vert.bitangentIndex == INVALID_INDEX)
		//		vert.bitangentIndex = 0;
		//}
	}
}

void Geometry::CalculateNormals()
{
	calculateTBN(true, false, false);
}


void Geometry::buildNormalSmoothGroup(Vector3 curPos, const TriIDList& overAllTriGroup, int curTriID, 
							TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriIDList>& triSmoothGroups)
{
	if(triSmoothGroupMap[curTriID] != NO_GROUP)		// 已处理(分配好smoothgroup)的直接返回
		return;

	const Triangle& curTri = mGeoData.tris[curTriID];

	int neighb0ID = -1;
	int neighb1ID = -1;

	Triangle* neighb0 = NULL;
	Triangle* neighb1 = NULL;

	findNeighbourTriangles(overAllTriGroup, curTriID, &neighb0ID, &neighb1ID);

	if(neighb0ID != -1)
		neighb0 = &(mGeoData.tris[neighb0ID]);

	if(neighb1ID != -1)
		neighb1 = &(mGeoData.tris[neighb1ID]);

	const float minCreaseAngle = PI / 3.0f;
	if(neighb0 != NULL &&
		canSmoothNormal(curTri, *neighb0, minCreaseAngle) && triSmoothGroupMap[neighb0ID] != NO_GROUP)
	{
		triSmoothGroupMap[curTriID] = triSmoothGroupMap[neighb0ID];
		triSmoothGroups[triSmoothGroupMap[curTriID]].push_back(curTriID);
		return;
	}
	else if(neighb1 != NULL &&
		canSmoothNormal(curTri, *neighb1, minCreaseAngle) && triSmoothGroupMap[neighb1ID] != NO_GROUP)
	{
		triSmoothGroupMap[curTriID] = triSmoothGroupMap[neighb1ID];
		triSmoothGroups[triSmoothGroupMap[curTriID]].push_back(curTriID);
		return;
	} 
	else
	{
		triSmoothGroupMap[curTriID] = triSmoothGroups.size();

		TriIDList newSmoothGroup;
		newSmoothGroup.push_back(curTriID);

		triSmoothGroups.push_back(newSmoothGroup);
	}

	if(neighb0 != NULL)
		buildNormalSmoothGroup(curPos, overAllTriGroup, neighb0ID, triSmoothGroupMap, triSmoothGroups);

	if(neighb1 != NULL)
		buildNormalSmoothGroup(curPos, overAllTriGroup, neighb1ID, triSmoothGroupMap, triSmoothGroups);
}


void Geometry::buildTangentSmoothGroup(Vector3 curPos, const TriIDList& overAllTriGroup, int curTriID, 
									   TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriIDList>& triSmoothGroups)
{
	if(triSmoothGroupMap[curTriID] != NO_GROUP)		// 已处理(分配好smoothgroup)的直接返回
		return;

	const Triangle& curTri = mGeoData.tris[curTriID];

	int neighb0ID = -1;
	int neighb1ID = -1;

	Triangle* neighb0 = NULL;
	Triangle* neighb1 = NULL;

	findNeighbourTriangles(overAllTriGroup, curTriID, &neighb0ID, &neighb1ID);

	if(neighb0ID != -1)
		neighb0 = &(mGeoData.tris[neighb0ID]);

	if(neighb1ID != -1)
		neighb1 = &(mGeoData.tris[neighb1ID]);


	const float minCreaseAngle = PI / 3.0f;
	if(neighb0 != NULL &&
		canSmoothTangent(curTri, *neighb0, minCreaseAngle) && triSmoothGroupMap[neighb0ID] != NO_GROUP)
	{
		triSmoothGroupMap[curTriID] = triSmoothGroupMap[neighb0ID];
		triSmoothGroups[triSmoothGroupMap[curTriID]].push_back(curTriID);
		return;
	}
	else if(neighb1 != NULL &&
		canSmoothTangent(curTri, *neighb1, minCreaseAngle) && triSmoothGroupMap[neighb1ID] != NO_GROUP)
	{
		triSmoothGroupMap[curTriID] = triSmoothGroupMap[neighb1ID];
		triSmoothGroups[triSmoothGroupMap[curTriID]].push_back(curTriID);
		return;
	}
	else
	{
		triSmoothGroupMap[curTriID] = triSmoothGroups.size();

		TriIDList newSmoothGroup;
		newSmoothGroup.push_back(curTriID);

		triSmoothGroups.push_back(newSmoothGroup);
	}

	if(neighb0 != NULL)
		buildTangentSmoothGroup(curPos, overAllTriGroup, neighb0ID, triSmoothGroupMap, triSmoothGroups);

	if(neighb1 != NULL)
		buildTangentSmoothGroup(curPos, overAllTriGroup, neighb1ID, triSmoothGroupMap, triSmoothGroups);
}


void Geometry::buildBitangentSmoothGroup(Vector3 curPos, const TriIDList& overAllTriGroup, int curTriID, 
										 TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriIDList>& triSmoothGroups)
{
	if(triSmoothGroupMap[curTriID] != NO_GROUP)		// 已处理(分配好smoothgroup)的直接返回
		return;

	const Triangle& curTri = mGeoData.tris[curTriID];

	int neighb0ID = -1;
	int neighb1ID = -1;

	Triangle* neighb0 = NULL;
	Triangle* neighb1 = NULL;

	findNeighbourTriangles(overAllTriGroup, curTriID, &neighb0ID, &neighb1ID);

	if(neighb0ID != -1)
		neighb0 = &(mGeoData.tris[neighb0ID]);

	if(neighb1ID != -1)
		neighb1 = &(mGeoData.tris[neighb1ID]);


	const float minCreaseAngle = PI / 3.0f;
	if(neighb0 != NULL &&
		canSmoothBitangent(curTri, *neighb0, minCreaseAngle) && triSmoothGroupMap[neighb0ID] != NO_GROUP)
	{
		triSmoothGroupMap[curTriID] = triSmoothGroupMap[neighb0ID];
		triSmoothGroups[triSmoothGroupMap[curTriID]].push_back(curTriID);
		return;
	}
	else if(neighb1 != NULL &&
		canSmoothBitangent(curTri, *neighb1, minCreaseAngle) && triSmoothGroupMap[neighb1ID] != NO_GROUP)
	{
		triSmoothGroupMap[curTriID] = triSmoothGroupMap[neighb1ID];
		triSmoothGroups[triSmoothGroupMap[curTriID]].push_back(curTriID);
		return;
	}
	else
	{
		triSmoothGroupMap[curTriID] = triSmoothGroups.size();

		TriIDList newSmoothGroup;
		newSmoothGroup.push_back(curTriID);

		triSmoothGroups.push_back(newSmoothGroup);
	}

	if(neighb0 != NULL)
		buildBitangentSmoothGroup(curPos, overAllTriGroup, neighb0ID, triSmoothGroupMap, triSmoothGroups);

	if(neighb1 != NULL)
		buildBitangentSmoothGroup(curPos, overAllTriGroup, neighb1ID, triSmoothGroupMap, triSmoothGroups);
}

void Geometry::findNeighbourTriangles(const TriIDList& triGroup, int curTriID, int* neighb0ID, int* neighb1ID)
{
	_Assert(NULL != neighb0ID);
	_Assert(NULL != neighb1ID);

	const Triangle& curTri = mGeoData.tris[curTriID];

	int numNeighboursFound = 0;
	*neighb0ID = -1;
	*neighb1ID = -1;

	for(size_t i = 0; i < triGroup.size(); ++i)
	{
		int triID = triGroup[i];
		const Triangle& tri = mGeoData.tris[triID];

		if(tri.vertexIndex[0] == curTri.vertexIndex[0] &&
			tri.vertexIndex[1] == curTri.vertexIndex[1] &&
			tri.vertexIndex[2] == curTri.vertexIndex[2])		// 排除自己
			continue;
		
		if(hasSharedEdge(curTri, tri))
		{
			if(numNeighboursFound == 0)
				*neighb0ID = triID;
			else
				*neighb1ID = triID;

			numNeighboursFound++;
		}
	}
}

bool Geometry::hasSharedEdge(const Triangle& triangle0, const Triangle& triangle1)
{
	int numSharedVerts = 0;

	for(int i = 0; i < 3; ++i)
	{
		Vector3& tri0Pos = mGeoData.posData[mGeoData.verts[triangle0.vertexIndex[i]].posIndex];
		for(int j = 0; j < 3; ++j)
		{
			Vector3& tri1Pos = mGeoData.posData[mGeoData.verts[triangle1.vertexIndex[j]].posIndex];
			if(Vector3Equal(tri1Pos, tri0Pos, 0.0001f))
			{
				++numSharedVerts;
			}
		}
	}

	if(numSharedVerts > 1)
		return true;
	else
		return false;
}

bool Geometry::canSmoothNormal(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle)
{
	Vector3 triNormal0;
	Vector3 triNormal1;

	calculateTriangleNormal(triangle0, &triNormal0);
	calculateTriangleNormal(triangle1, &triNormal1);

	if(triNormal0 == Vector3::Zero || triNormal1 == Vector3::Zero)
		return false;
	
	float normalAngle = VectorAngle(triNormal0, triNormal1);

	if(normalAngle < minCreaseAngle)
		return true;
	else
		return false;
}

bool Geometry::canSmoothTangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle)
{
	Vector3 triTangent0;
	Vector3 triTangent1;

	calculateTriangleTangent(triangle0, &triTangent0);
	calculateTriangleTangent(triangle1, &triTangent1);

	if(triTangent0 == Vector3::Zero || triTangent1 == Vector3::Zero)
		return false;

	float tangentAngle = VectorAngle(triTangent0, triTangent1);

	if(tangentAngle < minCreaseAngle)
		return true;
	else
		return false;
}

bool Geometry::canSmoothBitangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle)
{
	Vector3 triBitangent0;
	Vector3 triBitangent1;

	calculateTriangleBitanget(triangle0, &triBitangent0);
	calculateTriangleBitanget(triangle1, &triBitangent1);

	if(triBitangent0 == Vector3::Zero || triBitangent1 == Vector3::Zero)
		return false;

	float bitangentAngle = VectorAngle(triBitangent0, triBitangent1);

	if(bitangentAngle < minCreaseAngle)
		return true;
	else
		return false;
}

void Geometry::calculateTriangleNormalWeighted(const Triangle& triangle, const Vector3& pos, Vector3* normal)
{
	Vector3 pos0 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p1;
	Vector3 p2;

	if(Vector3Equal(pos0, pos, 0.0001f))
	{
		p1 = pos2 - pos0;
		p2 = pos1 - pos0;
	}
	else if(Vector3Equal(pos1, pos, 0.0001f))
	{
		p1 = pos0 - pos1;
		p2 = pos2 - pos1;
	}
	else if(Vector3Equal(pos2, pos, 0.0001f))
	{
		p1 = pos1 - pos2;
		p2 = pos0 - pos2;
	}
	else
	{
		_Assert(false);
		return;
	}

	if(p1.Length() == 0 || p2.Length() == 0)
	{
		*normal = Vector3::Zero;
	}
	else
	{
		*normal = p1.Cross(p2);

		normal->Normalize();
		*normal *= VectorAngle(p1, p2) / (2*PI);
	}
}

void Geometry::calculateTriangleNormal(const Triangle& triangle, Vector3* normal)
{
	Vector3 pos0 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	*normal = p0p2.Cross(p0p1);

	normal->Normalize();
}

void Geometry::calculateTriangleTangent(const Triangle& triangle, Vector3* tangent)
{
	Vector3 pos0 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	Vector2 uv0 = mGeoData.uvData[mGeoData.verts[triangle.vertexIndex[0]].uvIndex];
	Vector2 uv1 = mGeoData.uvData[mGeoData.verts[triangle.vertexIndex[1]].uvIndex];
	Vector2 uv2 = mGeoData.uvData[mGeoData.verts[triangle.vertexIndex[2]].uvIndex];

	float s1 = uv1.x - uv0.x;
	float t1 = uv1.y - uv0.y;
	float s2 = uv2.x - uv0.x;
	float t2 = uv2.y - uv0.y;

	float tmp = 0.0f;
	if(fabsf(s1*t2 - s2*t1) <= 0.0001f)
	{
		tmp = 1.0f;
	}
	else
	{
		tmp = 1.0f/(s1*t2 - s2*t1 );
	}

	tangent->x = (t2 * p0p1.x - t1 * p0p2.x);
	tangent->y = (t2 * p0p1.y - t1 * p0p2.y);
	tangent->z = (t2 * p0p1.z - t1 * p0p2.z);

	*tangent = tmp * (*tangent);
}

void Geometry::calculateTriangleBitanget(const Triangle& triangle, Vector3* bitangent)
{
	Vector3 pos0 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mGeoData.posData[mGeoData.verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	Vector2 uv0 = mGeoData.uvData[mGeoData.verts[triangle.vertexIndex[0]].uvIndex];
	Vector2 uv1 = mGeoData.uvData[mGeoData.verts[triangle.vertexIndex[1]].uvIndex];
	Vector2 uv2 = mGeoData.uvData[mGeoData.verts[triangle.vertexIndex[2]].uvIndex];

	float s1 = uv1.x - uv0.x;
	float t1 = uv1.y - uv0.y;
	float s2 = uv2.x - uv0.x;
	float t2 = uv2.y - uv0.y;

	float tmp = 0.0f;
	if(fabsf(s1*t2 - s2*t1) <= 0.0001f)
	{
		tmp = 1.0f;
	}
	else
	{
		tmp = 1.0f/(s1*t2 - s2*t1 );
	}

	bitangent->x = (s1 * p0p2.x - s2 * p0p1.x);
	bitangent->y = (s1 * p0p2.y - s2 * p0p1.y);
	bitangent->z = (s1 * p0p2.z - s2 * p0p1.z);

	*bitangent = tmp * (*bitangent);
}

//void Mesh::CalculateUVs(UVMappingMode uvMode)
//{
//	switch(uvMode)
//	{
//	case SphereUV:
//		{
//			for(int i = 0; i < numVertices; ++i)
//			{
//				Vector3 posDir = positionData[i].Normalized();
//
//				float u = (atan2(posDir.z, posDir.x) + PI) / (2*PI);
//				float v = -0.5f * (posDir.y - 1.0f);
//
//				uvData[i] = Vector2(u, v);
//			}
//			break;
//		}
//	}
//}

//void Mesh::calculateBoundingBox()
//{
//	boundingBox.max = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
//	boundingBox.min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
//
//	for(int i = 0; i < positionData.size(); ++i)
//	{
//		if(positionData[i].x > boundingBox.max.x)
//			boundingBox.max.x = positionData[i].x;
//		if(positionData[i].x < boundingBox.min.x)
//			boundingBox.min.x = positionData[i].x;
//
//
//		if(positionData[i].y > boundingBox.max.y)
//			boundingBox.max.y = positionData[i].y;
//		if(positionData[i].y < boundingBox.min.y)
//			boundingBox.min.y = positionData[i].y;
//
//		if(positionData[i].z > boundingBox.max.z)
//			boundingBox.max.z = positionData[i].z;
//		if(positionData[i].z < boundingBox.min.z)
//			boundingBox.min.z = positionData[i].z;
//	}
//}


//void Mesh::LoadDataFromFile(std::string filename, MeshFileFormat format)
//{
//	std::fstream fin(filename.c_str());
//
//	if(!fin)
//		Log("load file \"%s\" failed!\n", filename.c_str());
//
//	switch(format)
//	{
//	case OBJ:
//		{
//			std::vector<Vector3> filePosData;
//			std::vector<DWORD> fileIndexData;
//
//			char line[100];
//			while(!fin.eof())
//			{
//				fin.getline(line, 100);
//				OBJParseLine(line, filePosData, fileIndexData);
//			}
//
//			numVertices = filePosData.size();
//			numTriangles = fileIndexData.size() / 3;
//
//			positionData = new Vector3[numVertices];
//			uvData = new Vector2[numVertices];
//			normalData = new Vector3[numVertices];
//			tangentData = new Vector3[numVertices];
//			bitangentData = new Vector3[numVertices];
//			indexData = new DWORD[3*numTriangles];
//
//			for(int i = 0; i < numVertices; ++i)
//			{
//				positionData[i] = filePosData[i];
//			}
//
//			for(int i = 0; i < 3*numTriangles; ++i)
//			{
//				indexData[i] = fileIndexData[i];
//			}
//
//			break;
//		}
//	}
//}
//

void Geometry::CalculateTBN()
{
	calculateTBN(true, true, true);
}

wchar_t* Geometry::GetName()
{
	return mName;
}

void Geometry::SetID(DWORD id)
{
	mID = id;
}

void Geometry::calculateAABBox()
{
	mAABBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	mAABBox.mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);

	for(size_t i = 0; i < mGeoData.posData.size(); ++i)
	{
		if(mGeoData.posData[i].x > mAABBox.mMax.x)
			mAABBox.mMax.x = mGeoData.posData[i].x;
		if(mGeoData.posData[i].x < mAABBox.mMin.x)
			mAABBox.mMin.x = mGeoData.posData[i].x;

		if(mGeoData.posData[i].y > mAABBox.mMax.y)
			mAABBox.mMax.y = mGeoData.posData[i].y;
		if(mGeoData.posData[i].y < mAABBox.mMin.y)
			mAABBox.mMin.y = mGeoData.posData[i].y;

		if(mGeoData.posData[i].z > mAABBox.mMax.z)
			mAABBox.mMax.z = mGeoData.posData[i].z;
		if(mGeoData.posData[i].z < mAABBox.mMin.z)
			mAABBox.mMin.z = mGeoData.posData[i].z;
	}
}

AABBox Geometry::GetAABBox()
{
	return mAABBox;
}

void Geometry::CalcDynamicAABBox(const D3DXMATRIX& matWorld, AABBox* box)
{
	_Assert(NULL != box);

	box->mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	box->mMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(size_t i = 0; i < mGeoData.posData.size(); ++i)
	{
		D3DXVECTOR3 vertPos(mGeoData.posData[i].x, mGeoData.posData[i].y, mGeoData.posData[i].z);
		D3DXVec3TransformCoord(&vertPos, &vertPos, &matWorld);

		if(vertPos.x > box->mMax.x)
			box->mMax.x = vertPos.x;
		if(vertPos.x < box->mMin.x)
			box->mMin.x = vertPos.x;

		if(vertPos.y > box->mMax.y)
			box->mMax.y = vertPos.y;
		if(vertPos.y < box->mMin.y)
			box->mMin.y = vertPos.y;

		if(vertPos.z > box->mMax.z)
			box->mMax.z = vertPos.z;
		if(vertPos.z < box->mMin.z)
			box->mMin.z = vertPos.z;
	}
}

VertexType Geometry::GetVertexType()
{
	return mVertexType;
}

void Geometry::SaveToFile(const wchar_t* dirPath)
{
	wchar_t filePath[MAX_PATH_LEN];
	YString::Copy(filePath, _countof(filePath), dirPath);
	YString::Concat(filePath, _countof(filePath), L"/");
	YString::Concat(filePath, _countof(filePath), mName, L".geo");

	if(YString::isEmpty(mFilePath))
	{
		YString::Copy(mFilePath, _countof(mFilePath), filePath);
	}

	if(YFile::Exist(filePath))
	{
		Log(L"warning: file already exists. Geometry::SaveToFile(%s)\n", filePath);
		return;
	}

	// TODO: dirPath不存在

	YFile* file = YFile::Open(filePath, YFile::WRITE_BINARY);

	GeometryDataHeader header;
	ZeroStruct(header);

	header.posDataSize = (int)mGeoData.posData.size();
	header.uvDataSize = (int)mGeoData.uvData.size();
	header.normalDataSize = (int)mGeoData.normalData.size();
	header.tangentDataSize = (int)mGeoData.tangentData.size();
	header.bitangentDataSize = (int)mGeoData.bitangentData.size();
	header.numVerts = (int)mGeoData.verts.size();
	header.numTris = (int)mGeoData.tris.size();

	file->Write((void*)&header, sizeof(GeometryDataHeader), 1);

	_Assert(header.posDataSize > 0);
	file->Write((void*)&mGeoData.posData[0], sizeof(Vector3), header.posDataSize);

	if(header.uvDataSize > 0)
		file->Write((void*)&mGeoData.uvData[0], sizeof(Vector2), header.uvDataSize);

	if(header.normalDataSize > 0)
		file->Write((void*)&mGeoData.normalData[0], sizeof(Vector3), header.normalDataSize);

	if(header.tangentDataSize > 0)
		file->Write((void*)&mGeoData.tangentData[0], sizeof(Vector3), header.tangentDataSize);

	if(header.bitangentDataSize > 0)
		file->Write((void*)&mGeoData.bitangentData[0], sizeof(Vector3), header.bitangentDataSize);

	_Assert(header.numVerts > 0);
	_Assert(header.numTris > 0);
	file->Write((void*)&mGeoData.verts[0], sizeof(Vert), header.numVerts);
	file->Write((void*)&mGeoData.tris[0], sizeof(Triangle), header.numTris);

	file->Close();
}

void Geometry::LoadDataFromFile(const wchar_t* filePath)
{
	_Assert(YFile::Exist(filePath));

	wchar_t suffix[MAX_STR_LEN];
	YString::GetFileSuffix(suffix, _countof(suffix), filePath);
	_Assert(YString::Compare(suffix, L"geo", true) == 0);

	YFile* file = YFile::Open(filePath, YFile::READ_BINARY);

	GeometryDataHeader header;
	ZeroStruct(header);

	file->Read((void*)&header, sizeof(GeometryDataHeader), sizeof(GeometryDataHeader), 1);

	mGeoData.posData.resize(header.posDataSize);
	mGeoData.uvData.resize(header.uvDataSize);
	mGeoData.normalData.resize(header.normalDataSize);
	mGeoData.tangentData.resize(header.tangentDataSize);
	mGeoData.bitangentData.resize(header.bitangentDataSize);
	mGeoData.verts.resize(header.numVerts);
	mGeoData.tris.resize(header.numTris);

	_Assert(header.posDataSize > 0);
	file->Read((void*)&mGeoData.posData[0], header.posDataSize * sizeof(Vector3), sizeof(Vector3), header.posDataSize);

	if(header.uvDataSize > 0)
		file->Read((void*)&mGeoData.uvData[0], header.uvDataSize * sizeof(Vector2), sizeof(Vector2), header.uvDataSize);

	if(header.normalDataSize > 0)
		file->Read((void*)&mGeoData.normalData[0], header.normalDataSize * sizeof(Vector3), sizeof(Vector3), header.normalDataSize);

	if(header.tangentDataSize > 0)
		file->Read((void*)&mGeoData.tangentData[0], header.tangentDataSize * sizeof(Vector3), sizeof(Vector3), header.tangentDataSize);

	if(header.bitangentDataSize > 0)
		file->Read((void*)&mGeoData.bitangentData[0], header.bitangentDataSize * sizeof(Vector3), sizeof(Vector3), header.bitangentDataSize);
	
	_Assert(header.numVerts > 0);
	_Assert(header.numTris > 0);
	file->Read((void*)&mGeoData.verts[0], header.numVerts * sizeof(Vert), sizeof(Vert), header.numVerts);
	file->Read((void*)&mGeoData.tris[0], header.numTris * sizeof(Triangle), sizeof(Triangle), header.numTris);

	file->Close();
}

bool Geometry::hasUVData()
{
	return mGeoData.uvData.size() != 0;
}

bool Geometry::hasNormalData()
{
	return mGeoData.normalData.size() != 0;
}

bool Geometry::hasTBNData()
{
	return mGeoData.normalData.size() != 0 &&
		mGeoData.tangentData.size() != 0 &&
		mGeoData.bitangentData.size() != 0;
}

wchar_t* Geometry::GetFilePath()
{
	return mFilePath;
}
