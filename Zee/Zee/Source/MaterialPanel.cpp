#include "MaterialPanel.h"
#include "Engine.h"

BEGIN_EVENT_TABLE(MaterialPanel, wxPanel)
EVT_CLOSE(MaterialPanel::OnClose)
END_EVENT_TABLE()

MaterialPanel::MaterialPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
					   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
					   :wxPanel(parent, id, pos, size)
{
	mIconList = new wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/billboard.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void MaterialPanel::createWxCtrls()
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

void MaterialPanel::OnClose(wxCloseEvent& event)
{
	delete mIconList;
}

void MaterialPanel::LoadDataFromScene()
{
	MaterialManager* materialMgr = gEngine->GetMaterialManager();
	std::list<Material*> materialList = materialMgr->GetMaterialList();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, new MaterialTreeItemData(NULL));

	for(std::list<Material*>::iterator iter = materialList.begin(); iter != materialList.end(); ++iter)
	{
		Material* mtl = *iter;
		appendMaterial(rootId, mtl);
	}
}

void MaterialPanel::appendMaterial(wxTreeItemId parentItemId, Material* material)
{
	mTreeCtrl->AppendItem(parentItemId, material->GetName(), 0, 0, new MaterialTreeItemData(material));
}

MaterialTreeItemData::MaterialTreeItemData(Material* material)
:mMaterial(material)
{

}
