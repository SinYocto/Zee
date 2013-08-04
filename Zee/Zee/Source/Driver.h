#ifndef DRIVER_H
#define DRIVER_H

#include"d3d9.h"
#include"d3dx9.h"

class Driver
{
public:
	static void CreateD3DDevice(HWND _hWnd, _D3DMULTISAMPLE_TYPE _multisampleType);
	static bool Reset();

	static void SetViewPort(int offsetX, int offsetY, int width, int height);
	static void Destory();

	static HRESULT Clear(DWORD flag, D3DCOLOR color);
	static HRESULT BeginScene();
	static HRESULT EndScene();
	static HRESULT Present();

public:
	static HWND hWnd;
	static IDirect3DDevice9* D3DDevice;
	static D3DVIEWPORT9 viewPort;

private:
	static IDirect3D9* D3D;

	static D3DPRESENT_PARAMETERS presentParams;
	static _D3DMULTISAMPLE_TYPE multiSampleType;
};

#endif