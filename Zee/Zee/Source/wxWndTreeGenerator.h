#ifndef WXWND_TREEGENRATOR_H
#define WXWND_TREEGENRATOR_H

#include "wx/wx.h"
#include <wx/spinctrl.h>
#include "wx/collpane.h"

#include "Tree.h"

class TreeGeneratorCanvas;
class ModelNode;

class TreeGeneratorFrame : public wxFrame
{
	enum
	{
		ID_MENU_OPEN,
		ID_MENU_SAVE,
		ID_GENERAL_PARAMS,
		ID_LEVEL0_PARAMS,
		ID_BUTTON_GENERATE,
	};

public:
	TreeGeneratorFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);

	void Setup();
	void CleanupAndDestory();

protected:
	void OnClose(wxCloseEvent& event);
	void OnOpenFile(wxCommandEvent& event);

private:
	void OnCollapsPaneGeneralParams(wxCollapsiblePaneEvent& event);
	void OnCollapsPaneLevelParams0(wxCollapsiblePaneEvent& event);
	void OnButtonGenerate(wxCommandEvent& event);
	void OnSpinLevels(wxSpinEvent& event);
	void OnChoiceLevel(wxCommandEvent& event);

	void createWxCtrls();

	void setValueGeneralParams(TreeGeneralParams generalParams);
	void setValueLevelParams(TreeLevelParams levelParams);

	void getValueGeneralParams(TreeGeneralParams* generalParams);
	void getValueLevelParams(TreeLevelParams* levelParams);

	void refreshChoiceLevel();

private:
	DECLARE_EVENT_TABLE()
	wxScrolledWindow* mCtrlsPanel;

	wxCollapsiblePane* mPanelGeneralParams;
	wxSpinCtrl* mSpinLevels;
	wxTextCtrl* mTextScale;
	wxTextCtrl* mTextScaleV;
	wxTextCtrl* mTextBaseSize;
	wxTextCtrl* mTextRadiusRatio;
	wxTextCtrl* mTextRatioPower;
	wxSpinCtrl* mSpinShape;
	wxSpinCtrl* mSpinBaseSplits;

	wxCollapsiblePane* mPanelLevelParmas0;

	wxChoice* mChoiceLevel;
	int mSelectedLevel;

	wxSpinCtrl* mSpinLevel;
	wxSpinCtrl* mSpinSegSegsW;
	wxSpinCtrl* mSpinSegSegsH;

	wxTextCtrl* mTextLength;
	wxTextCtrl* mTextLengthV;
	wxTextCtrl* mTextTaper;

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

	wxButton* mButtonGenerate;

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

	void OnLostDevice();
	void OnResetDevice();

	void SetSwapChainIndex(int index);
	void Setup();

	void LoadParamsFromFile(const wchar_t* filePath);

	void RenderWindow();
	void CleanupAndDestory();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnClose(wxCloseEvent& event);

private:
	DECLARE_EVENT_TABLE()

	ModelNode* mCube;
	Tree* mTree;

	int swapChainIndex;
};


#endif