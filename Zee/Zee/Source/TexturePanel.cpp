#include "TexturePanel.h"
#include "Engine.h"

BEGIN_EVENT_TABLE(TexturePanel, wxPanel)
END_EVENT_TABLE()

TexturePanel::TexturePanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
					   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
					   :wxPanel(parent, id, pos, size)
{
	mIconList = New wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/texturePage.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void TexturePanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	// listCtrl
	mTreePanel = New wxPanel(this, -1);
	mTreePanel->SetMinSize(wxSize(200, 300));

	wxBoxSizer* boxSizer2 = New wxBoxSizer(wxVERTICAL);

	mTreeCtrl = New TextureListTree(mTreePanel, ID_TEXTURELIST_TREE, wxDefaultPosition, wxDefaultSize, 
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_HIDE_ROOT);

	mTreeCtrl->SetMinSize(wxSize(180, 260));
	mTreeCtrl->AssignImageList(mIconList);


	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = New TextureInspectorPanel(this, -1);
	mInspectorPanel->SetMinSize(wxSize(200, 300));
	mTreeCtrl->AttachInspectorPanel(mInspectorPanel);

	boxSizer1->Add(mInspectorPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void TexturePanel::LoadDataFromScene()
{
	TextureManager* textureMgr = gEngine->GetTextureManger();
	TexHashMap texHashMap = textureMgr->GetTextureHashMap();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, New TextureTreeItemData(NULL));

	for(TexHashMap::iterator iter = texHashMap.begin(); iter != texHashMap.end(); ++iter)
	{
		appendTexture(rootId, iter->second);
	}
}

void TexturePanel::appendTexture(wxTreeItemId parentItemId, Texture* texture)
{
	mTreeCtrl->AppendItem(parentItemId, texture->GetName(), 0, 0, New TextureTreeItemData(texture));
}

TextureTreeItemData::TextureTreeItemData(Texture* texture)
:mTexture(texture)
{

}

Texture* TextureTreeItemData::GetTexture()
{
	return mTexture;
}


BEGIN_EVENT_TABLE(TextureListTree, wxTreeCtrl)
EVT_TREE_ITEM_ACTIVATED (ID_TEXTURELIST_TREE, TextureListTree::OnItemActivated)
EVT_TREE_SEL_CHANGED(ID_TEXTURELIST_TREE, TextureListTree::OnItemSelected)
END_EVENT_TABLE()

TextureListTree::TextureListTree( wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
							   const wxSize&size /*= wxDefaultSize*/, long style /*= wxTR_DEFAULT_STYLE*/ )
							   :wxTreeCtrl(parent, id, pos, size, style)
{

}

void TextureListTree::OnItemActivated(wxTreeEvent& event)
{
	TextureTreeItemData* itemData = (TextureTreeItemData*)GetItemData(event.GetItem());

	Texture* texture = itemData->GetTexture();

	event.SetString(L"TexturePanel");
	event.SetClientData((void*)texture);
	event.Skip();
}

void TextureListTree::OnItemSelected(wxTreeEvent& event)
{
	TextureTreeItemData* itemData = (TextureTreeItemData*)GetItemData(event.GetItem());
	Texture* texture = itemData->GetTexture();

	mInspectorPanel->AttachTexture(texture);
	mInspectorPanel->GetTextureInfoPanel()->LoadDataFromTexture(texture);
}

void TextureListTree::AttachInspectorPanel(TextureInspectorPanel* inspectorPanel)
{
	mInspectorPanel = inspectorPanel;
}

wxTreeItemId TextureListTree::findItemByTexture(Texture* texture)
{
	return findItemByTexture(texture, GetRootItem());
}

wxTreeItemId TextureListTree::findItemByTexture(Texture* texture, wxTreeItemId itemId)
{
	wxTreeItemId rootId = GetRootItem();
	wxTreeItemId resultItemId = rootId;
	wxTreeItemId curItemId = itemId;

	TextureTreeItemData* curItemData = (TextureTreeItemData*)GetItemData(curItemId);

	if(curItemData->GetTexture() == texture)
	{
		resultItemId = curItemId;
		return resultItemId;
	}

	wxTreeItemIdValue cookie;
	curItemId = GetFirstChild(itemId, cookie);
	while(curItemId.IsOk())
	{
		resultItemId = findItemByTexture(texture, curItemId);

		if(resultItemId != rootId)
		{
			return resultItemId;
		}

		curItemId = GetNextChild(itemId, cookie);
	}

	return resultItemId;
}


TextureInspectorPanel::TextureInspectorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
											 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
											 :wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
											 ,mTexture(NULL)
{
	createWxCtrls();
}

void TextureInspectorPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mTextureInfoPanel = New TextureInfoPanel(this, wxID_ANY);

	boxSizer1->Add(mTextureInfoPanel, 0, wxALL, 5);

	this->SetScrollRate(0, 5);
	this->SetSizer(boxSizer1);
	this->FitInside();
	this->Layout();
}

TextureInfoPanel* TextureInspectorPanel::GetTextureInfoPanel()
{
	return mTextureInfoPanel;
}

void TextureInspectorPanel::AttachTexture(Texture* texture)
{
	mTexture = texture;
}

BEGIN_EVENT_TABLE(TextureInfoPanel, wxPanel)
END_EVENT_TABLE()

TextureInfoPanel::TextureInfoPanel( wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/ )
								   :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void TextureInfoPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mImage = New wxImagePanel(this, wxT("./Assets/Textures/black128x128.jpg"), wxSize(128, 128));

	boxSizer1->Add(mImage, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void TextureInfoPanel::LoadDataFromTexture(Texture* texture)
{
	if(!texture)
		return;

	mImage->SetImage(texture->GetFilePath());
}
