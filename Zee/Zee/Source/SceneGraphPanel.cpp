#include "SceneGraphPanel.h"
#include "Engine.h"
#include "Camera.h"
#include "DirectionalLightNode.h"
#include "PointLightNode.h"
#include "MeshNode.h"

BEGIN_EVENT_TABLE(SceneGraphPanel, wxPanel)
END_EVENT_TABLE()

SceneGraphPanel::SceneGraphPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								 :wxPanel(parent, id, pos, size)
{
	mIconList = New wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/empty.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/mesh.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/model.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/billboard.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/lightPage.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void SceneGraphPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	// treeList
	mTreePanel = New wxPanel(this, -1);
	mTreePanel->SetMinSize(wxSize(200, 250));

	wxBoxSizer* boxSizer2 = New wxBoxSizer(wxVERTICAL);

	mTreeCtrl = New SceneGraphTree(mTreePanel, ID_SCENE_GRAPH_TREE, wxDefaultPosition, wxDefaultSize, 
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_EDIT_LABELS);

	mTreeCtrl->SetMinSize(wxSize(170, 240));
	mTreeCtrl->AssignImageList(mIconList);

	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = New SceneNodeInspectorPanel(this, -1);
	mInspectorPanel->SetMinSize(wxSize(180, 240));	// TODO: 暂时写死
	mTreeCtrl->AttachInspectorPanel(mInspectorPanel);
 
	boxSizer1->Add(mInspectorPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void SceneGraphPanel::LoadDataFromScene()
{
	SceneManager* sceneMgr = gEngine->GetSceneManager();
	
	SceneNode* root = sceneMgr->GetRoot();
	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, New SceneNodeTreeItemData(root));

	std::list<Object*> children = root->GetChildren();
	for(std::list<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		AppendSceneNode(rootId, node);
	}

	mTreeCtrl->Expand(rootId);
}

void SceneGraphPanel::AppendSceneNode(wxTreeItemId parentItemId, SceneNode* node)
{
	SceneNodeTreeItemData* itemData = New SceneNodeTreeItemData(node);

	int sceneNodeIcon = 0;

	SceneNode::NODE_TYPE nodeType = node->GetNodeType();
	switch(nodeType)
	{
	case SceneNode::SCENE_NODE_NULL:
		sceneNodeIcon = 0;
		break;

	case SceneNode::SCENE_NODE_MESH:
		sceneNodeIcon = 1;
		break;

	case SceneNode::SCENE_NODE_MODEL:
		sceneNodeIcon = 2;
		break;

	case SceneNode::SCENE_NODE_BILLBOARD:
		sceneNodeIcon = 3;
		break;

	case SceneNode::SCENE_NODE_DIR_LIGHT:
	case SceneNode::SCENE_NODE_POINT_LIGHT:
		sceneNodeIcon = 4;
		break;

	default:
		_Assert(false);
	}

	wxTreeItemId itemId = mTreeCtrl->AppendItem(parentItemId, node->GetName(), sceneNodeIcon, sceneNodeIcon, itemData);

	std::list<Object*> children = node->GetChildren();
	for(std::list<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		AppendSceneNode(itemId, node);
	}
}

void SceneGraphPanel::AppendSceneNode(SceneNode* node)
{
	SceneNodeTreeItemData* itemData = New SceneNodeTreeItemData(node);

	int sceneNodeIcon = 0;

	SceneNode::NODE_TYPE nodeType = node->GetNodeType();
	switch(nodeType)
	{
	case SceneNode::SCENE_NODE_NULL:
		sceneNodeIcon = 0;
		break;

	case SceneNode::SCENE_NODE_MESH:
		sceneNodeIcon = 1;
		break;

	case SceneNode::SCENE_NODE_MODEL:
		sceneNodeIcon = 2;
		break;

	case SceneNode::SCENE_NODE_BILLBOARD:
		sceneNodeIcon = 3;
		break;

	case SceneNode::SCENE_NODE_DIR_LIGHT:
	case SceneNode::SCENE_NODE_POINT_LIGHT:
		sceneNodeIcon = 4;
		break;

	default:
		_Assert(false);
	}

	wxTreeItemId itemId = mTreeCtrl->AppendItem(mTreeCtrl->GetRootItem(), node->GetName(), sceneNodeIcon, sceneNodeIcon, itemData);

	std::list<Object*> children = node->GetChildren();
	for(std::list<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		AppendSceneNode(itemId, node);
	}
}

void SceneGraphPanel::RefreshInspector()
{
	if(mInspectorPanel)
		mInspectorPanel->RefreshInspector();
}



SceneNodeTreeItemData::SceneNodeTreeItemData(SceneNode* sceneNode)
:mSceneNode(sceneNode)
{
	
}

SceneNode* SceneNodeTreeItemData::GetSceneNode()
{
	return mSceneNode;
}


BEGIN_EVENT_TABLE(SceneGraphTree, wxTreeCtrl)
EVT_TREE_ITEM_ACTIVATED (ID_SCENE_GRAPH_TREE, SceneGraphTree::OnItemActivated)
EVT_TREE_END_LABEL_EDIT(ID_SCENE_GRAPH_TREE, SceneGraphTree::OnEndLabelEdit)
EVT_TREE_SEL_CHANGED(ID_SCENE_GRAPH_TREE, SceneGraphTree::OnItemSelected)
END_EVENT_TABLE()

SceneGraphTree::SceneGraphTree( wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
							   const wxSize&size /*= wxDefaultSize*/, long style /*= wxTR_DEFAULT_STYLE*/ )
							   :wxTreeCtrl(parent, id, pos, size, style)
{
	gEngine->GetGizmo()->RegisterEventHanlder(this);
}

void SceneGraphTree::OnItemActivated(wxTreeEvent& event)
{
	SceneNodeTreeItemData* itemData = (SceneNodeTreeItemData*)GetItemData(event.GetItem());

	SceneNode* sceneNode = itemData->GetSceneNode();

	Camera* mainCamera = gEngine->GetSceneManager()->GetMainCamera();
	mainCamera->FocusAt(sceneNode);
}

void SceneGraphTree::OnItemSelected(wxTreeEvent& event)
{
	SceneNodeTreeItemData* itemData = (SceneNodeTreeItemData*)GetItemData(event.GetItem());
	SceneNode* sceneNode = itemData->GetSceneNode();

	gEngine->GetGizmo()->SelectSceneNode(sceneNode);

	mInspectorPanel->AttachSceneNode(sceneNode);
	mInspectorPanel->GetTransformPanel()->LoadDataFromSceneNode(sceneNode);
	mInspectorPanel->GetLightInfoPanel()->LoadDataFromSceneNode(sceneNode);
	mInspectorPanel->GetMeshNodeInfoPanel()->LoadDataFromSceneNode(sceneNode);
	mInspectorPanel->GetAttributeInfoPanel()->LoadDataFromSceneNode(sceneNode);
}

void SceneGraphTree::OnEndLabelEdit(wxTreeEvent& event)
{
	SceneNodeTreeItemData* itemData = (SceneNodeTreeItemData*)GetItemData(event.GetItem());
	SceneNode* sceneNode = itemData->GetSceneNode();

	sceneNode->SetName(event.GetLabel().wchar_str());
}

void SceneGraphTree::AttachInspectorPanel(SceneNodeInspectorPanel* inspectorPanel)
{
	mInspectorPanel = inspectorPanel;
}

void SceneGraphTree::OnSelectNode(Gizmo* gizmo)
{
	wxTreeItemId itemId = findItemBySceneNode(gizmo->GetSelectedNode());

	if(itemId != GetRootItem())
	{
		SelectItem(itemId);
	}
}

wxTreeItemId SceneGraphTree::findItemBySceneNode(SceneNode* sceneNode)
{
	return findItemBySceneNode(sceneNode, GetRootItem());
}

wxTreeItemId SceneGraphTree::findItemBySceneNode(SceneNode* sceneNode, wxTreeItemId itemId)
{
	wxTreeItemId rootId = GetRootItem();
	wxTreeItemId resultItemId = rootId;
	wxTreeItemId curItemId = itemId;

	SceneNodeTreeItemData* curItemData = (SceneNodeTreeItemData*)GetItemData(curItemId);

	if(curItemData->GetSceneNode() == sceneNode)
	{
		resultItemId = curItemId;
		return resultItemId;
	}

	wxTreeItemIdValue cookie;
	curItemId = GetFirstChild(itemId, cookie);
	while(curItemId.IsOk())
	{
		resultItemId = findItemBySceneNode(sceneNode, curItemId);

		if(resultItemId != rootId)
		{
			return resultItemId;
		}

		curItemId = GetNextChild(itemId, cookie);
	}

	return resultItemId;
}

SceneNodeInspectorPanel::SceneNodeInspectorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
			const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
			:wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
			,mSceneNode(NULL)
{
	createWxCtrls();
}

void SceneNodeInspectorPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	// transform
	mTransformPanel = New TransformPanel(this, wxID_ANY);
	mMeshNodeInfoPanel = New MeshNodeInfoPanel(this, wxID_ANY);
	mLightInfoPanel = New LightInfoPanel(this, wxID_ANY);
	mAttributeInfoPanel = New AttributeInfoPanel(this, wxID_ANY);

	boxSizer1->Add(mTransformPanel, 0, wxALL, 5);
	boxSizer1->Add(mMeshNodeInfoPanel, 0, wxALL, 5);
	boxSizer1->Add(mLightInfoPanel, 0, wxALL, 5);
	boxSizer1->Add(mAttributeInfoPanel, 0, wxALL, 5);

	this->SetScrollRate(0, 5);
	this->SetSizer(boxSizer1);
	this->FitInside();
	this->Layout();
}

