#ifndef GEOMETRY_PANEL_H
#define GEOMETRY_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "Geometry.h"

class GeometryTreeItemData : public wxTreeItemData
{
public:
	GeometryTreeItemData(Geometry* geo);

private:
	Geometry* mGeometry;
};

class GeometryPanel : public wxPanel
{
public:
	GeometryPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendGeometry(wxTreeItemId parentItemId, Geometry* geo);

private:
	wxPanel* mTreePanel;
	wxTreeCtrl* mTreeCtrl;

	wxPanel* mInspectorPanel;

	wxImageList* mIconList;
};

#endif