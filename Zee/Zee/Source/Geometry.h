#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Utility.h"
#include "D3DUtility.h"
#include "IReferenceCounted.h"
#include "Bound.h"
#include "Math.h"
#include "YString.h"
#include <map>
#include <vector>

// #��Դ����
// �豸��ʧʱҪ�ͷ�VertexBuffer, IndexBuffer, VertexDeclaration(�����ȷ��)��Դ, �ָ�ʱ�ؽ�
// ��������3��, ���������Ҳ��new������, ��Ҫ������ʱdelete

// TODO:�������geometry�������ݵĹ�����ʽ(�ο�OBJ��ʽ)
// ��ǰ�������: ���ڶ���ĸ�����Ϣ(pos, uv, TBN)ʹ��posData, uvData����������������
// ��ƵĹؼ�����ЩData����ĳ�����һ�µ�, ͬһ���±��Ӧͬһ������, ������ȱ����Ϊ�˱�֤ͬһ�±�ͬһ����, 
// ����Data�����ظ�(����ͬpos��ͬuv, �����ظ����pos����), �ŵ��ǹ����ܵĶ������ݼ�
// ����Ҫ��Ϊ: ����data����, ���ر�֤ͬһ�±�ͬһ���㡣����ÿ�������, ����ʹ��һ��FaceIndex
// ������������һ����, ����ʹ�ø������ݵ�index�������ܵĶ�������, �ŵ��ǽ�ʡ�˵���data����Ŀռ�, 
// �����������չ����Ķ�������ռ�õĿռ��С(���չ����Ķ�����Ϊ3*numTrianges, ����indexData, indexΪ0, 1, 2, 3, 4, 5, ...)

enum UVMappingMode { SphereUV };
enum MeshFileFormat { OBJ };

class Geometry : public IReferenceCounted
{
public:
	Geometry(const wchar_t* _name)
		:vertexBuffer(NULL)
		,indexBuffer(NULL)
		,vertexDecl(NULL)
	{
		YString::Copy(name, _countof(name), _name);
	}

	~Geometry()
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

	wchar_t* GetName();
	void SetID(DWORD _id);

	void Draw();

protected:
	virtual void constructGeometryData() {}

public:
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

protected:
	enum
	{
		INVALID_INDEX = -1,
		NO_GROUP = -1,
	};

	struct CompareVector
	{
		bool operator()(const Vector3& vec1, const Vector3& vec2) const
		{
			if(vec1.x < vec2.x)
				return true;

			if(vec1.x > vec2.x)
				return false;

			if(vec1.y < vec2.y)
				return true;

			if(vec1.y > vec2.y)
				return false;

			if(vec1.z < vec2.z)
				return true;

			return false;
		}
	};
	
	typedef std::map<Vector3, std::vector<int>, CompareVector> VertexTrianglesMap;		// vertPos -> triListByID

	typedef std::map<int, int> TriangleSmoothGroupMap;		// triID -> sgID
	typedef std::vector<int> TriIDList;

private:
	//void OBJParseLine(char *line, std::vector<Vector3> &filePosData, std::vector<DWORD> &fileIndexData);
	void createVertexBuffer(void* vertexData);
	void createIndexBuffer();
	void createVertexDeclaration();
	//void calculateBoundingBox();

	void calculateTBN(bool calculateNormal, bool calculateTangent, bool calculateBitangent);

	void processSmoothNormal(const Vector3& curPos, const TriIDList& overAllTriGroup);
	void processSmoothTangent(const Vector3& curPos, const TriIDList& overAllTriGroup);
	void processSmoothBitangent(const Vector3& curPos, const TriIDList& overAllTriGroup);

	void buildNormalSmoothGroup(Vector3 curPos, const TriIDList& overAllTriGroup, int curTriID, 
		TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriIDList>& triSmoothGroups);

	void buildTangentSmoothGroup(Vector3 curPos, const TriIDList& overAllTriGroup, int curTriID, 
		TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriIDList>& triSmoothGroups);

	void buildBitangentSmoothGroup(Vector3 curPos, const TriIDList& overAllTriGroup, int curTriID, 
		TriangleSmoothGroupMap& triSmoothGroupMap, std::vector<TriIDList>& triSmoothGroups);

	void findNeighbourTriangles(const TriIDList& triGroup, int curTriID, int* neighb0ID, int* neighb1ID);
	bool hasSharedEdge(const Triangle& triangle0, const Triangle& triangle1);

	bool canSmoothNormal(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle);
	bool canSmoothTangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle);
	bool canSmoothBitangent(const Triangle& triangle0, const Triangle& triangle1, float minCreaseAngle);

	void calculateTriangleNormal(const Triangle& triangle, Vector3* normal);
	void calculateTriangleTangent(const Triangle& triangle, Vector3* tangent);
	void calculateTriangleBitanget(const Triangle& triangle, Vector3* bitangent);

public:
	// TODO:OBJParserҪ������Щ����, ��ʱ��Ϊpublic
	std::vector<Vector3> positionData;
	std::vector<Vector2> uvData;
	std::vector<Vector3> normalData;
	std::vector<Vector3> tangentData; 
	std::vector<Vector3> bitangentData;

	std::vector<Vert> verts;
	TriangleList triangleList;

protected:
	DWORD id;
	wchar_t name[MAX_STR_LEN];

private:
	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DIndexBuffer9*  indexBuffer;

	VertexType vertexType;
	IDirect3DVertexDeclaration9 *vertexDecl;

	BoundingBox boundingBox;
};


#endif