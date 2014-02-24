#ifndef ENGINE_H
#define ENGINE_H

#include "Driver.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "GeometryManager.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "LightManager.h"
#include "ResourceMgr.h"
#include "Input.h"
#include "Time.h"
#include "IDAllocator.h"
#include "Gizmo.h"
#include "Profiler.h"

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
	ModelManager* GetModelManager();
	LightManager* GetLightManager();
	ResourceMgr* GetResourceManager();
	Input* GetInput();
	FrameTimer* GetFrameTimer();
	IDAllocator* GetIDAllocator();
	Gizmo* GetGizmo();

private:
	Driver* mDriver;

	SceneManager* mSceneMgr;
	TextureManager* mTextureMgr;
	GeometryManager* mGeometryMgr;
	MaterialManager* mMaterialMgr;
	ModelManager* mModelMgr;
	LightManager* mLightMgr;
	ResourceMgr* mResourceMgr;

	IDAllocator* mIDAllocator;

	Input* mInput;
	FrameTimer* mTimer;

	Gizmo* mGizmo;
};

extern Engine* gEngine;

#endif