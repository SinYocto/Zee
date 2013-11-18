#include "Engine.h"
//#include "LightManager.h"

Engine* gEngine = NULL;

Engine::Engine()
:mDriver(NULL)
,mSceneMgr(NULL)
,mGeometryMgr(NULL)
,mMaterialMgr(NULL)
,mLightMgr(NULL)
,mInput(NULL)
,mTimer(NULL)
{
	
}

void Engine::Init()
{
	mLightMgr = new LightManager();
	mLightMgr->Init();

	mTimer = new FrameTimer();
	mTimer->Start();

	mGeometryMgr = new GeometryManager();

	mMaterialMgr = new MaterialManager();
	mMaterialMgr->Init();
}

Input* Engine::GetInput()
{
	return mInput;
}

LightManager* Engine::GetLightManager()
{
	return mLightMgr;
}

void Engine::FrameUpdate()
{
	mTimer->Tick();
	mLightMgr->FrameUpdate();
	mMaterialMgr->FrameUpdate();
}

void Engine::Destroy()
{
	mLightMgr->Destroy();
	mGeometryMgr->Destroy();
	mMaterialMgr->Destroy();
}

FrameTimer* Engine::GetFrameTimer()
{
	return mTimer;
}

GeometryManager* Engine::GetGeometryManager()
{
	return mGeometryMgr;
}

MaterialManager* Engine::GetMaterialManager()
{
	return mMaterialMgr;
}

void Engine::OnLostDevice()
{
	mGeometryMgr->OnLostDevice();
	mMaterialMgr->OnLostDevice();
}

void Engine::OnResetDevice()
{
	mGeometryMgr->OnResetDevice();
	mMaterialMgr->OnResetDevice();
}