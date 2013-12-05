#ifndef SCENE_EDITOR_PANEL
#define SCENE_EDITOR_PANEL

#include "wx/wx.h"
#include "wx/notebook.h"

class SceneEditorPanel : public wxNotebook
{
public:
	SceneEditorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void OnCreateScene();

	void CreateEditorPages();

	void CleanupAndDestory();

private:
	void createWxCtrls();

private:
	wxImageList* mIconList;
};

#endif