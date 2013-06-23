#include "D3DUtility.h"
#include "Camera.h"
#include "Input.h"
#include "Time.h"
#include "GUI.h"
#include "Material.h"
#include "Shader.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "LightManager.h"
#include "RenderableObject.h"
#include "Primitive.h"
#include "Material.h"
#include "SubModel.h"
#include "Model.h"

const int wndWidth = 1024;
const int wndHeight = 600;

LabelStyle* leftAlignStyle;

// camera
Camera* camera = NULL;

// lights
DirectionalLight dirLight1;
DirectionalLight dirLight2;

PointLight pointLight1;
PointLight pointLight2;

// mesh
Cube* cubeMesh = NULL;

// material
Material* mtlBump = NULL;

// model
SubModel* parentNode;
SubModel* cubeRO;

Model* cubeModel;

void SetupGUIStyle();
void SetupShaders();

void GUIUpdate();
void ApplyFPCameraControllor(Camera* pCamera, float deltaTime);

void AppDestroy();

void OnLostDevice();
void OnResetDevice();

int GetFPS();

int main()
{
	int retCode = 1;
	{
        // create window
		gHWnd = RegisterAndCreateWindow(TEXT("Demos"), TEXT("Demos_Frame"), wndWidth, wndHeight, WndProc);
		Assert(NULL != gHWnd);

        // create d3ddevice
		Assert(CreateD3DDevice(gHWnd, wndWidth, wndHeight, D3DMULTISAMPLE_4_SAMPLES, &gD3D, &gD3DDevice, &gPresentParameters));

        // init
		Input::Init(GetModuleHandle(0), gHWnd);
        SetupGUIStyle();
		SetupShaders();

        // camera
		camera = new Camera(Vector3(0, 0.0f, -10.0f), Vector3::Zero, 
			PI/3, (float)wndWidth/(float)wndHeight, 0.1f, 1000.0f);

		// lights
		LightManager::SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);
		dirLight1.SetValue(D3DXCOLOR_RED, Vector3(1.0f, -0.5f, 1.0f), 1.0f);
		pointLight1.SetValue(D3DXCOLOR_YELLOW, Vector3(-4.0f, 0, 0), Vector3(1.0f, 0, 0), 1.0f);

		LightManager::AddDirectionalLight(&dirLight1);
		LightManager::AddPointLight(&pointLight1);

		// mesh
		cubeMesh = new Cube();
		cubeMesh->CalculateTBN();
		cubeMesh->BuildGeometry(XYZ_UV_TBN);

		// material
		mtlBump = new Material();
		mtlBump->SetShader(BumpSpecular);

		mtlBump->shader->SetColorTex(TEXT("./Assets/Textures/6133.jpg"));
		mtlBump->shader->SetNormalTex(TEXT("./Assets/Textures/6133Normal.jpg"));
		mtlBump->shader->SetSpecShiness(0.4f);

		// model
		parentNode = new SubModel(NULL, cubeMesh, mtlBump);
		parentNode->TranslateLocal(2.0f, 0, 0);

		/*cubeRO = new SubModel(parentNode, cubeMesh, mtlBump);
		cubeRO->RotateLocal(-PI/4, 0, 0);*/
		cubeRO = new SubModel(parentNode, cubeMesh, mtlBump);

		cubeModel = new Model(NULL, cubeMesh, mtlBump);

        // start loop
		Time::Start();
		while(run())
		{
			switch(gD3DDevice->TestCooperativeLevel())
			{
			case D3D_OK:
				{
					// update state
					Time::Tick();

					Input::GetDeviceState(gHWnd);

					GUIUpdate();

					LightManager::Update();

					ApplyFPCameraControllor(camera, Time::deltaTime);

					camera->Update();

					// render
					gD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff1e90ff, 1.0f, 0);
					gD3DDevice->BeginScene();

					//cubeRO->Draw(camera);
					cubeModel->Draw(camera);

					gGUISystem.Draw();

					gD3DDevice->EndScene();
					gD3DDevice->Present(0, 0, 0, 0);

					break;
				}
			case D3DERR_DEVICELOST:
				break;
			case D3DERR_DEVICENOTRESET:
				{
					OnLostDevice();
					OnResetDevice();
					break;
				}
			default:
				PostQuitMessage(0);
				break;
			}
		}
	}

	retCode = 0;
Exit:
	Input::Destroy();
	AppDestroy();
	SAFE_RELEASE(gD3DDevice);
	SAFE_RELEASE(gD3D);
	return retCode;
}

