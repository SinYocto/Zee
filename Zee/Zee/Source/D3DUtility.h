#ifndef D3DUTILITY_H
#define D3DUTILITY_H

#include <stdio.h>
#include <assert.h>
#include <Windows.h>

#include"d3d9.h"
#include"d3dx9.h"

#include "Math.h"

#define log printf
#define Assert(condition) \
	if(!(condition)) { printf("assert failed! condition:\"%s\"\n", #condition); goto Exit; }
#define _Assert(condition) \
	if(!(condition)) { printf("assert failed! condition:\"%s\"\n", #condition); }

#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) { delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p) = NULL; } }

#define MAX_STR_LEN 256
#define MAX_PATH_LEN 512

#define WHITE 0xffffffff
#define BLACK 0xff000000

#define D3DXCOLOR_WHITE D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define D3DXCOLOR_BLACK D3DXCOLOR(0, 0, 0, 1.0f)
#define D3DXCOLOR_RED D3DXCOLOR(1.0f, 0, 0, 1.0f)
#define D3DXCOLOR_YELLOW D3DXCOLOR(1.0f, 1.0f, 0, 1.0f)
#define D3DXCOLOR_BLUE D3DXCOLOR(0, 0, 1.0f, 1.0f)

extern HWND gHWnd;
extern IDirect3D9* gD3D;
extern IDirect3DDevice9 *gD3DDevice;

extern D3DPRESENT_PARAMETERS gPresentParameters;	// 设备恢复时需要使用

struct Vertex
{
	Vertex(float _x = 0, float _y = 0, float _z = 0)
		:x(_x)
		,y(_y)
		,z(_z)
	{

	}

	Vertex(const Vector3& pos)
		:x(pos.x)
		,y(pos.y)
		,z(pos.z)
	{

	}

	float x;
	float y;
	float z;

	static const DWORD FVF = D3DFVF_XYZ;
};

struct VertexUV
{
    VertexUV(float _x = 0, float _y = 0, float _z = 0, float _u = 0, float _v = 0)
        :x(_x)
        ,y(_y)
        ,z(_z)
        ,u(_u)
        ,v(_v)
	{

	}

    VertexUV(const Vector3& pos, float _u, float _v)
        :x(pos.x)
        ,y(pos.y)
        ,z(pos.z)
        ,u(_u)
        ,v(_v)
	{

	}

    float x;
    float y;
    float z;

    float u;
    float v;

    static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};

struct VertexN
{
	VertexN(float _x = 0, float _y = 0, float _z = 0, float _nx = 0, float _ny = 0, float _nz = 0)
		:x(_x)
		,y(_y)
		,z(_z)
		,nx(_nx)
		,ny(_ny)
		,nz(_nz)
	{

	}

	VertexN(const Vector3& pos, const Vector2& uv, const Vector3& normal)
		:x(pos.x)
		,y(pos.y)
		,z(pos.z)
		,nx(normal.x)
		,ny(normal.y)
		,nz(normal.z)
	{

	}

	float x;
	float y;
	float z;

	float nx;
	float ny;
	float nz;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
};

struct VertexUVN
{	
	VertexUVN(float _x = 0, float _y = 0, float _z = 0, float _u = 0, float _v = 0, float _nx = 0, float _ny = 0, float _nz = 0)
		:x(_x)
		,y(_y)
		,z(_z)
		,u(_u)
		,v(_v)
		,nx(_nx)
		,ny(_ny)
		,nz(_nz)
	{

	}

	VertexUVN(const Vector3& pos, const Vector2& uv, const Vector3& normal)
		:x(pos.x)
		,y(pos.y)
		,z(pos.z)
		,u(uv.x)
		,v(uv.y)
		,nx(normal.x)
		,ny(normal.y)
		,nz(normal.z)
	{

	}

	float x;
	float y;
	float z;
	
	float u;
	float v;

	float nx;
	float ny;
	float nz;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL;
};

struct VertexUVTBN
{
	VertexUVTBN(float _x = 0, float _y = 0, float _z = 0, float _u = 0, float _v = 0, float _tx = 0, float _ty = 0, float _tz = 0,
		float _bx = 0, float _by = 0, float _bz = 0, float _nx = 0, float _ny = 0, float _nz = 0)
		:x(_x)
		,y(_y)
		,z(_z)
		,u(_u)
		,v(_v)
		,tx(_tx)
		,ty(_ty)
		,tz(_tz)
		,bx(_bx)
		,by(_by)
		,bz(_bz)
		,nx(_nx)
		,ny(_ny)
		,nz(_nz)
	{

	}

	VertexUVTBN(const Vector3& pos, const Vector2& uv, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal)
		:x(pos.x)
		,y(pos.y)
		,z(pos.z)
		,u(uv.x)
		,v(uv.y)
		,tx(tangent.x)
		,ty(tangent.y)
		,tz(tangent.z)
		,bx(bitangent.x)
		,by(bitangent.y)
		,bz(bitangent.z)
		,nx(normal.x)
		,ny(normal.y)
		,nz(normal.z)
	{

	}

	float x;
	float y;
	float z;

	float u;
	float v;

	float tx;
	float ty;
	float tz;

	float bx;
	float by;
	float bz;

	float nx;
	float ny;
	float nz;
};

struct VertexXYZRHWUV
{
    VertexXYZRHWUV(float _x = 0, float _y = 0, float _z = 0, float _rhw = 0, float _u = 0, float _v = 0)
        :x(_x)
        ,y(_y)
        ,z(_z)
        ,rhw(_rhw)
        ,u(_u)
        ,v(_v)
	{}

    float x;
    float y;
    float z;
    float rhw;

    float u;
    float v;

    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
};

struct VertexXYZRHWD
{
	VertexXYZRHWD(float _x = 0, float _y = 0, float _z = 0, float _rhw = 0, D3DCOLOR _color = 0)
		:x(_x)
		,y(_y)
		,z(_z)
		,rhw(_rhw)
		,color(_color)
	{

	}

	float x;
	float y;
	float z;
	float rhw;

	D3DCOLOR color;

	static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

enum VertexType { XYZ, XYZ_UV, XYZ_N, XYZ_UV_N, XYZ_UV_TBN, XYZRHW_UV, XYZRHW_D };

HWND RegisterAndCreateWindow(LPCWSTR wndClassName, LPCWSTR wndName, int wndWidth, int wndHeight, WNDPROC WndProc);
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
bool CreateD3DDevice(HWND hWnd, int bufferWidth, int bufferHeight, _D3DMULTISAMPLE_TYPE multisampleType, 
					 IDirect3D9** ppD3D, IDirect3DDevice9** ppD3DDevice, D3DPRESENT_PARAMETERS* presentParams);
bool run();

void CreateVB(IDirect3DDevice9* d3dDevice, IDirect3DVertexBuffer9** vb, void* VertexData, int numVertices, VertexType vertexType);
void CreateIB(IDirect3DDevice9* d3dDevice, IDirect3DIndexBuffer9** ib, DWORD* IndexData, int numIndices);

int SizeofVertex(VertexType vertexType);

D3DCOLOR ColorLerp(D3DCOLOR color1, D3DCOLOR color2, float factor);
float Lerp(float value1, float value2, float factor);

float Hue2RGB(float v1, float v2, float vh);
D3DXCOLOR HSL2RGB(Vector3 hsl);
Vector3 RGB2HSL(D3DXCOLOR rgb);

#endif