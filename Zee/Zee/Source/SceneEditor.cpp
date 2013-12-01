#include "SceneEditor.h"

#include "D3DUtility.h"
#include "Engine.h"
#include "Input.h"
#include "GUI.h"

#include "Camera.h"
#include "CameraController.h"

#include "SceneManager.h"
#include "TextureManager.h"

#include "ModelNode.h"
#include "Primitive.h"
#include "Material.h"

#include "OBJParser.h"
#include "DebugDrawer.h"

#include "Gizmo.h"
#include "BillboardNode.h"
#include "ResourceMgr.h"

#include <hash_map>

#include "Terrain.h"

// ------------------------------------------
Gizmo* gizmo = NULL;
Terrain* terrain = NULL;

LabelStyle* leftAlignStyle;

void AppDestroy();

void OnLostDevice();
void OnResetDevice();

void SetupGUIStyle();
void GUIUpdate();

void CreateScene();
// -------------------------------------------

BEGIN_EVENT_TABLE(SceneEditorFrame, wxFrame)
EVT_MENU(ID_Quit, SceneEditorFrame::OnQuit)
EVT_MENU(ID_TreeGenerator, SceneEditorFrame::OnTreeGenerator)
EVT_MENU(ID_About, SceneEditorFrame::OnAbout)
EVT_CLOSE(SceneEditorFrame::OnClose)
END_EVENT_TABLE()

