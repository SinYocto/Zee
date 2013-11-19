#include "Gizmo.h"

#include "D3DUtility.h"
#include "Engine.h"
#include "Camera.h"
#include "Primitive.h"
#include "MeshNode.h"
#include "Input.h"

void Gizmo::OnLostDevice()
{
	_Assert(mCone && mCone->GetMesh() && mCone->GetMesh()->GetGeometry());
	_Assert(mLine && mLine->GetMesh() && mLine->GetMesh()->GetGeometry());
	_Assert(mTorus && mTorus->GetMesh() && mTorus->GetMesh()->GetGeometry());
	_Assert(mCube && mCube->GetMesh() && mCube->GetMesh()->GetGeometry());

	mCone->GetMesh()->GetGeometry()->OnLostDevice();
	mLine->GetMesh()->GetGeometry()->OnLostDevice();
	mTorus->GetMesh()->GetGeometry()->OnLostDevice();
	mCube->GetMesh()->GetGeometry()->OnLostDevice();

	SAFE_RELEASE(mRenderTarget);
	SAFE_RELEASE(mDepthStencil);
}

void Gizmo::OnResetDevice()
{
	_Assert(mCone && mCone->GetMesh() && mCone->GetMesh()->GetGeometry());
	_Assert(mLine && mLine->GetMesh() && mLine->GetMesh()->GetGeometry());
	_Assert(mTorus && mTorus->GetMesh() && mTorus->GetMesh()->GetGeometry());
	_Assert(mCube && mCube->GetMesh() && mCube->GetMesh()->GetGeometry());

	mCone->GetMesh()->GetGeometry()->OnResetDevice();
	mLine->GetMesh()->GetGeometry()->OnResetDevice();
	mTorus->GetMesh()->GetGeometry()->OnResetDevice();
	mCube->GetMesh()->GetGeometry()->OnResetDevice();

	createRenderTargetDepthStencile();
}

void Gizmo::Init()
{
	createRenderTargetDepthStencile();

	Cylinder* coneGeo = new Cylinder(L"", 0, 0.06f, 0.18f);
	Cylinder* lineGeo = new Cylinder(L"", 0.01f, 0.01f, 1.0f);
	Torus* torusGeo = new Torus(L"", 1.0f, 0.01f, 32, 8);
	Cube* cubeGeo = new Cube(L"", 0.1f);

	coneGeo->CalculateNormals();
	coneGeo->BuildGeometry(XYZ_N);

	lineGeo->CalculateNormals();
	lineGeo->BuildGeometry(XYZ_N);

	torusGeo->CalculateNormals();
	torusGeo->BuildGeometry(XYZ_N);

	cubeGeo->CalculateNormals();
	cubeGeo->BuildGeometry(XYZ_N);

	mCone = new MeshNode(L"", NULL, coneGeo, NULL);
	_Assert(mCone);

	mLine = new MeshNode(L"", NULL, lineGeo, NULL);
	_Assert(mLine);

	mTorus = new MeshNode(L"", NULL, torusGeo, NULL);
	_Assert(mTorus);

	mCube = new MeshNode(L"", NULL, cubeGeo, NULL);
	_Assert(mCube);

	SAFE_DROP(coneGeo);
	SAFE_DROP(lineGeo);
	SAFE_DROP(torusGeo);
	SAFE_DROP(cubeGeo);
}

void Gizmo::createRenderTargetDepthStencile()
{
	if(mRenderTarget)
		SAFE_RELEASE(mRenderTarget);

	if(mDepthStencil)
		SAFE_RELEASE(mDepthStencil);

	Driver* driver = gEngine->GetDriver();
	IDirect3DDevice9* d3dDevice = driver->GetD3DDevice();

	Vector2 vpSize;
	driver->GetViewPort(NULL, &vpSize);

	d3dDevice->CreateRenderTarget((UINT)vpSize.x, (UINT)vpSize.y, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 
		0, true, &mRenderTarget, NULL);

	d3dDevice->CreateDepthStencilSurface((UINT)vpSize.x, (UINT)vpSize.y, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 
		0, true, &mDepthStencil, NULL);
}

