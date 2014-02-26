#include "MaterialPanel.h"
#include "Engine.h"

BEGIN_EVENT_TABLE(MaterialPanel, wxPanel)
END_EVENT_TABLE()

MaterialPanel::MaterialPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
					   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
					   :wxPanel(parent, id, pos, size)
{
	mIconList = New wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/billboard.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void MaterialPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	// listCtrl
	mTreePanel = New wxPanel(this, -1);
	mTreePanel->SetMinSize(wxSize(200, 250));

	wxBoxSizer* boxSizer2 = New wxBoxSizer(wxVERTICAL);

	mTreeCtrl = New MaterialListTree(mTreePanel, ID_MATERIALLIST_TREE, wxDefaultPosition, wxDefaultSize, 
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS);

	mTreeCtrl->SetMinSize(wxSize(170, 240));
	mTreeCtrl->AssignImageList(mIconList);

	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = New MaterialInspectorPanel(this, wxID_ANY);
	mInspectorPanel->SetMinSize(wxSize(180, 240));
	mTreeCtrl->AttachInspectorPanel(mInspectorPanel);

	boxSizer1->Add(mInspectorPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void MaterialPanel::LoadDataFromScene()
{
	MaterialManager* materialMgr = gEngine->GetMaterialManager();
	std::vector<Material*> mtlList = materialMgr->GetAllMaterials();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, New MaterialTreeItemData(NULL));

	for(std::vector<Material*>::iterator iter = mtlList.begin(); iter != mtlList.end(); ++iter)
	{
		AppendMaterial(rootId, *iter);
	}
}

void MaterialPanel::AppendMaterial(wxTreeItemId parentItemId, Material* material)
{
	mTreeCtrl->AppendItem(parentItemId, material->GetName(), 0, 0, New MaterialTreeItemData(material));
}

void MaterialPanel::AppendMaterial(Material* material)
{
	mTreeCtrl->AppendItem(mTreeCtrl->GetRootItem(), material->GetName(), 0, 0, New MaterialTreeItemData(material));
}

void MaterialPanel::RefreshInspector()
{
	if(mInspectorPanel)
		mInspectorPanel->RefreshInspector();
}

MaterialTreeItemData::MaterialTreeItemData(Material* material)
:mMaterial(material)
{

}

Material* MaterialTreeItemData::GetMaterial()
{
	return mMaterial;
}


BEGIN_EVENT_TABLE(MaterialListTree, wxTreeCtrl)
	EVT_TREE_END_LABEL_EDIT(ID_MATERIALLIST_TREE, MaterialListTree::OnEndLabelEdit)
	EVT_TREE_ITEM_ACTIVATED (ID_MATERIALLIST_TREE, MaterialListTree::OnItemActivated)
	EVT_TREE_SEL_CHANGED(ID_MATERIALLIST_TREE, MaterialListTree::OnItemSelected)
	EVT_CONTEXT_MENU(MaterialListTree::OnContextMenu)
	EVT_MENU(ID_MENU_ADD_NEW_MATERIAL, MaterialListTree::OnContextMenuAddNewMaterial)
END_EVENT_TABLE()

MaterialListTree::MaterialListTree(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
								 const wxSize&size /*= wxDefaultSize*/, long style /*= wxTR_DEFAULT_STYLE*/ )
								 :wxTreeCtrl(parent, id, pos, size, style)
{

}

void MaterialListTree::AttachInspectorPanel(MaterialInspectorPanel* inspectorPanel)
{
	mInspectorPanel = inspectorPanel;
}

void MaterialListTree::OnItemActivated(wxTreeEvent& event)
{
	MaterialTreeItemData* itemData = (MaterialTreeItemData*)GetItemData(event.GetItem());

	Material* mtl = itemData->GetMaterial();

	event.SetString(L"MaterialPanel");
	event.SetClientData((void*)mtl);
	event.Skip();
}

void MaterialListTree::OnItemSelected(wxTreeEvent& event)
{
	MaterialTreeItemData* itemData = (MaterialTreeItemData*)GetItemData(event.GetItem());
	Material* mtl = itemData->GetMaterial();

	mInspectorPanel->AttachMaterial(mtl);
	mInspectorPanel->GetMaterialInfoPanel()->LoadDataFromMaterial(mtl);
}

void MaterialListTree::OnEndLabelEdit(wxTreeEvent& event)
{
	MaterialTreeItemData* itemData = (MaterialTreeItemData*)GetItemData(event.GetItem());
	Material* mtl = itemData->GetMaterial();

	mtl->SetName(event.GetLabel().wchar_str());
}

void MaterialListTree::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint pt = event.GetPosition();
	wxPoint clientPt = ScreenToClient(pt);

	wxMenu menu;  
	menu.Append(ID_MENU_ADD_NEW_MATERIAL, wxT("add new material"));

	PopupMenu(&menu, clientPt);
}

