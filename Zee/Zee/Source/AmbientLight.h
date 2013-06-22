#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

#include "D3DUtility.h"
#include"Math.h"

class AmbientLight{
public:
	AmbientLight(D3DXCOLOR _color = D3DXCOLOR_WHITE, float _intensity = 0)
		:color(_color)
		,intensity(_intensity)
	{

	}

	void SetValue(D3DXCOLOR _color, float _intensity)
	{
		color = _color;
		intensity = _intensity;
	}

	void SetColor(D3DXCOLOR _color)
	{
		color = _color;
	}

	void SetIntisity(float _intensity)
	{
		intensity = _intensity;
	}

	D3DXCOLOR FinalColor()
	{
		return intensity * color;
	}


private:
	D3DXCOLOR color;
	float intensity;
};

#endif