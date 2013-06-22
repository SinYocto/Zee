#ifndef CUBE_H
#define CUBE_H

#include "D3DUtility.h"

class Camera;

class Cube
{
public:
	Cube(const Vector3& _center = Vector3::Zero, float _width = 100.0f, float _height = 100.0f, float _length = 100.0f)
		:center(_center)
		,width(_width)
		,height(_height)
        ,length(_length)
		,vertexBuffer(NULL)
		,indexBuffer(NULL)
		,pEffect(NULL)
		,colorTex(NULL)
	{
		buildGeometry();
	}

	~Cube()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		SAFE_RELEASE(pEffect);
		SAFE_RELEASE(colorTex);
	}

	void OnLostDevice()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);

		pEffect->OnLostDevice();
	}

	void OnResetDevice()
	{
		buildGeometry();
		pEffect->OnResetDevice();
	}

	bool SetShader(LPCWSTR shaderFilePath);
	bool SetColorTexture(LPCWSTR texFilePath);
	bool Draw(Camera* pCamera);

private:
	bool buildGeometry();
	bool createVertexBuffer(IDirect3DDevice9* pD3DDevice, void* vertexData);
	bool createIndexBuffer(IDirect3DDevice9* pD3DDevice, DWORD* indexData);

public:
	LPD3DXEFFECT pEffect;

private:
	IDirect3DVertexBuffer9* vertexBuffer;
	IDirect3DIndexBuffer9*  indexBuffer;

	IDirect3DTexture9 *colorTex;

	Vector3 center;
	float width;
	float height;
	float length;
};

#endif