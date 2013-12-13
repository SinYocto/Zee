#include "wxWndTreeGenerator.h"
#include "D3DUtility.h"
#include "DebugDrawer.h"
#include "Camera.h"
#include "CameraController.h"
#include "ModelNode.h"
#include "Engine.h"
#include "Primitive.h"
#include "SceneManager.h"
#include "Time.h"

BEGIN_EVENT_TABLE(TreeGeneratorFrame, wxFrame)
EVT_CLOSE(TreeGeneratorFrame::OnClose)
EVT_MENU(ID_MENU_OPEN, TreeGeneratorFrame::OnOpenFile)
EVT_COLLAPSIBLEPANE_CHANGED(ID_GENERAL_PARAMS, TreeGeneratorFrame::OnCollapsPaneGeneralParams)
EVT_COLLAPSIBLEPANE_CHANGED(ID_LEVEL0_PARAMS, TreeGeneratorFrame::OnCollapsPaneLevelParams0)
EVT_BUTTON(ID_BUTTON_GENERATE, TreeGeneratorFrame::OnButtonGenerate)
END_EVENT_TABLE()

TreeGeneratorFrame::TreeGeneratorFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size)
:wxFrame(parent, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT)
,mSelectedLevel(0)
{
	createWxCtrls();

	wxSize clientSize = mCanvasPanel->GetClientSize();
	mCanvas = New TreeGeneratorCanvas(mCanvasPanel, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);

	D3DPRESENT_PARAMETERS presentParams = gEngine->GetDriver()->GetPresentParameters(0);
	presentParams.BackBufferWidth = clientSize.x;
	presentParams.BackBufferHeight = clientSize.y;

	int swapChainIndex = gEngine->GetDriver()->CreateAdditionalSwapChain(presentParams);
	mCanvas->SetSwapChainIndex(swapChainIndex);
}

