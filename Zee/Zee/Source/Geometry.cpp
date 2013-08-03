#include "Geometry.h"

void Geometry::createVertexBuffer(void* vertexData)
{
	CreateVB(gD3DDevice, &mVertexBuffer, vertexData, mVerts.size(), mVertexType);
}

void Geometry::createIndexBuffer()
{
	int numIndices = 3 * mTriangles.size();

	DWORD* indexData = new DWORD[numIndices];
	for(size_t triIndex = 0; triIndex < mTriangles.size(); ++triIndex)
	{
		indexData[3*triIndex + 0] = mTriangles[triIndex].vertexIndex[0];
		indexData[3*triIndex + 1] = mTriangles[triIndex].vertexIndex[1];
		indexData[3*triIndex + 2] = mTriangles[triIndex].vertexIndex[2];
	}

	CreateIB(gD3DDevice, &mIndexBuffer, indexData, numIndices);
}

// BuildGeometry之前先要获取相应VertexType所需的顶点数据
void Geometry::BuildGeometry(VertexType type)
{
	if(type == VERTEX_TYPE_INVALID)
		return;

	mVertexType = type;

	void* vertexData = NULL;
	bool needToDeleteVertexData = false;

	int numVertices = (int)mVerts.size();

	switch(mVertexType)
	{
		case XYZ:
			{
				vertexData = new Vertex[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					Vertex vert = Vertex(mPositionData[mVerts[i].posIndex].x, 
						mPositionData[mVerts[i].posIndex].y, 
						mPositionData[mVerts[i].posIndex].z);

					((Vertex*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV:
			{
				vertexData = new VertexUV[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexUV vert = VertexUV(mPositionData[mVerts[i].posIndex].x, 
						mPositionData[mVerts[i].posIndex].y, 
						mPositionData[mVerts[i].posIndex].z, 
						mUVData[mVerts[i].uvIndex].x, 
						mUVData[mVerts[i].uvIndex].y);

					((VertexUV*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_N:
			{
				vertexData = new VertexN[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexN vert = VertexN(mPositionData[mVerts[i].posIndex].x, 
						mPositionData[mVerts[i].posIndex].y, 
						mPositionData[mVerts[i].posIndex].z, 
						mNormalData[mVerts[i].normalIndex].x, 
						mNormalData[mVerts[i].normalIndex].y,
						mNormalData[mVerts[i].normalIndex].z);

					((VertexN*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV_N:
			{
				vertexData = new VertexUVN[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexUVN vert = VertexUVN(mPositionData[mVerts[i].posIndex].x, 
						mPositionData[mVerts[i].posIndex].y, 
						mPositionData[mVerts[i].posIndex].z, 
						mUVData[mVerts[i].uvIndex].x, 
						mUVData[mVerts[i].uvIndex].y,
						mNormalData[mVerts[i].normalIndex].x, 
						mNormalData[mVerts[i].normalIndex].y,
						mNormalData[mVerts[i].normalIndex].z);

					((VertexUVN*)vertexData)[i] = vert;
				}
				break;
			}
		case XYZ_UV_TBN:
			{
				vertexData = new VertexUVTBN[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					VertexUVTBN vert = VertexUVTBN(mPositionData[mVerts[i].posIndex].x, 
						mPositionData[mVerts[i].posIndex].y, 
						mPositionData[mVerts[i].posIndex].z, 
						mUVData[mVerts[i].uvIndex].x, 
						mUVData[mVerts[i].uvIndex].y,
						mTangentData[mVerts[i].tangentIndex].x, 
						mTangentData[mVerts[i].tangentIndex].y,
						mTangentData[mVerts[i].tangentIndex].z,
						mBitangentData[mVerts[i].bitangentIndex].x, 
						mBitangentData[mVerts[i].bitangentIndex].y,
						mBitangentData[mVerts[i].bitangentIndex].z,
						mNormalData[mVerts[i].normalIndex].x, 
						mNormalData[mVerts[i].normalIndex].y,
						mNormalData[mVerts[i].normalIndex].z);

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

	//calculateBoundingBox();		// TODO: 设备恢复时要调用BuildGeometry方法, 所以其他函数尽量别放在这里面

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

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
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

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
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

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
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

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
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

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &mVertexDecl);
			break;
		}
	}
}

void Geometry::SetVertexDeclaration()
{
	gD3DDevice->SetVertexDeclaration(mVertexDecl);
}

void Geometry::Draw()
{
	gD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVerts.size(), 0, mTriangles.size());
}

void Geometry::SetVertexStream()
{	
	gD3DDevice->SetStreamSource(0, mVertexBuffer, 0, SizeofVertex(mVertexType));
	gD3DDevice->SetIndices(mIndexBuffer);
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
			const Triangle& tri = mTriangles[curGroup[k]];

			Vector3 triNormal;
			calculateTriangleNormal(tri, &triNormal);

			groupNormal += triNormal;		// TODO:面积加成 
		}

		groupNormal.Normalize();

		mNormalData.push_back(groupNormal);		// normal加入到normaldata数据中

		int newVertIndex = INVALID_INDEX;
		for(size_t i = 0; i < curGroup.size(); ++i)
		{
			Triangle& curTri = mTriangles[curGroup[i]];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(mPositionData[mVerts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(mVerts[vertIx].normalIndex == INVALID_INDEX)
			{
				mVerts[vertIx].normalIndex = mNormalData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(mNormalData[mVerts[vertIx].normalIndex], groupNormal, 0.01f))
				{
					if(newVertIndex != INVALID_INDEX)
					{
						curTri.vertexIndex[indexInTri] = newVertIndex;
					}
					else
					{
						Vert newVert = mVerts[vertIx];
						newVert.normalIndex = mNormalData.size() - 1;

						mVerts.push_back(newVert);
						curTri.vertexIndex[indexInTri] = mVerts.size() - 1;

						newVertIndex = mVerts.size() - 1;
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
			const Triangle& tri = mTriangles[curGroup[k]];

			Vector3 triTangent;
			calculateTriangleTangent(tri, &triTangent);

			groupTangent += triTangent;		// TODO:面积加成 
		}

		groupTangent.Normalize();

		mTangentData.push_back(groupTangent);		// normal加入到normaldata数据中

		int newVertIndex = INVALID_INDEX;
		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = mTriangles[curGroup[triIx]];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(mPositionData[mVerts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(mVerts[vertIx].tangentIndex == INVALID_INDEX)
			{
				mVerts[vertIx].tangentIndex = mTangentData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(mTangentData[mVerts[vertIx].tangentIndex], groupTangent, 0.01f))
				{
					if(newVertIndex != INVALID_INDEX)
					{
						curTri.vertexIndex[indexInTri] = newVertIndex;
					}
					else
					{
						Vert newVert = mVerts[vertIx];
						newVert.tangentIndex = mTangentData.size() - 1;

						mVerts.push_back(newVert);
						curTri.vertexIndex[indexInTri] = mVerts.size() - 1;

						newVertIndex = mVerts.size() - 1;
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
			const Triangle& tri = mTriangles[curGroup[k]];

			Vector3 triBitangent;
			calculateTriangleBitanget(tri, &triBitangent);

			groupBitangent += triBitangent;		// TODO:面积加成 
		}

		groupBitangent.Normalize();

		mBitangentData.push_back(groupBitangent);		// normal加入到normaldata数据中

		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = mTriangles[curGroup[triIx]];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(mPositionData[mVerts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(mVerts[vertIx].bitangentIndex == INVALID_INDEX)
			{
				mVerts[vertIx].bitangentIndex = mBitangentData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(mBitangentData[mVerts[vertIx].bitangentIndex], groupBitangent, 0.01f))
				{
					if(newVertIndex != INVALID_INDEX)
					{
						curTri.vertexIndex[indexInTri] = newVertIndex;
					}
					else
					{
						Vert newVert = mVerts[vertIx];
						newVert.bitangentIndex = mBitangentData.size() - 1;

						mVerts.push_back(newVert);
						curTri.vertexIndex[indexInTri] = mVerts.size() - 1;

						newVertIndex = mVerts.size() - 1;
					}
				}
			}
		}
	}
}

void Geometry::calculateTBN(bool calculateNormal, bool calculateTangent, bool calculateBitangent)
{
	if(calculateNormal)
		mNormalData.clear();

	if(calculateTangent)
		mTangentData.clear();

	if(calculateBitangent)
		mBitangentData.clear();

	for(size_t i = 0; i < mVerts.size(); ++i)
	{
		if(calculateNormal)
			mVerts[i].normalIndex = INVALID_INDEX;

		if(calculateTangent)
			mVerts[i].tangentIndex = INVALID_INDEX;

		if(calculateBitangent)
			mVerts[i].bitangentIndex = INVALID_INDEX;
	}

	if(!calculateNormal && !calculateTangent && !calculateBitangent)
		return;

	VertexTrianglesMap vertexTrianglesMap;

	// get vertexTrianglesMap
	for(size_t triIndex = 0; triIndex < mTriangles.size(); ++triIndex)
	{
		int curTriID = triIndex;
		Triangle& curTri = mTriangles[curTriID];		// each triangle

		for(int i = 0; i < 3; ++i)
		{
			Vector3 curPos = mPositionData[mVerts[curTri.vertexIndex[i]].posIndex];	// each vertPos of curTri

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

		TriIDList overAllTriGroup = iter->second;

		if(calculateNormal)
			processSmoothNormal(curPos, overAllTriGroup);

		if(calculateTangent)
			processSmoothTangent(curPos, overAllTriGroup);

		if(calculateBitangent)
			processSmoothBitangent(curPos, overAllTriGroup);
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

	const Triangle& curTri = mTriangles[curTriID];

	int neighb0ID = -1;
	int neighb1ID = -1;

	Triangle* neighb0 = NULL;
	Triangle* neighb1 = NULL;

	findNeighbourTriangles(overAllTriGroup, curTriID, &neighb0ID, &neighb1ID);

	if(neighb0ID != -1)
		neighb0 = &(mTriangles[neighb0ID]);

	if(neighb1ID != -1)
		neighb1 = &(mTriangles[neighb1ID]);

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

	const Triangle& curTri = mTriangles[curTriID];

	int neighb0ID = -1;
	int neighb1ID = -1;

	Triangle* neighb0 = NULL;
	Triangle* neighb1 = NULL;

	findNeighbourTriangles(overAllTriGroup, curTriID, &neighb0ID, &neighb1ID);

	if(neighb0ID != -1)
		neighb0 = &(mTriangles[neighb0ID]);

	if(neighb1ID != -1)
		neighb1 = &(mTriangles[neighb1ID]);


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

	const Triangle& curTri = mTriangles[curTriID];

	int neighb0ID = -1;
	int neighb1ID = -1;

	Triangle* neighb0 = NULL;
	Triangle* neighb1 = NULL;

	findNeighbourTriangles(overAllTriGroup, curTriID, &neighb0ID, &neighb1ID);

	if(neighb0ID != -1)
		neighb0 = &(mTriangles[neighb0ID]);

	if(neighb1ID != -1)
		neighb1 = &(mTriangles[neighb1ID]);


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

	const Triangle& curTri = mTriangles[curTriID];

	int numNeighboursFound = 0;
	*neighb0ID = -1;
	*neighb1ID = -1;

	for(size_t i = 0; i < triGroup.size(); ++i)
	{
		int triID = triGroup[i];
		const Triangle& tri = mTriangles[triID];

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
		Vector3& tri0Pos = mPositionData[mVerts[triangle0.vertexIndex[i]].posIndex];
		for(int j = 0; j < 3; ++j)
		{
			Vector3& tri1Pos = mPositionData[mVerts[triangle1.vertexIndex[j]].posIndex];
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

void Geometry::calculateTriangleNormal(const Triangle& triangle, Vector3* normal)
{
	Vector3 pos0 = mPositionData[mVerts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mPositionData[mVerts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mPositionData[mVerts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	*normal = p0p1.Cross(p0p2);

	normal->Normalize();
}

void Geometry::calculateTriangleTangent(const Triangle& triangle, Vector3* tangent)
{
	Vector3 pos0 = mPositionData[mVerts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mPositionData[mVerts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mPositionData[mVerts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	Vector2 uv0 = mUVData[mVerts[triangle.vertexIndex[0]].uvIndex];
	Vector2 uv1 = mUVData[mVerts[triangle.vertexIndex[1]].uvIndex];
	Vector2 uv2 = mUVData[mVerts[triangle.vertexIndex[2]].uvIndex];

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
	Vector3 pos0 = mPositionData[mVerts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = mPositionData[mVerts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = mPositionData[mVerts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	Vector2 uv0 = mUVData[mVerts[triangle.vertexIndex[0]].uvIndex];
	Vector2 uv1 = mUVData[mVerts[triangle.vertexIndex[1]].uvIndex];
	Vector2 uv2 = mUVData[mVerts[triangle.vertexIndex[2]].uvIndex];

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
