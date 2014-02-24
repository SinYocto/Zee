#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "D3DUtility.h"

#define MAX_NUM_DIRECTIONAL_LIGHTS 1

struct DirectionalLightData
{
	DirectionalLightData(D3DXCOLOR _color = D3DXCOLOR_BLACK, Vector3 _dir = Vector3(1.0f, -1.0f, 1.0f)) 
		:color(_color)
		,direction(_dir)
		,pad(0)
	{

	}

	D3DXCOLOR color;
	Vector3 direction;
	float pad;
};

class DirectionalLight
{
public:
	DirectionalLight(const wchar_t* name, D3DXCOLOR color = D3DXCOLOR_WHITE, Vector3 dir = Vector3(1.0f, -1.0f, 1.0f), 
		float intensity = 1.0f)
		:mColor(color)
		,mDirection(dir)
		,mIntensity(intensity)
		,mIsEnabled(true)
	{
		YString::Copy(mName, _countof(mName), name);
	}

	DirectionalLightData GetData() 
	{ 
		return DirectionalLightData(mIntensity * mColor, mDirection.Normalized()); 
	}

	void SetID(DWORD id);
	void SetValue(D3DXCOLOR color, Vector3 dir, float intensity);

	void SetDirection(const Vector3& dir);
	void SetIntensity(float intensity);
	void SetColor(D3DXCOLOR color);

	wchar_t* GetName();
	Vector3 GetDirection();
	D3DXCOLOR GetFinalColor();
	D3DXCOLOR GetColor();
	float GetIntensity();

	void Enable(bool enable);
	bool IsEnabled();

private:
	DWORD mID;
	wchar_t mName[MAX_STR_LEN];

	D3DXCOLOR mColor;
	Vector3 mDirection;
	float mIntensity;

	bool mIsEnabled;
};

#endif