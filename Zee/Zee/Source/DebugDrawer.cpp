#include "DebugDrawer.h"
#include "D3DUtility.h"
#include "Camera.h"
#include "Primitive.h"
#include "Mesh.h"
#include "MaterialManager.h"
#include "Input.h"

bool DebugDrawer::DrawLine(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera, 
						   const D3DXMATRIX& matWorld /*= IDENTITY_MATRIX*/, LINE_TYPE lineType /* = LINE_STRIP */)
{
	bool isSucceed = false;

	Assert(NULL != camera);

	if(lineType == LINE_STRIP && points.size() > 1)
	{
		std::vector<VertexXYZD> verts;
		for(size_t i = 0; i < points.size(); ++i)
		{
			VertexXYZD vert(points[i].x, points[i].y, points[i].z, color);
			verts.push_back(vert);
		}

		Driver::D3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
		Driver::D3DDevice->SetTransform(D3DTS_VIEW, &camera->ViewMatrix());
		Driver::D3DDevice->SetTransform(D3DTS_PROJECTION, &camera->ProjMatrix());

		Driver::D3DDevice->SetRenderState(D3DRS_LIGHTING, false);
		Driver::D3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		Driver::D3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points.size() - 1, &verts[0], sizeof(VertexXYZD));
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawCircle(const Vector3& center, const Vector3& normal, float radius, D3DCOLOR color, bool isSolid,
							 Camera* camera, int numSegments /*= 64*/)
{
	bool isSucceed = false;

	{
		Assert(NULL != camera);

		std::vector<Vector3> points;

		float theta = 0;
		float step = 2 * PI / numSegments;
		for(int i = 0; i <= numSegments; ++i)
		{
			Vector3 point(radius * cos(theta), 0, radius * sin(theta));
			points.push_back(point);

			theta += step;
		}

		Vector3 normalizedN = normal.Normalized();
		if(FloatUnequal(fabsf(normalizedN.Dot(Vector3(0, 1, 0))), 1, 0.0001f))
		{
			Quaternion rotation = Quaternion::VectorRotation(Vector3(0, 1, 0), normalizedN);

			for(size_t i = 0; i < points.size(); ++i)
			{
				Vector3& point = points[i];
				point = point * rotation;

				point += center;
			}
		}

		if(isSolid)
		{
			Assert(drawSolidTriFan(points, color, camera));
		}
		else
		{
			Assert(DrawLine(points, color, camera));
		}
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawSquare(const Vector3& center, const Vector3& normal, float size, 
							 D3DCOLOR color, bool isSolid, Camera* camera)
{
	bool isSucceed = false;

	{
		Assert(NULL != camera);

		std::vector<Vector3> points;
		points.push_back(Vector3(-size / 2, 0, -size / 2));
		points.push_back(Vector3( size / 2, 0, -size / 2));
		points.push_back(Vector3( size / 2, 0,  size / 2));
		points.push_back(Vector3(-size / 2, 0,  size / 2));
		points.push_back(Vector3(-size / 2, 0, -size / 2));

		Vector3 normalizedN = normal.Normalized();
		//if(FloatUnequal(fabsf(normalizedN.Dot(Vector3(0, 1, 0))), 1, 0.0001f))
		{
			Quaternion rotation = Quaternion::VectorRotation(Vector3(0, 1, 0), normalizedN);

			for(size_t i = 0; i < points.size(); ++i)
			{
				Vector3& point = points[i];
				point = point * rotation;

				point += center;
			}
		}

		if(isSolid)
		{
			Assert(drawSolidTriFan(points, color, camera));
		}
		else
		{
			Assert(DrawLine(points, color, camera));
		}
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawBox(const Vector3& center, const Quaternion& rotation, const Vector3& size, D3DCOLOR color, Camera* camera)
{
	bool isSucceed = false;

	{
		Assert(NULL != camera);

		std::vector<Vector3> points;
		points.reserve(8);

		float posX = size.x / 2;
		float negX = - size.x / 2;

		float posY = size.y / 2;
		float negY = - size.y / 2;

		float posZ = size.z / 2;
		float negZ = - size.z / 2;

		points.push_back(Vector3(negX, posY, posZ));
		points.push_back(Vector3(posX, posY, posZ));
		points.push_back(Vector3(negX, posY, negZ));
		points.push_back(Vector3(posX, posY, negZ));

		points.push_back(Vector3(negX, negY, posZ));
		points.push_back(Vector3(posX, negY, posZ));
		points.push_back(Vector3(negX, negY, negZ));
		points.push_back(Vector3(posX, negY, negZ));

		for(size_t i = 0; i < points.size(); ++i)
		{
			points[i] = points[i] * rotation;
			points[i] += center;
		}

		std::vector<Vector3> pointSets;
		pointSets.push_back(points[0]);
		pointSets.push_back(points[1]);
		pointSets.push_back(points[3]);
		pointSets.push_back(points[2]);
		pointSets.push_back(points[0]);
		pointSets.push_back(points[4]);
		pointSets.push_back(points[6]);
		pointSets.push_back(points[7]);
		pointSets.push_back(points[5]);
		pointSets.push_back(points[4]);
		pointSets.push_back(points[6]);
		pointSets.push_back(points[2]);
		pointSets.push_back(points[3]);
		pointSets.push_back(points[7]);
		pointSets.push_back(points[5]);
		pointSets.push_back(points[1]);

		Assert(DrawLine(pointSets, color, camera));
	}

	isSucceed = true;
Exit:
	return isSucceed;
}

bool DebugDrawer::DrawAABBox(const AABBox& box, D3DCOLOR color, Camera* camera)
{
	Vector3 center = 0.5f * (box.mMin + box.mMax);
	Vector3 size = box.mMax - box.mMin;

	return DrawBox(center, Quaternion(0, 0, 0), size, 0xffff0000, camera);
}

bool DebugDrawer::drawSolidTriFan(const std::vector<Vector3>& points, D3DCOLOR color, Camera* camera)
{
	bool isSucceed = false;

	Assert(NULL != camera);

	if(points.size() >= 3)
	{
		std::vector<VertexXYZD> verts;
		for(size_t i = 0; i < points.size(); ++i)
		{
			VertexXYZD vert(points[i].x, points[i].y, points[i].z, color);
			verts.push_back(vert);
		}

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		Driver::D3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
		Driver::D3DDevice->SetTransform(D3DTS_VIEW, &camera->ViewMatrix());
		Driver::D3DDevice->SetTransform(D3DTS_PROJECTION, &camera->ProjMatrix());

		Driver::D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		Driver::D3DDevice->SetRenderState(D3DRS_LIGHTING, false);		// TODO:管理RenderState
		Driver::D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		
		Driver::D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Driver::D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		Driver::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		Driver::D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		Driver::D3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		Driver::D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points.size() - 1, &verts[0], sizeof(VertexXYZD));

		Driver::D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	isSucceed = true;
Exit:
	return isSucceed;
	
}

void TransGizmo::Init()
{
	Vector2 vpSize;
	Driver::GetViewPort(NULL, &vpSize);

	Driver::D3DDevice->CreateRenderTarget((UINT)vpSize.x, (UINT)vpSize.y, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 
		0, true, &mRenderTarget, NULL);

	Driver::D3DDevice->CreateDepthStencilSurface((UINT)vpSize.x, (UINT)vpSize.y, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 
		0, true, &mDepthStencil, NULL);

	Cylinder* coneGeo = new Cylinder(L"", 0, 0.06f, 0.18f);
	Cylinder* lineGeo = new Cylinder(L"", 0.01f, 0.01f, 1.0f);

	coneGeo->CalculateNormals();
	coneGeo->BuildGeometry(XYZ_N);

	lineGeo->CalculateNormals();
	lineGeo->BuildGeometry(XYZ_N);

	mCone = new Mesh(L"", NULL, coneGeo, NULL);
	_Assert(mCone);

	mLine = new Mesh(L"", NULL, lineGeo, NULL);
	_Assert(mLine);
}

void TransGizmo::getPickColor(D3DCOLOR* pickColor, const int pickPixelSize)
{
	_Assert(NULL != pickColor);
	*pickColor = 0;

	RECT pickRect;
	D3DLOCKED_RECT pickLockedRect;

	Vector2 vpSize;
	Driver::GetViewPort(NULL, &vpSize);

	SetRect(&pickRect, Input::cursorPos.x - pickPixelSize / 2, Input::cursorPos.y - pickPixelSize / 2, 
		Input::cursorPos.x + pickPixelSize / 2, Input::cursorPos.y + pickPixelSize / 2);

	if(pickRect.left < 0)
		pickRect.left = 0;
	if(pickRect.left > vpSize.x)
		pickRect.left = (LONG)vpSize.x;

	if(pickRect.right < 0)
		pickRect.right = 0;
	if(pickRect.right > vpSize.x)
		pickRect.right = (LONG)vpSize.x;

	if(pickRect.top < 0)
		pickRect.top = 0;
	if(pickRect.top > vpSize.y)
		pickRect.top = (LONG)vpSize.y;

	if(pickRect.bottom < 0)
		pickRect.bottom = 0;
	if(pickRect.bottom > vpSize.y)
		pickRect.bottom = (LONG)vpSize.y;

	HRESULT hr = mRenderTarget->LockRect(&pickLockedRect, &pickRect, D3DLOCK_READONLY);
	if(SUCCEEDED(hr))
	{
		bool bFind = false;
		for(int i = 0; i <= pickRect.bottom - pickRect.top; ++i)
		{
			DWORD* ptr = (DWORD*)pickLockedRect.pBits + i * pickLockedRect.Pitch / 4;
			for(int j = 0; j <= pickRect.right - pickRect.left; ++j)
			{
				D3DCOLOR pixelColor = *(D3DCOLOR*)ptr;

				if(pixelColor != COLOR_NONE)
				{
					*pickColor = pixelColor;
					break;
				}
				ptr++;
			}

			if(bFind)
				break;
		}

		mRenderTarget->UnlockRect(); 
	}
}

void TransGizmo::determinSelectType(Object* obj, Camera* camera)
{
	if(Input::GetLeftButton())		// 左键按下不更新, 保持之前选择状态
		return;

	draw(obj, camera, true);		// flatMtl绘制, 用于colorPick

	D3DCOLOR pickColor = 0;
	getPickColor(&pickColor, 8);

	switch(pickColor)
	{
	case COLOR_NONE:
		mSelected = SELECT_NONE;
		break;

	case COLOR_X:
		mSelected = SELECT_X;
		break;

	case COLOR_Y:
		mSelected = SELECT_Y;
		break;

	case COLOR_Z:
		mSelected = SELECT_Z;
		break;

	case COLOR_XY_PICK:
		mSelected = SELECT_XY;
		break;

	case COLOR_XZ_PICK:
		mSelected = SELECT_XZ;
		break;

	case COLOR_YZ_PICK:
		mSelected = SELECT_YZ;
		break;

	default:
		mSelected = SELECT_NONE;
		break;
	}
}

void TransGizmo::Draw(Object* obj, Camera* camera)
{
	determinSelectType(obj, camera);
	draw(obj, camera, false);
}

void TransGizmo::Destroy()
{
	SAFE_DELETE(mCone);
	SAFE_DELETE(mLine);

	SAFE_RELEASE(mRenderTarget);
	SAFE_RELEASE(mDepthStencil);
}

void TransGizmo::draw(Object* obj, Camera* camera, bool isColorPickPass)
{
	IDirect3DSurface9* oldRT = NULL;
	IDirect3DSurface9* oldDS = NULL;
	Material* mtl = NULL;

	if(isColorPickPass)
		mtl = MaterialManager::flatMtl;
	else
		mtl = MaterialManager::viewMtl;

	if(isColorPickPass)
	{
		Driver::D3DDevice->GetRenderTarget(0, &oldRT);
		Driver::D3DDevice->GetDepthStencilSurface(&oldDS);

		_Assert(NULL != oldRT);
		_Assert(NULL != oldDS);

		Driver::D3DDevice->SetRenderTarget(0, mRenderTarget);			
		Driver::D3DDevice->SetDepthStencilSurface(mDepthStencil);

		Driver::Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f);
	}

	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / 6.0f;

	// 这里清了zbuffer, 以使gizmo始终在屏幕上显示并内部有深度关系, 所以gizmo在渲染流程的最后
	Driver::Clear(D3DCLEAR_ZBUFFER, 0xff000000, 1.0f);	

	D3DCOLOR colorX = COLOR_X;
	D3DCOLOR colorY = COLOR_Y;
	D3DCOLOR colorZ = COLOR_Z;
	D3DCOLOR colorXY = COLOR_XY_PICK;
	D3DCOLOR colorXZ = COLOR_XZ_PICK;
	D3DCOLOR colorYZ = COLOR_YZ_PICK;

	if(!isColorPickPass)
	{
		colorXY = COLOR_XY;
		colorXZ = COLOR_XZ;
		colorYZ = COLOR_YZ;

		if(mSelected == SELECT_X)
			colorX = (D3DCOLOR)COLOR_SELECTED;

		if(mSelected == SELECT_Y)
			colorY = (D3DCOLOR)COLOR_SELECTED;

		if(mSelected == SELECT_Z)
			colorZ = (D3DCOLOR)COLOR_SELECTED;

		if(mSelected == SELECT_XY)
		{
			colorXY = (D3DCOLOR)COLOR_SELECTED;
			SETALPHA(colorXY, 0x7f);
		}

		if(mSelected == SELECT_XZ)
		{
			colorXZ = (D3DCOLOR)COLOR_SELECTED;
			SETALPHA(colorXZ, 0x7f);
		}

		if(mSelected == SELECT_YZ)
		{
			colorYZ = (D3DCOLOR)COLOR_SELECTED;
			SETALPHA(colorYZ, 0x7f);
		}
	}

	const float SQUARE_SIZE = 0.4f;
	// y
	Material* tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(colorY);

	mLine->SetWorldPosition(obj->GetWorldPosition());
	mLine->SetWorldOrientation(obj->GetWorldOrient());
	mLine->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mLine->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCone->SetWorldPosition(obj->GetWorldPosition());
	mCone->SetWorldOrientation(obj->GetWorldOrient());
	mCone->TranslateLocal(0, scaleFactor, 0);
	mCone->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mCone->SetMaterial(tempMtl);
	mCone->Draw(camera);

	DebugDrawer::DrawSquare(obj->GetWorldPosition(scaleFactor * Vector3(SQUARE_SIZE * 0.5f, 0, SQUARE_SIZE * 0.5f)), 
		Vector3(0, 1.0f, 0), 0.8f * scaleFactor * SQUARE_SIZE, colorXZ, true, camera);

	tempMtl->Drop();

	// x
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(colorX);

	mLine->RotateLocal(0, 0, -PI / 2.0f);

	mLine->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCone->TranslateLocal(scaleFactor, -scaleFactor, 0);
	mCone->RotateLocal(0, 0, -PI / 2.0f);

	mCone->SetMaterial(tempMtl);
	mCone->Draw(camera);

	DebugDrawer::DrawSquare(obj->GetWorldPosition(scaleFactor * Vector3(0, SQUARE_SIZE * 0.5f, SQUARE_SIZE * 0.5f)), 
		Vector3(1.0f, 0, 0), 0.8f * scaleFactor * SQUARE_SIZE, colorYZ, true, camera);

	tempMtl->Drop(); 

	// z
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(colorZ);

	mLine->RotateLocal(PI / 2.0f, 0, 0);

	mLine->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCone->TranslateLocal(0, -scaleFactor, scaleFactor);
	mCone->RotateLocal(PI / 2.0f, 0, 0);

	mCone->SetMaterial(tempMtl);
	mCone->Draw(camera);

	DebugDrawer::DrawSquare(obj->GetWorldPosition(scaleFactor * Vector3(SQUARE_SIZE * 0.5f, SQUARE_SIZE * 0.5f, 0)), 
		Vector3(0, 0, 1.0f), 0.8f * scaleFactor * SQUARE_SIZE, colorXY, true, camera);

	tempMtl->Drop();

	if(isColorPickPass)
	{
		Driver::D3DDevice->SetRenderTarget(0, oldRT);			
		Driver::D3DDevice->SetDepthStencilSurface(oldDS);
	}
}
