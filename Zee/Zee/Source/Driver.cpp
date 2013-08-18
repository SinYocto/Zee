#include "Driver.h"
#include "Common.h"

HWND Driver::hWnd = NULL;
IDirect3D9* Driver::D3D = NULL;
IDirect3DDevice9* Driver::D3DDevice = NULL;

D3DPRESENT_PARAMETERS Driver::presentParams;
_D3DMULTISAMPLE_TYPE Driver::multiSampleType;
D3DVIEWPORT9 Driver::viewPort;

void Driver::CreateD3DDevice(HWND _hWnd, _D3DMULTISAMPLE_TYPE _multisampleType)
{
	hWnd = _hWnd;
	D3D = Direct3DCreate9(D3D_SDK_VERSION);

	// identify adapter
	D3DADAPTER_IDENTIFIER9 adapterIdentifier;
	HRESULT hr = D3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifier);
	_Assert(SUCCEEDED(hr));

	Log(L"graphics card:%S\n", adapterIdentifier.Description);

	// device capacity
	D3DCAPS9 d3dcaps;
	D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);

	int VPMode = 0;
	if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		VPMode = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else 
		VPMode = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	multiSampleType = _multisampleType;
	DWORD multisampleQuality = 0;

	if(SUCCEEDED(D3D->CheckDeviceMultiSampleType(d3dcaps.AdapterOrdinal, d3dcaps.DeviceType, D3DFMT_A8B8G8R8, 
		FALSE, _multisampleType, &multisampleQuality)))		// TODO: Ê§°Ü
	{
		multiSampleType = _multisampleType;
	}
	else
	{
		multiSampleType = D3DMULTISAMPLE_NONE;
	}

	// present parameters
	D3DDISPLAYMODE displayMode;
	hr = D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
	_Assert(SUCCEEDED(hr));

	//D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory(&presentParams,sizeof(presentParams));

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int bufferWidth = clientRect.right - clientRect.left;
	int bufferHeight = clientRect.bottom - clientRect.top;

	presentParams.Windowed					= true;
	presentParams.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	presentParams.BackBufferWidth			= bufferWidth;
	presentParams.BackBufferHeight			= bufferHeight;
	presentParams.BackBufferFormat			= displayMode.Format;
	presentParams.MultiSampleType			= multiSampleType;
	presentParams.MultiSampleQuality		= multisampleQuality - 1;
	presentParams.EnableAutoDepthStencil	= TRUE;
	presentParams.AutoDepthStencilFormat	= D3DFMT_D24S8;
	presentParams.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	hr = D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, VPMode, &presentParams, &D3DDevice);
	_Assert(SUCCEEDED(hr));

	SetViewPort(0, 0, bufferWidth, bufferHeight);
}

bool Driver::Reset()
{
	if(FAILED(D3DDevice->Reset(&presentParams)))
		return false;

	SetViewPort(viewPort.X, viewPort.Y, viewPort.Width, viewPort.Height);

	return true;
}

void Driver::SetViewPort(int offsetX, int offsetY, int width, int height)
{
	_Assert(NULL != D3DDevice);

	viewPort.X = offsetX;
	viewPort.Y = offsetY;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinZ = 0;
	viewPort.MaxZ = 1.0f;

	D3DDevice->SetViewport(&viewPort);
}

void Driver::GetViewPort(Vector2* vpOrigin, Vector2* vpSize)
{
	_Assert(NULL != vpOrigin);
	_Assert(NULL != vpSize);
	_Assert(NULL != D3DDevice);

	D3DVIEWPORT9 vp;
	D3DDevice->GetViewport(&vp);

	*vpOrigin = Vector2((float)vp.X, (float)vp.Y);
	*vpSize = Vector2((float)vp.Width, (float)vp.Height);
}


void Driver::Destory()
{
	SAFE_RELEASE(D3D);
	SAFE_RELEASE(D3DDevice);
}

HRESULT Driver::Clear(DWORD flag, D3DCOLOR color)
{
	return D3DDevice->Clear(0, 0, flag, color, 1.0f, 0);
}

HRESULT Driver::BeginScene()
{
	return D3DDevice->BeginScene();
}

HRESULT Driver::EndScene()
{
	return D3DDevice->EndScene();
}

HRESULT Driver::Present()
{
	return D3DDevice->Present(0, 0, 0, 0);
}

void Driver::GetScreenLocation(const Vector2& screenPos, Vector2* screenLocation)
{
	Vector2 vpOrigin;
	Vector2 vpSize;

	GetViewPort(&vpOrigin, &vpSize);
	_Assert(vpSize.x != 0 && vpSize.y != 0);

	Vector2 vpPos = screenPos - vpOrigin;

	Vector2& location = *screenLocation;
	location.x = vpPos.x / vpSize.x;
	location.y = vpPos.y / vpSize.y;

	Clamp(location.x, 0, 1.0f);
	Clamp(location.y, 0, 1.0f);

	return;
}
