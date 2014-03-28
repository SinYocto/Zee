#include "Driver.h"

Driver::Driver()
:D3D(NULL)
,D3DDevice(NULL)
,hWnd(NULL)
,activeSwapChainIndex(0)
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

	ConsolePrint(L"graphics card:%S\n", adapterIdentifier.Description);

	// device capacity
	D3DCAPS9 d3dcaps;
	D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);

	int VPMode = 0;
	if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		VPMode = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else 
		VPMode = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// present parameters
	D3DDISPLAYMODE displayMode;
	hr = D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
	_Assert(SUCCEEDED(hr));

	// check multiSampleType
	multiSampleType = params.multisampleType;
	DWORD multisampleQuality = 0;

	if(SUCCEEDED(D3D->CheckDeviceMultiSampleType(d3dcaps.AdapterOrdinal, d3dcaps.DeviceType, displayMode.Format, 
		TRUE, params.multisampleType, &multisampleQuality)))
	{
		multiSampleType = params.multisampleType;
	}
	else
	{
		multiSampleType = D3DMULTISAMPLE_NONE;
	}

	//D3DPRESENT_PARAMETERS presentParams;
	D3DPRESENT_PARAMETERS presentParams;
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

	IDirect3DSwapChain9* swapChain = NULL;
	D3DDevice->GetSwapChain(0, &swapChain);

	D3DVIEWPORT9 primaryViewPort;
	primaryViewPort.X = 0;
	primaryViewPort.Y = 0;
	primaryViewPort.Width = bufferWidth;
	primaryViewPort.Height = bufferHeight;
	primaryViewPort.MinZ = 0;
	primaryViewPort.MaxZ = 1.0f;

	SwapChain* primarySwapChain = New SwapChain();
	primarySwapChain->d3dSwapChain = swapChain;
	primarySwapChain->presentParams = presentParams;
	primarySwapChain->viewPort = primaryViewPort;

	swapChainList.push_back(primarySwapChain);
}

bool Driver::Reset()
{
	if(FAILED(D3DDevice->Reset(&swapChainList[0]->presentParams)))
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

void Driver::SetViewPort(int swapChainIndex)
{
	_Assert(swapChainIndex == activeSwapChainIndex);
	D3DDevice->SetViewport(&swapChainList[activeSwapChainIndex]->viewPort);
}

void Driver::GetViewPort(int swapChainIndex, Vector2* vpOrigin, Vector2* vpSize)
{
	_Assert(NULL != D3DDevice);
	_Assert(swapChainIndex < (int)swapChainList.size());

	D3DVIEWPORT9 vp = GetViewPort(swapChainIndex);

	Vector2 origin = Vector2((float)vp.X, (float)vp.Y);
	Vector2 size = Vector2((float)vp.Width, (float)vp.Height);

	if(vpOrigin)
		*vpOrigin = origin;

	if(vpSize)
		*vpSize = size;
}


void Driver::Destory()
{
	for(std::vector<SwapChain*>::iterator iter = swapChainList.begin(); iter != swapChainList.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	swapChainList.clear();

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
	_Assert(activeSwapChainIndex < (int)swapChainList.size());
	if(activeSwapChainIndex == 0)
	{
		return swapChainList[activeSwapChainIndex]->d3dSwapChain->Present(NULL, NULL, hWnd, NULL, 0);
	}
	else
	{
		_Assert(_hWnd != NULL);
		return swapChainList[activeSwapChainIndex]->d3dSwapChain->Present(NULL, NULL, _hWnd, NULL, 0);
	}
}

void Driver::GetScreenLocation(const Vector2& screenPos, Vector2* screenLocation)
{
	Vector2 vpOrigin;
	Vector2 vpSize;

	GetViewPort(0, &vpOrigin, &vpSize);
	_Assert(vpSize.x != 0 && vpSize.y != 0);

	Vector2 vpPos = screenPos - vpOrigin;

	Vector2& location = *screenLocation;
	location.x = vpPos.x / vpSize.x;
	location.y = vpPos.y / vpSize.y;

	Clamp(location.x, 0, 1.0f);
	Clamp(location.y, 0, 1.0f);

	return;
}

void Driver::RenderToSwapChain(int swapChainIndex)
{
	_Assert(swapChainIndex < (int)swapChainList.size());
	activeSwapChainIndex = swapChainIndex;

	IDirect3DSurface9* backBuffer = NULL;

	IDirect3DSwapChain9* swapChain = swapChainList[swapChainIndex]->d3dSwapChain;
	_Assert(swapChain != NULL);
	swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	D3DDevice->SetRenderTarget(0, backBuffer);
	SetViewPort(activeSwapChainIndex);

	SAFE_RELEASE(backBuffer);
}

void Driver::OnLostDevice()
{
	for(size_t i = 1; i < swapChainList.size(); ++i)
	{
		SAFE_RELEASE(swapChainList[i]->d3dSwapChain);
	}
}

void Driver::OnResetDevice()
{
	for(size_t i = 1; i < swapChainList.size(); ++i)
	{
		D3DDevice->CreateAdditionalSwapChain(&swapChainList[i]->presentParams, &swapChainList[i]->d3dSwapChain);
	}
}

int Driver::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS params)
{
	SwapChain* swapChain = New SwapChain();

	D3DDevice->CreateAdditionalSwapChain(&params, &swapChain->d3dSwapChain);

	D3DVIEWPORT9 viewPort;
	viewPort.X = 0;
	viewPort.Y = 0;
	viewPort.Width = params.BackBufferWidth;
	viewPort.Height = params.BackBufferHeight;
	viewPort.MinZ = 0;
	viewPort.MaxZ = 1.0f;

	swapChain->presentParams = params;
	swapChain->viewPort = viewPort;

	swapChainList.push_back(swapChain);

	return swapChainList.size() - 1;
}

D3DPRESENT_PARAMETERS Driver::GetPresentParameters(int swapChainIndex)
{
	_Assert(swapChainIndex < (int)swapChainList.size());

	return swapChainList[swapChainIndex]->presentParams;
}

IDirect3DDevice9* Driver::GetD3DDevice()
{
	return D3DDevice;
}

HWND Driver::GetHWnd()
{
	return hWnd;
}

D3DVIEWPORT9 Driver::GetViewPort(int swapChainIndex)
{
	_Assert(swapChainIndex < (int)swapChainList.size());
	return swapChainList[swapChainIndex]->viewPort;
}
