#include "SceneGraphPanel.h"
#include "Engine.h"
#include "Camera.h"

BEGIN_EVENT_TABLE(SceneGraphPanel, wxPanel)
EVT_CLOSE(SceneGraphPanel::OnClose)
END_EVENT_TABLE()

SceneGraphPanel::SceneGraphPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								 :wxPanel(parent, id, pos, size)
{
	mIconList = new wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/empty.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/mesh.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/model.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/billboard.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/lightPage.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void SceneGraphPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);

	// treeList
	mTreePanel = new wxPanel(this, -1);
	mTreePanel->SetMinSize(wxSize(200, 300));

	wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);

	mTreeCtrl = new SceneGraphTree(mTreePanel, ID_SCENE_GRAPH_TREE, wxDefaultPosition, wxDefaultSize, 
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_EDIT_LABELS);

	mTreeCtrl->SetMinSize(wxSize(180, 260));
	mTreeCtrl->AssignImageList(mIconList);

	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = new SceneNodeInspectorPanel(this, -1);
	mInspectorPanel->SetMinSize(wxSize(200, 300));	// TODO: 暂时写死
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
	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, new SceneNodeTreeItemData(root));

	std::list<Object*> children = root->GetChildren();
	for(std::list<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		SceneNode* node = static_cast<SceneNode*>(*iter);
		appendSceneNode(rootId, node);
	}

	mTreeCtrl->Expand(rootId);
}

void SceneGraphPanel::OnClose(wxCloseEvent& event)
{
	delete mIconList;
}

void SceneGraphPanel::appendSceneNode(wxTreeItemId parentItemId, SceneNode* node)
{
	SceneNodeTreeItemData* itemData = new SceneNodeTreeItemData(node);

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
		appendSceneNode(itemId, node);
	}
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

SceneNodeInspectorPanel::SceneNodeInspectorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
			const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
			:wxPanel(parent, id, pos, size)
			,mSceneNode(NULL)
{
	createWxCtrls();
}

void SceneNodeInspectorPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = new wxBoxSizer(wxVERTICAL);

	// transform
	mTransformPanel = new TransformPanel(this, -1);

	boxSizer1->Add(mTransformPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
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
}

SceneNode* SceneNodeInspectorPanel::GetAttachedSceneNode()
{
	return mSceneNode;
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
	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0); 

	wxStaticText* textPosX = new wxStaticText(this, -1, L"X");
	mTextCtrlPosX = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textPosY = new wxStaticText(this, -1, L"Y");
	mTextCtrlPosY = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textPosZ = new wxStaticText(this, -1, L"Z");
	mTextCtrlPosZ = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textEulerX = new wxStaticText(this, -1, L"EulerX");
	mTextCtrlEulerX = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textEulerY = new wxStaticText(this, -1, L"EulerY");
	mTextCtrlEulerY = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textEulerZ = new wxStaticText(this, -1, L"EulerZ");
	mTextCtrlEulerZ = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleX = new wxStaticText(this, -1, L"ScaleX");
	mTextCtrlScaleX = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleY = new wxStaticText(this, -1, L"ScaleY");
	mTextCtrlScaleY = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
		wxTextValidator(wxFILTER_NUMERIC));

	wxStaticText* textScaleZ = new wxStaticText(this, -1, L"ScaleZ");
	mTextCtrlScaleZ = new wxTextCtrl(this, -1, L"0", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, 
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

	if(textCtrl == mTextCtrlPosX)
	{
		Vector3 pos = sceneNode->GetWorldPosition();
		sceneNode->SetWorldPosition(val, pos.y, pos.z);
	}
	else if(textCtrl == mTextCtrlPosY)
	{
		Vector3 pos = sceneNode->GetWorldPosition();
		sceneNode->SetWorldPosition(pos.x, val, pos.z);
	}
	else if(textCtrl == mTextCtrlPosZ)
	{
		Vector3 pos = sceneNode->GetWorldPosition();
		sceneNode->SetWorldPosition(pos.x, pos.y, val);
	}
	else if(textCtrl == mTextCtrlEulerX)
	{
		Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
		sceneNode->SetWorldOrientation(DEGREE_TO_RAD(val), eulerAngle.y, eulerAngle.z);
	}
	else if(textCtrl == mTextCtrlEulerY)
	{
		Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
		sceneNode->SetWorldOrientation(eulerAngle.x, DEGREE_TO_RAD(val), eulerAngle.z);
	}
	else if(textCtrl == mTextCtrlEulerZ)
	{
		Vector3 eulerAngle = sceneNode->GetWorldOrient().EulerAngle();
		sceneNode->SetWorldOrientation(eulerAngle.x, eulerAngle.y, DEGREE_TO_RAD(val));
	}
	else if(textCtrl == mTextCtrlScaleX)
	{
		Vector3 scale = sceneNode->GetScale();
		sceneNode->SetWorldOrientation(val, scale.y, scale.z);
	} 
	else if(textCtrl == mTextCtrlScaleY)
	{
		Vector3 scale = sceneNode->GetScale();
		sceneNode->SetWorldOrientation(scale.x, val, scale.z);
	}
	else if(textCtrl == mTextCtrlScaleZ)
	{
		Vector3 scale = sceneNode->GetScale();
		sceneNode->SetWorldOrientation(scale.x, scale.y, val);
	}
}
