#include "Engine.h"
//#include "LightManager.h"

Engine* gEngine = NULL;

Engine::Engine()
:mDriver(NULL)
,mSceneMgr(NULL)
,mTextureMgr(NULL)
,mGeometryMgr(NULL)
,mMaterialMgr(NULL)
,mLightMgr(NULL)
,mResourceMgr(NULL)
,mInput(NULL)
,mTimer(NULL)
{
	
}

void Engine::Init(D3DDeviceParams params)
{
	mDriver = new Driver();
	mDriver->CreateD3DDevice(params);

	mIDAllocator = new IDAllocator();

	mInput = new Input();
	mInput->Init(GetModuleHandle(0), mDriver->GetHWnd());

	mLightMgr = new LightManager();
	mLightMgr->Init();

	mTimer = new FrameTimer();
	mTimer->Start();

	mGeometryMgr = new GeometryManager();

	mTextureMgr = new TextureManager();

	mMaterialMgr = new MaterialManager();
	mMaterialMgr->Init();

	mResourceMgr = new ResourceMgr();

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
	SAFE_DELETE(mLightMgr);
	SAFE_DELETE(mGeometryMgr);
	SAFE_DELETE(mMaterialMgr);
	SAFE_DELETE(mTextureMgr);
	SAFE_DELETE(mSceneMgr);
	SAFE_DELETE(mTimer);
	SAFE_DELETE(mIDAllocator);
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
