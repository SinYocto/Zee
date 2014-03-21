#include "SceneEditorPanel.h"

#include "SceneGraphPanel.h"
#include "GeometryPanel.h"
#include "MaterialPanel.h"
#include "TexturePanel.h"
#include "ModelPanel.h"
#include "LightPanel.h"
#include "TerrainPanel.h"
#include "Engine.h"
#include "MeshNode.h"
#include "ModelNode.h"
#include "Camera.h"

BEGIN_EVENT_TABLE(SceneEditorPanel, wxNotebook)
	EVT_CLOSE(SceneEditorPanel::OnClose)
	EVT_BUTTON(wxID_ANY, SceneEditorPanel::OnBitmapButton)
	EVT_TREE_ITEM_ACTIVATED (wxID_ANY, SceneEditorPanel::OnItemActivated)
	EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, SceneEditorPanel::OnPageChanged)
	EVT_MENU(ID_MODEL_PANEL_ITEM_MENU_ADD_TO_SCENE, SceneEditorPanel::OnAddModelNode)
END_EVENT_TABLE()
SceneEditorPanel::SceneEditorPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/, 
								   const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/)
								   :wxNotebook(parent, id, pos, size, wxNB_MULTILINE)
								   ,mMtlEditoring(NULL)
								   ,mMtlTexLayer(0)
								   ,mSceneNodeEditoring(NULL)
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
	gEngine->GetGizmo()->RegisterEventHanlder(this);

	wxNotebookPage* sceneGraphPage = GetPage(SCENE_GRAPH_PAGE);		// TODO: page0ΪsceneGraph
	SceneGraphPanel* sceneGraphPanel = static_cast<SceneGraphPanel*>(sceneGraphPage);
	sceneGraphPanel->LoadDataFromScene();

	wxNotebookPage* geometryPage = GetPage(GEOMETRY_PAGE);
	GeometryPanel* geoPanel = static_cast<GeometryPanel*>(geometryPage);
	geoPanel->LoadDataFromScene();

	wxNotebookPage* materialPage = GetPage(MATERIAL_PAGE);
	MaterialPanel* materialPanel = static_cast<MaterialPanel*>(materialPage);
	materialPanel->LoadDataFromScene();

	wxNotebookPage* modelPage = GetPage(MODEL_PAGE);
	ModelPanel* modelPanel = static_cast<ModelPanel*>(modelPage);
	modelPanel->LoadDataFromScene();

	wxNotebookPage* texturePage = GetPage(TEXTURE_PAGE);
	TexturePanel* texturePanel = static_cast<TexturePanel*>(texturePage);
	texturePanel->LoadDataFromScene();

	wxNotebookPage* terrainPage = GetPage(TERRAIN_PAGE);
	TerrainPanel* terrainPanel = static_cast<TerrainPanel*>(terrainPage);
	terrainPanel->LoadDataFromScene();
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

	//LightPanel* lightPanel = New LightPanel(this, -1);
	//AddPage(lightPanel, L"Light", false, LIGHT_PAGE);

	TerrainPanel* terrainPanel = New TerrainPanel(this, -1);
	AddPage(terrainPanel, L"Terrain", false, TERRAIN_PAGE);

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

void SceneEditorPanel::OnSelectNode(Gizmo* gizmo)
{
	SetSelection(SCENE_GRAPH_PAGE);
}

void SceneEditorPanel::OnBitmapButton(wxCommandEvent& event)
{
	wxString eventInfo = event.GetString();

	if(eventInfo.Cmp(L"MaterialPanel_Tex") == 0)
	{
		ChangeSelection(TEXTURE_PAGE);

		mMtlEditoring = (Material*)event.GetClientData();
		mMtlTexLayer = event.GetInt();
	}
	else if(eventInfo.Cmp(L"SceneGraphPanel_Mesh_Mtl") == 0)
	{
		ChangeSelection(MATERIAL_PAGE);

		mSceneNodeEditoring = (SceneNode*)event.GetClientData();
	}
}

