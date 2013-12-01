#ifndef MATERIAL_PANEL_H
#define MATERIAL_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "Material.h"

class MaterialTreeItemData : public wxTreeItemData
{
public:
	MaterialTreeItemData(Material* material);

private:
	Material* mMaterial;
};

class MaterialPanel : public wxPanel
{
public:
	MaterialPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendMaterial(wxTreeItemId parentItemId, Material* material);

private:
	wxPanel* mTreePanel;
	wxTreeCtrl* mTreeCtrl;

	wxPanel* mInspectorPanel;

	wxImageList* mIconList;
};

#endif