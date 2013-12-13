#ifndef SCENE_EDITOR_PANEL
#define SCENE_EDITOR_PANEL

#include "wx/wx.h"
#include "wx/notebook.h"

class SceneEditorPanel : public wxNotebook
{
public:
	enum
	{
		SCENE_GRAPH_PAGE = 0,
		GEOMETRY_PAGE,
		MATERIAL_PAGE,
		TEXTURE_PAGE,
		MODEL_PAGE,
		LIGHT_PAGE,
		PAGE_COUNTS,
	};

	SceneEditorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void OnClose(wxCloseEvent& event);

	void OnCreateScene();

	void CreateEditorPages();

	void CleanupAndDestory();

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxImageList* mIconList;
};

#endif