#ifndef ENGINE_H
#define ENGINE_H

#include "Driver.h"
#include "SceneManager.h"
#include "GeometryManager.h"
#include "MaterialManager.h"
#include "LightManager.h"
#include "Input.h"
#include "Time.h"

class Engine
{
public:
	Engine();

	void Init();
	void Destroy();

	void OnLostDevice();
	void OnResetDevice();

	void FrameUpdate();

	Driver* GetDriver();
	SceneManager* GetSceneManager();
	GeometryManager* GetGeometryManager();
	MaterialManager* GetMaterialManager();
	LightManager* GetLightManager();
	Input* GetInput();
	FrameTimer* GetFrameTimer();

private:
	Driver* mDriver;

	SceneManager* mSceneMgr;
	GeometryManager* mGeometryMgr;
	MaterialManager* mMaterialMgr;
	LightManager* mLightMgr;

	Input* mInput;
	FrameTimer* mTimer;
};

extern Engine* gEngine;

#endif