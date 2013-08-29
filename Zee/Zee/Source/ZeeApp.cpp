#include "ZeeApp.h"
#include "D3DUtility.h"
#include "Input.h"
#include "GUI.h"

#include "Camera.h"

#include "LightManager.h"
#include "GeometryManager.h"
#include "MaterialManager.h"
#include "SceneManager.h"

#include "Model.h"
#include "Primitive.h"
#include "Material.h"

#include "OBJParser.h"
#include "DebugDrawer.h"

#include "Gizmo.h"

#include <Locale.h>

enum 
{
	ID_Quit = 1,
	ID_About,
};

IMPLEMENT_APP_CONSOLE(ZeeApp)

// ------------------------------------------
const int WND_WIDTH = 1280;
const int WND_HEIGHT = 720;

Model* cube = NULL;
Gizmo* gizmo = NULL;

LabelStyle* leftAlignStyle;

void ApplyFPCameraControllor(Camera* pCamera, float deltaTime);

void AppDestroy();

void OnLostDevice();
void OnResetDevice();

void SetupGUIStyle();
void GUIUpdate();

int GetFPS();

void SetUp();
void RenderLoop();
// -------------------------------------------

bool ZeeApp::OnInit()
{

	ZeeFrame* frame = new ZeeFrame(L"Zee", wxPoint(0, 0), wxSize(WND_WIDTH, WND_HEIGHT));
	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

BEGIN_EVENT_TABLE(ZeeFrame, wxFrame)
	EVT_MENU(ID_Quit, ZeeFrame::OnQuit)
	EVT_MENU(ID_About, ZeeFrame::OnAbout)
END_EVENT_TABLE()

ZeeFrame::ZeeFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
:wxFrame((wxFrame*)NULL, -1, title, pos, size, 
		 wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
{
	wxMenu* menuFile = new wxMenu;

	menuFile->Append(ID_About, L"&About");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"&Quit");

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText(L"Welcome To WanderLand!");

	wxSize clientSize = GetClientSize();

	mCanvas = new D3D9Canvas(this, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);
	mCanvas->InitDriver();

	SetUp();
}

void ZeeFrame::OnQuit(wxCommandEvent& event)
{
	AppDestroy();
	Close(true);
}

void ZeeFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(L"Siny - SinYocto@gmail.com", L"Author", wxOK | wxICON_INFORMATION, this);
}

BEGIN_EVENT_TABLE(D3D9Canvas, wxWindow)
	EVT_SIZE(D3D9Canvas::OnSize)
	EVT_IDLE(D3D9Canvas::OnIdle)
END_EVENT_TABLE()

D3D9Canvas::D3D9Canvas(wxWindow* parent, wxWindowID id /* = wxID_ANY */, 
					   const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, 
					   long style /* = 0 */, const wxString& name /* = wxT */)
					   :wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{
	
}

void D3D9Canvas::OnIdle(wxIdleEvent& event)
{
	if(Driver::D3DDevice)
		RenderLoop();

	event.RequestMore(true);
}

void D3D9Canvas::OnSize(wxSizeEvent& event)
{
	// TODO:暂且使用固定窗口, 以后再增加拉伸窗口功能
	wxSize vpSize = GetClientSize();
	Driver::SetViewPort(0, 0, vpSize.x, vpSize.y);
	SceneManager::mainCamera->SetAspect((float)vpSize.x / (float)vpSize.y);
}

void D3D9Canvas::InitDriver()
{
	Driver::CreateD3DDevice((HWND)GetHWND(), D3DMULTISAMPLE_4_SAMPLES);
}

