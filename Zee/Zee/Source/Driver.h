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

class Driver
{
public:
	static void CreateD3DDevice(HWND _hWnd, _D3DMULTISAMPLE_TYPE _multisampleType);
	static bool Reset();

	static void OnLostDevice();
	static void OnResetDevice();

	static void SetViewPort(int offsetX, int offsetY, int width, int height);
	static void SetViewPort(SWAPCHAIN_TYPE swapChainType);
	static void GetViewPort(Vector2* vpOrigin, Vector2* vpSize);
	static void Destory();

	static HRESULT Clear(DWORD flag, D3DCOLOR color, float z);
	static HRESULT BeginScene();
	static HRESULT EndScene();
	static HRESULT Present(HWND _hWnd = NULL);

	static void CreateSecondarySwapChain(D3DPRESENT_PARAMETERS params);
	static void RenderToSwapChain(SWAPCHAIN_TYPE swapChain);

	static void GetScreenLocation(const Vector2& screenPos, Vector2* screenLocation);	// 屏幕坐标转为0~1表示
	static D3DPRESENT_PARAMETERS GetPresentParameters();

public:
	static HWND hWnd;
	static IDirect3DDevice9* D3DDevice;
	static D3DVIEWPORT9 primaryViewPort;
	static D3DVIEWPORT9 secondaryViewPort;

private:
	static IDirect3D9* D3D;
	static IDirect3DSwapChain9* primarySwapChain;
	static IDirect3DSwapChain9* secondarySwapChain;

	static D3DPRESENT_PARAMETERS presentParams;
	static D3DPRESENT_PARAMETERS secondaryPresentParams;
	static _D3DMULTISAMPLE_TYPE multiSampleType;

	static SWAPCHAIN_TYPE activeSwapChain;
};

#endif