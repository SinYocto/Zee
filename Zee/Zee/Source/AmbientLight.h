#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

#include "D3DUtility.h"

class AmbientLight{
public:
	AmbientLight(D3DXCOLOR color = D3DXCOLOR_WHITE, float intensity = 0)
		:mColor(color)
		,mIntensity(intensity)
	{

	}

	void SetValue(D3DXCOLOR color, float intensity)
	{
		mColor = color;
		mIntensity = intensity;
	}

	void SetColor(D3DXCOLOR color)
	{
		mColor = color;
	}

	void SetIntisity(float intensity)
	{
		mIntensity = intensity;
	}

	D3DXCOLOR FinalColor()
	{
		return mIntensity * mColor;
	}


private:
	D3DXCOLOR mColor;
	float mIntensity;
};

#endif