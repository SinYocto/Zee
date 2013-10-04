#ifndef TERRAIN_H
#define TERRAIN_H

#include "D3DUtility.h"
#include "Common.h"
#include "QuadTreeNode.h"

class Camera;

class TerrainChunk
{
	friend class Terrain;

public:
	TerrainChunk(QuadTreeNode* node, int size)
		:mVertexBuffer(NULL)
		,mIndexBuffer(NULL)
		,mNode(node)
		,mSize(size)
	{

	}

	~TerrainChunk()
	{
		SAFE_RELEASE(mVertexBuffer);
		SAFE_RELEASE(mIndexBuffer);
	}

public:
	void CalculateChunkNormals();
	void CreateVertexBuffer();
	void CreateIndexBuffer();

private:
	int mSize;

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

	void SetColorTexes(const wchar_t* texFile0, const wchar_t* texFile1, const wchar_t* texFile2, const wchar_t* texFile3);
	void SetSplatMapTex(const wchar_t* texFile);
	void SetMtlParameters(float tilesU, float tilesV, D3DXCOLOR ambient, D3DXCOLOR diffuse);

	void FrameUpdate();

	void Draw(Camera* camera, bool isSolid);
	void createEffect();

private:
	//void createEffect();
	void buildChunks(QuadTreeNode* node, int depth);

	//void calculateChunkNormal(const Vector3* posData, const DWORD* indices, int numVerts, int numTris, Vector3** normalData);

	D3DXMATRIX uvTransformMatrix();

private:
	QuadTreeNode* mRootNode;
	std::vector<TerrainChunk*> mChunks;

	int mSize;			// 地形大小(顶点数)

	float mLength;		// 地形大小(单位为m)
	float mHeight;		// 高度图最大值代表的高度

	WORD* mHeightMapData;

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