TransformPanel* SceneNodeInspectorPanel::GetTransformPanel()
{
	return mTransformPanel;
}

void SceneNodeInspectorPanel::OnTransformChanged(SceneNode* sceneNode)
{
	mTransformPanel->LoadDataFromSceneNode(sceneNode);
}

void SceneNodeInspectorPanel::AttachSceneNode(SceneNode* sceneNode)
{
	if(mSceneNode)
		mSceneNode->UnRegisterEventHandler(this);	// TODO:此panel的析构?(需要ungister)

	mSceneNode = sceneNode;
	mSceneNode->RegisterEventHanlder(this);

	mMeshNodeInfoPanel->Hide();
	mLightInfoPanel->Hide();
	mAttributeInfoPanel->Hide();

	SceneNode::NODE_TYPE type = sceneNode->GetNodeType();
	switch(type)
	{
	case SceneNode::SCENE_NODE_DIR_LIGHT:
	case SceneNode::SCENE_NODE_POINT_LIGHT:
		mLightInfoPanel->Show();
		break;

	case SceneNode::SCENE_NODE_MESH:
		mMeshNodeInfoPanel->Show();
		mAttributeInfoPanel->Show();
		break;

	case SceneNode::SCENE_NODE_MODEL:
		mAttributeInfoPanel->Show();
		break;

	default:
		break;
	}

	this->Layout();
	this->Fit();
}