void Gizmo::getPickColor(D3DCOLOR* pickColor, const int pickPixelSize)
{
	_Assert(NULL != pickColor);
	*pickColor = 0;

	RECT pickRect;
	D3DLOCKED_RECT pickLockedRect;

	Vector2 vpSize;
	gEngine->GetDriver()->GetViewPort(NULL, &vpSize);

	POINT cursorPos = gEngine->GetInput()->GetCursorPos();

	SetRect(&pickRect, cursorPos.x - pickPixelSize / 2, cursorPos.y - pickPixelSize / 2, 
		cursorPos.x + pickPixelSize / 2, cursorPos.y + pickPixelSize / 2);

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

void Gizmo::determinSelectType(Object* obj, Camera* camera)
{
	_Assert(NULL != obj);
	_Assert(NULL != camera);

	if(gEngine->GetInput()->GetLeftButton())		// 左键按下不更新, 保持之前选择状态
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
		mSelected = AXIS_X;
		break;

	case COLOR_Y:
		mSelected = AXIS_Y;
		break;

	case COLOR_Z:
		mSelected = AXIS_Z;
		break;

	case COLOR_XY_PICK:
		mSelected = PLANE_XY;
		break;

	case COLOR_XZ_PICK:
		mSelected = PLANE_XZ;
		break;

	case COLOR_YZ_PICK:
		mSelected = PLANE_YZ;
		break;

	default:
		mSelected = SELECT_NONE;
		break;
	}
}

void Gizmo::Draw(Object* obj, Camera* camera)
{
	_Assert(NULL != camera);

	if(!obj || mActiveType == GIZMO_NONE)
		return;

	Input* input = gEngine->GetInput();

	determinSelectType(obj, camera);
	draw(obj, camera, false);

	static Vector3 tangentX;
	static Vector3 tangentY;
	static Vector3 tangentZ;

	if(mActiveType == GIZMO_TRANS)
	{
		if(input->GetLeftButtonDown())
			calTransTangent(obj, camera, &tangentX, &tangentY, &tangentZ);

		if(input->GetLeftButton())
			applyTrans(obj, camera, tangentX, tangentY, tangentZ);
	}

	if(mActiveType == GIZMO_ROTATE)
	{
		if(input->GetLeftButtonDown())
			calRotateTangent(obj, camera, &tangentX);

		if(input->GetLeftButton())
			applyRotate(obj, camera, tangentX);
	}

	if(mActiveType == GIZMO_SCALE)
	{
		if(input->GetLeftButtonDown())
			calTransTangent(obj, camera, &tangentX, &tangentY, &tangentZ);

		if(input->GetLeftButton())
			applyScale(obj, camera, tangentX, tangentY, tangentZ);
	}
}

void Gizmo::Destroy()
{
	SAFE_DELETE(mCone);
	SAFE_DELETE(mLine);
	SAFE_DELETE(mTorus);
	SAFE_DELETE(mCube);

	SAFE_RELEASE(mRenderTarget);
	SAFE_RELEASE(mDepthStencil);
}

