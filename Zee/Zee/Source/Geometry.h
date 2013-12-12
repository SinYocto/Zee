#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"
#include "Bound.h"

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

class AABBox;

class Geometry : public IReferenceCounted
{
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

private:
	enum
	{
		INVALID_INDEX = -1,
		NO_GROUP = -1,
	};

	typedef std::map<Vector3, std::vector<int>, Vector3::Comparer> VertexTrianglesMap;		// vertPos -> triListByID

	typedef std::map<int, int> TriangleSmoothGroupMap;		// triID -> sgID
	typedef std::vector<int> TriIDList;

public:
	Geometry(const wchar_t* name);

	~Geometry()
	{
		SAFE_RELEASE(mVertexBuffer);
		SAFE_RELEASE(mIndexBuffer);
		SAFE_RELEASE(mVertexDecl);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(mVertexBuffer);
		SAFE_RELEASE(mIndexBuffer);
		SAFE_RELEASE(mVertexDecl);
	}

	void OnResetDevice()
	{
		BuildGeometry(mVertexType);
	}

	wchar_t* GetName();
	void SetID(DWORD id);

	void CalculateTBN();
	void CalculateNormals();
//	void CalculateUVs(UVMappingMode uvMode);

	void BuildGeometry(VertexType type);

	void SetVertexDeclaration();
	void SetVertexStream();

	VertexType GetVertexType();

	AABBox GetAABBox();
	void CalcDynamicAABBox(const D3DXMATRIX& matWorld, AABBox* box);

	void Draw();

protected:
	virtual void constructGeometryData() {}

private:
	void createVertexBuffer(void* vertexData);
	void createIndexBuffer();
	void createVertexDeclaration();

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

	void calculateTriangleNormalWeighted(const Triangle& triangle, const Vector3& pos, Vector3* normal);
	//void calculateTriangleTangentWeighted(const Triangle& triangle, const Vector3& pos, Vector3* tangent);
	//void calculateTriangleBitangentWeighted(const Triangle& triangle, const Vector3& pos, Vector3* bitangent);

	void calculateAABBox();

public:
	// TODO:OBJParserҪ������Щ����, ��ʱ��Ϊpublic
	std::vector<Vector3> mPositionData;
	std::vector<Vector2> mUVData;
	std::vector<Vector3> mNormalData;
	std::vector<Vector3> mTangentData; 
	std::vector<Vector3> mBitangentData;

	std::vector<Vert> mVerts;
	TriangleList mTriangles;

protected:
	DWORD mID;
	wchar_t mName[MAX_STR_LEN];

private:
	IDirect3DVertexBuffer9* mVertexBuffer;
	IDirect3DIndexBuffer9*  mIndexBuffer;

	VertexType mVertexType;
	IDirect3DVertexDeclaration9 *mVertexDecl;

	AABBox mAABBox;
};


#endif