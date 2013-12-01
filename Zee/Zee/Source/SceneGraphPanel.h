#ifndef SCENE_GRAPH_PANEL_H
#define SCENE_GRAPH_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "SceneNode.h"

class SceneNodeTreeItemData : public wxTreeItemData
{
public:
	SceneNodeTreeItemData(SceneNode* sceneNode);

private:
	SceneNode* mSceneNode;
};

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
	wxTreeCtrl* mTreeCtrl;

	wxPanel* mInspectorPanel;

	wxImageList* mIconList;
};

#endif