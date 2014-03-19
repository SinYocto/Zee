#include "ModelPanel.h"
#include "Engine.h"
#include "Camera.h"
#include "CameraController.h"
#include "OBJParser.h"
#include "ModelNode.h"

BEGIN_EVENT_TABLE(ModelPanel, wxPanel)
END_EVENT_TABLE()

ModelPanel::ModelPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								 const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								 :wxPanel(parent, id, pos, size)
{
	mIconList = New wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/model.ico", wxBITMAP_TYPE_ICO));

	createWxCtrls();
}

void ModelPanel::createWxCtrls()
{
	wxBoxSizer* boxSizer1 = New wxBoxSizer(wxVERTICAL);

	// listCtrl
	mTreePanel = New wxPanel(this, -1);
	mTreePanel->SetMinSize(wxSize(200, 300));

	wxBoxSizer* boxSizer2 = New wxBoxSizer(wxVERTICAL);

	mTreeCtrl = New ModelListTree(mTreePanel, ID_MODELLIST_TREE, wxDefaultPosition, wxDefaultSize, 
		wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_HIDE_ROOT);

	mTreeCtrl->SetMinSize(wxSize(180, 260));
	mTreeCtrl->AssignImageList(mIconList);

	boxSizer2->Add(mTreeCtrl, 0, wxALL, 5);

	mTreePanel->SetSizer(boxSizer2);
	mTreePanel->Layout();
	mTreePanel->Fit();

	boxSizer1->Add(mTreePanel, 0, wxALL, 5);

	// inspector
	mInspectorPanel = New ModelInspectorPanel(this, -1);
	mInspectorPanel->SetMinSize(wxSize(200, 300));
	mTreeCtrl->AttachInspectorPanel(mInspectorPanel);

	boxSizer1->Add(mInspectorPanel, 0, wxALL, 5);

	this->SetSizer(boxSizer1);
	this->Layout();
	this->Fit();
}

void ModelPanel::CleanUp()
{
	if(mInspectorPanel)
		mInspectorPanel->CleanUp();
}

void ModelPanel::LoadDataFromScene()
{
	ModelManager* modelMgr = gEngine->GetModelManager();
	std::list<Model*> modelList = modelMgr->GetModelList();

	wxTreeItemId rootId = mTreeCtrl->AddRoot(L"root", 0, 0, New ModelTreeItemData(NULL));

	for(std::list<Model*>::iterator iter = modelList.begin(); iter != modelList.end(); ++iter)
	{
		Model* model = *iter;
		appendModel(rootId, model);
	}
}

void ModelPanel::appendModel(wxTreeItemId parentItemId, Model* model)
{
	mTreeCtrl->AppendItem(parentItemId, model->GetName(), 0, 0, New ModelTreeItemData(model));
}

ModelTreeItemData::ModelTreeItemData(Model* model)
:mModel(model)
{

}

Model* ModelTreeItemData::GetModel()
{
	return mModel;
}

BEGIN_EVENT_TABLE(ModelListTree, wxTreeCtrl)
EVT_TREE_SEL_CHANGED(ID_MODELLIST_TREE, ModelListTree::OnItemSelected)
EVT_TREE_BEGIN_DRAG(ID_MODELLIST_TREE, ModelListTree::OnBeginDrag)
EVT_TREE_END_DRAG(ID_MODELLIST_TREE, ModelListTree::OnEndDrag)
EVT_CONTEXT_MENU(ModelListTree::OnContextMenu)
EVT_TREE_ITEM_MENU(ID_MODELLIST_TREE, ModelListTree::OnItemMenu)
EVT_MENU(ID_MODEL_PANEL_CONTEXT_MENU_IMPORT, ModelListTree::OnContextMenuImport)
EVT_MENU(ID_MODEL_PANEL_ITEM_MENU_ADD_TO_SCENE, ModelListTree::OnItemMenuAddToScene)
END_EVENT_TABLE()
ModelListTree::ModelListTree( wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
							 const wxSize&size /*= wxDefaultSize*/, long style /*= wxTR_DEFAULT_STYLE*/ )
							 :wxTreeCtrl(parent, id, pos, size, style)
{

}

void ModelListTree::AttachInspectorPanel(ModelInspectorPanel* inspectorPanel)
{
	mInspectorPanel = inspectorPanel;
}

void ModelListTree::OnItemSelected(wxTreeEvent& event)
{
	ModelTreeItemData* itemData = (ModelTreeItemData*)GetItemData(event.GetItem());
	Model* model = itemData->GetModel();

	mInspectorPanel->AttachModel(model);
	mInspectorPanel->GetModelPreviewCanvas()->SetPreviewModel(model);
	mInspectorPanel->GetModelPreviewCanvas()->AddjustCameraPos();
}

void ModelListTree::OnBeginDrag(wxTreeEvent& event)
{
	ModelTreeItemData* itemData = (ModelTreeItemData*)GetItemData(event.GetItem());
	Model* model = itemData->GetModel();

	//event.Allow();

}

void ModelListTree::OnEndDrag( wxTreeEvent& event )
{
	mInspectorPanel;

}

void ModelListTree::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint pt = event.GetPosition();
	wxPoint clientPt = ScreenToClient(pt);

	wxMenu menu;  
	menu.Append(ID_MODEL_PANEL_CONTEXT_MENU_IMPORT, wxT("&import"));

	PopupMenu(&menu, clientPt);
}

void ModelListTree::OnItemMenu(wxTreeEvent& event)
{
	ModelTreeItemData* itemData = (ModelTreeItemData*)GetItemData(event.GetItem());
	Model* model = itemData->GetModel();

	wxPoint pt = event.GetPoint();

	wxMenu menu;
	menu.Append(ID_MODEL_PANEL_ITEM_MENU_ADD_TO_SCENE, wxT("&add to scene"));

	PopupMenu(&menu, pt);
}

