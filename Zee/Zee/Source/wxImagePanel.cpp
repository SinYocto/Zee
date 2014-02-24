#include "wxImagePanel.h"
#include "Common.h"

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
EVT_PAINT(wxImagePanel::paintEvent)
END_EVENT_TABLE()

wxImagePanel::wxImagePanel(wxWindow* parent, wxString file, wxSize size) 
:wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
{
	mSize = size;
	SetImage(file);
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

void wxImagePanel::SetImage(wxString file)
{
	wxBitmapType imageType = determinBitmapType(file);
	wxImage originImage;
	originImage.LoadFile(file, imageType);

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

wxBitmapType wxImagePanel::determinBitmapType(wxString imageFilePath)
{
	wchar_t suffix[MAX_STR_LEN];
	YString::GetFileSuffix(suffix, _countof(suffix), imageFilePath.wc_str());

	if(YString::Compare(suffix, L"jpg", true) == 0)
	{
		return wxBITMAP_TYPE_JPEG;
	}
	else if(YString::Compare(suffix, L"tga", true) == 0)
	{
		return wxBITMAP_TYPE_TGA;
	}
	else if(YString::Compare(suffix, L"png", true) == 0)
	{
		return wxBITMAP_TYPE_PNG;
	}
	else
	{
		_Assert(false);
		return wxBITMAP_TYPE_JPEG;
	}
}
