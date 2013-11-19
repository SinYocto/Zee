#include "D3DUtility.h"
#include "Input.h"

//LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
//{
//	switch(Msg)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break; 
//
//	case WM_ACTIVATEAPP:
//		if(Input::DIDKeyboard) 
//			Input::DIDKeyboard->Acquire();
//
//		if(Input::DIDMouse) 
//			Input::DIDMouse->Acquire();
//
//		break;
//	}
//
//	return DefWindowProc(hWnd, Msg, wParam, lParam);
//}
//
//HWND RegisterAndCreateWindow(LPCWSTR wndClassName, LPCWSTR wndName, int wndWidth, int wndHeight, WNDPROC WndProc)
//{
//	HINSTANCE hInstance = GetModuleHandle(0);
//	HWND hWnd = NULL;
//
//	WNDCLASSEX wndClass;
//
//	wndClass.cbSize			= sizeof(WNDCLASSEX); 
//	wndClass.style			= CS_HREDRAW | CS_VREDRAW;
//	wndClass.lpfnWndProc	= WndProc;
//	wndClass.cbClsExtra		= 0;
//	wndClass.cbWndExtra		= 0;
//	wndClass.hInstance		= hInstance;
//	wndClass.hIcon			= NULL;
//	wndClass.hCursor		= NULL;
//	wndClass.hbrBackground  = NULL;
//	wndClass.lpszMenuName	= NULL;
//	wndClass.lpszClassName  = wndClassName;
//	wndClass.hIconSm		= NULL;
//
//	RegisterClassEx(&wndClass);
//
//	hWnd = CreateWindow(wndClassName, wndName, WS_CAPTION | WS_SYSMENU, 0, 0, wndWidth, wndHeight, NULL, NULL, hInstance, NULL);
//	Assert(NULL != hWnd);
//
//	ShowWindow(hWnd, SW_NORMAL);
//	UpdateWindow(hWnd);
//
//Exit:
//	return hWnd;
//}

bool CreateD3DDevice(HWND hWnd, int bufferWidth, int bufferHeight, _D3DMULTISAMPLE_TYPE multisampleType, 
					 IDirect3D9** ppD3D, IDirect3DDevice9** ppD3DDevice, D3DPRESENT_PARAMETERS* presentParams)
{
	bool bResult = false;
	{
		Assert(NULL != ppD3D);
		Assert(NULL != ppD3DDevice);

		*ppD3D = Direct3DCreate9(D3D_SDK_VERSION);

		// identify adapter
		D3DADAPTER_IDENTIFIER9 adapterIdentifier;
		HRESULT hr = (*ppD3D)->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifier);
		Assert(SUCCEEDED(hr));

		Log(L"graphics card:%S\n", adapterIdentifier.Description);

		// device capacity
		D3DCAPS9 d3dcaps;
		(*ppD3D)->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);

		int VPMode = 0;
		if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			VPMode = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else 
			VPMode = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		_D3DMULTISAMPLE_TYPE multisample = multisampleType;
		DWORD multisampleQuality = 0;
		if(SUCCEEDED((*ppD3D)->CheckDeviceMultiSampleType(d3dcaps.AdapterOrdinal, d3dcaps.DeviceType, D3DFMT_A8B8G8R8, FALSE, multisampleType, &multisampleQuality)))
			multisample = multisampleType;
		else
			multisample = D3DMULTISAMPLE_NONE;

		// present parameters
		D3DDISPLAYMODE displayMode;
		hr = (*ppD3D)->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
		Assert(SUCCEEDED(hr));

		ZeroMemory(presentParams,sizeof(*presentParams));

		(*presentParams).Windowed				= true;
		(*presentParams).SwapEffect				= D3DSWAPEFFECT_DISCARD;
		(*presentParams).BackBufferWidth		= bufferWidth;
		(*presentParams).BackBufferHeight		= bufferHeight;
		(*presentParams).BackBufferFormat		= displayMode.Format;
		(*presentParams).MultiSampleType		= multisampleType;
		(*presentParams).MultiSampleQuality		= multisampleQuality - 1;
		(*presentParams).EnableAutoDepthStencil	= TRUE;
		(*presentParams).AutoDepthStencilFormat	= D3DFMT_D24S8;
		(*presentParams).PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;

		hr = (*ppD3D)->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, VPMode, presentParams, ppD3DDevice);
		Assert(SUCCEEDED(hr));
	}

	bResult = true;
Exit:
	return bResult;
}

bool run()
{
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			return false;
	}

	return true;
}

void CreateVB(IDirect3DDevice9* d3dDevice,IDirect3DVertexBuffer9** vb, void* vertexData, int numVertices, VertexType vertexType)
{
	int sizeofVertex;
	DWORD vertexFVF = D3DFVF_XYZ;

	switch(vertexType)
	{
		case XYZ:
			sizeofVertex = 12;
			vertexFVF = D3DFVF_XYZ;
			break;

		case XYZ_UV:
			sizeofVertex = 20;
			vertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;
			break;

		case XYZ_N:
			sizeofVertex = 24;
			vertexFVF = D3DFVF_XYZ | D3DFVF_NORMAL;
			break;

		case XYZ_UV_N:
			sizeofVertex = 32;
			vertexFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
			break;

		case XYZ_UV_TBN:
			sizeofVertex = 56;
			vertexFVF = 0;
			break;

		case XYZRHW_UV:
			sizeofVertex = 24;
			vertexFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
			break;

		case XYZRHW_D:
			sizeofVertex = sizeof(VertexXYZRHWD);
			vertexFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
			break;
	}

	void *ptr;
	HRESULT hr = d3dDevice->CreateVertexBuffer(sizeofVertex*numVertices, 0, vertexFVF, D3DPOOL_DEFAULT, vb, 0);
	_Assert(SUCCEEDED(hr));
	(*vb)->Lock(0,0, (void**)&ptr, 0);
	memcpy(ptr, vertexData, sizeofVertex*numVertices);
	(*vb)->Unlock();
}