void SetUp()
{
	_wsetlocale(LC_ALL, L"chs");

	Input::Init(GetModuleHandle(0), Driver::hWnd);
	MaterialManager::Init();
	SceneManager::Init();
	Time::Start();

	SetupGUIStyle();

	// camera
	SceneManager::CreateMainCamera(Vector3(0, 4.0f, -4.0f), Vector3::Zero,
		PI/3, (float)Driver::viewPort.Width / (float)Driver::viewPort.Height, 0.1f, 1000.0f);

	// lights
	DirectionalLight* dirLight1 = new DirectionalLight(L"dirLight1", D3DXCOLOR_RED, Vector3(1.0f, -1.0f, 1.0f));
	PointLight* pointLight1 = new PointLight(L"pointLight1", D3DXCOLOR_YELLOW, Vector3(0, 0, 0), Vector3(1.0f, 0, 0));

	LightManager::SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);

	LightManager::AddDirectionalLight(dirLight1);
	LightManager::AddPointLight(pointLight1);

	// geo
	Cube* cubeGeo = new Cube(L"cubeGeo");
	GeometryManager::AddGeometry(cubeGeo);

	cubeGeo->CalculateTBN();
	cubeGeo->BuildGeometry(XYZ_UV_TBN);

	Cylinder* coneGeo = new Cylinder(L"coneGeo", 0, 0.5f, 2.0f);
	GeometryManager::AddGeometry(coneGeo);

	coneGeo->CalculateNormals();
	coneGeo->BuildGeometry(XYZ_N);

	Cylinder* cylinderGeo = new Cylinder(L"cylinderGeo", 0.5f, 0.5f, 2.0f);
	GeometryManager::AddGeometry(cylinderGeo);

	cylinderGeo->CalculateNormals();
	cylinderGeo->BuildGeometry(XYZ_N);

	Torus* torusGeo = new Torus(L"torusGeo", 1.0f, 0.025f, 32, 8);
	GeometryManager::AddGeometry(torusGeo);

	torusGeo->CalculateNormals();
	torusGeo->BuildGeometry(XYZ_N);

	// material
	Material* mtlBump = new Material(L"mtl1");
	MaterialManager::AddMaterial(mtlBump);

	mtlBump->SetShader(BumpSpecular);
	mtlBump->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlBump->mShader->SetNormalTex(L"./Assets/Textures/6133Normal.jpg");
	mtlBump->mShader->SetSpecShiness(0.4f);

	Material* mtlDiff = new Material(L"mtlDiff");
	MaterialManager::AddMaterial(mtlDiff);

	mtlDiff->SetShader(Diffuse);
	//mtl2->shader->SetColorTex(L"./Assets/Textures/6133.jpg");

	Material* mtlSpec = new Material(L"mtlSpec");
	MaterialManager::AddMaterial(mtlSpec);

	mtlSpec->SetShader(Specular);
	mtlSpec->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlSpec->mShader->SetSpecShiness(0.4f);

	Material* mtlView = new Material(L"mtlView");
	MaterialManager::AddMaterial(mtlView);

	mtlView->SetShader(View);
	mtlView->SetDiffuseColor(D3DXCOLOR_RED);

	Material* mtlFlat = new Material(L"mtlFlat");
	MaterialManager::AddMaterial(mtlFlat);

	mtlFlat->SetShader(Flat);
	mtlFlat->SetDiffuseColor(D3DXCOLOR_GREEN);

	// model
	cube = new Model(L"cube", SceneManager::root, cubeGeo, mtlBump);
	cube->Translate(2, 0, 0);

	Model* cylinder = new Model(L"cylinder", SceneManager::root, cylinderGeo, mtlDiff);
	cylinder->Translate(-2, 0, 0);

	Model* cone = new Model(L"cone", SceneManager::root, coneGeo, mtlView);
	cone->Translate(0, 0, -2);

	Model* torus = new Model(L"torus", SceneManager::root, torusGeo, mtlFlat);
	torus->Translate(0, 0, 2);

	gizmo = new Gizmo;
	gizmo->Init();

}