void MaterialListTree::OnContextMenuAddNewMaterial(wxCommandEvent& event)
{
	MaterialManager* materialMgr = gEngine->GetMaterialManager();

	Material* material = new Material(L"newMtl");
	material->SetShader(Diffuse);
	materialMgr->AddMaterial(material);

	MaterialPanel* materialPanel = (MaterialPanel*)(GetParent()->GetParent());
	materialPanel->AppendMaterial(material);
}

MaterialInspectorPanel::MaterialInspectorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
											 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
											 :wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
											 ,mMaterial(NULL)
{
	createWxCtrls();
}

void MaterialInspectorPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mMaterialInfoPanel = New MaterialInfoPanel(this, wxID_ANY);

	boxSizer1->Add(mMaterialInfoPanel, 0, wxALL, 5);

	this->SetScrollRate(0, 5);
	this->SetSizer(boxSizer1);
	this->FitInside();
	this->Layout();
}

MaterialInfoPanel* MaterialInspectorPanel::GetMaterialInfoPanel()
{
	return mMaterialInfoPanel;
}

void MaterialInspectorPanel::AttachMaterial(Material* mtl)
{
	mMaterial = mtl;
}

Material* MaterialInspectorPanel::GetAttachedMaterial()
{
	return mMaterial;
}

void MaterialInspectorPanel::RefreshInspector()
{	
	if(mMaterial)
		mMaterialInfoPanel->LoadDataFromMaterial(mMaterial);
}

