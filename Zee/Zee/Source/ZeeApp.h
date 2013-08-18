#ifndef ZEEAPP_H
#define ZEEAPP_H

#include "wx/wx.h"

class D3D9Canvas;

class ZeeApp : public wxApp
{
public:
	bool OnInit();
};

class ZeeFrame : public wxFrame
{
public:
	ZeeFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	D3D9Canvas* mCanvas;
};

class D3D9Canvas : public wxWindow
{
public:
	D3D9Canvas(wxWindow* parent, 
		wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxT("Zee"));

	void InitDriver();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);

private:
	DECLARE_EVENT_TABLE()
};

#endif