#include "ModelPanel.h"
#include "Engine.h"

BEGIN_EVENT_TABLE(ModelPanel, wxPanel)
EVT_CLOSE(ModelPanel::OnClose)
END_EVENT_TABLE()

ModelPanel::ModelPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								 :wxPanel(parent, id, pos, size)
{
	mIconList = new wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/model.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void ModelPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);

	// listCtrl
	mTreePanel = new wxPanel(this, -1);
	mTreePanel->SetMinSize(wxSize(200, 300));

	wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);

	mTreeCtrl = new wxTreeCtrl(mTreePanel, -1, wxDefaultPosition, wxDefaultSize, 
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_HIDE_ROOT);

	mTreeCtrl->SetMinSize(wxSize(180, 260));
	mTreeCtrl->AssignImageList(mIconList);

	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = new wxPanel(this, -1);
	mInspectorPanel->SetMinSize(wxSize(200, 300));

	boxSizer1->Add(mInspectorPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void ModelPanel::OnClose(wxCloseEvent& event)
{
	delete mIconList;
}

void ModelPanel::LoadDataFromScene()
{
	ModelManager* modelMgr = gEngine->GetModelManager();
	std::list<Model*> modelList = modelMgr->GetModelList();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, new ModelTreeItemData(NULL));

	for(std::list<Model*>::iterator iter = modelList.begin(); iter != modelList.end(); ++iter)
	{
		Model* model = *iter;
		appendModel(rootId, model);
	}
}

void ModelPanel::appendModel(wxTreeItemId parentItemId, Model* model)
{
	mTreeCtrl->AppendItem(parentItemId, model->GetName(), 0, 0, new ModelTreeItemData(model));
}

ModelTreeItemData::ModelTreeItemData(Model* model)
:mModel(model)
{

}

