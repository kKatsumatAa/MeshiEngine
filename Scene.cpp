#include "Scene.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "FbxLoader.h"


void SceneState::SetScene(Scene* scene)
{
	//stateではなくSceneクラスのインスタンス//
	this->scene = scene;
}

//--------------------------------------------------------------------------------------
void SceneLoad::Load()
{

}

void SceneLoad::Initialize()
{

	//非同期処理(ステージ作成中にもロード画面出す的な)
	async.StartAsyncFunction([=]() { this->Load(); });
}

void SceneLoad::Update()
{


	////シーン遷移
	//if (async.GetLockFlag())
	//{
	//	async.EndThread();

	//	//ステージ作り終わったら
	//	scene->ChangeState(new Scene5);
	//}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
	count++;
	loadObj.DrawBoxSprite({ 0,0 + sinf(count * 0.1f) * 3.0f,0 }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, scene->texhandle[3]);
}

//---------------------------------------------------------------------------------------
void SceneTitle::Initialize()
{
	scene->StopWaveAllScene();
}

void SceneTitle::Update()
{


	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new SceneGame);
	}
}

void SceneTitle::Draw()
{
}

void SceneTitle::DrawSprite()
{
	scene->draw[0].DrawBoxSprite({ 0,WindowsApp::GetInstance().window_height / 2.0f,0 },
		0.5f, { 1.0f,1.0f,1.0f,1.0f }, scene->texhandle[2]);
	scene->draw[1].DrawBoxSprite({ WindowsApp::GetInstance().window_width - 300,
		WindowsApp::GetInstance().window_height / 2.0f,0 },
		1.0f, { 1.0f,1.0f,1.0f,1.0f }, scene->texhandle[1]);

	scene->debugText.Print("[Basic]", 10, 10);
}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	Sound::GetInstance().PlayWave("Stage_BGM.wav", 0.4f, true);
}

void SceneGame::Update()
{
	ImGui::ShowDemoWindow();
	scene->imGuiManager->End();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		scene->ChangeState(new SceneTitle);
	}
}

void SceneGame::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
	//scene->draw[3].DrawSphere(scene->draw[3].worldMat, &scene->camera->viewMat,
	//	&scene->camera->projectionMat, { 1.0f,0.5f,0.2f,0.8f });
	//scene->draw[4].DrawCube3D(scene->draw[4].worldMat, &scene->camera->viewMat,
	//	&scene->camera->projectionMat, { 1.0f,0.2f,0.7f,1.0f });

	scene->draw[5].DrawFBX(scene->draw[5].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->modelFBX, { 1.0f,0.2f,0.7f,1.0f });
}

void SceneGame::DrawSprite()
{
	scene->debugText.Print("[1]", 10, 10);
}


//---------------------------------------------------------------------------------------
//デストラクタ
Scene::~Scene()
{
	delete state;
	for (int i = 0; i < _countof(model); i++)
	{
		delete model[i];
	}
	imGuiManager->Finalize();
	delete imGuiManager;
	delete lightManager;
	camera.reset();
	//音データ解放

}

void Scene::ChangeState(SceneState* state)
{
	if (state) {
		delete this->state;
	}
	this->state = state;
	state->SetScene(this);
	this->state->Initialize();
}

void Scene::StopWaveAllScene()
{
	Sound::GetInstance().StopWave("Stage_BGM.wav");
}

void Scene::Initialize()
{
	{
		TextureManager::LoadGraph(L"Resources/image/loading.png", texhandle[3]);
		//白い画像
		TextureManager::LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

		//画像
		TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);

		TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);
		TextureManager::LoadGraph(L"Resources/image/a.png", texhandle[2]);
	}

	//fbx読み込み
	modelFBX = FbxLoader::GetInstance()->LoadModelFromFile("cube");

	{
		Sound::GetInstance().LoadWave("Stage_BGM.wav", false);
	}

	model[0] = Model::LoadFromOBJ("skydome", true, true);
	model[1] = Model::LoadFromOBJ("ground");
	model[2] = Model::LoadFromOBJ("player");
	model[4] = Model::LoadFromOBJ("MiG-25PD", true);
	model[3] = Model::LoadFromOBJ("player", true);

	//Object::effectFlags.isGlassFilter = true;

	//model
	Model::StaticInitialize();

	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[0].worldMat->SetWorld();
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 0.0f, -10.0f, 0 };
	draw[1].worldMat->SetWorld();
	draw[2].worldMat->scale = { 5,5,5 };
	draw[2].worldMat->rot.y = { -pi / 2.0f };
	draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };
	draw[2].worldMat->SetWorld();
	draw[3].worldMat->scale = { 5,5,5 };
	draw[3].worldMat->rot.y = { -pi / 2.0f };
	draw[3].worldMat->trans = { 15.0f,0,0 };
	draw[3].worldMat->SetWorld();


	//球
	draw[4].worldMat->scale = { 5.0f,5.0f,5.0f };
	draw[4].worldMat->trans = { -20.0f,0,-10.0f };
	draw[4].worldMat->SetWorld();

	draw[5].worldMat->scale = { 0.1f,0.1f,0.1f };
	draw[5].worldMat->trans = { -20.0f,0,0 };
	draw[5].worldMat->SetWorld();


	//imgui
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize();

	//Light
	LightManager::StaticInitialize();
	//インスタンス生成
	lightManager = LightManager::Create();
	//ライト色を設定
	lightManager->SetDirLightColor(0, { 1,1,1 });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Object::SetLight(lightManager);
	lightManager->SetDirLightActive(0, true);
	lightManager->SetDirLightActive(1, true);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetDirLightDir(0, { 0, 0, 1.0 });
	lightManager->SetDirLightDir(1, { 0, -1.0, 0 });
	//点光源
	for (int i = 0; i < 6; i++)
	{
		lightManager->SetPointLightActive(i, false);
	}
	//丸影
	lightManager->SetCircleShadowActive(0, false);

	//カメラ
	camera = std::make_unique<Camera>();
	camera->Initialize();


	//objPlayer->Initialize();

	//draw[4].SetCollider(new PlaneCollider);
	/*draw[4].SetIsValid(false);*/

	ParticleManager::GetInstance()->Initialize();

	//ステート変更
	ChangeState(new SceneGame);
}

void Scene::Update()
{
	//imgui
	imGuiManager->Begin();


	lightManager->Update();

	state->Update();


	//#ifdef _DEBUG
		//if (KeyboardInput::GetInstance().KeyTrigger(DIK_E)) ChangeState(new SceneTitle);

	{
		cameraWorldMat.rot.y += (KeyboardInput::GetInstance().KeyPush(DIK_A) - KeyboardInput::GetInstance().KeyPush(DIK_D)) * 0.05f;

		cameraWorldMat.rot.x += (KeyboardInput::GetInstance().KeyPush(DIK_W) - KeyboardInput::GetInstance().KeyPush(DIK_S)) * 0.05f;
		cameraWorldMat.rot.x = min(cameraWorldMat.rot.x, pi / 2.0f);
		cameraWorldMat.rot.x = max(cameraWorldMat.rot.x, -pi / 2.0f);

		cameraWorldMat.SetWorld();
		camera->viewMat.eye = cameraPos;
		Vec3xM4(camera->viewMat.eye, cameraWorldMat.matWorld, 0);
		camera->UpdateViewMatrix();
	}
	//#endif 

		//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	state->Draw();
}

void Scene::DrawPostEffect()
{
	Object::DrawPera();
}

void Scene::DrawPostEffect2()
{
	Object::DrawPera2();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}


