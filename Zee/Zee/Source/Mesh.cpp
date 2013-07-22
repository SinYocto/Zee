#include "Mesh.h"
#include <fstream>
#include <map>

void Mesh::createVertexBuffer(void* vertexData)
{
	CreateVB(gD3DDevice, &vertexBuffer, vertexData, verts.size(), vertexType);
}

void Mesh::createIndexBuffer()
{
	int numIndices = 3 * triangleList.size();

	DWORD* indexData = new DWORD[numIndices];
	for(size_t triIndex = 0; triIndex < triangleList.size(); ++triIndex)
	{
		indexData[3*triIndex + 0] = triangleList[triIndex].vertexIndex[0];
		indexData[3*triIndex + 1] = triangleList[triIndex].vertexIndex[1];
		indexData[3*triIndex + 2] = triangleList[triIndex].vertexIndex[2];
	}

	CreateIB(gD3DDevice, &indexBuffer, indexData, numIndices);
}

// BuildGeometry之前先要获取相应VertexType所需的顶点数据
void Mesh::BuildGeometry(VertexType type)
{
	vertexType = type;

	void* vertexData = NULL;
	bool needToDeleteVertexData = false;

	int numVertices = (int)verts.size();

	switch(vertexType)
	{
		case XYZ:
			{
				vertexData = new Vertex[numVertices];
				needToDeleteVertexData = true;

				for(int i = 0; i < numVertices; ++i)
				{
					Vertex vert = Vertex(positionData[verts[i].posIndex].x, 
						positionData[verts[i].posIndex].y, 
						positionData[verts[i].posIndex].z);

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
					VertexUV vert = VertexUV(positionData[verts[i].posIndex].x, 
						positionData[verts[i].posIndex].y, 
						positionData[verts[i].posIndex].z, 
						uvData[verts[i].uvIndex].x, 
						uvData[verts[i].uvIndex].y);

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
					VertexN vert = VertexN(positionData[verts[i].posIndex].x, 
						positionData[verts[i].posIndex].y, 
						positionData[verts[i].posIndex].z, 
						normalData[verts[i].normalIndex].x, 
						normalData[verts[i].normalIndex].y,
						normalData[verts[i].normalIndex].z);

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
					VertexUVN vert = VertexUVN(positionData[verts[i].posIndex].x, 
						positionData[verts[i].posIndex].y, 
						positionData[verts[i].posIndex].z, 
						uvData[verts[i].uvIndex].x, 
						uvData[verts[i].uvIndex].y,
						normalData[verts[i].normalIndex].x, 
						normalData[verts[i].normalIndex].y,
						normalData[verts[i].normalIndex].z);

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
					VertexUVTBN vert = VertexUVTBN(positionData[verts[i].posIndex].x, 
						positionData[verts[i].posIndex].y, 
						positionData[verts[i].posIndex].z, 
						uvData[verts[i].uvIndex].x, 
						uvData[verts[i].uvIndex].y,
						tangentData[verts[i].tangentIndex].x, 
						tangentData[verts[i].tangentIndex].y,
						tangentData[verts[i].tangentIndex].z,
						bitangentData[verts[i].bitangentIndex].x, 
						bitangentData[verts[i].bitangentIndex].y,
						bitangentData[verts[i].bitangentIndex].z,
						normalData[verts[i].normalIndex].x, 
						normalData[verts[i].normalIndex].y,
						normalData[verts[i].normalIndex].z);

					((VertexUVTBN*)vertexData)[i] = vert;
				}
				break;
			}
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

void Mesh::createVertexDeclaration()
{
	switch(vertexType)
	{
	case XYZ:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},      
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	case XYZ_UV:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},         
				{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},      
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	case XYZ_N:
		{
			D3DVERTEXELEMENT9 NMVertexElements[] =  {
				{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},   
				{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},        
				D3DDECL_END()
			};

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
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

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
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

			if(vertexDecl)
				SAFE_RELEASE(vertexDecl);

			gD3DDevice->CreateVertexDeclaration(NMVertexElements, &vertexDecl);
			break;
		}
	}
}