SceneEditorFrame::SceneEditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
:wxFrame((wxFrame*)NULL, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
,mWndTreeGenerator(NULL)
{
	createWxCtrls();

	// canvas
	wxSize clientSize = mCanvasPanel->GetClientSize(); 
	mCanvas = new SceneEditorCanvas(mCanvasPanel, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);

	D3DDeviceParams deviceParams;
	deviceParams.hWnd = (HWND)mCanvas->GetHWND();
	deviceParams.multisampleType = D3DMULTISAMPLE_4_SAMPLES;

	// init engine
	gEngine = new Engine();
	gEngine->Init(deviceParams);

	// gui
	::SetupGUIStyle();

	// gizmo
	gizmo = new Gizmo;
	gizmo->Init();

	// load/create scene
	::CreateScene();

	mEditorPanel->OnCreateScene();

	//// treeGenerator
	//mWndTreeGenerator = new TreeGeneratorFrame(this, L"Tree Generator", wxDefaultPosition, wxDefaultSize);
	//mWndTreeGenerator->Centre();
	//mWndTreeGenerator->Show(false);

	//mWndTreeGenerator->Setup();
}

void SceneEditorFrame::createWxCtrls()
{
	SetIcon(wxIcon(L"./Assets/Icons/Zee.xpm", wxBITMAP_TYPE_XPM));

	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Quit, L"&Quit");

	wxMenu* menuTerrain = new wxMenu;
	menuTerrain->Append(ID_TreeGenerator, L"&Tree Generator");

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(ID_About, L"&About\tF1");

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");
	menuBar->Append(menuTerrain, L"&Terrain");
	menuBar->Append(menuHelp, L"&Help");

	SetMenuBar(menuBar);
  
	CreateStatusBar();
	SetStatusText(L"Welcome To WanderLand!");

	// panel & canvas
	wxBoxSizer* boxSizer1 = new wxBoxSizer(wxHORIZONTAL);

	mEditorPanel = new SceneEditorPanel(this, -1);
	mEditorPanel->SetMinSize(wxSize(200, 680));
	boxSizer1->Add(mEditorPanel, 0, wxALL, 5);

	// canvas
	mCanvasPanel = new wxPanel(this, -1);
	mCanvasPanel->SetMinSize(wxSize(1080, 680));	// TODO: 暂时写死
	boxSizer1->Add(mCanvasPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void SceneEditorFrame::OnQuit(wxCommandEvent& event)
{
	Close(true);
}

void SceneEditorFrame::OnClose(wxCloseEvent& event)
{
	cleanupAndDestory();
}

void SceneEditorFrame::cleanupAndDestory()
{
	::AppDestroy();

	if(mEditorPanel)
		mEditorPanel->CleanupAndDestory();

	if(mCanvas)
		mCanvas->Destroy();

	if(mWndTreeGenerator)
		mWndTreeGenerator->CleanupAndDestory();

	Destroy();
}

void SceneEditorFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(L"Siny - SinYocto@gmail.com", L"Author", wxOK | wxICON_INFORMATION, this);
}

void SceneEditorFrame::OnTreeGenerator(wxCommandEvent& event)
{
	if(mWndTreeGenerator)
	{
		mWndTreeGenerator->Show(true);
		mWndTreeGenerator->Raise();
	}
}

BEGIN_EVENT_TABLE(SceneEditorCanvas, wxWindow)
EVT_SIZE(SceneEditorCanvas::OnSize)
EVT_IDLE(SceneEditorCanvas::OnIdle)
END_EVENT_TABLE()

SceneEditorCanvas::SceneEditorCanvas(wxWindow* parent, wxWindowID id /* = wxID_ANY */, 
									 const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, 
									 long style /* = 0 */, const wxString& name /* = wxT */)
									 :wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{

}

void SceneEditorCanvas::OnIdle(wxIdleEvent& event)
{
	if(gEngine && gEngine->GetDriver() && gEngine->GetDriver()->GetD3DDevice())
		RenderLoop();

	event.RequestMore(true);
}

void SceneEditorCanvas::OnSize(wxSizeEvent& event)
{
	// TODO:暂且使用固定窗口, 以后再增加拉伸窗口功能
	wxSize vpSize = GetClientSize();
	gEngine->GetDriver()->SetViewPort(0, 0, vpSize.x, vpSize.y);
	gEngine->GetSceneManager()->GetMainCamera()->SetAspect((float)vpSize.x / (float)vpSize.y);
}

void CreateScene()
{
	Driver* driver = gEngine->GetDriver();

	// camera
	SceneManager* sceneMgr = gEngine->GetSceneManager();

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetPrimaryViewPort();
	sceneMgr->CreateMainCamera(Vector3(0, 4.0f, -4.0f), Vector3::Zero,
		PI/3, (float)viewPort.Width / (float)viewPort.Height, 0.1f, 1000.0f);

	Camera* mainCamera = sceneMgr->GetMainCamera();

	FPCameraController* fpCameraController = new FPCameraController(6.0f, 2.0f, 4.0f);
	//HoverCameraController* hoverCameraController = new HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 2.0f, 100.0f);
	mainCamera->SetCameraController(fpCameraController);

	// lights
	DirectionalLight* dirLight1 = new DirectionalLight(L"dirLight1", D3DXCOLOR_WHITE, Vector3(1.0f, -1.0f, 1.0f));
	PointLight* pointLight1 = new PointLight(L"pointLight1", D3DXCOLOR_YELLOW, Vector3(0, 0, 0), Vector3(1.0f, 0.05f, 0));

	LightManager* lightMgr = gEngine->GetLightManager();
	lightMgr->SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);


	lightMgr->AddDirectionalLight(dirLight1);
	lightMgr->AddPointLight(pointLight1);
	pointLight1->Enable(false);

	// geo
	GeometryManager* geometryMgr = gEngine->GetGeometryManager();

	Cube* cubeGeo = new Cube(L"cubeGeo");
	geometryMgr->AddGeometry(cubeGeo);

	cubeGeo->CalculateTBN();
	cubeGeo->BuildGeometry(XYZ_UV_TBN);

	Cylinder* coneGeo = new Cylinder(L"coneGeo", 0, 0.5f, 2.0f);
	geometryMgr->AddGeometry(coneGeo);

	coneGeo->CalculateNormals();
	coneGeo->BuildGeometry(XYZ_N);

	Cylinder* cylinderGeo = new Cylinder(L"cylinderGeo", 0.5f, 0.5f, 2.0f);
	geometryMgr->AddGeometry(cylinderGeo);

	cylinderGeo->CalculateNormals();
	cylinderGeo->BuildGeometry(XYZ_N);

	Torus* torusGeo = new Torus(L"torusGeo", 1.0f, 0.025f, 32, 8);
	geometryMgr->AddGeometry(torusGeo);

	torusGeo->CalculateNormals();
	torusGeo->BuildGeometry(XYZ_N);

	// material
	MaterialManager* materialMgr = gEngine->GetMaterialManager();

	Material* mtlBump = new Material(L"mtl1");
	materialMgr->AddMaterial(mtlBump);

	mtlBump->SetShader(BumpSpecular);
	mtlBump->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlBump->mShader->SetNormalTex(L"./Assets/Textures/6133Normal.jpg");
	mtlBump->mShader->SetSpecShiness(0.4f);

	Material* mtlDiff = new Material(L"mtlDiff");
	materialMgr->AddMaterial(mtlDiff);

	mtlDiff->SetShader(Diffuse);
	//mtl2->shader->SetColorTex(L"./Assets/Textures/6133.jpg");

	Material* mtlSpec = new Material(L"mtlSpec");
	materialMgr->AddMaterial(mtlSpec);

	mtlSpec->SetShader(Specular);
	mtlSpec->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlSpec->mShader->SetSpecShiness(0.4f);

	Material* mtlView = new Material(L"mtlView");
	materialMgr->AddMaterial(mtlView);

	mtlView->SetShader(View);
	mtlView->SetDiffuseColor(D3DXCOLOR_RED);

	Material* mtlFlat = new Material(L"mtlFlat");
	materialMgr->AddMaterial(mtlFlat);

	mtlFlat->SetShader(Flat);
	mtlFlat->SetDiffuseColor(D3DXCOLOR_GREEN);

	// model
	ModelNode* cube = new ModelNode(L"cube", NULL, cubeGeo, mtlBump);
	sceneMgr->AddSceneNode(cube);
	cube->Translate(2, 0, 0);

	ModelNode* cylinder = new ModelNode(L"cylinder", NULL, cylinderGeo, mtlDiff);
	sceneMgr->AddSceneNode(cylinder);
	cylinder->Translate(-2, 0, 0);

	ModelNode* cone = new ModelNode(L"cone", NULL, coneGeo, mtlView);
	sceneMgr->AddSceneNode(cone);
	cone->Translate(0, 0, -2);

	ModelNode* torus = new ModelNode(L"torus", NULL, torusGeo, mtlFlat);
	sceneMgr->AddSceneNode(torus);
	torus->Translate(0, 0, 2);

	// billboard
	BillboardNode* billboard = new BillboardNode(L"billboard", 1.0f, 1.0f, D3DXCOLOR_YELLOW);
	sceneMgr->AddSceneNode(billboard);
	billboard->GetBillboard()->SetTexture(L"./Assets/Textures/light.jpg");

	// terrain
	PerformanceTimer::Begin(L"building 257 terrain");
	terrain = new Terrain(257, 200.0f, 40.0f);
	terrain->LoadFromHeightMap(L"./Assets/Textures/heightMap257_bit16.raw", 257);
	terrain->BuildTerrain(4);
	terrain->CalcChunkLODDist(mainCamera, 1.0f);

	terrain->SetColorTexes(L"./Assets/Textures/Cliff.jpg", L"./Assets/Textures/Grass_Hill.jpg", 
		L"./Assets/Textures/DirtGrass.jpg", L"./Assets/Textures/Pebbles.jpg");

	terrain->SetSplatMapTex(L"./Assets/Textures/splat.tga");
	terrain->SetMtlParameters(30.0f, 30.0f, D3DXCOLOR_WHITE, D3DXCOLOR_WHITE);
	PerformanceTimer::End();
}