void RenderLoop()
{
	switch(Driver::D3DDevice->TestCooperativeLevel())
	{
	case D3D_OK:
		{
			// update state
			Time::Tick();

			Input::GetDeviceState(Driver::hWnd);

			GUIUpdate();

			LightManager::Update();

			ApplyFPCameraControllor(SceneManager::mainCamera, Time::deltaTime);

			SceneManager::FrameUpdate();
			MaterialManager::FrameUpdate();

			// render
			Driver::Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
			Driver::BeginScene();

			Vector2 screenPos(Input::cursorPos.x, Input::cursorPos.y);

			Vector3 rayPos;
			Vector3 rayDir;
			SceneManager::mainCamera->GetScreenRay(screenPos, &rayPos, &rayDir);

			static SceneNode* hitNode = NULL;
			if(Input::GetLeftButtonDown())
			{
				hitNode = SceneManager::RayIntersect(rayPos, rayDir, NULL, NULL);
			}

			SceneManager::root->SetDrawBBoxFlag(true);
			SceneManager::DrawAll();

			//gGUISystem.Draw();

			gizmo->SetActiveType(Gizmo::GIZMO_TRANS);
			gizmo->Draw(hitNode, SceneManager::mainCamera);

			Driver::EndScene();
			Driver::Present();

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


void ApplyFPCameraControllor(Camera* pCamera, float deltaTime)
{
	Vector3 moveVector = Vector3::Zero;
	float moveSpeed = 2.0f;
	float rotateSpeed = 2.0f;

	if(Input::GetKey(DIK_LSHIFT))
		moveSpeed *= 4;

	if(Input::GetKey(DIK_W))
		moveVector += moveSpeed * deltaTime * Vector3(0, 0, 1);
	if(Input::GetKey(DIK_S))
		moveVector += moveSpeed * deltaTime * Vector3(0, 0, -1);

	if(Input::GetKey(DIK_A))
	{
		moveVector += moveSpeed * deltaTime * Vector3(-1, 0, 0);
	}
	if(Input::GetKey(DIK_D))
		moveVector += moveSpeed * deltaTime * Vector3(1, 0, 0);

	moveVector = pCamera->LocalVectorToWorld(moveVector);

	if(Input::GetKey(DIK_Q))
		moveVector += moveSpeed * deltaTime * Vector3(0, -1, 0);
	if(Input::GetKey(DIK_E))
		moveVector += moveSpeed * deltaTime * Vector3(0, 1, 0);


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

void GUIUpdate()
{
	gGUISystem.clear();

	Vector2 screenPos(Input::cursorPos.x, Input::cursorPos.y);
	Vector2 screenLocation;
	Driver::GetScreenLocation(screenPos, &screenLocation);

	wchar_t text[MAX_STR_LEN];
	YString::Format(text, TEXT("fps:%d"), GetFPS());
	gGUISystem.GUILabel(text, Rect(10, 10, 300, 25), leftAlignStyle);

	YString::Format(text, L"screenPos:%d, %d(%f, %f)", Input::cursorPos.x, Input::cursorPos.y, 
		screenLocation.x, screenLocation.y);
	gGUISystem.GUILabel(text, Rect(10, 40, 300, 25), leftAlignStyle);


	static bool enableDirLight1 = false;
	// enableDirLight1 = gGUISystem.GUIToggle(Rect(10, 150, 20, 20), enableDirLight1);

	DirectionalLight* dirLight1 = NULL;
	LightManager::GetDirLight(L"dirLight1", &dirLight1);
	dirLight1->Enable(enableDirLight1);
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
	gizmo->Destroy();
	SAFE_DELETE(gizmo);

	Input::Destroy();
	SceneManager::Destory();
	GeometryManager::DeleteAll();
	MaterialManager::DeleteAll();
	LightManager::Destroy();
	//TransGizmo::Destroy();
	Driver::Destory();
}

void OnLostDevice()
{
	gDefaultLabelStyle.OnLostDevice();
	leftAlignStyle->OnLostDevice();

	gGUISystem.OnLostDevice();

	MaterialManager::OnLostDevice();
	GeometryManager::OnLostDevice();
}

void OnResetDevice()
{
	if(!Driver::Reset())
		return;

	gDefaultLabelStyle.OnResetDevice();
	leftAlignStyle->OnResetDevice();

	gGUISystem.OnResetDevice();

	MaterialManager::OnResetDevice();
	GeometryManager::OnResetDevice();
}


