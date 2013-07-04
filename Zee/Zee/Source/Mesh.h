#ifndef MESH_H
#define MESH_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"
#include "Bound.h"
#include "Math.h"
#include <map>
#include <vector>

// #��Դ����
// �豸��ʧʱҪ�ͷ�VertexBuffer, IndexBuffer, VertexDeclaration(�����ȷ��)��Դ, �ָ�ʱ�ؽ�
// ��������3��, ���������Ҳ��new������, ��Ҫ������ʱdelete

// TODO:�������mesh�������ݵĹ�����ʽ(�ο�OBJ��ʽ)
// ��ǰ�������: ���ڶ���ĸ�����Ϣ(pos, uv, TBN)ʹ��posData, uvData����������������
// ��ƵĹؼ�����ЩData����ĳ�����һ�µ�, ͬһ���±��Ӧͬһ������, ������ȱ����Ϊ�˱�֤ͬһ�±�ͬһ����, 
// ����Data�����ظ�(����ͬpos��ͬuv, �����ظ����pos����), �ŵ��ǹ����ܵĶ������ݼ�
// ����Ҫ��Ϊ: ����data����, ���ر�֤ͬһ�±�ͬһ���㡣����ÿ�������, ����ʹ��һ��FaceIndex
// ������������һ����, ����ʹ�ø������ݵ�index�������ܵĶ�������, �ŵ��ǽ�ʡ�˵���data����Ŀռ�, 
// �����������չ����Ķ�������ռ�õĿռ��С(���չ����Ķ�����Ϊ3*numTrianges, ����indexData, indexΪ0, 1, 2, 3, 4, 5, ...)

enum UVMappingMode { SphereUV };
enum MeshFileFormat { OBJ };

class Mesh : public IReferenceCounted
{
public:
	Mesh()
		:vertexBuffer(NULL)
		,indexBuffer(NULL)
		,vertexDecl(NULL)
	{

	}

	~Mesh()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		SAFE_RELEASE(vertexDecl);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		SAFE_RELEASE(vertexDecl);
	}

	void OnResetDevice()
	{
		BuildGeometry(vertexType);
	}

//	void LoadDataFromFile(std::string filename, MeshFileFormat format);

	void BuildGeometry(VertexType type);

	void CalculateTBN();
	void CalculateNormals();

//	void CalculateUVs(UVMappingMode uvMode);

	void SetVertexDeclaration();
	void SetVertexStream();

	void Draw();

protected:
	virtual void constructGeometryData() {}

protected:
	enum
	{
		INVALID_INDEX = 0xffffffff,
		NO_GROUP = 0xffffffff,
	};

	struct Vert			// Vert�ṹ������Ǹ��������������Ӧdata�е�����
	{
		Vert(int _posIndex = INVALID_INDEX, int _uvIndex = INVALID_INDEX, int _normalIndex = INVALID_INDEX, 
			int _tangentIndex = INVALID_INDEX, int _bitangentIndex = INVALID_INDEX)
			:posIndex(_posIndex)
			,uvIndex(_uvIndex)
			,normalIndex(_normalIndex)
			,tangentIndex(_tangentIndex)
			,bitangentIndex(_bitangentIndex)
		{

		}

		int posIndex;
		int uvIndex;
		int normalIndex;
		int tangentIndex;
		int bitangentIndex;
	};

	struct Triangle		// Triangle��������������Verts�е�����
	{
		Triangle(int vertIndex0 = 0, int vertIndex1 = 0, int vertIndex2 = 0)
		{
			vertexIndex[0] = vertIndex0;
			vertexIndex[1] = vertIndex1;
			vertexIndex[2] = vertIndex2;
		}

		int vertexIndex[3];
	};
	typedef std::vector<Triangle> TriangleList;

	struct CompareVector
	{
		bool operator()(const Vector3& vec1, const Vector3& vec2)
		{
			return true;
		}
	};
	typedef std::map<Vector3, TriangleList, CompareVector> VertexTrianglesMap;

	struct CompareTriangle
	{
		bool operator()(const Triangle& tri1, const Triangle& tri2)
		{
			return true;
		}
	};
	typedef std::map<Triangle, int, CompareTriangle> TriangleSmoothGroupMap;

private:
	//void OBJParseLine(char *line, std::vector<Vector3> &filePosData, std::vector<DWORD> &fileIndexData);
	void createVertexBuffer(void* vertexData);
	void createIndexBuffer();
	void createVertexDeclaration();
	//void calculateBoundingBox();

	void calculateTBN(bool calculateTangent, bool calculateBitangent);

	void processSmoothNormal(const Vector3& curPos, const TriangleList& overAllTriGroup);
	void processSmoothTangent(const Vector3& curPos, const TriangleList& overAllTriGroup);
	void processSmoothBitangent(const Vector3& curPos, const TriangleList& overAllTriGroup);

	void buildNormalSmoothGroup(Vector3 curPos, const TriangleList& overAllTriGroup, const Triangle& curTri, 
		TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriangleList>& triSmoothGroups);

	void buildTangentSmoothGroup(Vector3 curPos, const TriangleList& overAllTriGroup, const Triangle& curTri, 
		TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriangleList>& triSmoothGroups);

	void buildBitangentSmoothGroup(Vector3 curPos, const TriangleList& overAllTriGroup, const Triangle& curTri, 
		TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriangleList>& triSmoothGroups);

	void findNeighbourTriangles(const TriangleList& triGroup, const Triangle& curTri, const Triangle** neighbour0, const Triangle** neighbour1);
	bool hasSharedEdge(const Triangle& triangle0, const Triangle& triangle1);

	bool canSmoothNormal(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle);
	bool canSmoothTangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle);
	bool canSmoothBitangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle);

	void calculateTriangleNormal(const Triangle& triangle, Vector3* normal);
	void calculateTriangleTangent(const Triangle& triangle, Vector3* tangent);
	void calculateTriangleBitanget(const Triangle& triangle, Vector3* bitangent);

protected:
	std::vector<Vector3> positionData;
	std::vector<Vector2> uvData;
	std::vector<Vector3> normalData;
	std::vector<Vector3> tangentData;
	std::vector<Vector3> bitangentData;

	std::vector<Vert> verts;
	TriangleList triangleList;

private:
	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DIndexBuffer9*  indexBuffer;

	VertexType vertexType;
	IDirect3DVertexDeclaration9 *vertexDecl;

	BoundingBox boundingBox;
};


#endif