void ModelListTree::OnContextMenuImport(wxCommandEvent& event)
{
	wxFileDialog dialog(this, L"Import", wxEmptyString, wxEmptyString, _T("OBJ files (*.obj*)|*.obj*"), wxFD_MULTIPLE);
	if (dialog.ShowModal() == wxID_OK)
	{
		Model* model = NULL;
		OBJParser::Parse(dialog.GetPath().wchar_str(), &model);

		_Assert(NULL != model);
		AppendItem(GetRootItem(), model->GetName(), 0, 0, New ModelTreeItemData(model));
	}
}

void ModelListTree::OnItemMenuAddToScene(wxCommandEvent& event)
{
	ModelTreeItemData* itemData = (ModelTreeItemData*)GetItemData(GetSelection());
	Model* model = itemData->GetModel();

	event.SetClientData((void*)model);
	event.Skip();
}

BEGIN_EVENT_TABLE(ModelInspectorPanel, wxScrolledWindow)
END_EVENT_TABLE()
ModelInspectorPanel::ModelInspectorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
											   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
											   :wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
											   ,mModel(NULL)
{
	createWxCtrls();
}

void ModelInspectorPanel::AttachModel(Model* model)
{
	mModel = model;
}

ModelPreviewCanvas* ModelInspectorPanel::GetModelPreviewCanvas()
{
	return mModelPreviewCanvas;
}

void ModelInspectorPanel::createWxCtrls()
{
	mModelPreviewCanvas = New ModelPreviewCanvas(this, wxID_ANY, wxDefaultPosition, wxSize(160, 180), wxSUNKEN_BORDER);
	mModelPreviewCanvas->SetMinSize(wxSize(160, 180));

	this->SetScrollRate(0, 5);
	this->FitInside();
	this->Layout();
}

void ModelInspectorPanel::CleanUp()
{
	if(mModelPreviewCanvas)
		mModelPreviewCanvas->CleanUp();
}


BEGIN_EVENT_TABLE(ModelPreviewCanvas, wxWindow)
EVT_IDLE(ModelPreviewCanvas::OnIdle)
END_EVENT_TABLE()
ModelPreviewCanvas::ModelPreviewCanvas(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, 
									   const wxSize& size /*= wxDefaultSize*/, long style /*= 0*/)
									   :wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE)
									   ,mModel(NULL)
{
	D3DPRESENT_PARAMETERS presentParams = gEngine->GetDriver()->GetPresentParameters(0);
	presentParams.BackBufferWidth = 160;
	presentParams.BackBufferHeight = 180;

	int swapChainIndex = gEngine->GetDriver()->CreateAdditionalSwapChain(presentParams);
	SetSwapChainIndex(swapChainIndex);

	D3DVIEWPORT9 viewPort = gEngine->GetDriver()->GetViewPort(mSwapChainIndex);

	mCamera = New Camera(Vector3(0, 0, -10.0f), Vector3::Zero,
		PI/3, (float)viewPort.Width / (float)viewPort.Height, 0.1f, 1000.0f);
}

void ModelPreviewCanvas::OnIdle( wxIdleEvent& event )
{
	if(gEngine->GetDriver()->GetD3DDevice())
		RenderWindow();

	event.RequestMore(true);
}

void ModelPreviewCanvas::RenderWindow()
{
	if(!IsShownOnScreen())
		return;

	Driver* driver = gEngine->GetDriver();
	IDirect3DDevice9* d3dDevice = driver->GetD3DDevice();

	mCamera->FrameUpdate();

	if(wxWindow::FindFocus() == this)
		mCamera->ApplyCameraController();

	driver->RenderToSwapChain(mSwapChainIndex);
	driver->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
	driver->BeginScene();

	std::vector<Vector3> points;
	points.push_back(Vector3::Zero);
	points.push_back(Vector3(2, 0, 0));
	DebugDrawer::DrawLine(points, 0xffff0000, mCamera);

	Material* diffMtl = gEngine->GetMaterialManager()->GetDefaultViewMtl();

	if(mModel)
	{
		mModel->Draw(IDENTITY_MATRIX, mCamera, true);
	}

	driver->EndScene();
	driver->Present((HWND)GetHWND());
}

void ModelPreviewCanvas::SetSwapChainIndex(int index)
{
	mSwapChainIndex = index;
}

void ModelPreviewCanvas::SetPreviewModel(Model* model)
{
	mModel = model;
}

void ModelPreviewCanvas::CleanUp()
{
	SAFE_DELETE(mCamera);
}

void ModelPreviewCanvas::AddjustCameraPos()
{
	_Assert(mCamera != NULL);
	_Assert(mModel != NULL);

	AABBox bound;
	mModel->CalcDynamicAABBox(IDENTITY_MATRIX, &bound);

	float width = bound.GetSize().x;
	float height = bound.GetSize().y;
	float length = bound.GetSize().z;

	float aspect = 0;
	float fov = 0;
	mCamera->GetCameraParams(NULL, NULL, &fov, &aspect);

	float distY = (height / 2.0f) / tan(fov / 2.0f);
	float distX = (height / 2.0f) / (aspect * tan(fov / 2.0f));

	float dist = Max(distX, distY);

	mCamera->SetWorldPosition(bound.GetCenter());
	mCamera->Translate(0, 0, -dist - length);
	mCamera->LookAt(bound.GetCenter());

	HoverCameraController* hoverCameraController = New HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 
		2.0f, 1000.0f, bound.GetCenter(), dist + length);

	mCamera->SetCameraController(hoverCameraController);
}
