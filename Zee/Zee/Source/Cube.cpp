#include "Cube.h"
#include "Camera.h"

bool Cube::buildGeometry()
{
    bool bResult = false;

    VertexUV* vertexData = new VertexUV[24];
    DWORD* indexData = new DWORD[36];
    {
        /*      
            5- - - - - -6        
           /|		   /|        y   
          / |         / |        ^  z
         4- - - - - -7  |        | /
         |  |        |  |        |/
         |  1- - - - - -2		 *---->x
         | /	     | /
         |/          |/
         0- - - - - -3        
        */
        Vector3 pos[8];
        pos[0] = center + Vector3(-width/2.0f, -height/2.0f, -length/2.0f);
        pos[1] = center + Vector3(-width/2.0f, -height/2.0f,  length/2.0f);
        pos[2] = center + Vector3( width/2.0f, -height/2.0f,  length/2.0f);
        pos[3] = center + Vector3( width/2.0f, -height/2.0f, -length/2.0f);

        pos[4] = center + Vector3(-width/2.0f, height/2.0f, -length/2.0f);
        pos[5] = center + Vector3(-width/2.0f, height/2.0f,  length/2.0f);
        pos[6] = center + Vector3( width/2.0f, height/2.0f,  length/2.0f);
        pos[7] = center + Vector3( width/2.0f, height/2.0f, -length/2.0f);

        vertexData[0] = VertexUV(pos[0], 0, 0);
        vertexData[1] = VertexUV(pos[3], 1, 0);
        vertexData[2] = VertexUV(pos[1], 0, 1);
        vertexData[3] = VertexUV(pos[2], 1, 1);

        vertexData[4] = VertexUV(pos[5], 0, 0);
        vertexData[5] = VertexUV(pos[6], 1, 0);
        vertexData[6] = VertexUV(pos[4], 0, 1);
        vertexData[7] = VertexUV(pos[7], 1, 1);

        vertexData[8] = VertexUV(pos[4], 0, 0);
        vertexData[9] = VertexUV(pos[7], 1, 0);
        vertexData[10] = VertexUV(pos[0], 0, 1);
        vertexData[11] = VertexUV(pos[3], 1, 1);

        vertexData[12] = VertexUV(pos[6], 0, 0);
        vertexData[13] = VertexUV(pos[5], 1, 0);
        vertexData[14] = VertexUV(pos[2], 0, 1);
        vertexData[15] = VertexUV(pos[1], 1, 1);

        vertexData[16] = VertexUV(pos[5], 0, 0);
        vertexData[17] = VertexUV(pos[4], 1, 0);
        vertexData[18] = VertexUV(pos[1], 0, 1);
        vertexData[19] = VertexUV(pos[0], 1, 1);

        vertexData[20] = VertexUV(pos[7], 0, 0);
        vertexData[21] = VertexUV(pos[6], 1, 0);
        vertexData[22] = VertexUV(pos[3], 0, 1);
        vertexData[23] = VertexUV(pos[2], 1, 1);

        for(int i = 0; i < 6; i++){
            indexData[6*i + 0] = 4*i;
            indexData[6*i + 1] = 4*i + 1;
            indexData[6*i + 2] = 4*i + 2;

            indexData[6*i + 3] = 4*i + 2;
            indexData[6*i + 4] = 4*i + 1;
            indexData[6*i + 5] = 4*i + 3;
        }

        Assert(createVertexBuffer(gD3DDevice, vertexData));
        Assert(createIndexBuffer(gD3DDevice, indexData));
    }

	bResult = true;
Exit:
    SAFE_DELETE_ARRAY(vertexData);
    SAFE_DELETE_ARRAY(indexData);
	return bResult;
}

bool Cube::createVertexBuffer(IDirect3DDevice9* pD3DDevice, void* vertexData)
{
	{
		int sizeofVertex = sizeof(VertexUV);
		int numVertices = 24;

		HRESULT hr = pD3DDevice->CreateVertexBuffer(numVertices * sizeofVertex, 0, 0, D3DPOOL_DEFAULT, &vertexBuffer, 0);
		Assert(SUCCEEDED(hr));

		void* bufferPtr;
		vertexBuffer->Lock(0, 0, (void**)&bufferPtr, 0);

		memcpy_s(bufferPtr, numVertices * sizeofVertex, vertexData, numVertices * sizeofVertex);

		vertexBuffer->Unlock();
	}

	return true;
Exit:
	return false;
}

bool Cube::createIndexBuffer(IDirect3DDevice9* pD3DDevice, DWORD* indexData)
{
	{
		int numIndices = 36;

		HRESULT hr = pD3DDevice->CreateIndexBuffer(sizeof(DWORD)*numIndices, 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &indexBuffer, 0);
		Assert(SUCCEEDED(hr));

		void* bufferPtr;
		indexBuffer->Lock(0, 0, (void**)&bufferPtr, 0);

		memcpy_s(bufferPtr, numIndices * sizeof(DWORD), indexData, numIndices * sizeof(DWORD));

		indexBuffer->Unlock();
	}

	return true;
Exit:
	return false;
}

bool Cube::SetShader(LPCWSTR shaderFilePath)
{
	HRESULT hr = D3DXCreateEffectFromFile(gD3DDevice, shaderFilePath, NULL, NULL, D3DXSHADER_DEBUG, NULL, &pEffect, NULL);
	Assert(SUCCEEDED(hr));

	return true;
Exit:
	return false;
}

bool Cube::Draw(Camera* pCamera)
{
	{
		Assert(pCamera);
		Assert(pEffect);

		gD3DDevice->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexUV));
		gD3DDevice->SetIndices(indexBuffer);
		gD3DDevice->SetFVF(VertexUV::FVF);

		D3DXMATRIX matWorld, matView, matProj;
		D3DXMatrixIdentity(&matWorld);
		matView = pCamera->ViewMatrix();
		matProj = pCamera->ProjMatrix();

		Assert(SUCCEEDED(pEffect->SetTechnique("Texture")));
		Assert(SUCCEEDED(pEffect->SetMatrix("matWVP", &(matWorld * matView * matProj))));
		Assert(SUCCEEDED(pEffect->SetTexture("colorTex", colorTex)));

		pEffect->Begin(0, 0);
		pEffect->BeginPass(0);

		gD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

		pEffect->EndPass();
		pEffect->End();
	}

Exit:
	return true;
}

bool Cube::SetColorTexture(LPCWSTR texFilePath)
{
	if(colorTex)
		SAFE_RELEASE(colorTex);

	Assert(SUCCEEDED(D3DXCreateTextureFromFile(gD3DDevice, texFilePath, &colorTex)));

	return true;
Exit:
	return false;
}
