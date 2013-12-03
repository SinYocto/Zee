#ifndef DRIVER_H
#define DRIVER_H

#include"d3d9.h"
#include"d3dx9.h"
#include "Math.h"

enum SWAPCHAIN_TYPE
{
	PRIMARY_SWAPCHAIN = 1,
	SECONDARY_SWAPCHAIN
};

struct D3DDeviceParams
{
	HWND hWnd;
	_D3DMULTISAMPLE_TYPE multisampleType;
};

class Driver
{
public:
	Driver();

	void Destory();

	void CreateD3DDevice(D3DDeviceParams params);
	bool Reset();

	void OnLostDevice();
	void OnResetDevice();

	IDirect3DDevice9* GetD3DDevice();
	HWND GetHWnd();
	D3DVIEWPORT9 GetPrimaryViewPort();
	D3DVIEWPORT9 GetSecondaryViewPort();

	void SetViewPort(int offsetX, int offsetY, int width, int height);
	void SetViewPort(SWAPCHAIN_TYPE swapChainType);
	void GetViewPort(Vector2* vpOrigin, Vector2* vpSize);

	HRESULT Clear(DWORD flag, D3DCOLOR color, float z);
	HRESULT BeginScene();
	HRESULT EndScene();
	HRESULT Present(HWND _hWnd = NULL);

	void CreateSecondarySwapChain(D3DPRESENT_PARAMETERS params);
	void RenderToSwapChain(SWAPCHAIN_TYPE swapChain);

	void GetScreenLocation(const Vector2& screenPos, Vector2* screenLocation);	// 屏幕坐标转为0~1表示
	D3DPRESENT_PARAMETERS GetPresentParameters();

public:
	HWND hWnd;
	IDirect3DDevice9 * D3DDevice;
	D3DVIEWPORT9 primaryViewPort;
	D3DVIEWPORT9 secondaryViewPort;

private:
	IDirect3D9* D3D;
	IDirect3DSwapChain9* primarySwapChain;
	IDirect3DSwapChain9* secondarySwapChain;

	D3DPRESENT_PARAMETERS presentParams;
	D3DPRESENT_PARAMETERS secondaryPresentParams;
	_D3DMULTISAMPLE_TYPE multiSampleType;

	SWAPCHAIN_TYPE activeSwapChain;
};

#endif