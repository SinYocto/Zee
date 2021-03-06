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

#include "HashMap.h"

#include "Terrain.h"
#include "SkyBox.h"

// ------------------------------------------
LabelStyle* leftAlignStyle;

void AppDestroy();

void OnLostDevice();
void OnResetDevice();

void SetupGUIStyle();
void GUIUpdate();
void GUIUpdateAfterEngineUpdate();

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
	//deviceParams.multisampleType = D3DMULTISAMPLE_4_SAMPLES;
	deviceParams.multisampleType = D3DMULTISAMPLE_NONE;

	// init engine
	gEngine = New Engine();
	gEngine->Init(deviceParams);

	// toolbar 因为toolbar有个控件和gizmo关联, 所以需要engine初始化后才创建
	SceneEditorToolBar* toolBar = New SceneEditorToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER);
	SetToolBar(toolBar);

	mEditorPanel->CreateEditorPages();	// 有个控件需要注册gizmo的eventhandler, 所以在engine初始化后创建

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
	mCanvasPanel->SetMinSize(wxSize(1080, 620));	// TODO: 暂时写死
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

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(0);
	sceneMgr->CreateMainCamera(Vector3(0, 4.0f, -14.0f), Vector3::Zero,
		PI/3, (float)viewPort.Width / (float)viewPort.Height, 0.1f, 500.0f);

	Camera* mainCamera = sceneMgr->GetMainCamera();

	FPCameraController* fpCameraController = New FPCameraController(6.0f, 2.0f, 4.0f);
	//HoverCameraController* hoverCameraController = new HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 2.0f, 100.0f);
	mainCamera->SetCameraController(fpCameraController);

	// lights
	//DirectionalLight* dirLight1 = New DirectionalLight(L"dirLight1", D3DXCOLOR_WHITE, Vector3(1.0f, -1.0f, 1.0f));
	//PointLight* pointLight1 = New PointLight(L"pointLight1", D3DXCOLOR_YELLOW, Vector3(0, 0, 0), Vector3(1.0f, 0.05f, 0));

	LightManager* lightMgr = gEngine->GetLightManager();
	lightMgr->SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);

	//lightMgr->AddDirectionalLight(dirLight1);
	//lightMgr->AddPointLight(pointLight1);
	//pointLight1->Enable(false);

	//DirectionalLightNode* dirLightNode1 = New DirectionalLightNode(NULL, dirLight1);
	//sceneMgr->AddSceneNode(dirLightNode1);

	//PointLightNode* pointLightNode1 = New PointLightNode(NULL, pointLight1);
	//sceneMgr->AddSceneNode(pointLightNode1);

	// geo
	//GeometryManager* geometryMgr = gEngine->GetGeometryManager();

	//Geometry* cubeGeo = New Geometry(L"cubeGeo", L"Assets/Geometries/defaultCube.geo");
	//Geometry* coneGeo = New Geometry(L"coneGeo", L"Assets/Geometries/defaultCone.geo");
	//Geometry* cylinderGeo = New Geometry(L"cylinderGeo", L"Assets/Geometries/defaultCylinder.geo");
	//Geometry* torusGeo = New Geometry(L"torusGeo", L"Assets/Geometries/defaultTorus.geo");
 //
	//geometryMgr->AddGeometry(cubeGeo);
	//geometryMgr->AddGeometry(coneGeo);
	//geometryMgr->AddGeometry(cylinderGeo);
	//geometryMgr->AddGeometry(torusGeo);

	// textures
	TextureManager* textureMgr = gEngine->GetTextureManger();

	std::vector<std::wstring> filesVec;
	YString::GetDirFiles(L"./Assets/Textures", L"jpg", filesVec);

	for(std::vector<std::wstring>::iterator iter = filesVec.begin(); iter != filesVec.end(); ++iter)
	{
		textureMgr->GetOrCreateTexture((*iter).c_str());
	}

	textureMgr->CreateCubeTexture(L"skybox_sunny", 
		L"Assets/Textures/CubeMaps/Sunny1_right.jpg",
		L"Assets/Textures/CubeMaps/Sunny1_left.jpg",
		L"Assets/Textures/CubeMaps/Sunny1_up.jpg",
		L"Assets/Textures/CubeMaps/Sunny1_down.jpg",
		L"Assets/Textures/CubeMaps/Sunny1_front.jpg",
		L"Assets/Textures/CubeMaps/Sunny1_back.jpg");

	// material
	//MaterialManager* materialMgr = gEngine->GetMaterialManager();

	//Material* mtlFlat = New Material(L"mtlFlat", L"Assets/Materials/flat.material");
	//Material* mtlView = New Material(L"mtlView", L"Assets/Materials/view.material");
	//Material* mtlDiff = New Material(L"mtlDiff", L"Assets/Materials/diff.material");
	//Material* mtlSpec = New Material(L"mtlSpec", L"Assets/Materials/spec.material");
	//Material* mtlBump = New Material(L"mtlBump", L"Assets/Materials/bump.material");

	//materialMgr->AddMaterial(mtlFlat);
	//materialMgr->AddMaterial(mtlView);
	//materialMgr->AddMaterial(mtlDiff);
	//materialMgr->AddMaterial(mtlSpec);
	//materialMgr->AddMaterial(mtlBump);

	// model


	//ModelNode* cube = New ModelNode(L"cube", NULL, new Model(L"cube", L"Assets/Models/cube.model"));	
	//ModelNode* cylinder = New ModelNode(L"cylinder", NULL, new Model(L"cylinder", L"Assets/Models/cylinder.model"));	
	//ModelNode* cone = New ModelNode(L"cone", NULL, new Model(L"cone", L"Assets/Models/cone.model"));	
	//ModelNode* torus = New ModelNode(L"torus", NULL, new Model(L"torus", L"Assets/Models/torus.model"));	

	//sceneMgr->AddSceneNode(cube);
	//sceneMgr->AddSceneNode(cylinder);
	//sceneMgr->AddSceneNode(cone);
	//sceneMgr->AddSceneNode(torus);

	//cube->Translate(2, 0, 0);
	//cylinder->Translate(-2, 0, 0);
	//cone->Translate(0, 0, -2);
	//torus->Translate(0, 0, 2);

	//Model* barrelModel = NULL;
	//OBJParser::Parse(L"Assets/Models_Extra/barrel01.obj", &barrelModel);

	////barrelModel->SaveToFile(L"Assets/Models");

	//ModelNode* barrel = New ModelNode(L"barrel", NULL, barrelModel);

	//sceneMgr->AddSceneNode(barrel);
	//barrel->Translate(0, 0, -4);
	//barrel->Scale(0.01f, 0.01f, 0.01f);

	//Model* crateModel = NULL;
	//OBJParser::Parse(L"Assets/Models_Extra/crate.obj", &crateModel);


	//ModelNode* crate = New ModelNode(L"crate", NULL, crateModel);

	//sceneMgr->AddSceneNode(crate);
	//crate->Translate(0, 0, -6);
	//crate->Scale(0.01f, 0.01f, 0.01f);
	

	// skybox
	SkyBox* skyBox = New SkyBox();
	skyBox->SetCubeTex(L"skybox_sunny");
	sceneMgr->AddSkyBox(skyBox);

	// terrain
	PerformanceTimer::Begin(L"building 257 terrain");
	Terrain* terrain = New Terrain(257, 200.0f, 40.0f);
	terrain->LoadFromHeightMap(L"./Assets/Textures/Terrain/heightMap257_bit16.raw", 257);
	terrain->BuildTerrain(4);
	terrain->CalcChunkLODDist(mainCamera, 1.0f);

	terrain->SetColorTexes(L"./Assets/Textures/Terrain/Cliff.jpg", L"./Assets/Textures/Terrain/Grass_Hill.jpg", 
		L"./Assets/Textures/Terrain/DirtGrass.jpg", L"./Assets/Textures/Terrain/Pebbles.jpg");

	terrain->SetSplatMapTex(L"Assets/Textures/Terrain/splat.tga");
	terrain->SetBrushTex(L"Assets/Textures/Editor/pointLight.jpg");
	terrain->SetMtlParameters(30.0f, 30.0f, D3DXCOLOR_WHITE, D3DXCOLOR_WHITE);
	PerformanceTimer::End();

	sceneMgr->AddTerrain(terrain);
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
			GUIUpdate();
			gEngine->FrameUpdate();
			GUIUpdateAfterEngineUpdate();

			if(wxWindow::FindFocus() == this)
				mainCamera->ApplyCameraController();

			Terrain* terrain =sceneMgr->GetTerrain();
			if(input->GetLeftButtonDown())
			{
				Vector2 screenPos((float)input->GetCursorPos().x, (float)input->GetCursorPos().y);

				Vector3 rayPos;
				Vector3 rayDir;
				mainCamera->GetScreenRay(screenPos, &rayPos, &rayDir);

				Vector3 hitPos;
				if(terrain->RayIntersect(rayPos, rayDir, &hitPos, NULL))
				{
					float terrainLength = terrain->GetLength();
					float x = hitPos.x / terrainLength + 0.5f;
					float z = 0.5f - hitPos.z / terrainLength;

					const float BRUSH_RECT_SIZE = 0.2f;

					Vector4 brushRect(x - 0.5f * BRUSH_RECT_SIZE, z - 0.5f * BRUSH_RECT_SIZE,
						x + 0.5f * BRUSH_RECT_SIZE, z + 0.5f * BRUSH_RECT_SIZE);

					terrain->SetBrushRect(brushRect);

					ConsolePrint(L"hitPos(%f, %f, %f) rect(%f, %f, %f, %f)\n", hitPos.x, hitPos.y, hitPos.z, 
						brushRect.x, brushRect.y, brushRect.z, brushRect.w);
				}
			}

			// render
			driver->RenderToSwapChain(0);
			driver->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
			driver->BeginScene();

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
	YString::Format(text, TEXT("drawcalls:%d"), Profiler::GetNumDrawCalls());
	gGUISystem.GUILabel(text, Rect(10, 40, 300, 25), leftAlignStyle);

	YString::Format(text, TEXT("verts:%d"), Profiler::GetNumVerts());
	gGUISystem.GUILabel(text, Rect(10, 70, 300, 25), leftAlignStyle);

	YString::Format(text, TEXT("tris:%d"), Profiler::GetNumTris());
	gGUISystem.GUILabel(text, Rect(10, 100, 300, 25), leftAlignStyle);

	//YString::Format(text, L"screenPos:%d, %d(%f, %f)", cursorPos.x, cursorPos.y, 
	//	screenLocation.x, screenLocation.y);
	//gGUISystem.GUILabel(text, Rect(10, 130, 300, 25), leftAlignStyle);
}

void GUIUpdateAfterEngineUpdate()
{
	wchar_t text[MAX_STR_LEN];
	YString::Format(text, TEXT("fps:%d"), gEngine->GetFrameTimer()->GetFPS());
	gGUISystem.GUILabel(text, Rect(10, 10, 300, 25), leftAlignStyle);
}

void AppDestroy()
{
	SAFE_DELETE(leftAlignStyle);

	gEngine->Destroy();
	SAFE_DELETE(gEngine);
}

void OnLostDevice()
{
	leftAlignStyle->OnLostDevice();

	gDefaultLabelStyle.OnLostDevice();
	gGUISystem.OnLostDevice();

	gEngine->OnLostDevice();
}

void OnResetDevice()
{
	if(!gEngine->GetDriver()->Reset())
		return;

	leftAlignStyle->OnResetDevice();

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

	gEngine->GetGizmo()->RegisterEventHanlder(this);		// TODO:析构时ungegister?
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

//ModelDropTarget::ModelDropTarget( SceneEditorCanvas* canvas )
//:wxDropTarget()
//{
//	
//}
