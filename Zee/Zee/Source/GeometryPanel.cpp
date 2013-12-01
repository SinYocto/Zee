#include "GeometryPanel.h"
#include "Engine.h"

BEGIN_EVENT_TABLE(GeometryPanel, wxPanel)
EVT_CLOSE(GeometryPanel::OnClose)
END_EVENT_TABLE()

GeometryPanel::GeometryPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								 :wxPanel(parent, id, pos, size)
{
	mIconList = new wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/geometryPage.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void GeometryPanel::createWxCtrls()
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

void GeometryPanel::OnClose(wxCloseEvent& event)
{
	delete mIconList;
}

void GeometryPanel::LoadDataFromScene()
{
	GeometryManager* geoMgr = gEngine->GetGeometryManager();
	std::list<Geometry*> geoList = geoMgr->GetGeometryList();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, new GeometryTreeItemData(NULL));

	for(std::list<Geometry*>::iterator iter = geoList.begin(); iter != geoList.end(); ++iter)
	{
		Geometry* geo = *iter;
		appendGeometry(rootId, geo);
	}
}

void GeometryPanel::appendGeometry(wxTreeItemId parentItemId, Geometry* geo)
{
	mTreeCtrl->AppendItem(parentItemId, geo->GetName(), 0, 0, new GeometryTreeItemData(geo));
}

GeometryTreeItemData::GeometryTreeItemData(Geometry* geo)
:mGeometry(geo)
{

}
