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

#include "DirectionalLightNode.h"
#include "PointLightNode.h"

#include <hash_map>

#include "Terrain.h"

// ------------------------------------------
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
	mCanvas = New SceneEditorCanvas(mCanvasPanel, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);

	D3DDeviceParams deviceParams;
	deviceParams.hWnd = (HWND)mCanvas->GetHWND();
	deviceParams.multisampleType = D3DMULTISAMPLE_4_SAMPLES;

	// init engine
	gEngine = New Engine();
	gEngine->Init(deviceParams);

	// toolbar ��Ϊtoolbar�и��ؼ���gizmo����, ������Ҫengine��ʼ����Ŵ���
	SceneEditorToolBar* toolBar = New SceneEditorToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER);
	SetToolBar(toolBar);

	mEditorPanel->CreateEditorPages();	// �и��ؼ���Ҫע��gizmo��eventhandler, ������engine��ʼ���󴴽�

	// gui
	::SetupGUIStyle();

	// load/create scene
	::CreateScene();

	mEditorPanel->OnCreateScene();

	//// treeGenerator
	//mWndTreeGenerator = new TreeGeneratorFrame(this, L"Tree Generator", wxDefaultPosition, wxDefaultSize);
	//mWndTreeGenerator->Centre();
	//mWndTreeGenerator->Show(false);
 //
	//mWndTreeGenerator->Setup();
}

void SceneEditorFrame::createWxCtrls()
{
	SetIcon(wxIcon(L"./Assets/Icons/Zee.xpm", wxBITMAP_TYPE_XPM));

	wxMenu* menuFile = New wxMenu;
	menuFile->Append(ID_Quit, L"&Quit");

	wxMenu* menuTerrain = New wxMenu;
	menuTerrain->Append(ID_TreeGenerator, L"&Tree Generator");

	wxMenu* menuHelp = New wxMenu;
	menuHelp->Append(ID_About, L"&About\tF1");

	wxMenuBar* menuBar = New wxMenuBar;
	menuBar->Append(menuFile, L"&File");
	menuBar->Append(menuTerrain, L"&Terrain");
	menuBar->Append(menuHelp, L"&Help");

	SetMenuBar(menuBar);
  
	CreateStatusBar();
	SetStatusText(L"Welcome To WanderLand!");

	// panel & canvas
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxHORIZONTAL);

	mEditorPanel = New SceneEditorPanel(this, ID_EDITOR_NOTEBOOK);
	mEditorPanel->SetMinSize(wxSize(200, 620));
	boxSizer1->Add(mEditorPanel, 0, wxALL, 5);

	// canvas
	mCanvasPanel = New wxPanel(this, -1);
	mCanvasPanel->SetMinSize(wxSize(1080, 620));	// TODO: ��ʱд��
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
	if(mEditorPanel)
		mEditorPanel->Close(true);
	
	if(mWndTreeGenerator)
		mWndTreeGenerator->CleanupAndDestory();

	::AppDestroy();
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
	// TODO:����ʹ�ù̶�����, �Ժ����������촰�ڹ���
	wxSize vpSize = GetClientSize();
	gEngine->GetDriver()->SetViewPort(0, 0, vpSize.x, vpSize.y);
	gEngine->GetSceneManager()->GetMainCamera()->SetAspect((float)vpSize.x / (float)vpSize.y);
}

