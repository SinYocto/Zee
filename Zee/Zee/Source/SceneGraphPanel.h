#ifndef SCENE_GRAPH_PANEL_H
#define SCENE_GRAPH_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "SceneNode.h"

enum 
{
	ID_SCENE_GRAPH_TREE,

	ID_TEXT_POSX,
	ID_TEXT_POSY,
	ID_TEXT_POSZ,
	ID_TEXT_EULERX,
	ID_TEXT_EULERY,
	ID_TEXT_EULERZ,
	ID_TEXT_SCALEX,
	ID_TEXT_SCALEY,
	ID_TEXT_SCALEZ,
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
class SceneNodeInspectorPanel;

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

	SceneNodeInspectorPanel* mInspectorPanel;

	wxImageList* mIconList;
};

class SceneGraphTree : public wxTreeCtrl
{
public:
	SceneGraphTree(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize&size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);

	void OnItemActivated(wxTreeEvent& event);
	void OnEndLabelEdit(wxTreeEvent& event);
	void OnItemSelected(wxTreeEvent& event);

	void AttachInspectorPanel(SceneNodeInspectorPanel* inspectorPanel);

	DECLARE_EVENT_TABLE()

private:
	SceneNodeInspectorPanel* mInspectorPanel;
};

class TransformPanel;
class SceneNodeInspectorPanel : public wxPanel, public ISceneNodeEventHandler
{
public: 
	SceneNodeInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	TransformPanel* GetTransformPanel();

	void AttachSceneNode(SceneNode* sceneNode);
	SceneNode* GetAttachedSceneNode();

	virtual void OnTransformChanged(SceneNode* sceneNode);

private:
	void createWxCtrls();

private:
	SceneNode* mSceneNode;
	TransformPanel* mTransformPanel;
};

class TransformPanel : public wxPanel
{
public: 
	TransformPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromSceneNode(SceneNode* sceneNode);

	void OnTextEnter(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxTextCtrl* mTextCtrlPosX;
	wxTextCtrl* mTextCtrlPosY;
	wxTextCtrl* mTextCtrlPosZ;
	wxTextCtrl* mTextCtrlEulerX;
	wxTextCtrl* mTextCtrlEulerY;
	wxTextCtrl* mTextCtrlEulerZ;
	wxTextCtrl* mTextCtrlScaleX;
	wxTextCtrl* mTextCtrlScaleY;
	wxTextCtrl* mTextCtrlScaleZ;
};


#endif