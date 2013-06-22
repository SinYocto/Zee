#ifndef MESH_H
#define MESH_H

#include"D3DUtility.h"
//#include"Object.h"
#include"Bound.h"
#include<map>
#include<vector>

enum UVMappingMode { SphereUV };
enum MeshFileFormat { OBJ };

class Mesh// : public Object
{
public:
	Mesh()
		:vertexBuffer(NULL)
		,indexBuffer(NULL)
		,vertexDecl(NULL)
		,vertexData(NULL)
		,positionData(NULL)
		,uvData(NULL)
		,normalData(NULL)
		,tangentData(NULL)
		,bitangentData(NULL)
		,indexData(NULL)
	{

	}

	~Mesh()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		SAFE_RELEASE(vertexDecl);

		SAFE_DELETE(vertexData);
		SAFE_DELETE(positionData);
		SAFE_DELETE(uvData);
		SAFE_DELETE(normalData);
		SAFE_DELETE(tangentData);
		SAFE_DELETE(bitangentData);
		SAFE_DELETE(indexData);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		SAFE_RELEASE(vertexDecl);
	}

	void OnResetDevice()
	{
		createVertexBuffer();
		createIndexBuffer(3*numTriangles);
		createVertexDeclaration();
	}

	void LoadDataFromFile(std::string filename, MeshFileFormat format);

	void BuildGeometry(VertexType type);

	void CalculateTBN();

	void CalculateNormals();
	void CalculateTangents();
	void CalculateBitangents();

	void CalculateUVs(UVMappingMode uvMode);

	void SetVertexDeclaration();
	void SetVertexStream();

	void Draw();

protected:
	virtual void constructGeometryData()
	{

	}

private:
	void OBJParseLine(char *line, std::vector<Vector3> &filePosData, std::vector<DWORD> &fileIndexData);
	void createVertexBuffer();
	void createIndexBuffer(int numIndices);
	void createVertexDeclaration();
	void calculateBoundingBox();

protected:
	int numVertices;
	int numTriangles;

	void *vertexData;
	Vector3	*positionData;
	Vector2 *uvData;
	Vector3 *normalData;
	Vector3 *tangentData;
	Vector3 *bitangentData;

	DWORD *indexData;

	std::map<int, int> normalPairs;

private:
	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DIndexBuffer9*  indexBuffer;

	VertexType vertexType;
	IDirect3DVertexDeclaration9 *vertexDecl;

	BoundingBox boundingBox;
};


#endif