void Gizmo::draw(Object* obj, Camera* camera, bool isColorPickPass)
{
	_Assert(NULL != obj);
	_Assert(NULL != camera);

	Driver* driver = gEngine->GetDriver();
	IDirect3DDevice9* d3dDevice = driver->GetD3DDevice();

	IDirect3DSurface9* oldRT = NULL;
	IDirect3DSurface9* oldDS = NULL;
	Material* mtl = NULL;

	if(isColorPickPass)
		mtl = gEngine->GetMaterialManager()->GetDefaultFlatMtl();
	else
		mtl = gEngine->GetMaterialManager()->GetDefaultViewMtl();

	if(isColorPickPass)
	{
		d3dDevice->GetRenderTarget(0, &oldRT);
		d3dDevice->GetDepthStencilSurface(&oldDS);

		_Assert(NULL != oldRT);
		_Assert(NULL != oldDS);

		d3dDevice->SetRenderTarget(0, mRenderTarget);			
		d3dDevice->SetDepthStencilSurface(mDepthStencil);

		driver->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f);
	}

	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	// 这里清了zbuffer, 以使gizmo始终在屏幕上显示并内部有深度关系, 所以gizmo在渲染流程的最后
	driver->Clear(D3DCLEAR_ZBUFFER, 0xff000000, 1.0f);	

	D3DCOLOR elementColor[6];
	elementColor[AXIS_X] = COLOR_X;
	elementColor[AXIS_Y] = COLOR_Y;
	elementColor[AXIS_Z] = COLOR_Z;
	elementColor[PLANE_XY] = COLOR_XY_PICK;
	elementColor[PLANE_XZ] = COLOR_XZ_PICK;
	elementColor[PLANE_YZ] = COLOR_YZ_PICK;

	if(!isColorPickPass)
	{
		elementColor[PLANE_XY] = COLOR_XY;
		elementColor[PLANE_XZ] = COLOR_XZ;
		elementColor[PLANE_YZ] = COLOR_YZ;

		if(mSelected != SELECT_NONE)
		{
			elementColor[mSelected] = (D3DCOLOR)COLOR_SELECTED;
			SETALPHA(elementColor[mSelected], 0x7f);
		}
	}

	switch(mActiveType)
	{
	case GIZMO_TRANS:
		drawTransGizmo(obj, camera, mtl, elementColor);
		break;

	case GIZMO_ROTATE:
		drawRotateGizmo(obj, camera, mtl, elementColor);
		break;

	case GIZMO_SCALE:
		drawScaleGizmo(obj, camera, mtl, elementColor);
		break;
	}

	if(isColorPickPass)
	{
		d3dDevice->SetRenderTarget(0, oldRT);			
		d3dDevice->SetDepthStencilSurface(oldDS);
	}
	
	SAFE_RELEASE(oldRT);		// GetRenderTarget方法会增加引用计数, 需要release, 否则设备无法恢复
	SAFE_RELEASE(oldDS);
}

void Gizmo::drawTransGizmo(Object* obj, Camera* camera, Material* mtl, D3DCOLOR elementsColor[6])
{
	const float SQUARE_SIZE = 0.4f;

	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	// y
	Material* tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_Y]);

	mLine->SetWorldPosition(obj->GetWorldPosition());
	mLine->SetWorldOrientation(obj->GetWorldOrient());
	mLine->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mLine->GetMesh()->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCone->SetWorldPosition(obj->GetWorldPosition());
	mCone->SetWorldOrientation(obj->GetWorldOrient());
	mCone->TranslateLocal(0, scaleFactor, 0);
	mCone->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mCone->GetMesh()->SetMaterial(tempMtl);
	mCone->Draw(camera);

	DebugDrawer::DrawSquare(obj->GetWorldPosition(scaleFactor * Vector3(SQUARE_SIZE * 0.5f, 0, SQUARE_SIZE * 0.5f)), 
		Vector3(0, 1.0f, 0), 0.8f * scaleFactor * SQUARE_SIZE, elementsColor[PLANE_XZ], true, camera);

	tempMtl->Drop();

	// x
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_X]);

	mLine->RotateLocal(0, 0, -PI / 2.0f);

	mLine->GetMesh()->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCone->TranslateLocal(scaleFactor, -scaleFactor, 0);
	mCone->RotateLocal(0, 0, -PI / 2.0f);

	mCone->GetMesh()->SetMaterial(tempMtl);
	mCone->Draw(camera);

	DebugDrawer::DrawSquare(obj->GetWorldPosition(scaleFactor * Vector3(0, SQUARE_SIZE * 0.5f, SQUARE_SIZE * 0.5f)), 
		Vector3(1.0f, 0, 0), 0.8f * scaleFactor * SQUARE_SIZE, elementsColor[PLANE_YZ], true, camera);

	tempMtl->Drop(); 

	// z
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_Z]);

	mLine->RotateLocal(PI / 2.0f, 0, 0);

	mLine->GetMesh()->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCone->TranslateLocal(0, -scaleFactor, scaleFactor);
	mCone->RotateLocal(PI / 2.0f, 0, 0);

	mCone->GetMesh()->SetMaterial(tempMtl);
	mCone->Draw(camera);

	DebugDrawer::DrawSquare(obj->GetWorldPosition(scaleFactor * Vector3(SQUARE_SIZE * 0.5f, SQUARE_SIZE * 0.5f, 0)), 
		Vector3(0, 0, 1.0f), 0.8f * scaleFactor * SQUARE_SIZE, elementsColor[PLANE_XY], true, camera);

	tempMtl->Drop();
}