void Mesh::SetVertexDeclaration()
{
	gD3DDevice->SetVertexDeclaration(vertexDecl);
}

void Mesh::Draw()
{
	gD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, verts.size(), 0, triangleList.size());
}

void Mesh::SetVertexStream()
{	
	gD3DDevice->SetStreamSource(0, vertexBuffer, 0, SizeofVertex(vertexType));
	gD3DDevice->SetIndices(indexBuffer);
}

void Mesh::processSmoothNormal(const Vector3& curPos, const TriangleList& overAllTriGroup)
{
	TriangleSmoothGroupMap triangleSmoothGroupMap;		// 三角面属于哪个smoothgroup
	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		triangleSmoothGroupMap[overAllTriGroup[i]] = NO_GROUP;
	}

	std::vector<TriangleList> triSmoothGroups;		// 包含这个顶点的面可分为几个smoothgroup

	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		const Triangle& curTri = overAllTriGroup[i];
		buildNormalSmoothGroup(curPos, overAllTriGroup, curTri, triangleSmoothGroupMap, triSmoothGroups);
	}

	for(size_t i = 0; i < triSmoothGroups.size(); ++i)		// 对于每个smoothgroup, 计算normal并更新顶点数据
	{
		TriangleList& curGroup = triSmoothGroups[i];

		Vector3 groupNormal = Vector3::Zero;
		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Vector3 triNormal;
			calculateTriangleNormal(curGroup[triIx], &triNormal);

			groupNormal += triNormal;		// TODO:面积加成 
		}

		groupNormal.Normalize();

		normalData.push_back(groupNormal);		// normal加入到normaldata数据中

		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = curGroup[triIx];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(positionData[verts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(verts[vertIx].normalIndex == INVALID_INDEX)
			{
				verts[vertIx].normalIndex = normalData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(normalData[verts[vertIx].normalIndex], groupNormal, 0.01f))
				{
					Vert newVert = verts[vertIx];
					newVert.normalIndex = normalData.size() - 1;

					curTri.vertexIndex[indexInTri] = verts.size() - 1;
				}
			}
		}
	}
}


void Mesh::processSmoothTangent(const Vector3& curPos, const TriangleList& overAllTriGroup)
{
	TriangleSmoothGroupMap triangleSmoothGroupMap;		// 三角面属于哪个smoothgroup
	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		triangleSmoothGroupMap[overAllTriGroup[i]] = NO_GROUP;
	}

	std::vector<TriangleList> triSmoothGroups;		// 包含这个顶点的面可分为几个smoothgroup

	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		const Triangle& curTri = overAllTriGroup[i];
		buildTangentSmoothGroup(curPos, overAllTriGroup, curTri, triangleSmoothGroupMap, triSmoothGroups);
	}

	for(size_t i = 0; i < triSmoothGroups.size(); ++i)		// 对于每个smoothgroup, 计算normal并更新顶点数据
	{
		TriangleList& curGroup = triSmoothGroups[i];

		Vector3 groupTangent = Vector3::Zero;
		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Vector3 triTangent;
			calculateTriangleTangent(curGroup[triIx], &triTangent);

			groupTangent += triTangent;		// TODO:面积加成 
		}

		groupTangent.Normalize();

		tangentData.push_back(groupTangent);		// normal加入到normaldata数据中

		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = curGroup[triIx];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(positionData[verts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(verts[vertIx].tangentIndex == INVALID_INDEX)
			{
				verts[vertIx].tangentIndex = tangentData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(tangentData[verts[vertIx].tangentIndex], groupTangent, 0.01f))
				{
					Vert newVert = verts[vertIx];
					newVert.tangentIndex = tangentData.size() - 1;

					curTri.vertexIndex[indexInTri] = verts.size() - 1;
				}
			}
		}
	}
}


