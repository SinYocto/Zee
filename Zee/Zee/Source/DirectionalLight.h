#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "D3DUtility.h"
#include"Math.h"

#define MAX_NUM_DIRECTIONAL_LIGHTS 2

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
	DirectionalLight(D3DXCOLOR _color = D3DXCOLOR_WHITE, Vector3 _dir = Vector3(1.0f, -1.0f, 1.0f), 
		float _intensity = 1.0f)
		:color(_color)
		,direction(_dir)
		,intensity(_intensity)
		,isEnabled(true)
	{

	}

	// TODO:Data可以作为类的一个成员,不用每次get都创建个对象
	DirectionalLightData GetData() 
	{ 
		return DirectionalLightData(intensity * color, direction); 
	}

	void SetValue(D3DXCOLOR _color, Vector3 _dir, float _intensity);

	void Enable(bool enable);
	bool IsEnabled();

private:
	D3DXCOLOR color;
	Vector3 direction;
	float intensity;

	bool isEnabled;
};

#endif