#include "Driver.h"
#include "Common.h"

Driver::Driver()
:D3D(NULL)
,D3DDevice(NULL)
,hWnd(NULL)
,primarySwapChain(NULL)
,secondarySwapChain(NULL)
,activeSwapChain(PRIMARY_SWAPCHAIN)
{

}

void Driver::CreateD3DDevice(D3DDeviceParams params)
{
	hWnd = params.hWnd;
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

	multiSampleType = params.multisampleType;
	DWORD multisampleQuality = 0;

	if(SUCCEEDED(D3D->CheckDeviceMultiSampleType(d3dcaps.AdapterOrdinal, d3dcaps.DeviceType, D3DFMT_A8B8G8R8, 
		FALSE, params.multisampleType, &multisampleQuality)))		// TODO: Ê§°Ü
	{
		multiSampleType = params.multisampleType;
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

	D3DDevice->GetSwapChain(0, &primarySwapChain);

	primaryViewPort.X = 0;
	primaryViewPort.Y = 0;
	primaryViewPort.Width = bufferWidth;
	primaryViewPort.Height = bufferHeight;
	primaryViewPort.MinZ = 0;
	primaryViewPort.MaxZ = 1.0f;
}

bool Driver::Reset()
{
	if(FAILED(D3DDevice->Reset(&presentParams)))
		return false;

	return true;
}

void Driver::SetViewPort(int offsetX, int offsetY, int width, int height)
{
	_Assert(NULL != D3DDevice);

	D3DVIEWPORT9 viewPort;
	viewPort.X = offsetX;
	viewPort.Y = offsetY;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinZ = 0;
	viewPort.MaxZ = 1.0f;

	D3DDevice->SetViewport(&viewPort);
}

void Driver::SetViewPort(SWAPCHAIN_TYPE swapChainType)
{
	_Assert(swapChainType == activeSwapChain)
	if(activeSwapChain == PRIMARY_SWAPCHAIN)
		D3DDevice->SetViewport(&primaryViewPort);
	else
		D3DDevice->SetViewport(&secondaryViewPort);
}

void Driver::GetViewPort(Vector2* vpOrigin, Vector2* vpSize)
{
	_Assert(NULL != D3DDevice);

	D3DVIEWPORT9 vp;
	D3DDevice->GetViewport(&vp);

	Vector2 origin = Vector2((float)vp.X, (float)vp.Y);
	Vector2 size = Vector2((float)vp.Width, (float)vp.Height);

	if(vpOrigin)
		*vpOrigin = origin;

	if(vpSize)
		*vpSize = size;
}


void Driver::Destory()
{
	SAFE_RELEASE(primarySwapChain);
	SAFE_RELEASE(secondarySwapChain);
	SAFE_RELEASE(D3D);
	SAFE_RELEASE(D3DDevice);
}

HRESULT Driver::Clear(DWORD flag, D3DCOLOR color, float z)
{
	return D3DDevice->Clear(0, 0, flag, color, z, 0);
}

HRESULT Driver::BeginScene()
{
	return D3DDevice->BeginScene();
}

HRESULT Driver::EndScene()
{
	return D3DDevice->EndScene();
}

HRESULT Driver::Present(HWND _hWnd /* = NULL */)
{
	if(activeSwapChain == PRIMARY_SWAPCHAIN)
	{
		return primarySwapChain->Present(NULL, NULL, hWnd, NULL, 0);
	}
	else
	{
		_Assert(_hWnd != NULL);
		return secondarySwapChain->Present(NULL, NULL, _hWnd, NULL, 0);
	}
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

void Driver::RenderToSwapChain(SWAPCHAIN_TYPE swapChain)
{
	activeSwapChain = swapChain;

	IDirect3DSurface9* backBuffer = NULL;
	if(activeSwapChain == PRIMARY_SWAPCHAIN)
	{
		_Assert(primarySwapChain != NULL);
		primarySwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	}
	else
	{
		_Assert(secondarySwapChain != NULL);
		secondarySwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	}

	D3DDevice->SetRenderTarget(0, backBuffer);
	SetViewPort(activeSwapChain);

	SAFE_RELEASE(backBuffer);
}

void Driver::OnLostDevice()
{
	SAFE_RELEASE(secondarySwapChain);
}

void Driver::OnResetDevice()
{
	D3DDevice->CreateAdditionalSwapChain(&secondaryPresentParams, &secondarySwapChain);
}

void Driver::CreateSecondarySwapChain(D3DPRESENT_PARAMETERS params)
{
	SAFE_RELEASE(secondarySwapChain);

	secondaryPresentParams = params;
	D3DDevice->CreateAdditionalSwapChain(&secondaryPresentParams, &secondarySwapChain);

	secondaryViewPort.X = 0;
	secondaryViewPort.Y = 0;
	secondaryViewPort.Width = params.BackBufferWidth;
	secondaryViewPort.Height = params.BackBufferHeight;
	secondaryViewPort.MinZ = 0;
	secondaryViewPort.MaxZ = 1.0f;
}

D3DPRESENT_PARAMETERS Driver::GetPresentParameters()
{
	return presentParams;
}

IDirect3DDevice9* Driver::GetD3DDevice()
{
	return D3DDevice;
}

HWND Driver::GetHWnd()
{
	return hWnd;
}

D3DVIEWPORT9 Driver::GetPrimaryViewPort()
{
	return primaryViewPort;
}

D3DVIEWPORT9 Driver::GetSecondaryViewPort()
{
	return secondaryViewPort;
}