void Mesh::processSmoothBitangent(const Vector3& curPos, const TriangleList& overAllTriGroup)
{
	TriangleSmoothGroupMap triangleSmoothGroupMap;		// 三角面属于哪个smoothgroup
	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		triangleSmoothGroupMap[overAllTriGroup[i]] = NO_GROUP;
	}

	std::vector<TriangleList> triSmoothGroups;		// 包含这个顶点的面可分为几个smoothgroup

	for(size_t i = 0; i < overAllTriGroup.size(); ++i)
	{
		const Triangle& curTri = overAllTriGroup[i];
		buildBitangentSmoothGroup(curPos, overAllTriGroup, curTri, triangleSmoothGroupMap, triSmoothGroups);
	}

	for(size_t i = 0; i < triSmoothGroups.size(); ++i)		// 对于每个smoothgroup, 计算normal并更新顶点数据
	{
		TriangleList& curGroup = triSmoothGroups[i];

		Vector3 groupBitangent = Vector3::Zero;
		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Vector3 triBitangent;
			calculateTriangleBitanget(curGroup[triIx], &triBitangent);

			groupBitangent += triBitangent;		// TODO:面积加成 
		}

		groupBitangent.Normalize();

		bitangentData.push_back(groupBitangent);		// normal加入到normaldata数据中

		for(size_t triIx = 0; triIx < curGroup.size(); ++triIx)
		{
			Triangle& curTri = curGroup[triIx];

			int vertIx = INVALID_INDEX;		// smoothgroup共位置的顶点在当前三角面中是哪个顶点
			int indexInTri = 0;
			for(int k = 0; k < 3; ++k)
			{
				if(Vector3Equal(positionData[verts[curTri.vertexIndex[k]].posIndex], curPos, 0.0001f))
				{
					vertIx = curTri.vertexIndex[k];
					indexInTri = k;
					break;
				}
			}

			if(verts[vertIx].bitangentIndex == INVALID_INDEX)
			{
				verts[vertIx].bitangentIndex = bitangentData.size() - 1;
			}
			else
			{
				if(Vector3Unequal(bitangentData[verts[vertIx].bitangentIndex], groupBitangent, 0.01f))
				{
					Vert newVert = verts[vertIx];
					newVert.bitangentIndex = bitangentData.size() - 1;

					curTri.vertexIndex[indexInTri] = verts.size() - 1;
				}
			}
		}
	}
}

void Mesh::calculateTBN(bool calculateNormal, bool calculateTangent, bool calculateBitangent)
{
	if(calculateNormal)
		normalData.clear();

	if(calculateTangent)
		tangentData.clear();

	if(calculateBitangent)
		bitangentData.clear();

	for(size_t i = 0; i < verts.size(); ++i)
	{
		if(calculateNormal)
			verts[i].normalIndex = INVALID_INDEX;

		if(calculateTangent)
			verts[i].tangentIndex = INVALID_INDEX;

		if(calculateBitangent)
			verts[i].bitangentIndex = INVALID_INDEX;
	}

	if(!calculateNormal && !calculateTangent && !calculateBitangent)
		return;

	VertexTrianglesMap vertexTrianglesMap;

	// get vertexTrianglesMap
	for(size_t triIndex = 0; triIndex < triangleList.size(); ++triIndex)
	{
		Triangle& curTri = triangleList[triIndex];		// each triangle

		for(int i = 0; i < 3; ++i)
		{
			Vector3 curPos = positionData[verts[curTri.vertexIndex[i]].posIndex];	// each vertPos of curTri

			VertexTrianglesMap::iterator iter = vertexTrianglesMap.find(curPos);
			if(iter != vertexTrianglesMap.end())
			{
				iter->second.push_back(curTri);
			}
			else
			{
				TriangleList triList;
				triList.push_back(curTri);

				vertexTrianglesMap[curPos] = triList;
			}
		}
	}

	for(VertexTrianglesMap::iterator iter = vertexTrianglesMap.begin(); iter != vertexTrianglesMap.end(); ++iter)
	{
		Vector3 curPos = iter->first;		// for each unique position

		TriangleList& overAllTriGroup = iter->second;

		if(calculateNormal)
			processSmoothNormal(curPos, overAllTriGroup);

		if(calculateTangent)
			processSmoothTangent(curPos, overAllTriGroup);

		if(calculateBitangent)
			processSmoothBitangent(curPos, overAllTriGroup);
	}
}