void Gizmo::drawRotateGizmo(Object* obj, Camera* camera, Material* mtl, D3DCOLOR elementsColor[6])
{
	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	// y
	Material* tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_Y]);

	mTorus->SetWorldPosition(obj->GetWorldPosition());
	mTorus->SetWorldOrientation(obj->GetWorldOrient());
	mTorus->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mTorus->GetMesh()->SetMaterial(tempMtl);
	mTorus->Draw(camera);

	tempMtl->Drop();

	// x
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_X]);

	mTorus->RotateLocal(0, 0, - PI / 2.0f);

	mTorus->GetMesh()->SetMaterial(tempMtl);
	mTorus->Draw(camera);

	tempMtl->Drop();

	// z
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_Z]);

	mTorus->RotateLocal(- PI / 2.0f, 0, 0);

	mTorus->GetMesh()->SetMaterial(tempMtl);
	mTorus->Draw(camera);

	tempMtl->Drop();
}

void Gizmo::drawScaleGizmo(Object* obj, Camera* camera, Material* mtl, D3DCOLOR elementsColor[6])
{
	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	// y
	Material* tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_Y]);

	mLine->SetWorldPosition(obj->GetWorldPosition());
	mLine->SetWorldOrientation(obj->GetWorldOrient());
	mLine->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mLine->GetMesh()->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCube->SetWorldPosition(obj->GetWorldPosition());
	mCube->SetWorldOrientation(obj->GetWorldOrient());
	mCube->TranslateLocal(0, scaleFactor, 0);
	mCube->SetScale(Vector3(scaleFactor, scaleFactor, scaleFactor));

	mCube->GetMesh()->SetMaterial(tempMtl);
	mCube->Draw(camera);

	tempMtl->Drop();

	// x
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_X]);

	mLine->RotateLocal(0, 0, -PI / 2.0f);
	mLine->GetMesh()->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCube->TranslateLocal(scaleFactor, -scaleFactor, 0);
	mCube->GetMesh()->SetMaterial(tempMtl);
	mCube->Draw(camera);

	tempMtl->Drop(); 

	// z
	tempMtl = new Material(*mtl);
	tempMtl->SetDiffuseColor(elementsColor[AXIS_Z]);

	mLine->RotateLocal(PI / 2.0f, 0, 0);
	mLine->GetMesh()->SetMaterial(tempMtl);
	mLine->Draw(camera);

	mCube->TranslateLocal(-scaleFactor, 0, scaleFactor);
	mCube->GetMesh()->SetMaterial(tempMtl);
	mCube->Draw(camera);

	tempMtl->Drop();
}

void Gizmo::calTransTangent(Object* obj, Camera* camera, Vector3* tangentX, Vector3* tangentY, Vector3* tangentZ)
{
	D3DXMATRIX matWVP = obj->LocalToWorldMatrix() * camera->ViewMatrix() * camera->ProjMatrix();

	Vector3 posClipO;
	Vector3 posClipX;
	Vector3 posClipY;
	Vector3 posClipZ;

	GetClipSpacePos(Vector3::Zero, matWVP, &posClipO);
	GetClipSpacePos(Vector3(1.0f, 0, 0), matWVP, &posClipX);
	GetClipSpacePos(Vector3(0, 1.0f, 0), matWVP, &posClipY);
	GetClipSpacePos(Vector3(0, 0, 1.0f), matWVP, &posClipZ);

	Vector3 axisX = (posClipX - posClipO).Normalized();
	Vector3 axisY = (posClipY - posClipO).Normalized();
	Vector3 axisZ = (posClipZ - posClipO).Normalized();

	axisX.z = 0;
	axisY.z = 0;
	axisZ.z = 0;

	*tangentX = axisX;
	*tangentY = axisY;
	*tangentZ = axisZ;
}