void SceneEditorCanvas::RenderLoop()
{
	Driver* driver = gEngine->GetDriver();
	Input* input = gEngine->GetInput();

	SceneManager* sceneMgr = gEngine->GetSceneManager();
	Camera* mainCamera = sceneMgr->GetMainCamera();

	switch(driver->GetD3DDevice()->TestCooperativeLevel())
	{
	case D3D_OK:
		{
			// update state
			gEngine->FrameUpdate();

			GUIUpdate();

			if(wxWindow::FindFocus() == this)
				mainCamera->ApplyCameraController();

			terrain->FrameUpdate(mainCamera);

			gizmo->FrameUpdate(mainCamera);

			// render
			driver->RenderToSwapChain(PRIMARY_SWAPCHAIN);
			driver->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
			driver->BeginScene();

			//Vector2 screenPos(input->GetCursorPos().x, input->GetCursorPos().y);

			//Vector3 rayPos;
			//Vector3 rayDir;
			//mainCamera->GetScreenRay(screenPos, &rayPos, &rayDir);

			//static SceneNode* hitNode = NULL;
			//if(input->GetLeftButtonDown())
			//{
			//	if(!gizmo->IsSelected())
			//		hitNode = sceneMgr->RayIntersect(rayPos, rayDir, NULL, NULL);
			//}

			terrain->Draw(mainCamera, true);

			sceneMgr->GetRoot()->SetDrawBBoxFlag(true);
			//sceneMgr->DrawAll();
			sceneMgr->DrawAllUseRenderer();

			gGUISystem.Draw();

			gizmo->SetActiveType(Gizmo::GIZMO_TRANS);
			gizmo->Draw(gizmo->GetSelectedNode(), mainCamera);

			//if(hitNode && hitNode->GetNodeType() == SceneNode::SCENE_NODE_BILLBOARD)
			//{
			//	PointLight* pointLight1 = NULL;
			//	gEngine->GetLightManager()->GetPointLight(L"pointLight1", &pointLight1);
			//	pointLight1->SetPosition(hitNode->GetWorldPosition());
			//}

			driver->EndScene();
			driver->Present();

			break;
		}
	case D3DERR_DEVICELOST:
		break;
	case D3DERR_DEVICENOTRESET:
		{
			::OnLostDevice();
			::OnResetDevice();
			break;
		}
	default:
		PostQuitMessage(0);
		break;
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

	POINT cursorPos = gEngine->GetInput()->GetCursorPos();
	Vector2 screenPos(cursorPos.x, cursorPos.y);
	Vector2 screenLocation;
	gEngine->GetDriver()->GetScreenLocation(screenPos, &screenLocation);

	wchar_t text[MAX_STR_LEN];
	YString::Format(text, TEXT("fps:%d"), gEngine->GetFrameTimer()->GetFPS());
	gGUISystem.GUILabel(text, Rect(10, 10, 300, 25), leftAlignStyle);

	YString::Format(text, L"screenPos:%d, %d(%f, %f)", cursorPos.x, cursorPos.y, 
		screenLocation.x, screenLocation.y);
	gGUISystem.GUILabel(text, Rect(10, 40, 300, 25), leftAlignStyle);


	static bool enableDirLight1 = true;
	// enableDirLight1 = gGUISystem.GUIToggle(Rect(10, 150, 20, 20), enableDirLight1);

	DirectionalLight* dirLight1 = NULL;
	gEngine->GetLightManager()->GetDirLight(L"dirLight1", &dirLight1);
	dirLight1->Enable(enableDirLight1);
}

void AppDestroy()
{
	SAFE_DELETE(leftAlignStyle);
	gizmo->Destroy();
	SAFE_DELETE(gizmo);

	SAFE_DELETE(terrain);

	gEngine->Destroy();
	SAFE_DELETE(gEngine);
}

void OnLostDevice()
{
	leftAlignStyle->OnLostDevice();
	gizmo->OnLostDevice();
	terrain->OnLostDevice();

	gDefaultLabelStyle.OnLostDevice();
	gGUISystem.OnLostDevice();

	ResourceMgr::OnLostDevice();

	gEngine->OnLostDevice();
}

void OnResetDevice()
{
	if(!gEngine->GetDriver()->Reset())
		return;

	leftAlignStyle->OnResetDevice();
	gizmo->OnResetDevice();

	terrain->OnResetDevice();

	gDefaultLabelStyle.OnResetDevice();
	gGUISystem.OnResetDevice();

	ResourceMgr::OnResetDevice();

	gEngine->OnResetDevice();
}