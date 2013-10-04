#include "Terrain.h"
#include "Camera.h"
#include "Shader.h"

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

	CreateVB(Driver::D3DDevice, &mVertexBuffer, (void*)vertexData, numVerts, XYZ_UV_N);

	delete[] vertexData;
}

void TerrainChunk::CreateIndexBuffer()
{
	SAFE_RELEASE(mIndexBuffer);

	int numVerts = mSize * mSize;
	int numTris = (mSize - 1) * (mSize - 1) * 2;

	DWORD* indices = new DWORD[3 * numTris];
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

	CreateIB(Driver::D3DDevice, &mIndexBuffer, indices, 3 * numTris);
}


Terrain::Terrain(int size, float length, float height)
:mSize(size)
,mLength(length)
,mHeight(height)
,mHeightMapData(NULL)
,mRootNode(NULL)
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
		chunk->CreateIndexBuffer();
	}
}

void Terrain::createEffect()
{
	SAFE_RELEASE(sEffect);

	D3DXCreateEffectFromFile(Driver::D3DDevice, L"./Source/Shaders/Terrain.fx", NULL, NULL, 
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

	//YFile* debugFile = YFile::Open(L"debugHeightMap.txt", YFile::WRITE);
	//_Assert(NULL != debugFile);

	//for(int i = 0; i < heightMapSize; ++i)
	//{
	//	for(int j = 0; j < heightMapSize; ++j)
	//	{
	//		debugFile->WriteLine(L"data[%d][%d]: %d", i, j, mHeightMapData[heightMapSize * i + j]);
	//	}
	//}
	//debugFile->Close();
}

void Terrain::BuildTerrain(int depth)
{
	mRootNode = new QuadTreeNode(0, 0, mLength / 2.0f, depth);
	buildChunks(mRootNode, depth);
}

void Terrain::buildChunks(QuadTreeNode* node, int depth)
{
	if(node->GetDepth() == 0)
	{
		int chunkCounts = (int)pow(2.0f, depth);	// 总共的chunks数是chunkCounts*chunkCounts个
		int chunkSize = (mSize - 1) / chunkCounts + 1;  // 每个chunk的边顶点数
		float chunkLength = node->GetHalfSize() * 2;
		float chunkCenterX = node->GetCenter().x;
		float chunkCenterZ = node->GetCenter().y;

		int chunkColumn = (int)((chunkCenterX - chunkLength / 2.0f + (chunkCounts / 2)*chunkLength) / chunkLength);
		int chunkRow = (int)(((chunkCounts / 2)*chunkLength - (chunkCenterZ + chunkLength / 2.0f)) / chunkLength); 

		// chunk的第一个顶点对应在heightMapData中的row和column
		int heightMapBaseRow = (chunkSize - 1)*chunkRow;
		int heightMapBaseColumn = (chunkSize - 1)*chunkColumn;

		int numChunkVerts = chunkSize * chunkSize;
		int numChunkTris = (chunkSize - 1) * (chunkSize - 1) * 2;

		TerrainChunk* chunk = new TerrainChunk(node, chunkSize);
		chunk->mPosData.reserve(numChunkVerts);
		chunk->mUVData.reserve(numChunkVerts);
		chunk->mNormalData.reserve(numChunkVerts);

		for(int i = 0; i < numChunkVerts; ++i)
		{
			int row = i / chunkSize;
			int column = i % chunkSize;

			float x = chunkCenterX + ((float)column - (chunkSize - 1)/2) * chunkLength / (chunkSize - 1);
			float z = chunkCenterZ + ((chunkSize - 1)/2 - (float)row) * chunkLength / (chunkSize - 1);

			// 高度图使用了16位, 可表示的范围为0~65535, 偏移32767, 可表示-32767~32768
			float y = (mHeightMapData[(mSize - 1 - (heightMapBaseRow + row))*mSize + heightMapBaseColumn + column] - 32767) 
				* mHeight / 32768.0f;

			float u = (float)(heightMapBaseColumn + column)/(mSize - 1);
			float v = (float)(heightMapBaseRow + row)/(mSize - 1);

			chunk->mPosData.push_back(Vector3(x, y, z));
			chunk->mUVData.push_back(Vector2(u, v)); 
		}

		chunk->CalculateChunkNormals();
		chunk->CreateVertexBuffer();
		chunk->CreateIndexBuffer();		// 暂时加上的
		mChunks.push_back(chunk);
	}
	else
	{
		buildChunks(node->GetLeftTop(), depth);
		buildChunks(node->GetRightTop(), depth);
		buildChunks(node->GetLeftBottom(), depth);
		buildChunks(node->GetRightBottom(), depth);
	}
}

//void Terrain::calculateChunkNormal(const Vector3* posData, const DWORD* indices, int numVerts, int numTris, Vector3** normalData)
//{
//	_Assert(posData);
//	_Assert(normalData);
//	_Assert(*normalData);
//
//	for(int i = 0; i < numVerts; ++i){
//		(*normalData)[i] = Vector3::Zero;
//	}
//
//	for(int i = 0; i < numTris; ++i){
//		Vector3 v0 = posData[indices[3*i + 0]];
//		Vector3 v1 = posData[indices[3*i + 1]];
//		Vector3 v2 = posData[indices[3*i + 2]];
//
//		Vector3 v0v1 = v1 - v0;
//		Vector3 v0v2 = v2 - v0;
//
//		Vector3 normal = v0v2.Cross(v0v1);
//		normal.Normalize();
//
//		for(int k = 0; k < 3; ++k)
//		{
//			(*normalData)[indices[3 * i + k]] += normal;
//		}
//	}
//
//	for(int i = 0; i < numVerts; ++i)
//	{
//		(*normalData)[i].Normalize();
//	}
//}

void Terrain::FrameUpdate()
{
	for(std::vector<TerrainChunk*>::iterator iter = mChunks.begin(); iter != mChunks.end(); ++iter)
	{
		TerrainChunk* chunk = *iter;
		chunk->CreateIndexBuffer();
	}
}

void Terrain::Draw(Camera* camera, bool isSolid)
{
	_Assert(sEffect);

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
		Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	else
		Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	sEffect->Begin(0, 0);
	sEffect->BeginPass(0);

	for(std::vector<TerrainChunk*>::iterator iter = mChunks.begin(); iter != mChunks.end(); ++iter)
	{
		TerrainChunk* chunk = *iter;
		//chunk->CreateIndexBuffer();

		Driver::D3DDevice->SetStreamSource(0, chunk->mVertexBuffer, 0, SizeofVertex(XYZ_UV_N));
		Driver::D3DDevice->SetIndices(chunk->mIndexBuffer);
		Driver::D3DDevice->SetFVF(VertexUVN::FVF);

		int numVerts = chunk->mSize * chunk->mSize;
		int numTris = (chunk->mSize - 1) * (chunk->mSize - 1) * 2;

		Driver::D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numTris);
	}

	sEffect->EndPass();
	sEffect->End();

	Driver::D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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

	D3DXCreateTextureFromFile(Driver::D3DDevice, texFile0, &mMaterial.colorTexes[0]);	
	D3DXCreateTextureFromFile(Driver::D3DDevice, texFile1, &mMaterial.colorTexes[1]);	
	D3DXCreateTextureFromFile(Driver::D3DDevice, texFile2, &mMaterial.colorTexes[2]);	
	D3DXCreateTextureFromFile(Driver::D3DDevice, texFile3, &mMaterial.colorTexes[3]);		
}

void Terrain::SetSplatMapTex(const wchar_t* texFile)
{
	SAFE_RELEASE(mMaterial.splatMapTex);
	D3DXCreateTextureFromFile(Driver::D3DDevice, texFile, &mMaterial.splatMapTex);
}

void Terrain::SetMtlParameters(float tilesU, float tilesV, D3DXCOLOR ambient, D3DXCOLOR diffuse)
{
	mMaterial.tilesU = tilesU;
	mMaterial.tilesV = tilesV;
	mMaterial.ambientColor = ambient;
	mMaterial.diffuseColor = diffuse;
}