void CreateScene()
{
	Driver* driver = gEngine->GetDriver();

	// camera
	SceneManager* sceneMgr = gEngine->GetSceneManager();

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	sceneMgr->CreateMainCamera(Vector3(0, 4.0f, -4.0f), Vector3::Zero,
		PI/3, (float)viewPort.Width / (float)viewPort.Height, 0.1f, 1000.0f);

	Camera* mainCamera = sceneMgr->GetMainCamera();

	FPCameraController* fpCameraController = New FPCameraController(6.0f, 2.0f, 4.0f);
	//HoverCameraController* hoverCameraController = new HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 2.0f, 100.0f);
	mainCamera->SetCameraController(fpCameraController);

	// lights
	DirectionalLight* dirLight1 = New DirectionalLight(L"dirLight1", D3DXCOLOR_WHITE, Vector3(1.0f, -1.0f, 1.0f));
	PointLight* pointLight1 = New PointLight(L"pointLight1", D3DXCOLOR_YELLOW, Vector3(0, 0, 0), Vector3(1.0f, 0.05f, 0));

	LightManager* lightMgr = gEngine->GetLightManager();
	lightMgr->SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);

	lightMgr->AddDirectionalLight(dirLight1);
	lightMgr->AddPointLight(pointLight1);
	pointLight1->Enable(false);

	DirectionalLightNode* dirLightNode1 = New DirectionalLightNode(NULL, dirLight1);
	sceneMgr->AddSceneNode(dirLightNode1);

	PointLightNode* pointLightNode1 = New PointLightNode(NULL, pointLight1);
	sceneMgr->AddSceneNode(pointLightNode1);

	// geo
	GeometryManager* geometryMgr = gEngine->GetGeometryManager();

	Cube* cubeGeo = New Cube(L"cubeGeo");
	geometryMgr->AddGeometry(cubeGeo);

	cubeGeo->CalculateTBN();
	cubeGeo->BuildGeometry(XYZ_UV_TBN);

	Cylinder* coneGeo = New Cylinder(L"coneGeo", 0, 0.5f, 2.0f);
	geometryMgr->AddGeometry(coneGeo);

	coneGeo->CalculateNormals();
	coneGeo->BuildGeometry(XYZ_N);

	Cylinder* cylinderGeo = New Cylinder(L"cylinderGeo", 0.5f, 0.5f, 2.0f);
	geometryMgr->AddGeometry(cylinderGeo);

	cylinderGeo->CalculateNormals();
	cylinderGeo->BuildGeometry(XYZ_N);

	Torus* torusGeo = New Torus(L"torusGeo", 1.0f, 0.025f, 32, 8);
	geometryMgr->AddGeometry(torusGeo);

	torusGeo->CalculateNormals();
	torusGeo->BuildGeometry(XYZ_N);

	// textures
	TextureManager* textureMgr = gEngine->GetTextureManger();

	std::vector<std::wstring> filesVec;
	YString::GetDirFiles(L"./Assets/Textures", L"jpg", filesVec);

	for(std::vector<std::wstring>::iterator iter = filesVec.begin(); iter != filesVec.end(); ++iter)
	{
		textureMgr->GetOrCreateD3DTexture((*iter).c_str());
	}

	// material
	MaterialManager* materialMgr = gEngine->GetMaterialManager();

	Material* mtlBump = New Material(L"mtl1");
	materialMgr->AddMaterial(mtlBump);

	mtlBump->SetShader(BumpSpecular);
	//mtlBump->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	//mtlBump->mShader->SetNormalTex(L"./Assets/Textures/6133Normal.jpg");
	mtlBump->mShader->SetSpecShiness(0.4f);

	Material* mtlDiff = New Material(L"mtlDiff");
	materialMgr->AddMaterial(mtlDiff);

	mtlDiff->SetShader(Diffuse);
	mtlDiff->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");

	Material* mtlSpec = New Material(L"mtlSpec");
	materialMgr->AddMaterial(mtlSpec);

	mtlSpec->SetShader(Specular);
	mtlSpec->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlSpec->mShader->SetSpecShiness(0.4f);

	Material* mtlView = New Material(L"mtlView");
	materialMgr->AddMaterial(mtlView);

	mtlView->SetShader(View);
	mtlView->SetDiffuseColor(D3DXCOLOR_RED);

	Material* mtlFlat = New Material(L"mtlFlat");
	materialMgr->AddMaterial(mtlFlat);

	mtlFlat->SetShader(Flat);
	mtlFlat->SetDiffuseColor(D3DXCOLOR_GREEN);

	// model
	ModelNode* cube = New ModelNode(L"cube", NULL, cubeGeo, mtlBump);
	sceneMgr->AddSceneNode(cube);
	cube->Translate(2, 0, 0);

	ModelNode* cylinder = New ModelNode(L"cylinder", NULL, cylinderGeo, mtlBump);
	sceneMgr->AddSceneNode(cylinder);
	cylinder->Translate(-2, 0, 0);

	ModelNode* cone = New ModelNode(L"cone", NULL, coneGeo, mtlView);
	sceneMgr->AddSceneNode(cone);
	cone->Translate(0, 0, -2);

	ModelNode* torus = New ModelNode(L"torus", NULL, torusGeo, mtlFlat);
	sceneMgr->AddSceneNode(torus);
	torus->Translate(0, 0, 2);

	// terrain
	PerformanceTimer::Begin(L"building 257 terrain");
	terrain = New Terrain(257, 200.0f, 40.0f);
	terrain->LoadFromHeightMap(L"./Assets/Textures/Terrain/heightMap257_bit16.raw", 257);
	terrain->BuildTerrain(4);
	terrain->CalcChunkLODDist(mainCamera, 1.0f);

	terrain->SetColorTexes(L"./Assets/Textures/Terrain/Cliff.jpg", L"./Assets/Textures/Terrain/Grass_Hill.jpg", 
		L"./Assets/Textures/Terrain/DirtGrass.jpg", L"./Assets/Textures/Terrain/Pebbles.jpg");

	terrain->SetSplatMapTex(L"./Assets/Textures/Terrain/splat.tga");
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

			if(input->GetKeyDown(DIK_R))
			{
				mainCamera->DampMoveTo(Vector3::Zero, 0.5f, 0.01f);
			}

			// render
			driver->RenderToSwapChain(0);
			driver->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
			driver->BeginScene();

			terrain->Draw(mainCamera, true);

			sceneMgr->GetRoot()->SetDrawBBoxFlag(true);
			sceneMgr->DrawAllUseRenderer();

			gGUISystem.Draw();

			gEngine->GetGizmo()->Draw();

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
	leftAlignStyle = New LabelStyle(TEXT("Consolas"), 7, 15, 0, 0xffe3b706, DT_LEFT | DT_VCENTER);
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


	//static bool enableDirLight1 = true;
	//// enableDirLight1 = gGUISystem.GUIToggle(Rect(10, 150, 20, 20), enableDirLight1);

	//DirectionalLight* dirLight1 = NULL;
	//gEngine->GetLightManager()->GetDirLight(L"dirLight1", &dirLight1);
	//dirLight1->Enable(enableDirLight1);
}

