#include "SceneEditorPanel.h"

#include "SceneGraphPanel.h"
#include "GeometryPanel.h"
#include "MaterialPanel.h"
#include "TexturePanel.h"
#include "ModelPanel.h"
#include "LightPanel.h"

BEGIN_EVENT_TABLE(SceneEditorPanel, wxNotebook)
EVT_CLOSE(SceneEditorPanel::OnClose)
END_EVENT_TABLE()
SceneEditorPanel::SceneEditorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								   :wxNotebook(parent, id, pos, size, wxNB_MULTILINE)
{
	createWxCtrls();
}

void SceneEditorPanel::createWxCtrls()
{
	mIconList = New wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/scenegraphPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/geometryPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/materialPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/texturePage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/modelPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/lightPage.ico", wxBITMAP_TYPE_ICO));

	SetImageList(mIconList); 
}

void SceneEditorPanel::OnCreateScene()
{
	wxNotebookPage* sceneGraphPage = GetPage(0);		// TODO: page0ΪsceneGraph
	SceneGraphPanel* sceneGraphPanel = static_cast<SceneGraphPanel*>(sceneGraphPage);
	sceneGraphPanel->LoadDataFromScene();

	wxNotebookPage* geometryPage = GetPage(1);
	GeometryPanel* geoPanel = static_cast<GeometryPanel*>(geometryPage);
	geoPanel->LoadDataFromScene();

	wxNotebookPage* materialPage = GetPage(2);
	MaterialPanel* materialPanel = static_cast<MaterialPanel*>(materialPage);
	materialPanel->LoadDataFromScene();

	wxNotebookPage* modelPage = GetPage(4);
	ModelPanel* modelPanel = static_cast<ModelPanel*>(modelPage);
	modelPanel->LoadDataFromScene();

	wxNotebookPage* texturePage = GetPage(3);
	TexturePanel* texturePanel = static_cast<TexturePanel*>(texturePage);
	texturePanel->LoadDataFromScene();
}

void SceneEditorPanel::CleanupAndDestory()
{
	delete mIconList;
	Destroy();
}

void SceneEditorPanel::CreateEditorPages()
{
	SceneGraphPanel* sceneGraphPanel = New SceneGraphPanel(this, -1);
	AddPage(sceneGraphPanel, L"SceneGraph", true, SCENE_GRAPH_PAGE);

	GeometryPanel* geometryPanel = New GeometryPanel(this, -1);
	AddPage(geometryPanel, L"Geometry", false, GEOMETRY_PAGE);

	MaterialPanel* materialPanel = New MaterialPanel(this, -1);
	AddPage(materialPanel, L"Material", false, MATERIAL_PAGE);

	TexturePanel* texturePanel = New TexturePanel(this, -1);
	AddPage(texturePanel, L"Texture", false, TEXTURE_PAGE);

	ModelPanel* modelPanel = New ModelPanel(this, -1);
	AddPage(modelPanel, L"Model", false, MODEL_PAGE);

	LightPanel* lightPanel = New LightPanel(this, -1);
	AddPage(lightPanel, L"Light", false, LIGHT_PAGE);

	this->Fit();
	this->Layout();
}

void SceneEditorPanel::OnClose(wxCloseEvent& event)
{
	delete mIconList;

	wxNotebookPage* modelPage = GetPage(MODEL_PAGE);
	ModelPanel* modelPanel = static_cast<ModelPanel*>(modelPage);
	modelPanel->CleanUp();

	Destroy();
}



