// [ref]:http://www.flipcode.com/archives/article_geomipmaps.pdf

#include "Terrain.h"
#include "Camera.h"
#include "Shader.h"
#include "DebugDrawer.h"
#include "Engine.h"

LPD3DXEFFECT Terrain::sEffect = NULL;

void TerrainChunk::CalculateChunkNormals()
{
	int numVerts = mSize * mSize;
	int numTris = (mSize - 1) * (mSize - 1) * 2;

	DWORD* indices = new DWORD[3 * numTris];	// 只是用于计算normal, 没实际使用
	for(int i = 0; i < mSize - 1; ++i)
	{
		for(int j = 0; j < mSize - 1; ++j)
		{
			indices[6*((mSize - 1)*i + j) + 0] = mSize*i + j;
			indices[6*((mSize - 1)*i + j) + 1] = mSize*i + j+1;
			indices[6*((mSize - 1)*i + j) + 2] = mSize*(i+1) + j;

			indices[6*((mSize - 1)*i + j) + 3] = mSize*(i+1) + j;
			indices[6*((mSize - 1)*i + j) + 4] = mSize*i + j+1;
			indices[6*((mSize - 1)*i + j) + 5] = mSize*(i+1) + j+1;
		}
	}

	mNormalData.clear();

	for(int i = 0; i < numVerts; ++i){
		mNormalData.push_back(Vector3::Zero);
	}

	for(int i = 0; i < numTris; ++i){
		Vector3 v0 = mPosData[indices[3*i + 0]];
		Vector3 v1 = mPosData[indices[3*i + 1]];
		Vector3 v2 = mPosData[indices[3*i + 2]];

		Vector3 v0v1 = v1 - v0;
		Vector3 v0v2 = v2 - v0;

		Vector3 normal = v0v2.Cross(v0v1);
		normal.Normalize();

		for(int k = 0; k < 3; ++k)
		{
			mNormalData[indices[3 * i + k]] += normal;
		}
	}

	for(int i = 0; i < numVerts; ++i)
	{
		mNormalData[i].Normalize();
	}

	delete[] indices;
}

void TerrainChunk::CreateVertexBuffer()
{
	SAFE_RELEASE(mVertexBuffer);

	int numVerts = mSize * mSize;
	VertexUVN* vertexData = new VertexUVN[numVerts];

	for(int i = 0; i < numVerts; ++i)
	{
		VertexUVN vert = VertexUVN(mPosData[i].x, mPosData[i].y, mPosData[i].z, 
			mNormalData[i].x, mNormalData[i].y, mNormalData[i].z, mUVData[i].x, mUVData[i].y);

		vertexData[i] = vert;
	}

	CreateVB(gEngine->GetDriver()->GetD3DDevice(), &mVertexBuffer, (void*)vertexData, numVerts, XYZ_UV_N);

	delete[] vertexData;
}

