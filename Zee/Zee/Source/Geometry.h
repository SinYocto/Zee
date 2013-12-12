#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"
#include "Bound.h"

// #资源管理
// 设备丢失时要释放VertexBuffer, IndexBuffer, VertexDeclaration(这个不确定)资源, 恢复时重建
// 除了上面3个, 顶点的数据也是new出来的, 需要在析构时delete

// TODO:重新设计geometry顶点数据的构建方式(参考OBJ格式)
// 当前的设计是: 对于顶点的各项信息(pos, uv, TBN)使用posData, uvData等数组来保存数据
// 设计的关键是这些Data数组的长度是一致的, 同一个下标对应同一个顶点, 这样的缺点是为了保证同一下标同一顶点, 
// 会有Data数据重复(比如同pos不同uv, 必须重复多个pos数据), 优点是构建总的顶点数据简单
// 现在要改为: 各个data独立, 不必保证同一下标同一顶点。对于每项顶点数据, 单独使用一个FaceIndex
// 这样对于任意一个面, 独立使用各项数据的index来构建总的顶点数据, 优点是节省了单个data数组的空间, 
// 但增加了最终构建的顶点数据占用的空间大小(最终构建的顶点数为3*numTrianges, 无需indexData, index为0, 1, 2, 3, 4, 5, ...)

class AABBox;

class Geometry : public IReferenceCounted
{
public:
	struct Vert			// Vert结构保存的是各个数据项的在相应data中的索引
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

	struct Triangle		// Triangle保存三个顶点在Verts中的索引
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
	// TODO:OBJParser要更改这些数据, 暂时设为public
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