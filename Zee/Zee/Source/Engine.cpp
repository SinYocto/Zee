#include "Engine.h"
//#include "LightManager.h"

Engine* gEngine = NULL;

Engine::Engine()
:mDriver(NULL)
,mSceneMgr(NULL)
,mTextureMgr(NULL)
,mGeometryMgr(NULL)
,mMaterialMgr(NULL)
,mModelMgr(NULL)
,mLightMgr(NULL)
,mResourceMgr(NULL)
,mInput(NULL)
,mTimer(NULL)
,mGizmo(NULL)
{
	
}

void Engine::Init(D3DDeviceParams params)
{
	mDriver = New Driver();
	mDriver->CreateD3DDevice(params);

	mIDAllocator = New IDAllocator();

	mInput = New Input();
	mInput->Init(GetModuleHandle(0), mDriver->GetHWnd());

	mLightMgr = New LightManager();
	mLightMgr->Init();

	mTimer = New FrameTimer();
	mTimer->Start();

	mGeometryMgr = New GeometryManager();

	mTextureMgr = New TextureManager();

	mMaterialMgr = New MaterialManager();
	mMaterialMgr->Init();

	mModelMgr = New ModelManager();
	mModelMgr->Init();

	mResourceMgr = New ResourceMgr();

	mSceneMgr = New SceneManager();
	mSceneMgr->Init();

	mGizmo = New Gizmo();
	mGizmo->Init();
	mGizmo->SetActiveType(Gizmo::GIZMO_TRANS);
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
	mGizmo->FrameUpdate();

	Profiler::FrameUpdate();
}

void Engine::Destroy()
{
	mGizmo->Destroy();
	mModelMgr->Destroy();
	mResourceMgr->Destroy();
	mLightMgr->Destroy();
	mGeometryMgr->Destroy();
	mMaterialMgr->Destroy();
	mTextureMgr->Destory();
	mSceneMgr->Destory();

	mInput->Destroy();
	mDriver->Destory();

	SAFE_DELETE(mDriver);
	SAFE_DELETE(mInput);
	SAFE_DELETE(mResourceMgr);
	SAFE_DELETE(mModelMgr);
	SAFE_DELETE(mLightMgr);
	SAFE_DELETE(mGeometryMgr);
	SAFE_DELETE(mMaterialMgr);
	SAFE_DELETE(mTextureMgr);
	SAFE_DELETE(mSceneMgr);
	SAFE_DELETE(mTimer);
	SAFE_DELETE(mIDAllocator);
	SAFE_DELETE(mGizmo);
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
	mResourceMgr->OnLostDevice();
	mGizmo->OnLostDevice();
}

void Engine::OnResetDevice()
{
	mDriver->OnResetDevice();
	mGeometryMgr->OnResetDevice();
	mMaterialMgr->OnResetDevice();
	mResourceMgr->OnResetDevice();
	mGizmo->OnResetDevice();
}

SceneManager* Engine::GetSceneManager()
{
	return mSceneMgr;
}

IDAllocator* Engine::GetIDAllocator()
{
	return mIDAllocator;
}

TextureManager* Engine::GetTextureManger()
{
	return mTextureMgr;
}

ResourceMgr* Engine::GetResourceManager()
{
	return mResourceMgr;
}

ModelManager* Engine::GetModelManager()
{
	return mModelMgr;
}

Gizmo* Engine::GetGizmo()
{
	return mGizmo;
}