void AppDestroy()
{
	SAFE_DELETE(leftAlignStyle);

	SAFE_DELETE(terrain);

	gEngine->Destroy();
	SAFE_DELETE(gEngine);
}

void OnLostDevice()
{
	leftAlignStyle->OnLostDevice();
	terrain->OnLostDevice();

	gDefaultLabelStyle.OnLostDevice();
	gGUISystem.OnLostDevice();

	gEngine->OnLostDevice();
}

void OnResetDevice()
{
	if(!gEngine->GetDriver()->Reset())
		return;

	leftAlignStyle->OnResetDevice();

	terrain->OnResetDevice();

	gDefaultLabelStyle.OnResetDevice();
	gGUISystem.OnResetDevice();

	gEngine->OnResetDevice();
}

BEGIN_EVENT_TABLE(SceneEditorToolBar, wxToolBar)
EVT_TOOL(ID_TOOL_TRANS, SceneEditorToolBar::OnToolTransClicked)
EVT_TOOL(ID_TOOL_ROTATE, SceneEditorToolBar::OnToolRotateClicked)
EVT_TOOL(ID_TOOL_SCALE, SceneEditorToolBar::OnToolScaleClicked)
EVT_RADIOBUTTON(ID_RADIO_BUTTON_GLOBAL, SceneEditorToolBar::OnToolRadioButtonGlobal)
EVT_RADIOBUTTON(ID_RADIO_BUTTON_LOCAL, SceneEditorToolBar::OnToolRadioButtonLocal)
END_EVENT_TABLE()

SceneEditorToolBar::SceneEditorToolBar(wxWindow* parent, wxWindowID id, const wxPoint& pos /*= wxDefaultPosition*/, 
									   const wxSize& size /*= wxDefaultSize*/, long style /*= wxTB_HORIZONTAL*/)
									   :wxToolBar(parent, id, pos, size, style)
{
	createWxCtrls();

	gEngine->GetGizmo()->RegisterEventHanlder(this);		// TODO:����ʱungegister?
}

void SceneEditorToolBar::createWxCtrls()
{
	wxBitmap iconTrans(L"./Assets/Textures/Editor/trans.png", wxBITMAP_TYPE_PNG);
	wxBitmap iconRotate(L"./Assets/Textures/Editor/rotate.png", wxBITMAP_TYPE_PNG);
	wxBitmap iconScale(L"./Assets/Textures/Editor/scale.png", wxBITMAP_TYPE_PNG);

	AddTool(ID_TOOL_TRANS, iconTrans, L"translate");
	AddTool(ID_TOOL_ROTATE, iconRotate, L"rotate");
	AddTool(ID_TOOL_SCALE, iconScale, L"scale");

	mButtonGlobal = New wxRadioButton(this, ID_RADIO_BUTTON_GLOBAL, wxT("Global"), 
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);

	mButtonLocal = New wxRadioButton(this, ID_RADIO_BUTTON_LOCAL, wxT("Local"));

	AddSeparator();
	AddControl(mButtonGlobal);
	AddControl(mButtonLocal);

	Realize();
}

void SceneEditorToolBar::OnCoordTypeChanged(Gizmo* gizmo)
{
	if(gizmo->GetCoordinateType() == Gizmo::COORDINATE_GLOBAL)
	{
		mButtonGlobal->SetValue(true);
	}
	else
	{
		mButtonLocal->SetValue(true);
	}
}

void SceneEditorToolBar::OnToolTransClicked(wxCommandEvent& event)
{
	gEngine->GetGizmo()->SetActiveType(Gizmo::GIZMO_TRANS);
}

void SceneEditorToolBar::OnToolRotateClicked( wxCommandEvent& event )
{
	gEngine->GetGizmo()->SetActiveType(Gizmo::GIZMO_ROTATE);
}

void SceneEditorToolBar::OnToolScaleClicked( wxCommandEvent& event )
{
	gEngine->GetGizmo()->SetActiveType(Gizmo::GIZMO_SCALE);
}

void SceneEditorToolBar::OnToolRadioButtonGlobal(wxCommandEvent& event)
{
	gEngine->GetGizmo()->SetCoordinateType(Gizmo::COORDINATE_GLOBAL);
}

void SceneEditorToolBar::OnToolRadioButtonLocal(wxCommandEvent& event)
{
	gEngine->GetGizmo()->SetCoordinateType(Gizmo::COORDINATE_LOCAL);
}
