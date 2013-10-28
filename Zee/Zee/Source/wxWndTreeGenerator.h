#ifndef WXWND_TREEGENRATOR_H
#define WXWND_TREEGENRATOR_H

#include "wx/wx.h"
#include <wx/spinctrl.h>

class TreeGeneratorCanvas;

class TreeGeneratorFrame : public wxFrame
{
public:
	TreeGeneratorFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size);

	void CleanupAndDestory();

protected:
	void OnClose(wxCloseEvent& event);

private:
	void OnSpinValue1(wxSpinEvent& event);
	void OnSpinValue2(wxSpinEvent& event);

private:
	DECLARE_EVENT_TABLE()

	wxPanel* mCtrlsPanel;
	wxPanel* mCanvasPanel;

	wxStaticText* mValue1;
	wxSpinCtrl* mSpin1;
	wxStaticText* mValue2;
	wxSpinCtrl* mSpin2;

	TreeGeneratorCanvas* mCanvas;
};

class TreeGeneratorCanvas : public wxWindow
{
public:
	TreeGeneratorCanvas(wxWindow* parent, 
		wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxT("TreeGenerator"));

	void RenderWindow();
	void CleanupAndDestory();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnClose(wxCloseEvent& event);

private:
	DECLARE_EVENT_TABLE()
};


#endif