void TreeGeneratorFrame::createWxCtrls()
{
	// menu
	SetIcon(wxIcon(L"./Assets/Icons/Zee.xpm", wxBITMAP_TYPE_XPM));

	wxMenu* menuFile = New wxMenu;
	menuFile->Append(ID_MENU_OPEN, L"&Open");
	menuFile->Append(ID_MENU_SAVE, L"&Save");

	wxMenuBar* menuBar = New wxMenuBar;
	menuBar->Append(menuFile, L"&File");

	SetMenuBar(menuBar);

	// wnd
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxHORIZONTAL);

	// left
	mCtrlsPanel = New wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	mCtrlsPanel->SetScrollRate(0, 5);

	wxBoxSizer* boxSizer2 = New wxBoxSizer(wxVERTICAL);

	// -- generalParams
	mPanelGeneralParams = New wxCollapsiblePane(mCtrlsPanel, ID_GENERAL_PARAMS, L"General Params");
	wxWindow* generalParamsWnd = mPanelGeneralParams->GetPane();

	wxFlexGridSizer* fgSizer1 = New wxFlexGridSizer(2, 2, 0, 0);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* textLevels = New wxStaticText(generalParamsWnd, -1, L"Levels");
	mSpinLevels = New wxSpinCtrl(generalParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 5, 1);

	wxStaticText* textScale = New wxStaticText(generalParamsWnd, -1, L"Scale");
	mTextScale = New wxTextCtrl(generalParamsWnd, -1, L"10.0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleV = New wxStaticText(generalParamsWnd, -1, L"ScaleV");
	mTextScaleV = New wxTextCtrl(generalParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textBaseSize = New wxStaticText(generalParamsWnd, -1, L"BaseSize");
	mTextBaseSize = New wxTextCtrl(generalParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textRadiusRatio = New wxStaticText(generalParamsWnd, -1, L"RadiusRatio");
	mTextRadiusRatio = New wxTextCtrl(generalParamsWnd, -1, L"0.02", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textRatioPower = New wxStaticText(generalParamsWnd, -1, L"RatioPower");
	mTextRatioPower = New wxTextCtrl(generalParamsWnd, -1, L"1", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textShape = New wxStaticText(generalParamsWnd, -1, L"Shape");
	mSpinShape = New wxSpinCtrl(generalParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	wxStaticText* textBaseSplits = New wxStaticText(generalParamsWnd, -1, L"BaseSplits");
	mSpinBaseSplits = New wxSpinCtrl(generalParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 10, 0);

	fgSizer1->Add(textLevels, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer1->Add(mSpinLevels, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

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

	// -- LevelParams
	mPanelLevelParmas0 = New wxCollapsiblePane(mCtrlsPanel, ID_GENERAL_PARAMS, L"Level Params");
	wxWindow* levelParamsWnd = mPanelLevelParmas0->GetPane();

	wxFlexGridSizer* fgSizer2 = New wxFlexGridSizer(2, 2, 0, 0);
	fgSizer2->SetFlexibleDirection(wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* textLevel = New wxStaticText(levelParamsWnd, -1, L"Level");
	//mSpinLevel = new wxSpinCtrl(levelParamsWnd0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
	//	wxSP_ARROW_KEYS, 0, 10, 0);
	wxArrayString choices;
	choices.Add(wxT("0"));
	mChoiceLevel = New wxChoice(levelParamsWnd, -1, wxDefaultPosition, wxDefaultSize, choices);
	mChoiceLevel->SetSelection(0);
	mSelectedLevel = 0;

	wxStaticText* textSegSegsW = New wxStaticText(levelParamsWnd, -1, L"SegSegsW");
	mSpinSegSegsW = New wxSpinCtrl(levelParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 100, 8);

	wxStaticText* textSegSegsH = New wxStaticText(levelParamsWnd, -1, L"SegSegsH");
	mSpinSegSegsH = New wxSpinCtrl(levelParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 100, 4);

	wxStaticText* textLength = New wxStaticText(levelParamsWnd, -1, L"Length");
	mTextLength = New wxTextCtrl(levelParamsWnd, -1, L"1.0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textLengthV = New wxStaticText(levelParamsWnd, -1, L"LengthV");
	mTextLengthV = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textTaper = New wxStaticText(levelParamsWnd, -1, L"Taper");
	mTextTaper = New wxTextCtrl(levelParamsWnd, -1, L"1", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textCurveRes = New wxStaticText(levelParamsWnd, -1, L"CurveRes");
	mSpinCurveRes = New wxSpinCtrl(levelParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 100, 3);

	wxStaticText* textCurve = New wxStaticText(levelParamsWnd, -1, L"Curve");
	mTextCurve = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textCurveV = New wxStaticText(levelParamsWnd, -1, L"CurveV");
	mTextCurveV = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textCurveBack = New wxStaticText(levelParamsWnd, -1, L"CurveBack");
	mTextCurveBack = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textSegSplits = New wxStaticText(levelParamsWnd, -1, L"SegSplits");
	mTextSegSplits = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textSplitAngle = New wxStaticText(levelParamsWnd, -1, L"SplitAngle");
	mTextSplitAngle = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textSplitAngleV = New wxStaticText(levelParamsWnd, -1, L"SplitAngleV");
	mTextSplitAngleV = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textBranches = New wxStaticText(levelParamsWnd, -1, L"Branches");
	mSpinBranches = New wxSpinCtrl(levelParamsWnd, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxSP_ARROW_KEYS, 0, 100, 0);

	wxStaticText* textDownAngle = New wxStaticText(levelParamsWnd, -1, L"DownAngle");
	mTextDownAngle = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textDownAngleV = New wxStaticText(levelParamsWnd, -1, L"DownAngleV");
	mTextDownAngleV = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textRotateAngle = New wxStaticText(levelParamsWnd, -1, L"RotateAngle");
	mTextRotateAngle = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textRotateAngleV = New wxStaticText(levelParamsWnd, -1, L"RotateAngleV");
	mTextRotateAngleV = New wxTextCtrl(levelParamsWnd, -1, L"0", wxDefaultPosition, wxDefaultSize, 0, 
		wxTextValidator(wxFILTER_NUMERIC));


	fgSizer2->Add(textLevel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mChoiceLevel, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

	fgSizer2->Add(textSegSegsW, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinSegSegsW, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textSegSegsH, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mSpinSegSegsH, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textLength, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextLength, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textLengthV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextLengthV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	fgSizer2->Add(textTaper, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	fgSizer2->Add(mTextTaper, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

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

	levelParamsWnd->SetSizer(fgSizer2);
	levelParamsWnd->Layout();
	levelParamsWnd->Fit();

	// button
	mButtonGenerate = New wxButton(mCtrlsPanel, ID_BUTTON_GENERATE, L"Generate");

	boxSizer2->Add(mPanelGeneralParams, 0, wxALL, 5);
	boxSizer2->Add(mPanelLevelParmas0, 0, wxALL, 5);
	boxSizer2->Add(mButtonGenerate, 0, wxALL, 5);

	mCtrlsPanel->SetSizer(boxSizer2);
	mCtrlsPanel->SetMinSize(wxSize(240, 600));
	mCtrlsPanel->FitInside();
	mCtrlsPanel->Layout();

	// right
	mCanvasPanel = New wxPanel(this, -1);
	mCanvasPanel->SetMinSize(wxSize(480, 600));

	boxSizer1->Add(mCtrlsPanel, 0, wxALL, 5);
	boxSizer1->Add(mCanvasPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();

	mSpinLevels->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(TreeGeneratorFrame::OnSpinLevels), NULL, this);
	mChoiceLevel->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(TreeGeneratorFrame::OnChoiceLevel), NULL, this);
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

void TreeGeneratorFrame::OnOpenFile(wxCommandEvent& event)
{
	wxFileDialog dialog(this, L"Open", wxEmptyString, wxEmptyString, _T("Tree ini files (*.ini*)|*.ini*"), wxFD_MULTIPLE);
	if (dialog.ShowModal() == wxID_OK)
	{
		// ...
		mCanvas->LoadParamsFromFile(dialog.GetPath().wchar_str());

		setValueGeneralParams(mCanvas->mTree->GetGeneralParams());
		setValueLevelParams(mCanvas->mTree->GetLevelParams(0));

		refreshChoiceLevel();
	}
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

void TreeGeneratorFrame::getValueGeneralParams(TreeGeneralParams* generalParams)
{
	_Assert(NULL != generalParams);

	double scale = 0;
	double scaleV = 0;
	double baseSize = 0;
	double radiusRatio = 0;
	double ratioPower = 0;

	mTextScale->GetValue().ToDouble(&scale);
	mTextScaleV->GetValue().ToDouble(&scaleV);
	mTextBaseSize->GetValue().ToDouble(&baseSize);
	mTextRadiusRatio->GetValue().ToDouble(&radiusRatio);
	mTextRatioPower->GetValue().ToDouble(&ratioPower);

	generalParams->scale = scale;
	generalParams->scaleV = scaleV;
	generalParams->baseSize = baseSize;
	generalParams->radiusRatio = radiusRatio;
	generalParams->ratioPower = ratioPower;

	generalParams->levels = mSpinLevels->GetValue();
	generalParams->shape = mSpinShape->GetValue();
	generalParams->baseSplits = mSpinBaseSplits->GetValue();
}

void TreeGeneratorFrame::getValueLevelParams(TreeLevelParams* levelParams)
{
	_Assert(NULL != levelParams);

	double length = 0;
	double lengthV = 0;
	double taper = 0;

	double curve = 0;
	double curveV = 0;
	double curveBack = 0;

	double segSplits = 0;
	double splitAngle = 0;
	double splitAngleV = 0;
	double downAngle = 0;
	double downAngleV = 0;
	double rotateAngle = 0;
	double rotateAngleV = 0;

	mTextLength->GetValue().ToDouble(&length);
	mTextLengthV->GetValue().ToDouble(&lengthV);
	mTextTaper->GetValue().ToDouble(&taper);

	mTextCurve->GetValue().ToDouble(&curve);
	mTextCurveV->GetValue().ToDouble(&curveV);
	mTextCurveBack->GetValue().ToDouble(&curveBack);

	mTextSegSplits->GetValue().ToDouble(&segSplits);
	mTextSplitAngle->GetValue().ToDouble(&splitAngle);
	mTextSplitAngleV->GetValue().ToDouble(&splitAngleV);
	mTextDownAngle->GetValue().ToDouble(&downAngle);
	mTextDownAngleV->GetValue().ToDouble(&downAngleV);
	mTextRotateAngle->GetValue().ToDouble(&rotateAngle);
	mTextRotateAngleV->GetValue().ToDouble(&rotateAngleV);

	levelParams->length = length;
	levelParams->lengthV = lengthV;
	levelParams->taper = taper;

	levelParams->curve = curve;
	levelParams->curveV = curveV;
	levelParams->curveBack = curveBack;

	levelParams->segSplits = segSplits;
	levelParams->splitAngle = splitAngle;
	levelParams->splitAngleV = splitAngleV;
	levelParams->downAngle = downAngle;
	levelParams->downAngleV = downAngleV;
	levelParams->rotateAngle = rotateAngle;
	levelParams->rotateAngleV = rotateAngleV;

	levelParams->level = mChoiceLevel->GetCurrentSelection();
	levelParams->segSegsW = mSpinSegSegsW->GetValue();
	levelParams->segSegsH = mSpinSegSegsH->GetValue();
	levelParams->curveRes = mSpinCurveRes->GetValue();
	levelParams->branches = mSpinBranches->GetValue();
}

void TreeGeneratorFrame::OnButtonGenerate(wxCommandEvent& event)
{
	TreeGeneralParams generalParams;
	TreeLevelParams levelParams;

	getValueGeneralParams(&generalParams);
	getValueLevelParams(&levelParams);  

	mCanvas->mTree->SetGeneralParams(generalParams);
	mCanvas->mTree->SetLevelParams(levelParams.level, levelParams);

	PerformanceTimer::Begin(L"start building trunk...");
	mCanvas->mTree->Generate();
	PerformanceTimer::End();
}

void TreeGeneratorFrame::OnSpinLevels(wxSpinEvent& event)
{
	int levels = mSpinLevels->GetValue();
	mCanvas->mTree->SetGeneralParamsLevels(levels);

	refreshChoiceLevel();
}

void TreeGeneratorFrame::OnChoiceLevel(wxCommandEvent& event)
{
	TreeLevelParams levelParams;
	getValueLevelParams(&levelParams);
	levelParams.level = mSelectedLevel;

	mCanvas->mTree->SetLevelParams(mSelectedLevel, levelParams);

	mSelectedLevel = mChoiceLevel->GetCurrentSelection();
	setValueLevelParams(mCanvas->mTree->GetLevelParams(mSelectedLevel));
}

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

	setValueGeneralParams(mCanvas->mTree->GetGeneralParams());
	setValueLevelParams(mCanvas->mTree->GetLevelParams(0));

	refreshChoiceLevel();
}

void TreeGeneratorFrame::setValueGeneralParams(TreeGeneralParams generalParams)
{
	wxString scale = wxString::Format(wxT("%.2f"), generalParams.scale);
	wxString scaleV = wxString::Format(wxT("%.2f"), generalParams.scaleV);
	wxString baseSize = wxString::Format(wxT("%.2f"), generalParams.baseSize);
	wxString radiusRatio = wxString::Format(wxT("%.3f"), generalParams.radiusRatio);
	wxString ratioPower = wxString::Format(wxT("%.2f"), generalParams.ratioPower);

	mTextScale->SetValue(scale);
	mTextScaleV->SetValue(scaleV);
	mTextBaseSize->SetValue(baseSize);
	mTextRadiusRatio->SetValue(radiusRatio);
	mTextRatioPower->SetValue(ratioPower);

	mSpinLevels->SetValue(generalParams.levels);
	mSpinShape->SetValue(generalParams.shape);
	mSpinBaseSplits->SetValue(generalParams.baseSplits);
}

void TreeGeneratorFrame::setValueLevelParams(TreeLevelParams levelParams)
{
	wxString length = wxString::Format(wxT("%.2f"), levelParams.length);
	wxString lengthV = wxString::Format(wxT("%.2f"), levelParams.lengthV);
	wxString taper = wxString::Format(wxT("%.2f"), levelParams.taper);

	wxString curve = wxString::Format(wxT("%.2f"), levelParams.curve);
	wxString curveV = wxString::Format(wxT("%.2f"), levelParams.curveV);
	wxString curveBack = wxString::Format(wxT("%.2f"), levelParams.curveBack);

	wxString segSplits = wxString::Format(wxT("%.2f"), levelParams.segSplits);
	wxString splitAngle = wxString::Format(wxT("%.2f"), levelParams.splitAngle);
	wxString splitAngleV = wxString::Format(wxT("%.2f"), levelParams.splitAngleV);
	wxString downAngle = wxString::Format(wxT("%.2f"), levelParams.downAngle);
	wxString downAngleV = wxString::Format(wxT("%.2f"), levelParams.downAngleV);
	wxString rotateAngle = wxString::Format(wxT("%.2f"), levelParams.rotateAngle);
	wxString rotateAngleV = wxString::Format(wxT("%.2f"), levelParams.rotateAngleV);

	mTextLength->SetValue(length);
	mTextLengthV->SetValue(lengthV);
	mTextTaper->SetValue(taper);

	mTextCurve->SetValue(curve);
	mTextCurveV->SetValue(curveV);
	mTextCurveBack->SetValue(curveBack);

	mTextSegSplits->SetValue(segSplits);
	mTextSplitAngle->SetValue(splitAngle);
	mTextSplitAngleV->SetValue(splitAngleV);
	mTextDownAngle->SetValue(downAngle);
	mTextDownAngleV->SetValue(downAngleV);
	mTextRotateAngle->SetValue(rotateAngle);
	mTextRotateAngleV->SetValue(rotateAngleV);

	//mSpinLevel->SetValue(levelParams.level);
	mSpinSegSegsW->SetValue(levelParams.segSegsW);
	mSpinSegSegsH->SetValue(levelParams.segSegsH);
	mSpinCurveRes->SetValue(levelParams.curveRes);
	mSpinBranches->SetValue(levelParams.branches);
}

void TreeGeneratorFrame::refreshChoiceLevel()
{
	int levels = mSpinLevels->GetValue();

	int curSelection = mChoiceLevel->GetCurrentSelection();
	mChoiceLevel->Clear();

	wxArrayString choices;
	for(int i = 0; i < levels; ++i)
	{
		wxString level = wxString::Format(wxT("%i"), i);
		choices.Add(level);
	}

	mChoiceLevel->Append(choices);

	int selection = 0;
	if(curSelection < levels)
		selection = curSelection;

	mChoiceLevel->SetSelection(selection);
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
					   ,mTree(NULL)
{

}

void TreeGeneratorCanvas::OnSize(wxSizeEvent& event)
{
	// ...
}


void TreeGeneratorCanvas::OnIdle(wxIdleEvent& event)
{
	if(gEngine->GetDriver()->GetD3DDevice())
		RenderWindow();

	event.RequestMore(true);
}
 
void TreeGeneratorCanvas::RenderWindow()
{
	Driver* driver = gEngine->GetDriver();
	IDirect3DDevice9* d3dDevice = driver->GetD3DDevice();
	Camera* extraCamera = gEngine->GetSceneManager()->GetExtraCamera();

	switch(d3dDevice->TestCooperativeLevel())
	{
	case D3D_OK:
		{
			extraCamera->FrameUpdate();

			if(wxWindow::FindFocus() == this)
				extraCamera->ApplyCameraController();

			driver->RenderToSwapChain(swapChainIndex);
			driver->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
			driver->BeginScene();

			std::vector<Vector3> points;
			points.push_back(Vector3::Zero);
			points.push_back(Vector3(2, 0, 0));
			DebugDrawer::DrawLine(points, 0xffff0000, extraCamera);

			if(mTree)
				mTree->Draw(extraCamera);

			driver->EndScene();
			driver->Present((HWND)GetHWND());

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
	SceneManager* sceneMgr = gEngine->GetSceneManager();

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(swapChainIndex);
	sceneMgr->CreateExtraCamera(Vector3(0, 0, -10.0f), Vector3::Zero,
		PI/3, (float)viewPort.Width / (float)viewPort.Height, 0.1f, 1000.0f);

	Camera* extraCamera = sceneMgr->GetExtraCamera();

	HoverCameraController* hoverCameraController = New HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 2.0f, 100.0f);
	extraCamera->SetCameraController(hoverCameraController);

	Cube* cubeGeo = New Cube(L"cubeGeo1");
	gEngine->GetGeometryManager()->AddGeometry(cubeGeo);

	cubeGeo->CalculateTBN();
	cubeGeo->BuildGeometry(XYZ_UV_TBN);

	mCube = New ModelNode(L"cube", NULL, cubeGeo, gEngine->GetMaterialManager()->GetDefaultFlatMtl());

	mTree = New Tree();
}

void TreeGeneratorCanvas::OnLostDevice()
{
	if(mTree)
		mTree->OnLostDevice();
}

void TreeGeneratorCanvas::OnResetDevice()
{
	Driver* driver = gEngine->GetDriver();

	if(!driver->Reset())
		return;

	if(mTree)
		mTree->OnResetDevice();
}

void TreeGeneratorCanvas::LoadParamsFromFile(const wchar_t* filePath)
{
	_Assert(mTree);
	mTree->LoadParamsFromFile(filePath);
}

void TreeGeneratorCanvas::SetSwapChainIndex(int index)
{
	swapChainIndex = index;
}
