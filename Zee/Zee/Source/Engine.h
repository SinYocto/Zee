#ifndef ENGINE_H
#define ENGINE_H

#include "Driver.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "GeometryManager.h"
#include "MaterialManager.h"
#include "LightManager.h"
#include "Input.h"
#include "Time.h"
#include "IDAllocator.h"

class Engine
{
public:
	Engine();

	void Init(D3DDeviceParams params);
	void Destroy();

	void OnLostDevice();
	void OnResetDevice();

	void FrameUpdate();

	Driver* GetDriver();
	SceneManager* GetSceneManager();
	TextureManager* GetTextureManger();
	GeometryManager* GetGeometryManager();
	MaterialManager* GetMaterialManager();
	LightManager* GetLightManager();
	Input* GetInput();
	FrameTimer* GetFrameTimer();
	IDAllocator* GetIDAllocator();

private:
	Driver* mDriver;

	SceneManager* mSceneMgr;
	TextureManager* mTextureMgr;
	GeometryManager* mGeometryMgr;
	MaterialManager* mMaterialMgr;
	LightManager* mLightMgr;

	IDAllocator* mIDAllocator;

	Input* mInput;
	FrameTimer* mTimer;
};

extern Engine* gEngine;

#endif