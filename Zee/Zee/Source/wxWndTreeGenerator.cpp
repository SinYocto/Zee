#include "wxWndTreeGenerator.h"
#include "D3DUtility.h"
#include "DebugDrawer.h"
#include "Camera.h"
#include "CameraController.h"
#include "ModelNode.h"
#include "GeometryManager.h"
#include "MaterialManager.h"
#include "Primitive.h"
#include "SceneManager.h"

BEGIN_EVENT_TABLE(TreeGeneratorFrame, wxFrame)
EVT_CLOSE(TreeGeneratorFrame::OnClose)
EVT_COLLAPSIBLEPANE_CHANGED(ID_GENERAL_PARAMS, TreeGeneratorFrame::OnCollapsPaneGeneralParams)
EVT_COLLAPSIBLEPANE_CHANGED(ID_LEVEL0_PARAMS, TreeGeneratorFrame::OnCollapsPaneLevelParams0)
END_EVENT_TABLE()

TreeGeneratorFrame::TreeGeneratorFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size):
wxFrame(parent, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT)
{
	wxBoxSizer* boxSizer1 = new wxBoxSizer(wxHORIZONTAL);

	// left
	mCtrlsPanel = new wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	mCtrlsPanel->SetScrollRate(0, 5);

	wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);

	// -- generalParams
	mPanelGeneralParams = new wxCollapsiblePane(mCtrlsPanel, ID_GENERAL_PARAMS, L"General Params");
	wxWindow* generalParamsWnd = mPanelGeneralParams->GetPane();

	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* textScale = new wxStaticText(generalParamsWnd, -1, L"Scale");
	mTextScale = new wxTextCtrl(generalParamsWnd, -1, L"10.0");

	wxStaticText* textScaleV = new wxStaticText(generalParamsWnd, -1, L"ScaleV");
	mTextScaleV = new wxTextCtrl(generalParamsWnd, -1, L"0");

	wxStaticText* textBaseSize = new wxStaticText(generalParamsWnd, -1, L"BaseSize");
	mTextBaseSize = new wxTextCtrl(generalParamsWnd, -1, L"0");

	wxStaticText* textRadiusRatio = new wxStaticText(generalParamsWnd, -1, L"RadiusRatio");
	mTextRadiusRatio = new wxTextCtrl(generalParamsWnd, -1, L"0.02");

	wxStaticText* textRatioPower = new wxStaticText(generalParamsWnd, -1, L"RatioPower");
	mTextRatioPower = new wxTextCtrl(generalParamsWnd, -1, L"1");

	wxStaticText* textShape = new wxStaticText(generalParamsWnd, -1, L"Shape");
	mSpinShape = new wxSpinCtrl(generalParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textBaseSplits = new wxStaticText(generalParamsWnd, -1, L"BaseSplits");
	mSpinBaseSplits = new wxSpinCtrl(generalParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);


	fgSizer1->Add(textScale, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mTextScale, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer1->Add(textScaleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mTextScaleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer1->Add(textBaseSize, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mTextBaseSize, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer1->Add(textRadiusRatio, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mTextRadiusRatio, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer1->Add(textRatioPower, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mTextRatioPower, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer1->Add(textShape, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mSpinShape, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer1->Add(textBaseSplits, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mSpinBaseSplits, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	generalParamsWnd->SetSizer(fgSizer1);
	generalParamsWnd->Layout();
	generalParamsWnd->Fit();

	// -- LevelParams0
	mPanelLevelParmas0 = new wxCollapsiblePane(mCtrlsPanel, ID_GENERAL_PARAMS, L"Level0 Params");
	wxWindow* levelParamsWnd0 = mPanelLevelParmas0->GetPane();

	wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
	fgSizer2->SetFlexibleDirection(wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* textLevel = new wxStaticText(levelParamsWnd0, -1, L"Level");
	mSpinLevel = new wxSpinCtrl(levelParamsWnd0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textSegSegsW = new wxStaticText(levelParamsWnd0, -1, L"SegSegsW");
	mSpinSegSegsW = new wxSpinCtrl(levelParamsWnd0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textSegSegsH = new wxStaticText(levelParamsWnd0, -1, L"SegSegsH");
	mSpinSegSegsH = new wxSpinCtrl(levelParamsWnd0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textCurveRes = new wxStaticText(levelParamsWnd0, -1, L"CurveRes");
	mSpinCurveRes = new wxSpinCtrl(levelParamsWnd0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textCurve = new wxStaticText(levelParamsWnd0, -1, L"Curve");
	mTextCurve = new wxTextCtrl(levelParamsWnd0, -1, L"10.0");

	wxStaticText* textCurveV = new wxStaticText(levelParamsWnd0, -1, L"CurveV");
	mTextCurveV = new wxTextCtrl(levelParamsWnd0, -1, L"0");

	wxStaticText* textCurveBack = new wxStaticText(levelParamsWnd0, -1, L"CurveBack");
	mTextCurveBack = new wxTextCtrl(levelParamsWnd0, -1, L"0");

	wxStaticText* textSegSplits = new wxStaticText(levelParamsWnd0, -1, L"SegSplits");
	mTextSegSplits = new wxTextCtrl(levelParamsWnd0, -1, L"0.02");

	wxStaticText* textSplitAngle = new wxStaticText(levelParamsWnd0, -1, L"SplitAngle");
	mTextSplitAngle = new wxTextCtrl(levelParamsWnd0, -1, L"1");

	wxStaticText* textSplitAngleV = new wxStaticText(levelParamsWnd0, -1, L"SplitAngleV");
	mTextSplitAngleV = new wxTextCtrl(levelParamsWnd0, -1, L"10.0");

	wxStaticText* textBranches = new wxStaticText(levelParamsWnd0, -1, L"Branches");
	mSpinBranches = new wxSpinCtrl(levelParamsWnd0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textDownAngle = new wxStaticText(levelParamsWnd0, -1, L"DownAngle");
	mTextDownAngle = new wxTextCtrl(levelParamsWnd0, -1, L"0");

	wxStaticText* textDownAngleV = new wxStaticText(levelParamsWnd0, -1, L"DownAngleV");
	mTextDownAngleV = new wxTextCtrl(levelParamsWnd0, -1, L"0.02");

	wxStaticText* textRotateAngle = new wxStaticText(levelParamsWnd0, -1, L"RotateAngle");
	mTextRotateAngle = new wxTextCtrl(levelParamsWnd0, -1, L"1");

	wxStaticText* textRotateAngleV = new wxStaticText(levelParamsWnd0, -1, L"RotateAngleV");
	mTextRotateAngleV = new wxTextCtrl(levelParamsWnd0, -1, L"1");


	fgSizer2->Add(textLevel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinLevel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textSegSegsW, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinSegSegsW, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textSegSegsH, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinSegSegsH, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textCurveRes, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinCurveRes, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textCurve, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextCurve, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textCurveV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextCurveV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textCurveBack, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextCurveBack, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textSegSplits, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextSegSplits, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textSplitAngle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextSplitAngle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textSplitAngleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextSplitAngleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textBranches, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinBranches, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textDownAngle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextDownAngle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textDownAngleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextDownAngleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textRotateAngle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextRotateAngle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textRotateAngleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextRotateAngleV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	levelParamsWnd0->SetSizer(fgSizer2);
	levelParamsWnd0->Layout();
	levelParamsWnd0->Fit();

	boxSizer2->Add(mPanelGeneralParams, 0, wxALL | wxEXPAND, 5);
	boxSizer2->Add(mPanelLevelParmas0, 0, wxALL | wxEXPAND, 5);

	mCtrlsPanel->SetSizer(boxSizer2);
	mCtrlsPanel->SetMinSize(wxSize(240, 600));
	mCtrlsPanel->FitInside();
	mCtrlsPanel->Layout();

	// right
	mCanvasPanel = new wxPanel(this, -1);
	mCanvasPanel->SetMinSize(wxSize(480, 600));

	boxSizer1->Add(mCtrlsPanel, 0, wxALL, 5);
	boxSizer1->Add(mCanvasPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();

	wxSize clientSize = mCanvasPanel->GetClientSize();
	mCanvas = new TreeGeneratorCanvas(mCanvasPanel, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);

	D3DPRESENT_PARAMETERS presentParams = Driver::GetPresentParameters();
	presentParams.BackBufferWidth = clientSize.x;
	presentParams.BackBufferHeight = clientSize.y;

	Driver::CreateSecondarySwapChain(presentParams);

	mSpinLevel->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(TreeGeneratorFrame::OnSpinLevel), NULL, this);
	////mSpin2->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(TreeGeneratorFrame::OnSpinValue2), NULL, this);
}

void TreeGeneratorFrame::CleanupAndDestory()
{
	mCanvas->CleanupAndDestory();
	mCtrlsPanel->Destroy();
	mCanvasPanel->Destroy();
	Destroy();
}

void TreeGeneratorFrame::OnClose(wxCloseEvent& event)
{
	Show(false);		// 点右上角x时, 不关闭, 隐藏
}

void TreeGeneratorFrame::OnCollapsPaneGeneralParams(wxCollapsiblePaneEvent& event)
{
	mCtrlsPanel->FitInside();
	mCtrlsPanel->Layout();
	mCtrlsPanel->Refresh();
}

void TreeGeneratorFrame::OnCollapsPaneLevelParams0(wxCollapsiblePaneEvent& event)
{
	mCtrlsPanel->FitInside();
	mCtrlsPanel->Layout();
	mCtrlsPanel->Refresh();
}

void TreeGeneratorFrame::OnSpinLevel(wxSpinEvent& event)
{
	// ...
	Vector3 pos = mCanvas->mCube->GetWorldPosition();

	pos.y = mSpinLevel->GetValue();

	mCanvas->mCube->SetWorldPosition(pos);
}

//void TreeGeneratorFrame::OnSpinValue2(wxSpinEvent& event)
//{
//	// ...
//}
//
//void TreeGeneratorFrame::OnTextValue3(wxCommandEvent& event)
//{
//	wxString valStr = mTextCtrl3->GetValue();
//
//	double val;
//	valStr.ToDouble(&val);
//
//	Vector3 pos = mCanvas->mCube->GetWorldPosition();
//
//	pos.y = val;
//
//	mCanvas->mCube->SetWorldPosition(pos);
//
//}


void TreeGeneratorFrame::Setup()
{
	mCanvas->Setup();
}

BEGIN_EVENT_TABLE(TreeGeneratorCanvas, wxWindow)
EVT_SIZE(TreeGeneratorCanvas::OnSize)
EVT_IDLE(TreeGeneratorCanvas::OnIdle)
END_EVENT_TABLE()

TreeGeneratorCanvas::TreeGeneratorCanvas(wxWindow* parent, wxWindowID id /* = wxID_ANY */, 
					   const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, 
					   long style /* = 0 */, const wxString& name /* = wxT */)
					   :wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
					   ,mCube(NULL)
{

}

void TreeGeneratorCanvas::OnSize(wxSizeEvent& event)
{
	// ...
}


void TreeGeneratorCanvas::OnIdle(wxIdleEvent& event)
{
	if(Driver::D3DDevice)
		RenderWindow();

	event.RequestMore(true);
}
 
void TreeGeneratorCanvas::RenderWindow()
{
	if(Driver::D3DDevice->TestCooperativeLevel() == D3D_OK)
	{
		SceneManager::extraCamera->FrameUpdate();

		if(wxWindow::FindFocus() == this)
			SceneManager::extraCamera->ApplyCameraController();

		Driver::RenderToSwapChain(SECONDARY_SWAPCHAIN);
		Driver::Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
		Driver::BeginScene();

		std::vector<Vector3> points;
		points.push_back(Vector3::Zero);
		points.push_back(Vector3(2, 0, 0));
		DebugDrawer::DrawLine(points, 0xffff0000, SceneManager::extraCamera);

		mCube->Draw(SceneManager::extraCamera);

		Driver::EndScene();
		Driver::Present((HWND)GetHWND());
	}
}

void TreeGeneratorCanvas::OnClose(wxCloseEvent& event)
{
	CleanupAndDestory();
}

void TreeGeneratorCanvas::CleanupAndDestory()
{
	SAFE_DELETE(mCube);
	Destroy();
}

void TreeGeneratorCanvas::Setup()
{
	SceneManager::CreateExtraCamera(Vector3(0, 0, -10.0f), Vector3::Zero,
		PI/3, (float)Driver::secondaryViewPort.Width / (float)Driver::secondaryViewPort.Height, 0.1f, 1000.0f);

	HoverCameraController* hoverCameraController = new HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 2.0f, 100.0f);
	SceneManager::extraCamera->SetCameraController(hoverCameraController);

	Cube* cubeGeo = new Cube(L"cubeGeo1");
	GeometryManager::AddGeometry(cubeGeo);

	cubeGeo->CalculateTBN();
	cubeGeo->BuildGeometry(XYZ_UV_TBN);

	mCube = new ModelNode(L"cube", NULL, cubeGeo, MaterialManager::flatMtl);
}
