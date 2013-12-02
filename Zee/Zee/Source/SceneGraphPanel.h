#ifndef SCENE_GRAPH_PANEL_H
#define SCENE_GRAPH_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "SceneNode.h"

enum 
{
	ID_SCENE_GRAPH_TREE,
};

class SceneNodeTreeItemData : public wxTreeItemData
{
public:
	SceneNodeTreeItemData(SceneNode* sceneNode);

	SceneNode* GetSceneNode();

private:
	SceneNode* mSceneNode;
};

class SceneGraphTree;

class SceneGraphPanel : public wxPanel
{
public:
	SceneGraphPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendSceneNode(wxTreeItemId parentItemId, SceneNode* node);

private:
	wxPanel* mTreePanel;
	SceneGraphTree* mTreeCtrl;

	wxPanel* mInspectorPanel;

	wxImageList* mIconList;
};

class SceneGraphTree : public wxTreeCtrl
{
public:
	SceneGraphTree(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize&size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);

	void OnItemActivated(wxTreeEvent& event);

	DECLARE_EVENT_TABLE()
};


#endif