void TerrainChunk::CreateIndexBuffer(int lodLeft, int lodTop, int lodRight, int lodBottom)
{
	SAFE_RELEASE(mIndexBuffer);

	std::vector<DWORD> indices;
	int lodChunkSize = (mSize - 1) / (int)pow(2.0f, mLODLevel) + 1;

	if(lodChunkSize == 2)
	{
		indices.resize(6, 0);
		indices[0] = 0;
		indices[1] = mSize - 1;
		indices[2] = mSize * (mSize - 1);

		indices[3] = mSize * (mSize - 1);
		indices[4] = mSize - 1;
		indices[5] = mSize * mSize - 1;
	}
	else
	{
		int interval = (int)pow(2.0f, mLODLevel);

		// left
		{
			int leftInterval = (int)pow(2.0f, lodLeft);
			if(leftInterval < interval)
				leftInterval = interval;

			for(int row = 0; row < mSize - 1; row += leftInterval)
			{
				for(int r = row; r < row + leftInterval; r += interval)
				{
					if(r >= 0 && r + interval <= mSize - 1)
					{
						indices.push_back(row * mSize);
						indices.push_back(r * mSize + interval);
						indices.push_back((r + interval) * mSize + interval);
					}
				}

				indices.push_back(row * mSize);
				indices.push_back((row + leftInterval) * mSize + interval);
				indices.push_back((row + leftInterval) * mSize);
			}
		}
		 
		// right
		{
			int rightInterval = (int)pow(2.0f, lodRight);
			if(rightInterval < interval)
				rightInterval = interval;

			for(int row = 0; row < mSize - 1; row += rightInterval)
			{
				for(int r = row; r < row + rightInterval; r += interval)
				{
					if(r >= 0 && r + interval <= mSize - 1)
					{
						indices.push_back(r * mSize + mSize - 1 - interval);
						indices.push_back(row * mSize + mSize - 1);
						indices.push_back((r + interval) * mSize + mSize - 1 - interval);
					}
				}

				indices.push_back(row * mSize + mSize - 1);
				indices.push_back((row + rightInterval) * mSize + mSize - 1);
				indices.push_back((row + rightInterval) * mSize + mSize - 1 - interval);
			}
		}

		// top
		{
			int topInterval = (int)pow(2.0f, lodTop);
			if(topInterval < interval)
				topInterval = interval;

			for(int column = 0; column < mSize - 1; column += topInterval)
			{
				for(int c = column; c < column + topInterval; c += interval)
				{
					if(c >= 0 && c + interval <= mSize - 1)
					{
						indices.push_back(column);
						indices.push_back(interval * mSize + c + interval);
						indices.push_back(interval * mSize + c);
					}
				}

				indices.push_back(column);
				indices.push_back(column + topInterval);
				indices.push_back(interval * mSize + column + topInterval);
			}
		}

		// bottom
		{
			int bottomInterval = (int)pow(2.0f, lodBottom);
			if(bottomInterval < interval)
				bottomInterval = interval;

			for(int column = 0; column < mSize - 1; column += bottomInterval)
			{
				for(int c = column; c < column + bottomInterval; c += interval)
				{
					if(c >= 0 && c + interval <= mSize - 1)
					{
						indices.push_back((mSize - 1) * mSize + column);
						indices.push_back((mSize - 1 - interval) * mSize + c);
						indices.push_back((mSize - 1 - interval) * mSize + c + interval);
					}
				}

				indices.push_back((mSize - 1) * mSize + column);
				indices.push_back((mSize - 1 - interval) * mSize + column + bottomInterval);
				indices.push_back((mSize - 1) * mSize + column + bottomInterval);
			}
		}

		// center 
		{
			for(int row = interval; row < mSize - 1 - interval; row += interval)
			{
				for(int column = interval; column < mSize - 1 - interval; column += interval)
				{
					indices.push_back(mSize * row + column);
					indices.push_back(mSize * row + column + interval);
					indices.push_back(mSize * (row + interval) + column);

					indices.push_back(mSize * (row + interval) + column);
					indices.push_back(mSize * row + column + interval);
					indices.push_back(mSize * (row + interval) + column + interval);
				}
			}
		}
	}

	_Assert(indices.size() % 3 == 0);
	mNumTris = (int)indices.size() / 3;

	CreateIB(gEngine->GetDriver()->GetD3DDevice(), &mIndexBuffer, (DWORD*)(&indices[0]), 3 * mNumTris);
}

bool TerrainChunk::IsInFrustum()
{
	_Assert(NULL != mNode);
	return mNode->IsInFrustum();
}

int TerrainChunk::GetLODLevel()
{
	return mLODLevel;
}

int TerrainChunk::GetTriCounts()
{
	return mNumTris;
}

DWORD TerrainChunk::GetNeighbLODLevel()
{
	return mNeighbLODLevel;
}

void TerrainChunk::SetNeighbLODLevel(DWORD neighbLod)
{
	mNeighbLODLevel = neighbLod;
}

void TerrainChunk::CalcChunkLODDist(Camera* camera, float pixelTolerance)
{
	_Assert(mPosData.size() == mSize * mSize);
	_Assert(pixelTolerance > 0);

	float factor = 0;
	{
		float nz = 0;
		float fov = 0;
		float aspect = 0;
		Vector2 res(0, 0);

		camera->GetCameraParams(&nz, NULL, &fov, &aspect);
		gEngine->GetDriver()->GetViewPort(NULL, &res);

		float top = tan(fov / 2.0f) * nz;
		factor = nz * res.x / (top * 2 * pixelTolerance);
	}

	mLODDelta.push_back(0);		// delta[0] = 0, 不使用
	mLODDist.push_back(0);

	int interval = 2;			// level 1
	while(interval < mSize)
	{
		float maxDelta = 0;
		for(int row = 0; row < mSize; row += interval)
		{
			for(int column = 0; column < mSize - 1; column += interval)
			{
				float h0 = mPosData[row * mSize + column].y;
				float h1 = mPosData[row * mSize + column + interval].y;
				for(int c = column + 1; c < column + interval; ++c)
				{
					float vertHeight = mPosData[row * mSize + column].y;
					float interpolateHeight = h0 + (h1 - h0) * ((float)(c - column) / interval);

					float delta = fabsf(interpolateHeight - vertHeight);
					if(delta > maxDelta)
						maxDelta = delta;
				}
			}
		}

		mLODDelta.push_back(maxDelta);
		mLODDist.push_back(factor * maxDelta);
		interval *= 2;
	}
}

