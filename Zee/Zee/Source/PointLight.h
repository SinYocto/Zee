#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "D3DUtility.h"
#include"Math.h"

#define MAX_NUM_POINT_LIGHTS 8

struct PointLightData
{
	PointLightData(D3DXCOLOR _color = D3DXCOLOR_BLACK, Vector3 _pos = Vector3(0, 0, 0), 
		Vector3 _attenuation = Vector3(1.0f, 0, 0)) 
		:color(_color)
		,position(_pos)
		,attenuation(_attenuation)
		,pad1(0)
		,pad2(0)
	{

	}

	D3DXCOLOR color;
	Vector3 position;
	float pad1;
	Vector3 attenuation;
	float pad2;
};

class PointLight
{
public:
	PointLight(D3DXCOLOR color = D3DXCOLOR_WHITE, Vector3 pos = Vector3(0, 0, 0), 
		Vector3 attenuation = Vector3(1.0f, 0, 0), float intensity = 1.0f)
		:mColor(color)
		,mPosition(pos)
		,mAttenuation(attenuation)
		,mIntensity(intensity)
		,mIsEnabled(true){}

	PointLightData GetData() 
	{ 
		return PointLightData(mIntensity * mColor, mPosition, mAttenuation); 
	}

	void SetValue(D3DXCOLOR color, Vector3 pos, Vector3 attenuation, float intensity);

	void Enable(bool enable);
	bool IsEnabled();

private:
	D3DXCOLOR mColor;
	Vector3 mPosition;
	Vector3 mAttenuation;
	float mIntensity;

	bool mIsEnabled;
}; 

#endif