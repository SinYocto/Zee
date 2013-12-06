#include "wxImagePanel.h"

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
EVT_PAINT(wxImagePanel::paintEvent)
END_EVENT_TABLE()

wxImagePanel::wxImagePanel(wxWindow* parent, wxString file, wxBitmapType format, wxSize size) 
:wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
{
	mSize = size;
	SetImage(file, format);
}

void wxImagePanel::paintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void wxImagePanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void wxImagePanel::render(wxDC&  dc)
{
	dc.DrawBitmap(mImage, 0, 0, false);
}

void wxImagePanel::SetImage(wxString file, wxBitmapType format)
{
	wxImage originImage;
	originImage.LoadFile(file, format);

	int imageW = originImage.GetWidth();
	int imageH = originImage.GetHeight();

	float scale = 1.0f;
	if(imageW > mSize.x)
	{
		scale = (float)mSize.x / imageW;
	}

	if(scale * imageH > mSize.y)
	{
		scale *= (float)mSize.y / (scale * imageH);
	}

	originImage.Rescale(scale * imageW, scale * imageH);

	mImage = wxBitmap(originImage);

	Refresh();
}