void Mesh::CalculateNormals()
{
	calculateTBN(true, false, false);
}


void Mesh::buildNormalSmoothGroup(Vector3 curPos, const TriangleList& overAllTriGroup, const Triangle& curTri, 
							TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriangleList>& triSmoothGroups)
{
	if(triSmoothGroupMap[curTri] != NO_GROUP)		// 已处理(分配好smoothgroup)的直接返回
		return;

	const Triangle* neighbourTri0;
	const Triangle* neighbourTri1;

	findNeighbourTriangles(overAllTriGroup, curTri, &neighbourTri0, &neighbourTri1);

	const float minCreaseAngle = PI / 3.0f;
	if(neighbourTri0 != NULL &&
		canSmoothNormal(curTri, *neighbourTri0, minCreaseAngle) && triSmoothGroupMap[*neighbourTri0] != NO_GROUP)
	{
		triSmoothGroupMap[curTri] = triSmoothGroupMap[*neighbourTri0];
		triSmoothGroups[triSmoothGroupMap[curTri]].push_back(curTri);
		return;
	}
	else if(neighbourTri1 != NULL &&
		canSmoothNormal(curTri, *neighbourTri1, minCreaseAngle) && triSmoothGroupMap[*neighbourTri1] != NO_GROUP)
	{
		triSmoothGroupMap[curTri] = triSmoothGroupMap[*neighbourTri1];
		triSmoothGroups[triSmoothGroupMap[curTri]].push_back(curTri);
		return;
	}
	else
	{
		triSmoothGroupMap[curTri] = triSmoothGroups.size();

		TriangleList newSmoothGroup;
		newSmoothGroup.push_back(curTri);

		triSmoothGroups.push_back(newSmoothGroup);
	}

	if(neighbourTri0 != NULL)
		buildNormalSmoothGroup(curPos, overAllTriGroup, *neighbourTri0, triSmoothGroupMap, triSmoothGroups);

	if(neighbourTri1 != NULL)
		buildNormalSmoothGroup(curPos, overAllTriGroup, *neighbourTri1, triSmoothGroupMap, triSmoothGroups);
}


void Mesh::buildTangentSmoothGroup(Vector3 curPos, const TriangleList& overAllTriGroup, const Triangle& curTri, 
								  TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriangleList>& triSmoothGroups)
{
	if(triSmoothGroupMap[curTri] != NO_GROUP)		// 已处理(分配好smoothgroup)的直接返回
		return;

	const Triangle* neighbourTri0;
	const Triangle* neighbourTri1;

	findNeighbourTriangles(overAllTriGroup, curTri, &neighbourTri0, &neighbourTri1);

	const float minCreaseAngle = PI / 3.0f;
	if(neighbourTri0 != NULL &&
		canSmoothTangent(curTri, *neighbourTri0, minCreaseAngle) && triSmoothGroupMap[*neighbourTri0] != NO_GROUP)
	{
		triSmoothGroupMap[curTri] = triSmoothGroupMap[*neighbourTri0];
		triSmoothGroups[triSmoothGroupMap[curTri]].push_back(curTri);
		return;
	}
	else if(neighbourTri1 != NULL &&
		canSmoothTangent(curTri, *neighbourTri1, minCreaseAngle) && triSmoothGroupMap[*neighbourTri1] != NO_GROUP)
	{
		triSmoothGroupMap[curTri] = triSmoothGroupMap[*neighbourTri1];
		triSmoothGroups[triSmoothGroupMap[curTri]].push_back(curTri);
		return;
	}
	else
	{
		triSmoothGroupMap[curTri] = triSmoothGroups.size();

		TriangleList newSmoothGroup;
		newSmoothGroup.push_back(curTri);

		triSmoothGroups.push_back(newSmoothGroup);
	}

	if(neighbourTri0 != NULL)
		buildTangentSmoothGroup(curPos, overAllTriGroup, *neighbourTri0, triSmoothGroupMap, triSmoothGroups);

	if(neighbourTri1 != NULL)
		buildTangentSmoothGroup(curPos, overAllTriGroup, *neighbourTri1, triSmoothGroupMap, triSmoothGroups);
}


