#include "wxWndTreeGenerator.h"
#include "D3DUtility.h"
#include "DebugDrawer.h"
#include "Camera.h"

BEGIN_EVENT_TABLE(TreeGeneratorFrame, wxFrame)
EVT_CLOSE(TreeGeneratorFrame::OnClose)
END_EVENT_TABLE()

TreeGeneratorFrame::TreeGeneratorFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size):
wxFrame(parent, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT)
{
	// ctrls
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	mCtrlsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	mValue1 = new wxStaticText( mCtrlsPanel, wxID_ANY, wxT("value1"), wxDefaultPosition, wxDefaultSize, 0 );
	mValue1->Wrap( -1 );
	fgSizer3->Add( mValue1, 0, wxALL, 5 );

	mSpin1 = new wxSpinCtrl( mCtrlsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer3->Add( mSpin1, 0, wxALL, 5 );

	mValue2 = new wxStaticText( mCtrlsPanel, wxID_ANY, wxT("value2"), wxDefaultPosition, wxDefaultSize, 0 );
	mValue2->Wrap( -1 );
	fgSizer3->Add( mValue2, 0, wxALL, 5 );

	mSpin2 = new wxSpinCtrl( mCtrlsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer3->Add( mSpin2, 0, wxALL, 5 );

	mCtrlsPanel->SetSizer( fgSizer3 );
	mCtrlsPanel->Layout();
	fgSizer3->Fit( mCtrlsPanel );
	bSizer4->Add( mCtrlsPanel, 0, wxALL|wxEXPAND, 5 );

	mCanvasPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer4->Add( mCanvasPanel, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizer4 );
	this->Layout();

	this->Centre( wxBOTH );

	wxSize clientSize = mCanvasPanel->GetClientSize();

	mCanvas = new TreeGeneratorCanvas(mCanvasPanel, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);

	D3DPRESENT_PARAMETERS presentParams = Driver::GetPresentParameters();
	presentParams.BackBufferWidth = clientSize.x;
	presentParams.BackBufferHeight = clientSize.y;

	Driver::CreateSecondarySwapChain(presentParams);
}

void TreeGeneratorFrame::CleanupAndDestory()
{
	mCanvas->CleanupAndDestory();
	mCtrlsPanel->Destroy();
	mCanvasPanel->Destroy();
	Destroy();
}

void TreeGeneratorFrame::OnClose(wxCloseEvent& event)
{
	Show(false);		// 点右上角x时, 不关闭, 隐藏
}

void TreeGeneratorFrame::OnSpinValue1( wxSpinEvent& event )
{
	// ...
}

void TreeGeneratorFrame::OnSpinValue2( wxSpinEvent& event )
{
	// ...
}

BEGIN_EVENT_TABLE(TreeGeneratorCanvas, wxWindow)
EVT_SIZE(TreeGeneratorCanvas::OnSize)
EVT_IDLE(TreeGeneratorCanvas::OnIdle)
END_EVENT_TABLE()

TreeGeneratorCanvas::TreeGeneratorCanvas(wxWindow* parent, wxWindowID id /* = wxID_ANY */, 
					   const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, 
					   long style /* = 0 */, const wxString& name /* = wxT */)
					   :wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{

}

void TreeGeneratorCanvas::OnSize(wxSizeEvent& event)
{
	// ...
}


void TreeGeneratorCanvas::OnIdle(wxIdleEvent& event)
{
	if(Driver::D3DDevice)
		RenderWindow();

	event.RequestMore(true);
}
 
void TreeGeneratorCanvas::RenderWindow()
{
	if(Driver::D3DDevice->TestCooperativeLevel() == D3D_OK)
	{
		Camera* camera = new Camera(Vector3(0, 0, -2));

		Driver::RenderToSwapChain(SECONDARY_SWAPCHAIN);
		Driver::Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
		Driver::BeginScene();

		std::vector<Vector3> points;
		points.push_back(Vector3::Zero);
		points.push_back(Vector3(2, 0, 0));
		DebugDrawer::DrawLine(points, 0xffff0000, camera);

		Driver::EndScene();
		Driver::Present((HWND)GetHWND());
	}
}

void TreeGeneratorCanvas::OnClose(wxCloseEvent& event)
{
	CleanupAndDestory();
}

void TreeGeneratorCanvas::CleanupAndDestory()
{
	Destroy();
}