void Gizmo::calRotateTangent(Object* obj, Camera* camera, Vector3* tangent)
{
	D3DXMATRIX matWVP = obj->LocalToWorldMatrix() * camera->ViewMatrix() * camera->ProjMatrix();

	Vector3 posScreenO;
	GetClipSpacePos(Vector3::Zero, matWVP, &posScreenO);
	posScreenO.y *= -1.0f;		// 屏幕原点使用左上方
	posScreenO = 0.5f * (posScreenO + Vector3(1.0f, 1.0f, 0));		// 转到0~1范围
	posScreenO.z = 0;

	POINT cursorPos = gEngine->GetInput()->GetCursorPos();

	Vector2 cursorLocation;
	gEngine->GetDriver()->GetScreenLocation(Vector2((float)cursorPos.x, (float)cursorPos.y), &cursorLocation);

	Vector3 posScreenCursor(cursorLocation.x, cursorLocation.y, 0);

	*tangent = (Vector3(0, 0, 1.0f).Cross(posScreenCursor -posScreenO)).Normalized();
}

void Gizmo::SetActiveType(GIZMO_TYPE type)
{
	mActiveType = type;
}

void Gizmo::applyTrans(Object* obj, Camera* camera, const Vector3& tangentX, const Vector3& tangentY, const Vector3& tangentZ)
{
	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	DIMOUSESTATE mouseState = gEngine->GetInput()->GetMouseState();
	Vector3 screenMoveVector((float)mouseState.lX / 1000, -(float)mouseState.lY / 1000, 0);
	screenMoveVector = TRANS_SPEED * scaleFactor * screenMoveVector;

	float dx = 0;
	float dy = 0;
	float dz = 0;

	if(mSelected == AXIS_X || mSelected == PLANE_XY || mSelected == PLANE_XZ)
		dx = screenMoveVector.Dot(tangentX);

	if(mSelected == AXIS_Y || mSelected == PLANE_XY || mSelected == PLANE_YZ)
		dy = screenMoveVector.Dot(tangentY);

	if(mSelected == AXIS_Z || mSelected == PLANE_XZ || mSelected == PLANE_YZ)
		dz = screenMoveVector.Dot(tangentZ);

	obj->TranslateLocal(dx, dy, dz);
}

void Gizmo::applyRotate(Object* obj, Camera* camera, const Vector3& tangent)
{
	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	DIMOUSESTATE mouseState = gEngine->GetInput()->GetMouseState();
	Vector3 screenMoveVector((float)mouseState.lX / 1000, (float)mouseState.lY / 1000, 0);
	screenMoveVector = ROTATE_SPEED * scaleFactor * screenMoveVector;

	Vector3 lookDir = obj->GetWorldPosition() - camera->GetWorldPosition();

	int signX = sign(lookDir.Dot(obj->GetWorldRight()));
	int signY = sign(lookDir.Dot(obj->GetWorldUp()));
	int signZ = sign(lookDir.Dot(obj->GetWorldForward()));

	float delta = screenMoveVector.Dot(tangent);

	if(mSelected == AXIS_X)
		obj->RotateLocal(signX * delta, 0, 0);

	if(mSelected == AXIS_Y)
		obj->RotateLocal(0, signY * delta, 0);

	if(mSelected == AXIS_Z)
		obj->RotateLocal(0, 0, signZ * delta);
}

void Gizmo::applyScale(Object* obj, Camera* camera, const Vector3& tangentX, const Vector3& tangentY, const Vector3& tangentZ)
{
	float dist = VectorLength(camera->GetWorldPosition() - obj->GetWorldPosition());
	float scaleFactor = dist / SCALE_FACTOR;

	DIMOUSESTATE mouseState = gEngine->GetInput()->GetMouseState();
	Vector3 screenMoveVector((float)mouseState.lX / 1000, -(float)mouseState.lY / 1000, 0);
	screenMoveVector = SCALE_SPEED * scaleFactor * screenMoveVector;

	if(mSelected == AXIS_X)
		obj->Scale(Vector3(1 + screenMoveVector.Dot(tangentX), 1.0f, 1.0f));

	if(mSelected == AXIS_Y)
		obj->Scale(Vector3(1.0f, 1 + screenMoveVector.Dot(tangentY), 1.0f));

	if(mSelected == AXIS_Z)
		obj->Scale(Vector3(1.0f, 1.0f, 1 + screenMoveVector.Dot(tangentZ)));
}

bool Gizmo::IsSelected()
{
	return mSelected !=  SELECT_NONE;
}
