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

void Engine::Init(D3DDeviceParams params)
{
	mDriver = new Driver();
	mDriver->CreateD3DDevice(params);

	mInput = new Input();
	mInput->Init(GetModuleHandle(0), mDriver->GetHWnd());

	mLightMgr = new LightManager();
	mLightMgr->Init();

	mTimer = new FrameTimer();
	mTimer->Start();

	mGeometryMgr = new GeometryManager();

	mMaterialMgr = new MaterialManager();
	mMaterialMgr->Init();

	mSceneMgr = new SceneManager();
	mSceneMgr->Init();
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
	mInput->GetDeviceState(mDriver->GetHWnd());

	mSceneMgr->FrameUpdate();
	mLightMgr->FrameUpdate();
	mMaterialMgr->FrameUpdate();
}

void Engine::Destroy()
{
	mLightMgr->Destroy();
	mGeometryMgr->Destroy();
	mMaterialMgr->Destroy();
	mSceneMgr->Destory();

	mInput->Destroy();
	mDriver->Destory();
}

Driver* Engine::GetDriver()
{
	return mDriver;
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
	mDriver->OnLostDevice();
	mGeometryMgr->OnLostDevice();
	mMaterialMgr->OnLostDevice();
}

void Engine::OnResetDevice()
{
	mDriver->OnResetDevice();
	mGeometryMgr->OnResetDevice();
	mMaterialMgr->OnResetDevice();
}

SceneManager* Engine::GetSceneManager()
{
	return mSceneMgr;
}