void TerrainChunk::AdjustLODLevel(const Vector3 cameraPos)
{
	Vector3 chunkCenter = mNode->GetAABBox().GetCenter();

	float dist = VectorLength(chunkCenter - cameraPos);

	int lodLevel = 0;
	for(size_t i = 1; i < mLODDist.size(); ++i, ++lodLevel)
	{
		if(dist < mLODDist[i])
			break;
	}
	
	mForceRebuildIB = false;
	if(mLODLevel != lodLevel)
	{	
		mLODLevel = lodLevel;
		mForceRebuildIB = true;
	}
}

int TerrainChunk::GetChunkSize()
{
	return mSize;
}

bool TerrainChunk::NeedForceRebuildIB()
{
	return mForceRebuildIB;
}

void TerrainChunk::ResetLODLevel()
{
	mLODLevel = 0;
	mNeighbLODLevel = 0xffffffff;
}

Terrain::Terrain(int size, float length, float height)
:mSize(size)
,mLength(length)
,mHeight(height)
,mHeightMapData(NULL)
,mRootNode(NULL)
,mDrawBBox(false)
{
	if(!sEffect)
		createEffect();
} 

void Terrain::OnLostDevice()
{
	sEffect->OnLostDevice();

	for(std::vector<TerrainChunk*>::iterator iter = mChunks.begin(); iter != mChunks.end(); ++iter)
	{
		TerrainChunk* chunk = *iter;
		SAFE_RELEASE(chunk->mVertexBuffer);
		SAFE_RELEASE(chunk->mIndexBuffer);
	}
}

void Terrain::OnResetDevice()
{
	sEffect->OnResetDevice();

	for(std::vector<TerrainChunk*>::iterator iter = mChunks.begin(); iter != mChunks.end(); ++iter)
	{
		TerrainChunk* chunk = *iter;
		chunk->CreateVertexBuffer();
		chunk->ResetLODLevel();
	}
}

void Terrain::createEffect()
{
	SAFE_RELEASE(sEffect);

	D3DXCreateEffectFromFile(gEngine->GetDriver()->GetD3DDevice(), L"./Source/Shaders/Terrain.fx", NULL, NULL, 
		D3DXSHADER_DEBUG, UtilityShader::pool, &sEffect, NULL);

	_Assert(NULL != sEffect);
}

void Terrain::LoadFromHeightMap(const wchar_t* fileName, int heightMapSize)
{
	YFile* file = YFile::Open(fileName, YFile::READ_BINARY);
	_Assert(NULL != file);

	mSize = heightMapSize;
	mHeightMapData = new WORD[heightMapSize * heightMapSize];
	file->Read(mHeightMapData, heightMapSize * heightMapSize * sizeof(WORD), sizeof(WORD), heightMapSize * heightMapSize);

	file->Close();
}

void Terrain::BuildTerrain(int depth)
{
	mRootNode = new QuadTreeNode(0, 0, mLength / 2.0f, depth);

	mChunkCounts = (int)pow(2.0f, depth);
	mChunks.resize(mChunkCounts * mChunkCounts, NULL);
	buildChunks(mRootNode, depth);
}