void ApplyFPCameraControllor(Camera* pCamera, float deltaTime)
{
	Vector3 moveVector;
	float moveSpeed = 2.0f;
	float rotateSpeed = 2.0f;

	if(Input::GetKey(DIK_LSHIFT))
		moveSpeed *= 4;

	if(Input::GetKey(DIK_W))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, 0, 1);
	if(Input::GetKey(DIK_S))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, 0, -1);

	if(Input::GetKey(DIK_A))
	{
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(-1, 0, 0);
	}
	if(Input::GetKey(DIK_D))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(1, 0, 0);

	moveVector = pCamera->LocalVectorToWorld(moveVector);

	if(Input::GetKey(DIK_Q))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, -1, 0);
	if(Input::GetKey(DIK_E))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, 1, 0);


	if(moveVector != Vector3::Zero){
		pCamera->SetTransformDirty(true);
		pCamera->Translate(moveVector);
	}

	if(Input::mouseState.rgbButtons[1] & 0x80){
		float rotationY = rotateSpeed * Input::mouseState.lX / 1000.0f;
		float rotationX = rotateSpeed * Input::mouseState.lY / 1000.0f;

		if(rotationY != 0 || rotationX != 0){
			pCamera->SetTransformDirty(true);
			pCamera->Rotate(0, rotationY, 0);
			pCamera->RotateLocal(rotationX, 0, 0);
		}
	}

}

void SetupGUIStyle()
{
    gDefaultLabelStyle.CreateFont();
    leftAlignStyle = new LabelStyle(TEXT("Consolas"), 7, 15, 0, 0xffe3b706, DT_LEFT | DT_VCENTER);
    leftAlignStyle->CreateFont();

    gDefaultButtonStyle.CreateTextures();
    gDefaultSliderStyle.CreateTextures();
    gDefaultToggleStyle.CreateTextures();
    gDefaultListBoxStyle.CreateTextures();
}

void SetupShaders()
{
	DiffuseShader::CreateEffectFromFile(TEXT("./Source/Shaders/Diffuse.fx"));
	_Assert(NULL != DiffuseShader::effect);

	SpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/Specular.fx"));
	_Assert(NULL != SpecularShader::effect);

	BumpSpecularShader::CreateEffectFromFile(TEXT("./Source/Shaders/BumpSpecular.fx"));
	_Assert(NULL != BumpSpecularShader::effect);
}

void GUIUpdate()
{
    gGUISystem.clear();

    wchar_t text[MAX_STR_LEN];
    wsprintf(text, TEXT("fps:%d"), GetFPS());
    gGUISystem.GUILabel(text, Rect(10, 10, 100, 25), leftAlignStyle);

	static bool enableDirLight1 = false;
	enableDirLight1 = gGUISystem.GUIToggle(Rect(10, 50, 20, 20), enableDirLight1);

	dirLight1.Enable(enableDirLight1);
}

int GetFPS()
{
    static int fps = 0;
    static int fpsAccumulator = 0;
    static float elapseTime = 0;

    if(elapseTime > 1.0f)
    {
        fps = fpsAccumulator;

        fpsAccumulator = 0;
        elapseTime = 0;
    }

    elapseTime += Time::deltaTime;
    fpsAccumulator++;

    return fps;
}

void AppDestroy()
{
	SAFE_DELETE(leftAlignStyle);
	SAFE_DELETE(camera);

	SAFE_DROP(cubeMesh);
	SAFE_DROP(mtlBump);

	SAFE_DELETE(cubeRO);
	SAFE_DELETE(parentNode);

	SAFE_DELETE(cubeModel);
}

void OnLostDevice()
{
	gDefaultLabelStyle.OnLostDevice();
	leftAlignStyle->OnLostDevice();

	gGUISystem.OnLostDevice();

	DiffuseShader::OnLostDevice();
	SpecularShader::OnLostDevice();
	BumpSpecularShader::OnLostDevice();

	cubeMesh->OnLostDevice();
}

void OnResetDevice()
{
	if(FAILED(gD3DDevice->Reset(&gPresentParameters)))
		return;

	gDefaultLabelStyle.OnResetDevice();
	leftAlignStyle->OnResetDevice();

	gGUISystem.OnResetDevice();

	DiffuseShader::OnResetDevice();
	SpecularShader::OnResetDevice();
	BumpSpecularShader::OnResetDevice();

	cubeMesh->OnResetDevice();
}