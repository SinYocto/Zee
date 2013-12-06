#ifndef WX_IMAGE_PANEL_H
#define WX_IMAGE_PANEL_H

#include "wx/wx.h"

class wxImagePanel : public wxPanel
{
public:
	wxImagePanel(wxWindow* parent, wxString file, wxBitmapType format, wxSize size);

	void SetImage(wxString file, wxBitmapType format);

private:
	void paintEvent(wxPaintEvent & evt);
	void paintNow();

	void render(wxDC& dc);

	DECLARE_EVENT_TABLE()

private:
	wxBitmap mImage;
	wxSize mSize;
};
#endif