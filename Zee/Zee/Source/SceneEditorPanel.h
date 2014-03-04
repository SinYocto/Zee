#ifndef SCENE_EDITOR_PANEL
#define SCENE_EDITOR_PANEL

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/treectrl.h"
#include "Gizmo.h"

class Material;
class SceneNode;
class SceneEditorPanel : public wxNotebook , public IGizmoEventHandler
{
public:
	enum
	{
		SCENE_GRAPH_PAGE = 0,
		GEOMETRY_PAGE,
		MATERIAL_PAGE,
		TEXTURE_PAGE,
		MODEL_PAGE,
		//LIGHT_PAGE,
		TERRAIN_PAGE,
		PAGE_COUNTS,
	};

	SceneEditorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void OnCreateScene();
	void OnClose(wxCloseEvent& event);
	void OnPageChanged(wxNotebookEvent& event);
	void OnBitmapButton(wxCommandEvent& event);
	void OnItemActivated(wxTreeEvent& event);

	void OnAddModelNode(wxCommandEvent& event);

	virtual void OnSelectNode(Gizmo* gizmo);

	void CreateEditorPages();

	void CleanupAndDestory();

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxImageList* mIconList;

	Material* mMtlEditoring;
	int mMtlTexLayer;

	SceneNode* mSceneNodeEditoring;
};

#endif