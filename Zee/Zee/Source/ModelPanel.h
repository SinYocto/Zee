#ifndef MODEL_PANEL_H
#define MODEL_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "Model.h"

class ModelTreeItemData : public wxTreeItemData
{
public:
	ModelTreeItemData(Model* model);

private:
	Model* mModel;
};

class ModelPanel : public wxPanel
{
public:
	ModelPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendModel(wxTreeItemId parentItemId, Model* model);

private:
	wxPanel* mTreePanel;
	wxTreeCtrl* mTreeCtrl;

	wxPanel* mInspectorPanel;

	wxImageList* mIconList;
};

#endif