#include "TexturePanel.h"
#include "Engine.h"

BEGIN_EVENT_TABLE(TexturePanel, wxPanel)
EVT_CLOSE(TexturePanel::OnClose)
END_EVENT_TABLE()

TexturePanel::TexturePanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
					   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
					   :wxPanel(parent, id, pos, size)
{
	mIconList = new wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/texturePage.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void TexturePanel::createWxCtrls()
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

void TexturePanel::OnClose(wxCloseEvent& event)
{
	delete mIconList;
}

void TexturePanel::LoadDataFromScene()
{
	TextureManager* textureMgr = gEngine->GetTextureManger();
	TexHashMap texHashMap = textureMgr->GetTextureHashMap();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, new TextureTreeItemData(NULL));

	for(TexHashMap::iterator iter = texHashMap.begin(); iter != texHashMap.end(); ++iter)
	{
		appendTexture(rootId, iter->second);
	}
}

void TexturePanel::appendTexture(wxTreeItemId parentItemId, Texture* texture)
{
	mTreeCtrl->AppendItem(parentItemId, texture->GetName(), 0, 0, new TextureTreeItemData(texture));
}

TextureTreeItemData::TextureTreeItemData(Texture* texture)
:mTexture(texture)
{

}

