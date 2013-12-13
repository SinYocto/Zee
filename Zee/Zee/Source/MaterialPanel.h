#ifndef MATERIAL_PANEL_H
#define MATERIAL_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"
#include "wxImagePanel.h"
#include "wx/clrpicker.h"

#include "Material.h"

enum
{
	ID_MATERIALLIST_TREE,
};

class MaterialTreeItemData : public wxTreeItemData
{
public:
	MaterialTreeItemData(Material* material);

	Material* GetMaterial();

private:
	Material* mMaterial;
};

class MaterialListTree;
class MaterialInspectorPanel;
class MaterialPanel : public wxPanel
{
public:
	MaterialPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void appendMaterial(wxTreeItemId parentItemId, Material* material);

private:
	wxPanel* mTreePanel;
	MaterialListTree* mTreeCtrl;

	MaterialInspectorPanel* mInspectorPanel;

	wxImageList* mIconList;
};

class MaterialListTree : public wxTreeCtrl
{
public:
	MaterialListTree(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize&size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);

	void OnItemActivated(wxTreeEvent& event);
	void OnItemSelected(wxTreeEvent& event);

	void AttachInspectorPanel(MaterialInspectorPanel* inspectorPanel);

	DECLARE_EVENT_TABLE()

private:
	MaterialInspectorPanel* mInspectorPanel;
};

class MaterialInfoPanel;
class MaterialInspectorPanel : public wxScrolledWindow
{
public: 
	MaterialInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	MaterialInfoPanel* GetMaterialInfoPanel();

	void AttachMaterial(Material* mtl);
	Material* GetAttachedMaterial();

private:
	void createWxCtrls();

private:
	Material* mMaterial;
	MaterialInfoPanel* mMaterialInfoPanel;
};

class MaterialInfoPanel : public wxPanel
{
	enum 
	{
		ID_CHOICE_SHADER,
		ID_AMBIENT_COLOR,
		ID_DIFFUSE_COLOR,
		ID_SPECULAR_COLOR,
		ID_TEXT_SHINESS,
		ID_TEXT_GLOSS,
		ID_TEXT_TILES_U,
		ID_TEXT_TILES_V,
		ID_TEXT_OFFSET_U,
		ID_TEXT_OFFSET_V,
		ID_BITMAP_BTN_TEX0,
		ID_BITMAP_BTN_TEX1,
	};

public: 
	MaterialInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromMaterial(Material* mtl);

	void OnChoice(wxCommandEvent& event);
	void OnColorPick(wxColourPickerEvent& event);
	void OnTextEnter(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxChoice* mChoiceShader;

	wxColourPickerCtrl* mColorPickerAmbient;
	wxColourPickerCtrl* mColorPickerDiff;
	wxColourPickerCtrl* mColorPickerSpec;

	wxTextCtrl* mTextShiness;
	wxTextCtrl* mTextGloss;
	wxTextCtrl* mTextTilesU;
	wxTextCtrl* mTextTilesV;
	wxTextCtrl* mTextOffsetU;
	wxTextCtrl* mTextOffsetV;

	wxBitmapButton* mBitmapBtnTex0;
	wxBitmapButton* mBitmapBtnTex1;

	wxImagePanel* mTex0;
	wxImagePanel* mTex1;
};

#endif