void SceneEditorPanel::OnItemActivated(wxTreeEvent& event)
{

	wxString eventInfo = event.GetString();

	if(eventInfo.Cmp(L"TexturePanel") == 0)
	{
		if(!mMtlEditoring)
			return;

		Texture* texture = (Texture*)event.GetClientData();
		mMtlEditoring->SetTexture(mMtlTexLayer, texture->GetFilePath());

		ChangeSelection(MATERIAL_PAGE);
		wxNotebookPage* materialPage = GetPage(MATERIAL_PAGE);
		MaterialPanel* materialPanel = static_cast<MaterialPanel*>(materialPage);

		materialPanel->RefreshInspector();
	}
	else if(eventInfo.Cmp(L"MaterialPanel") == 0)
	{
		if(!mSceneNodeEditoring)
			return;

		Material* mtl = (Material*)event.GetClientData();

		MeshNode* meshNode = static_cast<MeshNode*>(mSceneNodeEditoring);
		meshNode->GetMesh()->SetMaterial(mtl);

		ChangeSelection(SCENE_GRAPH_PAGE);
		wxNotebookPage* sceneGraphPage = GetPage(SCENE_GRAPH_PAGE);
		SceneGraphPanel* sceneGraphPanel = static_cast<SceneGraphPanel*>(sceneGraphPage);

		sceneGraphPanel->RefreshInspector();
	}
}

void SceneEditorPanel::OnPageChanged(wxNotebookEvent& event)
{
	int prevPage = event.GetOldSelection();
	int curPage = event.GetSelection();

	if(!(prevPage == MATERIAL_PAGE && curPage == TEXTURE_PAGE))
	{
		mMtlEditoring = NULL;
		mMtlTexLayer = 0;
	}

	if(!(prevPage == SCENE_GRAPH_PAGE && curPage == MATERIAL_PAGE) &&
		!(prevPage == SCENE_GRAPH_PAGE && curPage == GEOMETRY_PAGE))
	{
		mSceneNodeEditoring = NULL;
	}

	event.Skip();
}

// 在ModelPanel中将model加入场景时, 需要更新SceneGraphPanel, MaterialPanel, GeometryPanel
void SceneEditorPanel::OnAddModelNode(wxCommandEvent& event)
{
	SceneManager* sceneMgr = gEngine->GetSceneManager();
	ModelManager* modelMgr = gEngine->GetModelManager();

	Model* model = (Model*)event.GetClientData();
	Model* newModelInstance = New Model(model->GetName(), model);

	ModelNode* modelNode = New ModelNode(newModelInstance->GetName(), NULL, newModelInstance);

	const float DIST = 10.0f;
	Camera* camera = gEngine->GetSceneManager()->GetMainCamera();
	Vector3 pos = camera->GetWorldPosition() + DIST * camera->GetWorldForward();
	modelNode->SetWorldPosition(pos);

	sceneMgr->AddSceneNode(modelNode);
	modelNode->OnTransformChanged();

	// 更新SceneGraphPanel
	wxNotebookPage* sceneGraphPage = GetPage(SCENE_GRAPH_PAGE);
	SceneGraphPanel* sceneGraphPanel = static_cast<SceneGraphPanel*>(sceneGraphPage); 

	sceneGraphPanel->AppendSceneNode(modelNode);

	// 更新MaterialPanel
	wxNotebookPage* materialPage = GetPage(MATERIAL_PAGE);
	MaterialPanel* materialPanel = static_cast<MaterialPanel*>(materialPage);

	std::list<Mesh*> meshList = newModelInstance->GetSubMeshList();

	for(std::list<Mesh*>::iterator iter = meshList.begin(); iter != meshList.end(); ++iter)
	{
		Mesh* mesh = *iter;
		AABBox subMeshBBox = AABBox::Invalid;

		Material* mtl = mesh->GetMaterial();
		materialPanel->AppendMaterial(mtl);
	}
}



