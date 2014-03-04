 #ifndef TERRAIN_PANEL_H
#define TERRAIN_PANEL_H

#include "wx/wx.h"
#include "Terrain.h"

class TerrainInspectorPanel;
class TerrainPanel : public wxPanel
{
public:
	TerrainPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

private:
	void createWxCtrls();

private:
	TerrainInspectorPanel* mInspectorPanel;
};

class TerrainInfoPanel;
class TerrainInspectorPanel : public wxScrolledWindow
{
public: 
	TerrainInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void AttatchToTerrain(Terrain* terrain);

	Terrain* GetAttachedTerrain();

//	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void loadData();

private:
	Terrain* mTerrain;
	TerrainInfoPanel* mInfoPanel;
};

class TerrainInfoPanel : public wxPanel
{
	enum
	{
		ID_CHECKBOX_WIREFRAME,
		ID_TEXT_LOD_TOLERANCE,
	};

public: 
	TerrainInfoPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadData(Terrain* terrain);

	void OnCheckBox(wxCommandEvent& event);
	void OnTextEnter(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxCheckBox* mCheckBoxWireFrame;
	wxTextCtrl* mTextCtrlLodTolerance;
};

#endif