#ifndef SCENE_EDITOR_H
#define SCENE_EDITOR_H

#include "wx/wx.h"
#include "wxWndTreeGenerator.h"

class SceneEditorCanvas;

class SceneEditorFrame : public wxFrame
{
	enum 
	{
		ID_Quit = 1,
		ID_About,
		ID_TreeGenerator,
		ID_OK,
	};

public:
	SceneEditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnTreeGenerator(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();
	void cleanupAndDestory();

private:
	SceneEditorCanvas* mCanvas;
	TreeGeneratorFrame* mWndTreeGenerator;
};

class SceneEditorCanvas : public wxWindow
{
public:
	SceneEditorCanvas(wxWindow* parent, 
		wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxT("Zee"));

	void RenderLoop();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);

private:
	DECLARE_EVENT_TABLE()
};

#endif