#ifndef TEXTURE_PANEL_H
#define TEXTURE_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"
#include "wxImagePanel.h"

#include "Texture.h"

enum
{
	ID_TEXTURELIST_TREE,
};

class TextureTreeItemData : public wxTreeItemData
{
public:
	TextureTreeItemData(Texture* texture);

	Texture* GetTexture();

private:
	Texture* mTexture;
};

class TextureListTree;
class TextureInspectorPanel;
class TexturePanel : public wxPanel
{
public:
	TexturePanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendTexture(wxTreeItemId parentItemId, Texture* texture);

private:
	wxPanel* mTreePanel;
	TextureListTree* mTreeCtrl;

	TextureInspectorPanel* mInspectorPanel;

	wxImageList* mIconList;
};

class TextureListTree : public wxTreeCtrl
{
public:
	TextureListTree(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize&size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);

	void OnItemActivated(wxTreeEvent& event);
	void OnItemSelected(wxTreeEvent& event);

	void AttachInspectorPanel(TextureInspectorPanel* inspectorPanel);

	DECLARE_EVENT_TABLE()

private:
	TextureInspectorPanel* mInspectorPanel;
};

class TextureInfoPanel;
class TextureInspectorPanel : public wxScrolledWindow
{
public: 
	TextureInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	TextureInfoPanel* GetTextureInfoPanel();

	void AttachTexture(Texture* texture);
	//SceneNode* GetAttachedSceneNode();

	//virtual void OnTransformChanged(SceneNode* sceneNode);

private:
	void createWxCtrls();

private:
	Texture* mTexture;
	TextureInfoPanel* mTextureInfoPanel;
};

class TextureInfoPanel : public wxPanel
{
public: 
	TextureInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromTexture(Texture* texture);

	//void OnTextEnter(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxImagePanel* mImage;
};
#endif