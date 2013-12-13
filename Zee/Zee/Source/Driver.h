#ifndef DRIVER_H
#define DRIVER_H

#include"d3d9.h"
#include"d3dx9.h"
#include "Math.h"
#include "Common.h"

struct D3DDeviceParams
{
	HWND hWnd;
	_D3DMULTISAMPLE_TYPE multisampleType;
};

struct SwapChain
{
	~SwapChain()
	{
		SAFE_RELEASE(d3dSwapChain);
	}

	IDirect3DSwapChain9* d3dSwapChain;
	D3DVIEWPORT9 viewPort;
	D3DPRESENT_PARAMETERS presentParams;
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

	void SetViewPort(int offsetX, int offsetY, int width, int height);
	void SetViewPort(int swapChainIndex);
	void GetViewPort(int swapChainIndex, Vector2* vpOrigin, Vector2* vpSize);
	D3DVIEWPORT9 GetViewPort(int swapChainIndex);

	HRESULT Clear(DWORD flag, D3DCOLOR color, float z);
	HRESULT BeginScene();
	HRESULT EndScene();
	HRESULT Present(HWND _hWnd = NULL);

	int CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS params);
	void RenderToSwapChain(int swapChainIndex);

	void GetScreenLocation(const Vector2& screenPos, Vector2* screenLocation);	// 屏幕坐标转为0~1表示
	D3DPRESENT_PARAMETERS GetPresentParameters(int swapChainIndex);

private:
	HWND hWnd;
	IDirect3D9* D3D;
	IDirect3DDevice9 * D3DDevice;
	//D3DVIEWPORT9 primaryViewPort;
	//D3DVIEWPORT9 secondaryViewPort;

	//IDirect3DSwapChain9* primarySwapChain;
	//IDirect3DSwapChain9* secondarySwapChain;

	//D3DPRESENT_PARAMETERS presentParams;
	//D3DPRESENT_PARAMETERS secondaryPresentParams;
	_D3DMULTISAMPLE_TYPE multiSampleType;

	std::vector<SwapChain*> swapChainList;
	int activeSwapChainIndex;
};

#endif