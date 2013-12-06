#ifndef SCENE_GRAPH_PANEL_H
#define SCENE_GRAPH_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"
#include "wx/clrpicker.h"

#include "SceneNode.h"
#include "Gizmo.h"

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

class SceneGraphTree : public wxTreeCtrl , public IGizmoEventHandler
{
public:
	SceneGraphTree(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize&size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);

	void OnItemActivated(wxTreeEvent& event);
	void OnEndLabelEdit(wxTreeEvent& event);
	void OnItemSelected(wxTreeEvent& event);

	void AttachInspectorPanel(SceneNodeInspectorPanel* inspectorPanel);

	virtual void OnSelectNode(Gizmo* gizmo);

	DECLARE_EVENT_TABLE()

private:
	wxTreeItemId findItemBySceneNode(SceneNode* sceneNode);		// 在整个tree中搜索
	wxTreeItemId findItemBySceneNode(SceneNode* sceneNode, wxTreeItemId item);	// 在item极其child中搜索

private:
	SceneNodeInspectorPanel* mInspectorPanel;
};

class TransformPanel;
class LightInfoPanel;
class MeshNodeInfoPanel;
class AttributeInfoPanel;
class SceneNodeInspectorPanel : public wxScrolledWindow, public ISceneNodeEventHandler
{
public: 
	SceneNodeInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	TransformPanel* GetTransformPanel();
	LightInfoPanel* GetLightInfoPanel();
	MeshNodeInfoPanel* GetMeshNodeInfoPanel();
	AttributeInfoPanel* GetAttributeInfoPanel();

	void AttachSceneNode(SceneNode* sceneNode);
	SceneNode* GetAttachedSceneNode();

	virtual void OnTransformChanged(SceneNode* sceneNode);

private:
	void createWxCtrls();

private:
	SceneNode* mSceneNode;
	TransformPanel* mTransformPanel;
	LightInfoPanel* mLightInfoPanel;
	MeshNodeInfoPanel* mMeshNodeInfoPanel;
	AttributeInfoPanel* mAttributeInfoPanel;
};

class TransformPanel : public wxPanel
{
	enum
	{
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

class LightInfoPanel : public wxPanel
{
	enum
	{
		ID_CHECKBOX_ENABLE,
		ID_SLIDER_INTENSITY,
		ID_LIGHT_COLOR,
	};

public:
	LightInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromSceneNode(SceneNode* sceneNode);

	void OnCheckBoxEnable(wxCommandEvent& event);
	void OnSliderIntensity(wxScrollEvent& event);
	void OnColorPick(wxColourPickerEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxCheckBox* mCheckBoxEnable;
	wxSlider* mSliderIntensity;
	wxColourPickerCtrl* mColorPicker;
};

class MeshNodeInfoPanel : public wxPanel
{
	enum
	{
		ID_BITMAP_BTN_GEO,
		ID_BITMAP_BTN_MTL,
	};

public:
	MeshNodeInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromSceneNode(SceneNode* sceneNode);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxStaticText* mTextGeoName;
	wxStaticText* mTextMtlName;
	wxBitmapButton* mBitmapBtnGeo;
	wxBitmapButton* mBitmapBtnMtl;
};

class AttributeInfoPanel : public wxPanel
{
	enum 
	{
		ID_CHECKBOX_WIREFRAME,
		ID_CHECKBOX_DRAW_BBOX,
	};

public:
	AttributeInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromSceneNode(SceneNode* sceneNode);

	void OnCheckBox(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
private:
	wxCheckBox* mCheckBoxWireFrame;
	wxCheckBox* mCheckBoxDrawBBox;
};


#endif