#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"

class Camera;

class Billboard : public IReferenceCounted
{
public:
	Billboard(float width = 1.0f, float height = 1.0f, D3DCOLOR color = D3DCOLOR_WHITE)
		:mVertexBuffer(NULL)
		,mTexture(NULL)
		,mWidth(width)
		,mHeight(height)
		,mColor(color)
		,mNeedResetEffect(false)
	{
		if(!effect)
			createEffect();

		createVertexBuffer();
	}

	~Billboard()
	{
		SAFE_RELEASE(mVertexBuffer);
		SAFE_RELEASE(mTexture);
	}

public:
	void OnLostDevice();
	void OnResetDevice();

	void SetTexture(const wchar_t* texFileName);
	void SetColor(D3DXCOLOR color);

	void Draw(const Vector3& pos, Camera* camera);

	float GetWidth();
	float GetHeight();

private:
	void createEffect();
	void createVertexBuffer();

private:
	D3DCOLOR mColor;
	float mWidth;
	float mHeight;

	IDirect3DVertexBuffer9* mVertexBuffer;
	IDirect3DTexture9* mTexture;

	static LPD3DXEFFECT effect;

	bool mNeedResetEffect;
};

#endif