void Terrain::buildChunks(QuadTreeNode* node, int depth)
{
	if(node->GetDepth() == 0)
	{
		int chunkSize = (mSize - 1) / mChunkCounts + 1;  // 每个chunk的边顶点数
		float chunkLength = node->GetHalfSize() * 2;
		float chunkCenterX = node->GetCenter().x;
		float chunkCenterZ = node->GetCenter().y;

		int chunkColumn = (int)((chunkCenterX - chunkLength / 2.0f + (mChunkCounts / 2)*chunkLength) / chunkLength);
		int chunkRow = (int)(((mChunkCounts / 2)*chunkLength - (chunkCenterZ + chunkLength / 2.0f)) / chunkLength); 

		// chunk的第一个顶点对应在heightMapData中的row和column
		int heightMapBaseRow = (chunkSize - 1)*chunkRow;
		int heightMapBaseColumn = (chunkSize - 1)*chunkColumn;

		int numChunkVerts = chunkSize * chunkSize;
		int numChunkTris = (chunkSize - 1) * (chunkSize - 1) * 2;

		TerrainChunk* chunk = new TerrainChunk(node, chunkSize);
		chunk->mRow = chunkRow;
		chunk->mColumn = chunkColumn;
		chunk->mPosData.reserve(numChunkVerts);
		chunk->mUVData.reserve(numChunkVerts);
		chunk->mNormalData.reserve(numChunkVerts);

		float minY = FLT_MAX;		// chunk的高度范围, 用于计算BBox然后进行ViewFrustumCulling
		float maxY = -FLT_MAX;
		for(int i = 0; i < numChunkVerts; ++i)
		{
			int row = i / chunkSize;
			int column = i % chunkSize;

			float x = chunkCenterX + ((float)column - (chunkSize - 1)/2) * chunkLength / (chunkSize - 1);
			float z = chunkCenterZ + ((chunkSize - 1)/2 - (float)row) * chunkLength / (chunkSize - 1);

			// 高度图使用了16位, 可表示的范围为0~65535, 偏移32767, 可表示-32767~32768
			float y = (mHeightMapData[(mSize - 1 - (heightMapBaseRow + row))*mSize + heightMapBaseColumn + column] - 32767) 
				* mHeight / 32768.0f;

			if(y < minY)
				minY = y;

			if(y > maxY)
				maxY = y;

			float u = (float)(heightMapBaseColumn + column)/(mSize - 1);
			float v = (float)(heightMapBaseRow + row)/(mSize - 1);

			chunk->mPosData.push_back(Vector3(x, y, z));
			chunk->mUVData.push_back(Vector2(u, v)); 
		}

		chunk->CalculateChunkNormals();
		chunk->CreateVertexBuffer();
		chunk->mNode->CalculateBoundingBox(minY, maxY);

		int chunkIndex = mChunkCounts * chunkRow + chunkColumn;
		_Assert(chunkIndex < mChunkCounts * mChunkCounts);
		mChunks[chunkIndex] = chunk;
	}
	else
	{
		buildChunks(node->GetLeftTop(), depth);
		buildChunks(node->GetRightTop(), depth);
		buildChunks(node->GetLeftBottom(), depth);
		buildChunks(node->GetRightBottom(), depth);
	}
}

void Terrain::CalcChunkLODDist(Camera* camera, float pixelTolerance)
{
	for(size_t i = 0; i < mChunks.size(); ++i)
	{
		TerrainChunk* chunk = mChunks[i];
		chunk->CalcChunkLODDist(camera, pixelTolerance);
	}
}

void Terrain::FrameUpdate(Camera* camera)
{
	mRootNode->EvaluateVisibility(camera);

	for(size_t i = 0; i < mChunks.size(); ++i)
	{
		TerrainChunk* chunk = mChunks[i];

		if(chunk->IsInFrustum())
			chunk->AdjustLODLevel(camera->GetWorldPosition());
	}

	int numVisibleChunks = 0;
	for(size_t i = 0; i < mChunks.size(); ++i)
	{
		TerrainChunk* chunk = mChunks[i];

		int row = i / mChunkCounts;
		int column = i % mChunkCounts;

		int lodLeft = 0;
		int lodTop = 0;
		int lodRight = 0;
		int lodBottom = 0;

		if(column > 0)
		{
			TerrainChunk* chunkLeft = mChunks[mChunkCounts * row + column - 1];
			lodLeft = chunkLeft->GetLODLevel();
		}

		if(column < mChunkCounts - 1)
		{
			TerrainChunk* chunkRight = mChunks[mChunkCounts * row + column + 1];
			lodRight = chunkRight->GetLODLevel();
		}

		if(row > 0)
		{
			TerrainChunk* chunkTop = mChunks[mChunkCounts * (row - 1) + column];
			lodTop = chunkTop->GetLODLevel();
		}

		if(row < mChunkCounts - 1)
		{
			TerrainChunk* chunkBottom = mChunks[mChunkCounts * (row + 1) + column];
			lodBottom = chunkBottom->GetLODLevel();
		}

		DWORD neighbLod = D3DCOLOR_ARGB(lodLeft, lodTop, lodRight, lodBottom);

		if(chunk->IsInFrustum())
			++numVisibleChunks;

		if(chunk->IsInFrustum() && (chunk->NeedForceRebuildIB() || neighbLod != chunk->GetNeighbLODLevel()))		// neighbLod改变才需要重新生成IB
		{
			chunk->CreateIndexBuffer(lodLeft, lodTop, lodRight, lodBottom);
			chunk->SetNeighbLODLevel(neighbLod);
		}
	}

	//Log(L"chunks: %d\n", numVisibleChunks);
}