void Mesh::buildBitangentSmoothGroup(Vector3 curPos, const TriangleList& overAllTriGroup, const Triangle& curTri, 
								   TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriangleList>& triSmoothGroups)
{
	_Assert(triSmoothGroupMap.find(curTri) != triSmoothGroupMap.end());

	if(triSmoothGroupMap[curTri] != NO_GROUP)		// 已处理(分配好smoothgroup)的直接返回
		return;

	const Triangle* neighbourTri0;
	const Triangle* neighbourTri1;

	findNeighbourTriangles(overAllTriGroup, curTri, &neighbourTri0, &neighbourTri1);

	const float minCreaseAngle = PI / 3.0f;
	if(neighbourTri0 != NULL &&
		canSmoothBitangent(curTri, *neighbourTri0, minCreaseAngle) && triSmoothGroupMap[*neighbourTri0] != NO_GROUP)
	{
		triSmoothGroupMap[curTri] = triSmoothGroupMap[*neighbourTri0];
		triSmoothGroups[triSmoothGroupMap[curTri]].push_back(curTri);
		return;
	}
	else if(neighbourTri1 != NULL &&
		canSmoothBitangent(curTri, *neighbourTri1, minCreaseAngle) && triSmoothGroupMap[*neighbourTri1] != NO_GROUP)
	{
		triSmoothGroupMap[curTri] = triSmoothGroupMap[*neighbourTri1];
		triSmoothGroups[triSmoothGroupMap[curTri]].push_back(curTri);
		return;
	}
	else
	{
		triSmoothGroupMap[curTri] = triSmoothGroups.size();

		TriangleList newSmoothGroup;
		newSmoothGroup.push_back(curTri);

		triSmoothGroups.push_back(newSmoothGroup);
	}

	if(neighbourTri0)
		buildBitangentSmoothGroup(curPos, overAllTriGroup, *neighbourTri0, triSmoothGroupMap, triSmoothGroups);

	if(neighbourTri1)
		buildBitangentSmoothGroup(curPos, overAllTriGroup, *neighbourTri1, triSmoothGroupMap, triSmoothGroups);
}

void Mesh::findNeighbourTriangles(const TriangleList& triGroup, const Triangle& curTri, const Triangle** neighbour0, const Triangle** neighbour1)
{
	_Assert(NULL != neighbour0);
	_Assert(NULL != neighbour1);

	int numNeighboursFound = 0;
	*neighbour0 = NULL;
	*neighbour1 = NULL;

	for(size_t i = 0; i < triGroup.size(); ++i)
	{
		const Triangle& tri = triGroup[i];

		if(tri.vertexIndex[0] == curTri.vertexIndex[0] &&
			tri.vertexIndex[1] == curTri.vertexIndex[1] &&
			tri.vertexIndex[2] == curTri.vertexIndex[2])		// 排除自己
			continue;
		
		if(hasSharedEdge(curTri, tri))
		{
			if(numNeighboursFound == 0)
				*neighbour0 = &tri;
			else
				*neighbour1 = &tri;

			numNeighboursFound++;
		}
	}
}

