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
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES);

	mTreeCtrl->SetMinSize(wxSize(180, 260));
	mTreeCtrl->AssignImageList(mIconList);

	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = new wxPanel(this, -1);
	mInspectorPanel->SetMinSize(wxSize(200, 300));	// TODO: ÔÝÊ±Ð´ËÀ

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
