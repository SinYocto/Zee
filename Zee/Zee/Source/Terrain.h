#ifndef TERRAIN_H
#define TERRAIN_H

#include "D3DUtility.h"
#include "Common.h"
#include "QuadTreeNode.h"

class Camera;

enum 
{
	INVALID_VALUE = -1
};

class TerrainChunk
{
	friend class Terrain;

public:
	TerrainChunk(QuadTreeNode* node, int size)
		:mVertexBuffer(NULL)
		,mIndexBuffer(NULL)
		,mNode(node)
		,mSize(size)
		,mLODLevel(0)
		,mForceRebuildIB(true)
		,mNeighbLODLevel(0xffffffff)
		,mNumTris(0)
	{

	}

	~TerrainChunk()
	{
		SAFE_RELEASE(mVertexBuffer);
		SAFE_RELEASE(mIndexBuffer);
	}

	void CalculateChunkNormals();
	void CreateVertexBuffer();
	void CreateIndexBuffer(int lodLeft, int lodTop, int lodRight, int lodBottom);

	void SetVertexStream();
	void Draw();

	void CalcChunkLODDist(Camera* camera, float pixelTolerance);
	void AdjustLODLevel(const Vector3 cameraPos);

	bool IsInFrustum();
	bool NeedForceRebuildIB();

	bool IntersectWithBBox(const AABBox& box);

	int GetLODLevel();
	int GetTriCounts();
	DWORD GetNeighbLODLevel();
	int GetChunkSize();

	void SetNeighbLODLevel(DWORD neighbLod);
	void ResetLODLevel();
private:
	int mSize;
	int mRow;
	int mColumn;

	int mLODLevel;
	DWORD mNeighbLODLevel;				// neightbour��lodLevel, left,top,right,bottom��ռ8λ
	bool mForceRebuildIB;

	std::vector<float> mLODDelta;		// delta[n], Ԥ�ȼ���ĸ���lodLevel�����߶�ƫ��
	std::vector<float> mLODDist;		// D[n], Ԥ�ȼ���ĸ���lodLevel�ľ���

	int mNumTris;						// lod���numTris

	std::vector<Vector3> mPosData;
	std::vector<Vector2> mUVData;
	std::vector<Vector3> mNormalData;

	IDirect3DVertexBuffer9* mVertexBuffer;
	IDirect3DIndexBuffer9* mIndexBuffer;

	QuadTreeNode* mNode;
};

class Terrain
{
public:
	Terrain(int size, float length, float height);
	~Terrain()
	{
		delete[] mHeightMapData;

		SAFE_DELETE(mRootNode);

		for(std::vector<TerrainChunk*>::iterator iter = mChunks.begin(); iter != mChunks.end(); ++iter)
		{
			SAFE_DELETE(*iter);
		}
	}

	void OnLostDevice();
	void OnResetDevice();

	void LoadFromHeightMap(const wchar_t* fileName, int heightMapSize);
	void BuildTerrain(int depth);
	void CalcChunkLODDist(Camera* camera, float pixelTolerance);

	void SetColorTexes(const wchar_t* texFile0, const wchar_t* texFile1, const wchar_t* texFile2, const wchar_t* texFile3);
	void SetSplatMapTex(const wchar_t* texFile);
	void SetMtlParameters(float tilesU, float tilesV, D3DXCOLOR ambient, D3DXCOLOR diffuse);

	void FrameUpdate(Camera* camera);

	void Draw(Camera* camera, bool isSolid);
	void createEffect();

	std::vector<TerrainChunk*> GetChunks();

private:
	//void createEffect();
	void buildChunks(QuadTreeNode* node, int depth);
	TerrainChunk* getChunk(int row, int column);

	//void calculateChunkNormal(const Vector3* posData, const DWORD* indices, int numVerts, int numTris, Vector3** normalData);

	D3DXMATRIX uvTransformMatrix();

private:
	QuadTreeNode* mRootNode;
	std::vector<TerrainChunk*> mChunks;
	int mChunkCounts;	// ��chunks��ΪmChunkCounts * mChunkCounts

	int mSize;			// ���δ�С(������)

	float mLength;		// ���δ�С(��λΪm)
	float mHeight;		// �߶�ͼ���ֵ����ĸ߶�

	WORD* mHeightMapData;

	bool mDrawBBox;

	struct TerrainMaterial
	{
		TerrainMaterial()
			:ambientColor(1.0f, 1.0f, 1.0f, 1.0f)
			,diffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
			,tilesU(10.0f)
			,tilesV(10.0f)
			,splatMapTex(NULL)
		{
			colorTexes[0] = NULL;
			colorTexes[1] = NULL;
			colorTexes[2] = NULL;
			colorTexes[3] = NULL;
		}
		~TerrainMaterial()
		{
			SAFE_RELEASE(splatMapTex);

			for(int i = 0; i < 4; ++i)
				SAFE_RELEASE(colorTexes[i]);
		}

		IDirect3DTexture9* colorTexes[4];
		IDirect3DTexture9* splatMapTex;

		D3DXCOLOR ambientColor;
		D3DXCOLOR diffuseColor;

		float tilesU;
		float tilesV;
	};

	TerrainMaterial mMaterial;
	static LPD3DXEFFECT sEffect;
};


#endif