bool Mesh::hasSharedEdge(const Triangle& triangle0, const Triangle& triangle1)
{
	int numSharedVerts = 0;

	for(int i = 0; i < 3; ++i)
	{
		Vector3& tri0Pos = positionData[verts[triangle0.vertexIndex[i]].posIndex];
		for(int j = 0; j < 3; ++j)
		{
			Vector3& tri1Pos = positionData[verts[triangle1.vertexIndex[j]].posIndex];
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

bool Mesh::canSmoothNormal(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle)
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

bool Mesh::canSmoothTangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle)
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

bool Mesh::canSmoothBitangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle)
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

void Mesh::calculateTriangleNormal(const Triangle& triangle, Vector3* normal)
{
	Vector3 pos0 = positionData[verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = positionData[verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = positionData[verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	*normal = p0p1.Cross(p0p2);

	normal->Normalize();
}

void Mesh::calculateTriangleTangent(const Triangle& triangle, Vector3* tangent)
{
	Vector3 pos0 = positionData[verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = positionData[verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = positionData[verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	Vector2 uv0 = uvData[verts[triangle.vertexIndex[0]].uvIndex];
	Vector2 uv1 = uvData[verts[triangle.vertexIndex[1]].uvIndex];
	Vector2 uv2 = uvData[verts[triangle.vertexIndex[2]].uvIndex];

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

void Mesh::calculateTriangleBitanget(const Triangle& triangle, Vector3* bitangent)
{
	Vector3 pos0 = positionData[verts[triangle.vertexIndex[0]].posIndex];
	Vector3 pos1 = positionData[verts[triangle.vertexIndex[1]].posIndex];
	Vector3 pos2 = positionData[verts[triangle.vertexIndex[2]].posIndex];

	Vector3 p0p1 = pos1 - pos0;
	Vector3 p0p2 = pos2 - pos0;

	Vector2 uv0 = uvData[verts[triangle.vertexIndex[0]].uvIndex];
	Vector2 uv1 = uvData[verts[triangle.vertexIndex[1]].uvIndex];
	Vector2 uv2 = uvData[verts[triangle.vertexIndex[2]].uvIndex];

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
//
//void Mesh::OBJParseLine(char *line, std::vector<Vector3> &filePosData, std::vector<DWORD> &fileIndexData)
//{
//
//	char *lineWordPtr;
//	lineWordPtr = strtok_s(line, " ", NULL);
//	if(!lineWordPtr)
//		return;
//
//	switch(lineWordPtr[0])
//	{
//	case '#':
//		return;
//		break;
//	case 'v':
//		{
//			Vector3 pos;
//
//			lineWordPtr = strtok_s(NULL, " ", NULL);
//			pos.x = (float)atof(lineWordPtr);
//			lineWordPtr = strtok_s(NULL, " ", NULL);
//			pos.y = (float)atof(lineWordPtr);
//			lineWordPtr = strtok_s(NULL, " ", NULL);
//			pos.z = (float)atof(lineWordPtr);
//
//			filePosData.push_back(pos);
//			break;
//		}
//	case 'f':
//		{
//			lineWordPtr = strtok_s(NULL, " ", NULL);
//			fileIndexData.push_back(atoi(lineWordPtr) - 1);
//			lineWordPtr = strtok_s(NULL, " ", NULL);
//			fileIndexData.push_back(atoi(lineWordPtr) - 1);
//			lineWordPtr = strtok_s(NULL, " ", NULL);
//			fileIndexData.push_back(atoi(lineWordPtr) - 1);
//			break;
//		}
//	}
//}

void Mesh::CalculateTBN()
{
	calculateTBN(true, true, true);
}

wchar_t* Mesh::GetName()
{
	return name;
}

void Mesh::SetID( DWORD _id )
{
	id = _id;
}
