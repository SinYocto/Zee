#ifndef WXWND_TREEGENRATOR_H
#define WXWND_TREEGENRATOR_H

#include "wx/wx.h"
#include <wx/spinctrl.h>
#include "wx/collpane.h"

class TreeGeneratorCanvas;
class ModelNode;

class TreeGeneratorFrame : public wxFrame
{
	enum
	{
		ID_GENERAL_PARAMS,
		ID_LEVEL0_PARAMS,
	};

public:
	TreeGeneratorFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);

	void Setup();
	void CleanupAndDestory();

protected:
	void OnClose(wxCloseEvent& event);

private:
	void OnCollapsPaneGeneralParams(wxCollapsiblePaneEvent& event);
	void OnCollapsPaneLevelParams0(wxCollapsiblePaneEvent& event);
	void OnSpinLevel(wxSpinEvent& event);
	//void OnSpinValue1(wxSpinEvent& event);
	//void OnSpinValue2(wxSpinEvent& event);
	//void OnTextValue3(wxCommandEvent& event);

private:
	DECLARE_EVENT_TABLE()

	wxScrolledWindow* mCtrlsPanel;

	wxCollapsiblePane* mPanelGeneralParams;
	wxTextCtrl* mTextScale;
	wxTextCtrl* mTextScaleV;
	wxTextCtrl* mTextBaseSize;
	wxTextCtrl* mTextRadiusRatio;
	wxTextCtrl* mTextRatioPower;
	wxSpinCtrl* mSpinShape;
	wxSpinCtrl* mSpinBaseSplits;

	wxCollapsiblePane* mPanelLevelParmas0;
	wxSpinCtrl* mSpinLevel;
	wxSpinCtrl* mSpinSegSegsW;
	wxSpinCtrl* mSpinSegSegsH;
	wxSpinCtrl* mSpinCurveRes;
	wxTextCtrl* mTextCurve;
	wxTextCtrl* mTextCurveV;
	wxTextCtrl* mTextCurveBack;
	wxTextCtrl* mTextSegSplits;
	wxTextCtrl* mTextSplitAngle;
	wxTextCtrl* mTextSplitAngleV;
	wxSpinCtrl* mSpinBranches;
	wxTextCtrl* mTextDownAngle;
	wxTextCtrl* mTextDownAngleV;
	wxTextCtrl* mTextRotateAngle;
	wxTextCtrl* mTextRotateAngleV;

	wxPanel* mCanvasPanel;
	TreeGeneratorCanvas* mCanvas;
};

class TreeGeneratorCanvas : public wxWindow
{
	friend class TreeGeneratorFrame;
public:
	TreeGeneratorCanvas(wxWindow* parent, 
		wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxT("TreeGenerator"));

	void Setup();

	void RenderWindow();
	void CleanupAndDestory();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnClose(wxCloseEvent& event);

private:
	DECLARE_EVENT_TABLE()

	ModelNode* mCube;
};


#endif