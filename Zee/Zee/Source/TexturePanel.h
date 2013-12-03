#ifndef TEXTURE_PANEL_H
#define TEXTURE_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "Texture.h"

class TextureTreeItemData : public wxTreeItemData
{
public:
	TextureTreeItemData(Texture* texture);

private:
	Texture* mTexture;
};

class TexturePanel : public wxPanel
{
public:
	TexturePanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendTexture(wxTreeItemId parentItemId, Texture* texture);

private:
	wxPanel* mTreePanel;
	wxTreeCtrl* mTreeCtrl;

	wxPanel* mInspectorPanel;

	wxImageList* mIconList;
};

#endif