void CreateIB(IDirect3DDevice9* d3dDevice, IDirect3DIndexBuffer9** ib, DWORD* indexData, int numIndices)
{
	BYTE* ptr;
	d3dDevice->CreateIndexBuffer(sizeof(DWORD)*numIndices, 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, ib, 0);
	(*ib)->Lock(0, 0, (void**)&ptr, 0);
	memcpy(ptr, indexData, sizeof(DWORD)*numIndices);
	(*ib)->Unlock();
}

D3DCOLOR ColorLerp(D3DCOLOR color1, D3DCOLOR color2, float factor)
{
	if(factor > 1)
		factor = 1;
	if(factor < 0)
		factor = 0;

	BYTE a1 = (color1>>24) & 0xff;
	BYTE r1 = (color1>>16) & 0xff;
	BYTE g1 = (color1>>8) & 0xff;
	BYTE b1 = (color1) & 0xff;

	BYTE a2 = (color2>>24) & 0xff;
	BYTE r2 = (color2>>16) & 0xff;
	BYTE g2 = (color2>>8) & 0xff;
	BYTE b2 = (color2) & 0xff;

	BYTE a = (BYTE)((1-factor)*a1 + factor*a2);
	BYTE r = (BYTE)((1-factor)*r1 + factor*r2);
	BYTE g = (BYTE)((1-factor)*g1 + factor*g2);
	BYTE b = (BYTE)((1-factor)*b1 + factor*b2);

	D3DCOLOR color;

	((BYTE*)&color)[0] = b;		// B
	((BYTE*)&color)[1] = g;			// G
	((BYTE*)&color)[2] = r;			// R
	((BYTE*)&color)[3] = a;			// A

	return color;
}

float Lerp(float value1, float value2, float factor)
{
	return (1-factor)*value1 + factor*value2;
}
D3DXCOLOR HSL2RGB(Vector3 hsl)
{
	float h = hsl.x;
	float s = hsl.y;
	float l = hsl.z;

	if(s == 0)
	{
		return D3DXCOLOR(l, l, l, 1.0f);
	}

	float v1, v2;
	if(l < 0.5f)
		v2 = l*(1 + s);
	else
		v2 = (l + s) - s*l;
	v1 = 2*l - v2;

	float r = Hue2RGB(v1, v2, h + 1.0f/3.0f);
	float g = Hue2RGB(v1, v2, h);
	float b = Hue2RGB(v1, v2, h - 1.0f/3.0f);

	return D3DXCOLOR(r, g, b, 1.0f);
}

float Hue2RGB(float v1, float v2, float vh)
{
	if(vh < 0)
		vh = vh + 1;
	if(vh > 1)
		vh = vh - 1;

	if(6.0f*vh < 1)
		return v1 + (v2 - v1)*6.0f*vh;

	if(2.0f*vh < 1)
		return v2;

	if(3.0f*vh < 2)
		return v1 + (v2 - v1)*(2.0f/3.0f - vh)*6.0f;

	return v1;
}


Vector3 RGB2HSL(D3DXCOLOR rgb)
{
	float h,s,l;

	float vMin = (rgb.r < rgb.g) ? ((rgb.r < rgb.b)?rgb.r:rgb.b) : ((rgb.g < rgb.b)?rgb.g:rgb.b);
	float vMax = (rgb.r > rgb.g) ? ((rgb.r > rgb.b)?rgb.r:rgb.b) : ((rgb.g > rgb.b)?rgb.g:rgb.b);
	float delMax = vMax - vMin;

	l = (vMin + vMax)/2.0f;
	if(delMax == 0)
	{
		h = 0;
		s = 0;
	}
	else
	{
		if(l < 0.5f)
			s = delMax / (vMax + vMin);
		else
			s = delMax / (2 - vMax -vMin);

		float delR = ((vMax - rgb.r)/6.0f + delMax/2.0f)/delMax;
		float delG = ((vMax - rgb.g)/6.0f + delMax/2.0f)/delMax;
		float delB = ((vMax - rgb.b)/6.0f + delMax/2.0f)/delMax;

		if(rgb.r == vMax)
			h = delB - delG;
		else if(rgb.g == vMax)
			h = 1.0f/3.0f + delR - delB;
		else if(rgb.b == vMax)
			h = 2.0f/3.0f + delG - delR;

		if(h < 0)
			h += 1;
		if(h > 1)
			h -= 1;
	}

	return Vector3(h, s, l);
}

int SizeofVertex(VertexType vertexType)
{
	int sizeofVertex = 0;

	switch(vertexType)
	{
	case XYZ:
		sizeofVertex = 12;
		break;
	case XYZ_UV:
		sizeofVertex = 20;
		break;
	case XYZ_N:
		sizeofVertex = 24;
		break;
	case XYZ_UV_N:
		sizeofVertex = 32;
		break;
	case XYZ_UV_TBN:
		sizeofVertex = 56;
		break;
	case XYZRHW_UV:
		sizeofVertex = 24;
		break;
	case XYZRHW_D:
		sizeofVertex = 20;
		break;
	}

	return sizeofVertex;
}