SceneNode* SceneNodeInspectorPanel::GetAttachedSceneNode()
{
	return mSceneNode;
}

LightInfoPanel* SceneNodeInspectorPanel::GetLightInfoPanel()
{
	return mLightInfoPanel;
}

MeshNodeInfoPanel* SceneNodeInspectorPanel::GetMeshNodeInfoPanel()
{
	return mMeshNodeInfoPanel;
}

AttributeInfoPanel* SceneNodeInspectorPanel::GetAttributeInfoPanel()
{
	return mAttributeInfoPanel;
}

void SceneNodeInspectorPanel::RefreshInspector()
{
	if(mMeshNodeInfoPanel && mSceneNode)
		mMeshNodeInfoPanel->LoadDataFromSceneNode(mSceneNode);
}

BEGIN_EVENT_TABLE(TransformPanel, wxPanel)
EVT_TEXT_ENTER(wxID_ANY, TransformPanel::OnTextEnter)
END_EVENT_TABLE()

TransformPanel::TransformPanel( wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
							   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/ )
							   :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void TransformPanel::createWxCtrls()
{
	wxFlexGridSizer* fgSizer1 = New wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textPosX = New wxStaticText(this, -1, L"X");
	mTextCtrlPosX = New wxTextCtrl(this, ID_TEXT_POSX, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textPosY = New wxStaticText(this, -1, L"Y");
	mTextCtrlPosY = New wxTextCtrl(this, ID_TEXT_POSY, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textPosZ = New wxStaticText(this, -1, L"Z");
	mTextCtrlPosZ = New wxTextCtrl(this, ID_TEXT_POSZ, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textEulerX = New wxStaticText(this, -1, L"EulerX");
	mTextCtrlEulerX = New wxTextCtrl(this, ID_TEXT_EULERX, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textEulerY = New wxStaticText(this, -1, L"EulerY");
	mTextCtrlEulerY = New wxTextCtrl(this, ID_TEXT_EULERY, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textEulerZ = New wxStaticText(this, -1, L"EulerZ");
	mTextCtrlEulerZ = New wxTextCtrl(this, ID_TEXT_EULERZ, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleX = New wxStaticText(this, -1, L"ScaleX");
	mTextCtrlScaleX = New wxTextCtrl(this, ID_TEXT_SCALEX, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleY = New wxStaticText(this, -1, L"ScaleY");
	mTextCtrlScaleY = New wxTextCtrl(this, ID_TEXT_SCALEY, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleZ = New wxStaticText(this, -1, L"ScaleZ");
	mTextCtrlScaleZ = New wxTextCtrl(this, ID_TEXT_SCALEZ, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	fgSizer1->Add(textPosX, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlPosX, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textPosY, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlPosY, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textPosZ, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlPosZ, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textEulerX, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlEulerX, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textEulerY, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlEulerY, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textEulerZ, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlEulerZ, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textScaleX, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlScaleX, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textScaleY, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlScaleY, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textScaleZ, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mTextCtrlScaleZ, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	this->SetSizer(fgSizer1);
	this->Layout();
	this->Fit();
}

void TransformPanel::LoadDataFromSceneNode(SceneNode* sceneNode)
{
	if(!sceneNode)
		return;

	Vector3 pos = sceneNode->GetWorldPosition();
	Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
	Vector3 scale = sceneNode->GetScale();

	wxString posX = wxString::Format(wxT("%.2f"), pos.x);
	wxString posY = wxString::Format(wxT("%.2f"), pos.y);
	wxString posZ = wxString::Format(wxT("%.2f"), pos.z);

	wxString eulerX = wxString::Format(wxT("%.2f"), RAD_TO_DEGREE(eulerAngle.x));
	wxString eulerY = wxString::Format(wxT("%.2f"), RAD_TO_DEGREE(eulerAngle.y));
	wxString eulerZ = wxString::Format(wxT("%.2f"), RAD_TO_DEGREE(eulerAngle.z));

	wxString scaleX = wxString::Format(wxT("%.2f"), scale.x);
	wxString scaleY = wxString::Format(wxT("%.2f"), scale.y);
	wxString scaleZ = wxString::Format(wxT("%.2f"), scale.z);

	mTextCtrlPosX->SetValue(posX);
	mTextCtrlPosY->SetValue(posY);
	mTextCtrlPosZ->SetValue(posZ);

	mTextCtrlEulerX->SetValue(eulerX);
	mTextCtrlEulerY->SetValue(eulerY);
	mTextCtrlEulerZ->SetValue(eulerZ);

	mTextCtrlScaleX->SetValue(scaleX);
	mTextCtrlScaleY->SetValue(scaleY);
	mTextCtrlScaleZ->SetValue(scaleZ);
}

void TransformPanel::OnTextEnter(wxCommandEvent& event)
{
	wxTextCtrl* textCtrl = (wxTextCtrl*)event.GetEventObject();

	double val = 0;
	textCtrl->GetValue().ToDouble(&val);

	SceneNodeInspectorPanel* inspectorPanel = (SceneNodeInspectorPanel*)GetParent();
	SceneNode* sceneNode = inspectorPanel->GetAttachedSceneNode();

	if(!sceneNode)
		return;

	switch(textCtrl->GetId())
	{
	case ID_TEXT_POSX:
		{
			Vector3 pos = sceneNode->GetWorldPosition();
			sceneNode->SetWorldPosition(val, pos.y, pos.z);
			break;
		}

	case ID_TEXT_POSY:
		{
			Vector3 pos = sceneNode->GetWorldPosition();
			sceneNode->SetWorldPosition(pos.x, val, pos.z);
			break;
		}

	case ID_TEXT_POSZ:
		{
			Vector3 pos = sceneNode->GetWorldPosition();
			sceneNode->SetWorldPosition(pos.x, pos.y, val);
			break;
		}

	case ID_TEXT_EULERX:
		{
			Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
			sceneNode->SetWorldOrientation(DEGREE_TO_RAD(val), eulerAngle.y, eulerAngle.z);
			break;
		}

	case ID_TEXT_EULERY:		
		{
			Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
			sceneNode->SetWorldOrientation(eulerAngle.x, DEGREE_TO_RAD(val), eulerAngle.z);
			break;
		}

	case ID_TEXT_EULERZ:		
		{
			Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
			sceneNode->SetWorldOrientation(eulerAngle.x, eulerAngle.y, DEGREE_TO_RAD(val));
			break;
		}

	case ID_TEXT_SCALEX:		
		{
			Vector3 scale = sceneNode->GetScale();
			sceneNode->SetScale(val, scale.y, scale.z);
			break;
		}

	case ID_TEXT_SCALEY:	
		{
			Vector3 scale = sceneNode->GetScale();
			sceneNode->SetScale(scale.x, val, scale.z);
			break;
		}

	case ID_TEXT_SCALEZ:		
		{
			Vector3 scale = sceneNode->GetScale();
			sceneNode->SetScale(scale.x, scale.y, val);
			break;
		}

	default:
		break;
	}
}

BEGIN_EVENT_TABLE(LightInfoPanel, wxPanel)
EVT_CHECKBOX(ID_CHECKBOX_ENABLE, LightInfoPanel::OnCheckBoxEnable)
EVT_COMMAND_SCROLL(ID_SLIDER_INTENSITY, LightInfoPanel::OnSliderIntensity)
EVT_COLOURPICKER_CHANGED(ID_LIGHT_COLOR, LightInfoPanel::OnColorPick)
END_EVENT_TABLE()
LightInfoPanel::LightInfoPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
							   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
							   :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void LightInfoPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mCheckBoxEnable = New wxCheckBox(this, ID_CHECKBOX_ENABLE, wxT("Enable"));

	wxFlexGridSizer* fgSizer1 = New wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textIntensity = New wxStaticText(this, -1, L"Intensity");
	mSliderIntensity = New wxSlider(this, ID_SLIDER_INTENSITY, 100, 0, 200, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);

	wxStaticText* textColor = New wxStaticText(this, -1, L"Color");
	mColorPicker = New wxColourPickerCtrl(this, ID_LIGHT_COLOR, *wxWHITE);

	fgSizer1->Add(textIntensity, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mSliderIntensity, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textColor, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mColorPicker, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	boxSizer1->Add(mCheckBoxEnable, 0, wxALL, 5);
	boxSizer1->Add(fgSizer1, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void LightInfoPanel::LoadDataFromSceneNode(SceneNode* sceneNode)
{
	SceneNode::NODE_TYPE type = sceneNode->GetNodeType();

	if(type == SceneNode::SCENE_NODE_DIR_LIGHT)
	{
		DirectionalLightNode* lightNode = static_cast<DirectionalLightNode*>(sceneNode);
		DirectionalLight* dirLight = lightNode->GetDirLight();

		mCheckBoxEnable->SetValue(dirLight->IsEnabled());
		mSliderIntensity->SetValue(dirLight->GetIntensity() * 100);

		D3DXCOLOR color = dirLight->GetColor();
		mColorPicker->SetColour(wxColour(255 * color.r, 255 * color.g, 255 * color.b));
	}
	else if(type == SceneNode::SCENE_NODE_POINT_LIGHT)
	{
		PointLightNode* lightNode = static_cast<PointLightNode*>(sceneNode);
		PointLight* pointLight = lightNode->GetPointLight();

		mCheckBoxEnable->SetValue(pointLight->IsEnabled());
		mSliderIntensity->SetValue(pointLight->GetIntensity() * 100);

		D3DXCOLOR color = pointLight->GetColor();
		mColorPicker->SetColour(wxColour(255 * color.r, 255 * color.g, 255 * color.b));
	}
}

void LightInfoPanel::OnCheckBoxEnable(wxCommandEvent& event)
{
	SceneNodeInspectorPanel* inspectorPanel = (SceneNodeInspectorPanel*)GetParent();
	SceneNode* sceneNode = inspectorPanel->GetAttachedSceneNode();

	bool enable = mCheckBoxEnable->GetValue();

	if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_DIR_LIGHT)
	{
		DirectionalLightNode* lightNode = static_cast<DirectionalLightNode*>(sceneNode);
		lightNode->GetDirLight()->Enable(enable);
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_POINT_LIGHT)
	{
		PointLightNode* lightNode = static_cast<PointLightNode*>(sceneNode);
		lightNode->GetPointLight()->Enable(enable);
	}
}

void LightInfoPanel::OnSliderIntensity(wxScrollEvent& event)
{
	SceneNodeInspectorPanel* inspectorPanel = (SceneNodeInspectorPanel*)GetParent();
	SceneNode* sceneNode = inspectorPanel->GetAttachedSceneNode();

	float intensity = 2 * (float)mSliderIntensity->GetValue() / (float)mSliderIntensity->GetMax();

	if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_DIR_LIGHT)
	{
		DirectionalLightNode* lightNode = static_cast<DirectionalLightNode*>(sceneNode);
		lightNode->SetLightIntensity(intensity);
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_POINT_LIGHT)
	{
		PointLightNode* lightNode = static_cast<PointLightNode*>(sceneNode);
		lightNode->SetLightIntensity(intensity);
	}
}

void LightInfoPanel::OnColorPick(wxColourPickerEvent& event)
{
	SceneNodeInspectorPanel* inspectorPanel = (SceneNodeInspectorPanel*)GetParent();
	SceneNode* sceneNode = inspectorPanel->GetAttachedSceneNode();

	wxColour color = mColorPicker->GetColour();
	D3DCOLOR d3dColor = D3DCOLOR_XRGB(color.Red(), color.Green(), color.Blue());

	if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_DIR_LIGHT)
	{
		DirectionalLightNode* lightNode = static_cast<DirectionalLightNode*>(sceneNode);
		lightNode->SetLightColor(d3dColor);
	}
	else if(sceneNode->GetNodeType() == SceneNode::SCENE_NODE_POINT_LIGHT)
	{
		PointLightNode* lightNode = static_cast<PointLightNode*>(sceneNode);
		lightNode->SetLightColor(d3dColor);
	}
}

BEGIN_EVENT_TABLE(MeshNodeInfoPanel, wxPanel)
EVT_BUTTON(wxID_ANY, MeshNodeInfoPanel::OnBitmapButton)
END_EVENT_TABLE()
MeshNodeInfoPanel::MeshNodeInfoPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
									 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
									 :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void MeshNodeInfoPanel::createWxCtrls()
{
	wxFlexGridSizer* fgSizer1 = New wxFlexGridSizer(4, 4, 0, 0); 

	// geo
	wxStaticText* textGeo = New wxStaticText(this, wxID_ANY, L"Geometry");
	wxStaticBitmap* bitmapGeo = New wxStaticBitmap(this, wxID_ANY, 
		wxBitmap(L"./Assets/Icons/geometry8x8.ico", wxBITMAP_TYPE_ICO));

	mTextGeoName = New wxStaticText(this, wxID_ANY, L"geoName");
	mBitmapBtnGeo = New wxBitmapButton(this, ID_BITMAP_BTN_GEO, wxBitmap(L"./Assets/Icons/settings8x8.ico", wxBITMAP_TYPE_ICO));

	// mtl
	wxStaticText* textMtl = New wxStaticText(this, wxID_ANY, L"Material");
	wxStaticBitmap* bitmapMtl = New wxStaticBitmap(this, wxID_ANY, 
		wxBitmap(L"./Assets/Icons/material8x8.ico", wxBITMAP_TYPE_ICO));

	mTextMtlName = New wxStaticText(this, wxID_ANY, L"mtlName");
	mBitmapBtnMtl = New wxBitmapButton(this, ID_BITMAP_BTN_MTL, wxBitmap(L"./Assets/Icons/settings8x8.ico", wxBITMAP_TYPE_ICO));

	fgSizer1->Add(textGeo, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(bitmapGeo, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(mTextGeoName, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mBitmapBtnGeo, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(textMtl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(bitmapMtl, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	fgSizer1->Add(mTextMtlName, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
	fgSizer1->Add(mBitmapBtnMtl, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 2);

	this->SetSizer(fgSizer1);
	this->Layout();
	this->Fit();
}

void MeshNodeInfoPanel::LoadDataFromSceneNode(SceneNode* sceneNode)
{
	SceneNode::NODE_TYPE type = sceneNode->GetNodeType();

	if(type == SceneNode::SCENE_NODE_MESH)
	{
		MeshNode* meshNode = static_cast<MeshNode*>(sceneNode);
		Mesh* mesh = meshNode->GetMesh();

		mTextGeoName->SetLabel(mesh->GetGeometry()->GetName());
		mTextMtlName->SetLabel(mesh->GetMaterial()->GetName());
	}
}

void MeshNodeInfoPanel::OnBitmapButton(wxCommandEvent& event)
{
	SceneNodeInspectorPanel* inspectorPanel = (SceneNodeInspectorPanel*)GetParent();
	SceneNode* sceneNode = inspectorPanel->GetAttachedSceneNode();

	if(!sceneNode)
		return;

	wxBitmapButton* btnCtrl = (wxBitmapButton*)event.GetEventObject();

	switch(btnCtrl->GetId())
	{
	case ID_BITMAP_BTN_GEO:
		event.SetString(L"SceneGraphPanel_Mesh_Geo");
		break;

	case ID_BITMAP_BTN_MTL:
		event.SetString(L"SceneGraphPanel_Mesh_Mtl");
		break;

	default:
		_Assert(false);
	}

	event.SetClientData((void*)sceneNode);
	event.Skip();
}

BEGIN_EVENT_TABLE(AttributeInfoPanel, wxPanel)
EVT_CHECKBOX(wxID_ANY, AttributeInfoPanel::OnCheckBox)
END_EVENT_TABLE()
AttributeInfoPanel::AttributeInfoPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
									   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
									   :wxPanel(parent, id, pos, size)
{
	createWxCtrls();
}

void AttributeInfoPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	mCheckBoxWireFrame = New wxCheckBox(this, ID_CHECKBOX_WIREFRAME, wxT("WireFrame"));
	mCheckBoxDrawBBox = New wxCheckBox(this, ID_CHECKBOX_DRAW_BBOX, wxT("DrawBBox"));

	boxSizer1->Add(mCheckBoxWireFrame, 0, wxALL, 5);
	boxSizer1->Add(mCheckBoxDrawBBox, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void AttributeInfoPanel::LoadDataFromSceneNode(SceneNode* sceneNode)
{
	if(!sceneNode)
		return;

	if(sceneNode->GetDisplayMode() == SceneNode::WIRE_FRAME)
	{
		mCheckBoxWireFrame->SetValue(true);
	}
	else
	{
		mCheckBoxWireFrame->SetValue(false);
	}

	mCheckBoxDrawBBox->SetValue(sceneNode->GetDrawBBoxFlag());
}

void AttributeInfoPanel::OnCheckBox(wxCommandEvent& event)
{
	SceneNodeInspectorPanel* inspectorPanel = (SceneNodeInspectorPanel*)GetParent();
	SceneNode* sceneNode = inspectorPanel->GetAttachedSceneNode();

	wxCheckBox* checkBox = (wxCheckBox*)event.GetEventObject();
	bool val = checkBox->GetValue();

	switch(checkBox->GetId())
	{
	case ID_CHECKBOX_WIREFRAME:
		if(val)
			sceneNode->SetDisplayMode(SceneNode::WIRE_FRAME);
		else
			sceneNode->SetDisplayMode(SceneNode::SOLID);

		break;

	case ID_CHECKBOX_DRAW_BBOX:
		sceneNode->SetDrawBBoxFlag(val);
		break;

	default:
		break;
	}
}
