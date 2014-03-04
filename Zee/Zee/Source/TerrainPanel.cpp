#include "TerrainPanel.h"
#include "Engine.h"

TerrainPanel::TerrainPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								 :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void TerrainPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mInspectorPanel = New TerrainInspectorPanel(this, wxID_ANY);
	mInspectorPanel->SetMinSize(wxSize(180, 240));

	boxSizer1->Add(mInspectorPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void TerrainPanel::LoadDataFromScene()
{
	Terrain* terrain = gEngine->GetSceneManager()->GetTerrain();
	if(terrain != NULL)
		mInspectorPanel->AttatchToTerrain(terrain);
}


TerrainInspectorPanel::TerrainInspectorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
								   const wxSize& size /*= wxDefaultSize*/)
								   :wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
								   ,mTerrain(NULL)
								   ,mInfoPanel(NULL)
{
	createWxCtrls();
}

void TerrainInspectorPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mInfoPanel = New TerrainInfoPanel(this, wxID_ANY);

	boxSizer1->Add(mInfoPanel, 0, wxALL, 5);

	this->SetScrollRate(0, 5);
	this->SetSizer(boxSizer1);
	this->FitInside();
	this->Layout();
}

void TerrainInspectorPanel::AttatchToTerrain(Terrain* terrain)
{
	_Assert(mTerrain == NULL);
	mTerrain = terrain;

	loadData();
}

void TerrainInspectorPanel::loadData()
{
	if(mInfoPanel)
		mInfoPanel->LoadData(mTerrain);
}

Terrain* TerrainInspectorPanel::GetAttachedTerrain()
{
	return mTerrain;
}


BEGIN_EVENT_TABLE(TerrainInfoPanel, wxPanel)
	EVT_CHECKBOX(wxID_ANY, TerrainInfoPanel::OnCheckBox)
	EVT_TEXT_ENTER(wxID_ANY, TerrainInfoPanel::OnTextEnter)
END_EVENT_TABLE()

TerrainInfoPanel::TerrainInfoPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
								   const wxSize& size /*= wxDefaultSize*/)
								   :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void TerrainInfoPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mCheckBoxWireFrame = New wxCheckBox(this, ID_CHECKBOX_WIREFRAME, wxT("WireFrame"));

	wxFlexGridSizer* fgSizer1 = New wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textLodTolerance = New wxStaticText(this, -1, L"LodTolerance");
	mTextCtrlLodTolerance = New wxTextCtrl(this, ID_TEXT_LOD_TOLERANCE, L"0", wxDefaultPosition, wxDefaultSize, 
		wxTE_PROCESS_ENTER, wxTextValidator(wxFILTER_NUMERIC));

	fgSizer1->Add(textLodTolerance, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlLodTolerance, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	boxSizer1->Add(mCheckBoxWireFrame, 0, wxALL, 5);
	boxSizer1->Add(fgSizer1, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void TerrainInfoPanel::LoadData(Terrain* terrain)
{
	if(!terrain)
		return;

	wxString lodTolerance = wxString::Format(wxT("%.2f"), terrain->GetLodTolerance());
	mTextCtrlLodTolerance->SetValue(lodTolerance);
}

void TerrainInfoPanel::OnCheckBox(wxCommandEvent& event)
{
	TerrainInspectorPanel* inspectorPanel = (TerrainInspectorPanel*)GetParent();
	Terrain* terrain = inspectorPanel->GetAttachedTerrain();

	if(!terrain)
		return;

	wxCheckBox* checkBox = (wxCheckBox*)event.GetEventObject();
	bool val = checkBox->GetValue();

	switch(checkBox->GetId())
	{
	case ID_CHECKBOX_WIREFRAME:
		terrain->SetWireFrameMode(val);
		break;

	default:
		break;
	}
}

void TerrainInfoPanel::OnTextEnter(wxCommandEvent& event)
{
	wxTextCtrl* textCtrl = (wxTextCtrl*)event.GetEventObject();

	double val = 0;
	textCtrl->GetValue().ToDouble(&val);

	Clamp(val, 0.0, 20.0);

	TerrainInspectorPanel* inspectorPanel = (TerrainInspectorPanel*)GetParent();
	Terrain* terrain = inspectorPanel->GetAttachedTerrain();

	if(!terrain)
		return;

	switch(textCtrl->GetId())
	{
	case ID_TEXT_LOD_TOLERANCE:
		terrain->CalcChunkLODDist(gEngine->GetSceneManager()->GetMainCamera(), val);
		break;

	default:
		break;
	}
}
