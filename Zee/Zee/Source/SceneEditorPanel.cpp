#include "SceneEditorPanel.h"

#include "SceneGraphPanel.h"
#include "GeometryPanel.h"
#include "MaterialPanel.h"
#include "TexturePanel.h"
#include "ModelPanel.h"
#include "LightPanel.h"

SceneEditorPanel::SceneEditorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								   :wxNotebook(parent, id, pos, size, wxNB_MULTILINE)
{
	createWxCtrls();
}

void SceneEditorPanel::createWxCtrls()
{
	mIconList = new wxImageList(16, 16, true);
	mIconList->Add(wxIcon(L"./Assets/Icons/scenegraphPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/geometryPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/materialPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/texturePage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/modelPage.ico", wxBITMAP_TYPE_ICO));
	mIconList->Add(wxIcon(L"./Assets/Icons/lightPage.ico", wxBITMAP_TYPE_ICO));

	SetImageList(mIconList); 

	SceneGraphPanel* sceneGraphPanel = new SceneGraphPanel(this, -1);
	AddPage(sceneGraphPanel, L"SceneGraph", true, 0);

	GeometryPanel* geometryPanel = new GeometryPanel(this, -1);
	AddPage(geometryPanel, L"Geometry", false, 1);

	MaterialPanel* materialPanel = new MaterialPanel(this, -1);
	AddPage(materialPanel, L"Material", false, 2);

	TexturePanel* texturePanel = new TexturePanel(this, -1);
	AddPage(texturePanel, L"Texture", false, 3);

	ModelPanel* modelPanel = new ModelPanel(this, -1);
	AddPage(modelPanel, L"Model", false, 4);

	LightPanel* lightPanel = new LightPanel(this, -1);
	AddPage(lightPanel, L"Light", false, 5);
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



