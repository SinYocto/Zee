#include "ZeeApp.h"
#include "SceneEditor.h"

#include <Locale.h>

IMPLEMENT_APP_CONSOLE(ZeeApp)

const int WND_WIDTH = 1280;
const int WND_HEIGHT = 720;

bool ZeeApp::OnInit()
{
	_wsetlocale(LC_ALL, L"chs");

	wxInitAllImageHandlers();

	SceneEditorFrame* frame = new SceneEditorFrame(L"Zee", wxPoint(0, 0), wxSize(WND_WIDTH, WND_HEIGHT));
	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

