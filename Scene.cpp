#include "Scene.h"
#include <sstream>
#include <iomanip>

void SceneState::SetScene(Scene* scene)
{
	//stateではなくSceneクラスのインスタンス//
	this->scene = scene;
}



//---------------------------------------------------------------------------------------
//タイトル
void SceneTitle::Initialize()
{
	//音
	scene->StopWaveAllScene();
}

void SceneTitle::Update()
{
	////シーン遷移
	//if ()
	//{
	//	scene->ChangeState(new SceneGame);
	//}
}

void SceneTitle::Draw()
{
}

void SceneTitle::DrawSprite()
{

}


//---------------------------------------------------------------------------------------
//ゲーム
void SceneGame::Initialize()
{
	//音
	scene->StopWaveAllScene();
}

void SceneGame::Update()
{

	////シーン遷移
	//if ()
	//{
	//	scene->ChangeState(new SceneClear);
	//}
	//else if ()
	//{
	//	scene->ChangeState(new SceneGameOver);
	//}
}

void SceneGame::Draw()
{

}

void SceneGame::DrawSprite()
{

}



//---------------------------------------------------------------------------------------
//終了画面
void SceneGameOver::Initialize()
{

	//音
	scene->StopWaveAllScene();
}

void SceneGameOver::Update()
{

	/*if ()
	{
		scene->ChangeState(new SceneLoad);
	}*/
}

void SceneGameOver::Draw()
{

}

void SceneGameOver::DrawSprite()
{
}


//----------------------------------------------------------------------------------------
void SceneClear::Initialize()
{
	//音
	scene->StopWaveAllScene();
}

void SceneClear::Update()
{

	/*if ()
	{
		scene->ChangeState(new SceneLoad);
	}*/
}

void SceneClear::Draw()
{
}

void SceneClear::DrawSprite()
{

}


//--------------------------------------------------------------------------------------

void SceneLoad::Initialize()
{
	//非同期処理
	//async.StartAsyncFunction([=]() { func() });
}

void SceneLoad::Update()
{

	/*if (async.GetLockFlag())
	{
		async.EndThread();

		scene->ChangeState(new SceneTitle);
	}*/
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{

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
}

void Scene::Initialize()
{
	//白い画像
	TextureManager::GetInstance().LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

	//画像
	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);

	TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);


	//音
	{

	}

	//model
	Model::StaticInitialize();

	model[0] = Model::LoadFromOBJ("skydome", true, true);
	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[0].worldMat->SetWorld();
	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 10.0f, -10.0f, 0 };
	draw[1].worldMat->SetWorld();
	model[2] = Model::LoadFromOBJ("player");
	draw[2].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[2].worldMat->rot.y = { -pi / 2.0f };
	draw[2].worldMat->trans = { 20.0f,10.0f,10.0f };
	draw[2].worldMat->SetWorld();



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
	lightManager->SetDirLightActive(0, false);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	//点光源
	for (int i = 0; i < 6; i++)
	{
		lightManager->SetPointLightActive(i, false);
	}
	lightManager->SetSpotLightActive(0, true);

	//カメラ
	camera = std::make_unique<Camera>();
	camera->Initialize();

	//ステート変更
	ChangeState(new SceneLoad);
}

void Scene::Update()
{
	//imgui
	imGuiManager->Begin();

	{
		//デモ
		ImGui::ShowDemoWindow();

		//点光源
		lightManager->SetSpotLightDir(0,
			XMVECTOR({ spotLightDir[0], spotLightDir[1], spotLightDir[2] }));
		lightManager->SetSpotLightPos(0, XMFLOAT3(spotLightPos));
		lightManager->SetSpotLightColor(0, XMFLOAT3(spotLightColor));
		lightManager->SetSpotLightAtten(0, XMFLOAT3(spotLightAtten));
		lightManager->SetSpotLightFactorAngle(0, XMFLOAT2(spotLightFactorAngle));

		static bool a = true;
		ImGui::Begin("spotLight", &a, ImGuiWindowFlags_MenuBar);
		ImGui::InputFloat3("spotLightDir", spotLightDir);
		ImGui::ColorEdit3("spotLightColor", spotLightColor, ImGuiColorEditFlags_Float);
		ImGui::InputFloat3("spotLightPos", spotLightPos);
		ImGui::InputFloat3("spotLightAtten", spotLightAtten);
		ImGui::InputFloat3("spotLightFactorAngle", spotLightFactorAngle);
		ImGui::End();
		lightManager->Update();

	}

	state->Update();


#ifdef _DEBUG
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
#endif 

	//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	for (int i = 0; i < 3; i++)
	{
		draw[i].DrawModel(draw[i].worldMat, &camera->viewMat, &camera->projectionMat, model[i]);
	}

	state->Draw();
}

void Scene::DrawPostEffect()
{
	draw[2].DrawPera();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

#ifdef _DEBUG
	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
#endif
}