BEGIN_EVENT_TABLE(MaterialInfoPanel, wxPanel)
EVT_CHOICE(wxID_ANY, MaterialInfoPanel::OnChoice)
EVT_COLOURPICKER_CHANGED(wxID_ANY, MaterialInfoPanel::OnColorPick)
EVT_TEXT_ENTER(wxID_ANY, MaterialInfoPanel::OnTextEnter)
EVT_BUTTON(wxID_ANY, MaterialInfoPanel::OnBitmapButton)
END_EVENT_TABLE()
MaterialInfoPanel::MaterialInfoPanel( wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/ )
								   :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void MaterialInfoPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer1 = New wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textShderType = New wxStaticText(this, wxID_ANY, L"Shader");

	wxArrayString shaderNameStrs;
	shaderNameStrs.Add(L"Flat");
	shaderNameStrs.Add(L"View");
	shaderNameStrs.Add(L"Diffuse");
	shaderNameStrs.Add(L"Specular");
	shaderNameStrs.Add(L"BumpSpecular");

	mChoiceShader = New wxChoice(this, ID_CHOICE_SHADER, wxDefaultPosition, wxDefaultSize, shaderNameStrs);

	wxStaticText* textAmbientColor = New wxStaticText(this, wxID_ANY, L"Ambient");
	mColorPickerAmbient = New wxColourPickerCtrl(this, ID_AMBIENT_COLOR, *wxWHITE);

	wxStaticText* textDifffuseColor = New wxStaticText(this, wxID_ANY, L"Diffuse");
	mColorPickerDiff = New wxColourPickerCtrl(this, ID_DIFFUSE_COLOR, *wxWHITE);

	wxStaticText* textSpecularColor = New wxStaticText(this, wxID_ANY, L"Specular");
	mColorPickerSpec = New wxColourPickerCtrl(this, ID_SPECULAR_COLOR, *wxWHITE);

	wxStaticText* textShiness = New wxStaticText(this, wxID_ANY, L"Shiness");
	mTextShiness = New wxTextCtrl(this, ID_TEXT_SHINESS, L"1", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textGloss = New wxStaticText(this, wxID_ANY, L"Gloss");
	mTextGloss= New wxTextCtrl(this, ID_TEXT_GLOSS, L"50", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textTilesU = New wxStaticText(this, wxID_ANY, L"TilesU");
	mTextTilesU= New wxTextCtrl(this, ID_TEXT_TILES_U, L"1", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textTilesV = New wxStaticText(this, wxID_ANY, L"TilesV");
	mTextTilesV= New wxTextCtrl(this, ID_TEXT_TILES_V, L"1", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textOffsetU = New wxStaticText(this, wxID_ANY, L"OffsetU");
	mTextOffsetU= New wxTextCtrl(this, ID_TEXT_OFFSET_U, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textOffsetV = New wxStaticText(this, wxID_ANY, L"OffsetV");
	mTextOffsetV= New wxTextCtrl(this, ID_TEXT_OFFSET_V, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	fgSizer1->Add(textShderType, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mChoiceShader, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textAmbientColor, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mColorPickerAmbient, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textDifffuseColor, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mColorPickerDiff, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textSpecularColor, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mColorPickerSpec, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textShiness, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextShiness, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textGloss, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextGloss, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textTilesU, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextTilesU, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textTilesV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextTilesV, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textOffsetU, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextOffsetU, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textOffsetV, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextOffsetV, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	wxFlexGridSizer* fgSizer2 = New wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textTex0 = New wxStaticText(this, wxID_ANY, L"Tex0");
	mBitmapBtnTex0 = New wxBitmapButton(this, ID_BITMAP_BTN_TEX0, wxBitmap(L"./Assets/Icons/settings8x8.ico", wxBITMAP_TYPE_ICO));

	fgSizer2->Add(textTex0, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer2->Add(mBitmapBtnTex0, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	wxFlexGridSizer* fgSizer3 = New wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textTex1 = New wxStaticText(this, wxID_ANY, L"Tex1");
	mBitmapBtnTex1 = New wxBitmapButton(this, ID_BITMAP_BTN_TEX1, wxBitmap(L"./Assets/Icons/settings8x8.ico", wxBITMAP_TYPE_ICO));

	fgSizer3->Add(textTex1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer3->Add(mBitmapBtnTex1, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	mTex0 = New wxImagePanel(this, wxT("./Assets/Textures/black128x128.jpg"), wxSize(128, 128));
	mTex1 = New wxImagePanel(this, wxT("./Assets/Textures/black128x128.jpg"), wxSize(128, 128));


	boxSizer1->Add(fgSizer1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	boxSizer1->Add(fgSizer2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	boxSizer1->Add(mTex0, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	boxSizer1->Add(fgSizer3, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	boxSizer1->Add(mTex1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void MaterialInfoPanel::LoadDataFromMaterial(Material* mtl)
{
	if(!mtl)
		return;

	ShadingMethod method = mtl->GetShadingMethod();
	switch(method)
	{
	case Flat:
	case View:
	case Diffuse:
		mChoiceShader->SetSelection(method);
		mColorPickerSpec->Disable();
		mTextShiness->Disable();
		mTextGloss->Disable();
		break;

	case Specular:
	case BumpSpecular:
		mChoiceShader->SetSelection(method);
		mColorPickerSpec->Enable();
		mTextShiness->Enable();
		mTextGloss->Enable();
		break;

	default:
		_Assert(false);
	}

	D3DXCOLOR ambientColor = mtl->GetAmbientColor();
	D3DXCOLOR diffuseColor = mtl->GetDiffuseColor();
	D3DXCOLOR specColor = mtl->GetSpecularColor();

	mColorPickerAmbient->SetColour(wxColour(255 * ambientColor.r, 255 * ambientColor.g, 255 * ambientColor.b));
	mColorPickerDiff->SetColour(wxColour(255 * diffuseColor.r, 255 * diffuseColor.g, 255 * diffuseColor.b));
	mColorPickerSpec->SetColour(wxColour(255 * specColor.r, 255 * specColor.g, 255 * specColor.b));

	wxString shiness = wxString::Format(wxT("%.2f"), mtl->GetSpecShiness());
	wxString gloss = wxString::Format(wxT("%.2f"), mtl->GetSpecGloss());
	wxString tilesU = wxString::Format(wxT("%.2f"), mtl->GetTilesU());
	wxString tilesV = wxString::Format(wxT("%.2f"), mtl->GetTilesV());
	wxString offsetU = wxString::Format(wxT("%.2f"), mtl->GetOffsetU());
	wxString offsetV = wxString::Format(wxT("%.2f"), mtl->GetOffsetV());

	mTextShiness->SetValue(shiness);
	mTextGloss->SetValue(gloss);
	mTextTilesU->SetValue(tilesU);
	mTextTilesV->SetValue(tilesV);
	mTextOffsetU->SetValue(offsetU);
	mTextOffsetV->SetValue(offsetV);

	int texLayerCounts = mtl->GetTextureLayerCounts();
	mTex0->Hide();
	mTex1->Hide();

	if(texLayerCounts > 0)
	{
		mTex0->SetImage(mtl->GetTexture(0)->GetFilePath());
		mTex0->Show();
	}

	if(texLayerCounts > 1)
	{
		mTex1->SetImage(mtl->GetTexture(1)->GetFilePath());
		mTex1->Show();
	}

	this->Layout();
}

void MaterialInfoPanel::OnChoice(wxCommandEvent& event)
{
	MaterialInspectorPanel* inspectorPanel = (MaterialInspectorPanel*)GetParent();
	Material* mtl = inspectorPanel->GetAttachedMaterial();

	if(!mtl)
		return;

	wxChoice* choiceCtrl = (wxChoice*)event.GetEventObject();

	switch(choiceCtrl->GetId())
	{
	case ID_CHOICE_SHADER:
		{
			ShadingMethod method = (ShadingMethod)choiceCtrl->GetSelection();
			mtl->SetShader(method);
			break;
		}
	default:
		_Assert(false);
	}
}

void MaterialInfoPanel::OnColorPick( wxColourPickerEvent& event )
{
	MaterialInspectorPanel* inspectorPanel = (MaterialInspectorPanel*)GetParent();
	Material* mtl = inspectorPanel->GetAttachedMaterial();

	if(!mtl)
		return;

	wxColourPickerCtrl* colorCtrl = (wxColourPickerCtrl*)event.GetEventObject();
	wxColour color = colorCtrl->GetColour();
	D3DXCOLOR d3dxColor(D3DCOLOR_XRGB(color.Red(), color.Green(), color.Blue()));

	switch(colorCtrl->GetId())
	{
	case ID_AMBIENT_COLOR:
		mtl->SetAmbientColor(d3dxColor);
		break;

	case ID_DIFFUSE_COLOR:
		mtl->SetDiffuseColor(d3dxColor);
		break;

	case ID_SPECULAR_COLOR:
		mtl->SetSpecularColor(d3dxColor);
		break;

	default:
		_Assert(false);
	}
}

void MaterialInfoPanel::OnTextEnter( wxCommandEvent& event )
{
	MaterialInspectorPanel* inspectorPanel = (MaterialInspectorPanel*)GetParent();
	Material* mtl = inspectorPanel->GetAttachedMaterial();

	if(!mtl)
		return;

	wxTextCtrl* textCtrl = (wxTextCtrl*)event.GetEventObject();

	double val = 0;
	textCtrl->GetValue().ToDouble(&val);

	switch(textCtrl->GetId())
	{
	case ID_TEXT_SHINESS:
		mtl->SetSpecShiness(val);
		break;

	case ID_TEXT_GLOSS:
		mtl->SetSpecGloss(val);
		break;

	case ID_TEXT_TILES_U:
		mtl->SetTilesU(val);
		break;

	case ID_TEXT_TILES_V:
		mtl->SetTilesV(val);
		break;

	case ID_TEXT_OFFSET_U:
		mtl->SetOffsetU(val);
		break;

	case ID_TEXT_OFFSET_V:
		mtl->SetOffsetV(val);
		break;

	default:
		_Assert(false);
	}
}

void MaterialInfoPanel::OnBitmapButton(wxCommandEvent& event)
{
	MaterialInspectorPanel* inspectorPanel = (MaterialInspectorPanel*)GetParent();
	Material* mtl = inspectorPanel->GetAttachedMaterial();

	if(!mtl)
		return;

	wxBitmapButton* btnCtrl = (wxBitmapButton*)event.GetEventObject();

	switch(btnCtrl->GetId())
	{
	case ID_BITMAP_BTN_TEX0:
		event.SetInt(0);		// ´«texture layerË÷Òý
		break;

	case ID_BITMAP_BTN_TEX1:
		event.SetInt(1);
		break;

	default:
		_Assert(false);
	}

	event.SetString(L"MaterialPanel_Tex");
	event.SetClientData((void*)mtl);
	event.Skip();
}