void Terrain::Draw(Camera* camera, bool isSolid)
{
	_Assert(sEffect);

	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	sEffect->SetTechnique("Terrain");
	sEffect->SetMatrix("matWVP", &(camera->ViewMatrix() * camera->ProjMatrix()));
	sEffect->SetMatrix("matWorld", &(IDENTITY_MATRIX));
	sEffect->SetMatrix("matUVTransform", &uvTransformMatrix());
	sEffect->SetTexture("colorTex0", mMaterial.colorTexes[0]);
	sEffect->SetTexture("colorTex1", mMaterial.colorTexes[1]);
	sEffect->SetTexture("colorTex2", mMaterial.colorTexes[2]);
	sEffect->SetTexture("colorTex3", mMaterial.colorTexes[3]);
	sEffect->SetTexture("splatMapTex", mMaterial.splatMapTex);
	sEffect->SetRawValue("mtlAmbient", &mMaterial.ambientColor, 0, sizeof(D3DXCOLOR));
	sEffect->SetRawValue("mtlDiffuse", &mMaterial.diffuseColor, 0, sizeof(D3DXCOLOR));

	if(isSolid)
		d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	else
		d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	sEffect->Begin(0, 0);
	sEffect->BeginPass(0);

	for(std::vector<TerrainChunk*>::iterator iter = mChunks.begin(); iter != mChunks.end(); ++iter)
	{
		TerrainChunk* chunk = *iter;

		if(!chunk->mNode->IsInFrustum())
			continue;

		d3dDevice->SetStreamSource(0, chunk->mVertexBuffer, 0, SizeofVertex(XYZ_UV_N));
		d3dDevice->SetIndices(chunk->mIndexBuffer);
		d3dDevice->SetFVF(VertexUVN::FVF);

		int numVerts = chunk->mSize * chunk->mSize;

		d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, chunk->GetTriCounts());

		if(mDrawBBox && chunk->mNode->GetAABBox().isValid())
		{
			DebugDrawer::DrawAABBox(chunk->mNode->GetAABBox(), 0xffff0000, camera);		// #耗时#
		}
	}

	sEffect->EndPass();
	sEffect->End();

	d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

D3DXMATRIX Terrain::uvTransformMatrix()
{
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, mMaterial.tilesU, mMaterial.tilesV, 1.0f);

	return matScale;
}

void Terrain::SetColorTexes(const wchar_t* texFile0, const wchar_t* texFile1, const wchar_t* texFile2, const wchar_t* texFile3)
{
	SAFE_RELEASE(mMaterial.colorTexes[0]);
	SAFE_RELEASE(mMaterial.colorTexes[1]);
	SAFE_RELEASE(mMaterial.colorTexes[2]);
	SAFE_RELEASE(mMaterial.colorTexes[3]);

	IDirect3DDevice9* d3dDevice = gEngine->GetDriver()->GetD3DDevice();

	D3DXCreateTextureFromFile(d3dDevice, texFile0, &mMaterial.colorTexes[0]);	
	D3DXCreateTextureFromFile(d3dDevice, texFile1, &mMaterial.colorTexes[1]);	
	D3DXCreateTextureFromFile(d3dDevice, texFile2, &mMaterial.colorTexes[2]);	
	D3DXCreateTextureFromFile(d3dDevice, texFile3, &mMaterial.colorTexes[3]);		
}

void Terrain::SetSplatMapTex(const wchar_t* texFile)
{
	SAFE_RELEASE(mMaterial.splatMapTex);
	D3DXCreateTextureFromFile(gEngine->GetDriver()->GetD3DDevice(), texFile, &mMaterial.splatMapTex);
}

void Terrain::SetMtlParameters(float tilesU, float tilesV, D3DXCOLOR ambient, D3DXCOLOR diffuse)
{
	mMaterial.tilesU = tilesU;
	mMaterial.tilesV = tilesV;
	mMaterial.ambientColor = ambient;
	mMaterial.diffuseColor = diffuse;
}

TerrainChunk* Terrain::getChunk(int row, int column)
{
	int chunkIndex = row * mChunkCounts + column;
	_Assert(chunkIndex < (int)mChunks.size());

	return mChunks[chunkIndex];
}
