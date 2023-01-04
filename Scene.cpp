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

}

void SceneTitle::Update()
{
	//弾
	scene->playerBulletM.get()->Update();
	//player
	scene->player.get()->Update();
	//敵
	scene->enemyM.get()->Update();
	//アイテム
	scene->itemM.get()->Update();
	//当たり判定
	scene->colliderM.get()->Update(scene->player.get(), scene->enemyM.get(), scene->playerBulletM.get(), scene->itemM.get(), scene->stage.get(),
		scene->camera.get());
	//カメラ
	Vec3 pos = scene->player.get()->GetWorldTransForm()->trans;
	{
		scene->camera->SetEye({ 0,pos.y - 10.0f,scene->camera->viewMat.eye.z });
		scene->camera->SetTarget({ 0,pos.y - 10.0f,1.0f });
	}
	scene->camera->Update();
	//ポイントライト
	scene->lightManager->SetPointLightPos(0, { pos.x,pos.y,pos.z - 10.0f });
	scene->lightManager->Update();

	ParticleManager::GetInstance()->Update(&scene->camera->viewMat, &scene->camera->projectionMat);
	ReloadEffectManager::GetInstance().Update();

	scene->cartridgeEffectM->Update();
	scene->breakEffectM->Update();

	//シーン遷移
	//マップステージに入ったら
	if (scene->player->GetWorldPos().y <= -scene->stage.get()->stageBeginNumY * scene->stage->blockRadius * 2.0f)
	{
		scene->ChangeState(new SceneGame);
	}
}

void SceneTitle::Draw()
{
	//player
	scene->player.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//弾
	scene->playerBulletM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//敵
	scene->enemyM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//アイテム
	scene->itemM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//ステージ
	scene->stage.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//パーティクル
	ParticleManager::GetInstance()->Draw(scene->texhandle[1]);
	//薬莢
	scene->cartridgeEffectM->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//
	scene->breakEffectM->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//
	ReloadEffectManager::GetInstance().Draw(scene->camera->viewMat, scene->camera->projectionMat);
}

void SceneTitle::DrawSprite()
{
	/*scene->playerBulletM.get()->DrawSprite();
	scene->player.get()->DrawSprite();*/
}


//---------------------------------------------------------------------------------------
//ゲーム
void SceneGame::Initialize()
{

}

void SceneGame::Update()
{
	//弾
	scene->playerBulletM.get()->Update();
	//player
	scene->player.get()->Update();
	//敵
	scene->enemyM.get()->Update();
	//アイテム
	scene->itemM.get()->Update();
	//当たり判定
	scene->colliderM.get()->Update(scene->player.get(), scene->enemyM.get(), scene->playerBulletM.get(), scene->itemM.get(), scene->stage.get(),
		scene->camera.get());
	//カメラ
	Vec3 pos = scene->player.get()->GetWorldTransForm()->trans;
	{
		scene->camera->SetEye({ 0,pos.y - 10.0f,scene->camera->viewMat.eye.z });
		scene->camera->SetTarget({ 0,pos.y - 10.0f,1.0f });
	}
	scene->camera->Update();
	//ポイントライト
	scene->lightManager->SetPointLightPos(0, { pos.x,pos.y,pos.z - 10.0f });
	scene->lightManager->Update();

	ParticleManager::GetInstance()->Update(&scene->camera->viewMat, &scene->camera->projectionMat);
	ReloadEffectManager::GetInstance().Update();

	scene->cartridgeEffectM->Update();
	scene->breakEffectM->Update();

	//シーン遷移
	//マップの下限より下に行ったら
	if (scene->player->GetWorldPos().y <= -scene->stage.get()->mapDownMaxNum * scene->stage->blockRadius * 2.0f + scene->stage->blockRadius
		&& !scene->stage->isPlayerRoom)
	{
		scene->ChangeState(new SceneClear);
	}
	else if (scene->player->GetHP() <= 0)
	{
		scene->ChangeState(new SceneGameOver);
	}
}

void SceneGame::Draw()
{
	//player
	scene->player.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//弾
	scene->playerBulletM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//敵
	scene->enemyM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//アイテム
	scene->itemM.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//ステージ
	scene->stage.get()->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//パーティクル
	ParticleManager::GetInstance()->Draw(scene->texhandle[1]);
	//薬莢
	scene->cartridgeEffectM->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//
	scene->breakEffectM->Draw(scene->camera->viewMat, scene->camera->projectionMat);
	//
	ReloadEffectManager::GetInstance().Draw(scene->camera->viewMat, scene->camera->projectionMat);
}

void SceneGame::DrawSprite()
{
	scene->playerBulletM.get()->DrawSprite();
	scene->player.get()->DrawSprite();
}



//---------------------------------------------------------------------------------------
//終了画面
void SceneGameOver::Initialize()
{
	//
	scene->camera->Initialize();
}

void SceneGameOver::Update()
{

	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new SceneLoad);
	}
}

void SceneGameOver::Draw()
{

}

void SceneGameOver::DrawSprite()
{
	scene->debugText.Print("GameOver", 100, 100, 114514, 5.0f);
}


//----------------------------------------------------------------------------------------
void SceneClear::Initialize()
{
	//
	scene->camera->Initialize();
}

void SceneClear::Update()
{

	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new SceneLoad);
	}
}

void SceneClear::Draw()
{
}

void SceneClear::DrawSprite()
{
	scene->debugText.Print("GameClear", 100, 100, 114514, 5.0f);
}


//--------------------------------------------------------------------------------------
void SceneLoad::StageCreate()
{
	scene->stage.get()->Initialize(scene->model[2], scene->enemyM.get(), scene->itemM.get(), scene->lightManager, scene->breakEffectM.get());
	scene->stage.get()->GenerateStage();
}

void SceneLoad::Initialize()
{
	//薬莢
	scene->cartridgeEffectM->Initialize();
	//
	scene->camera->Initialize();
	//プレイヤー弾
	scene->playerBulletM.get()->Initialize(scene->model[2], scene->cartridgeEffectM.get(), scene->camera.get());
	//player
	scene->player.get()->Initialize(scene->model[2], scene->model[3], scene->playerBulletM.get(), &scene->debugText, scene->camera.get());
	//敵
	scene->enemyM.get()->Initialize(scene->model[2], scene->player.get());
	//当たり判定クラス
	scene->colliderM.get()->Initialize();
	//アイテム
	scene->itemM.get()->Initialize(scene->model[2]);
	//
	scene->breakEffectM.get()->Initialize();

	//
	ParticleManager::GetInstance()->Initialize();
	ReloadEffectManager::GetInstance().Initialize();


	//非同期処理(ステージ作成中にもロード画面出す)
	async.StartAsyncFunction([=]() { StageCreate(); });
}

void SceneLoad::Update()
{
	//ステージ作り終わったら
	if (async.GetLockFlag())
	{
		async.EndThread();

		//ステージ作り終わったら
		scene->ChangeState(new SceneTitle);
	}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
	count++;

	scene->debugText.Print("Loading", 100, 100.0f + sinf(count * 0.1f) * 10.0f, 114514, 5.0f);
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
	player.reset();
	enemyM.reset();
	colliderM.reset();
	playerBulletM.reset();
	stage.reset();
	itemM.reset();
	breakEffectM.reset();
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

void Scene::Initialize()
{
	//白い画像
	TextureManager::GetInstance().LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

	//画像
	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);
	TextureManager::LoadGraph(L"Resources/image/white.png", texhandle[0]);

	TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);

	//model
	Model::StaticInitialize();

	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 100.0f, 100.0f, 100.0f };
	draw[1].worldMat->trans = { 0, 0, 10.0f };
	draw[1].worldMat->rot = { (-pi / 2.0f), 0, 0 };
	model[2] = Model::LoadFromOBJ("sphere");
	model[3] = Model::LoadFromOBJ("hanger");

	//音
	//Sound::LoadWave("a.wav", true);
	//Sound::LoadWave("BGM.wav", false);//

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

	//点光源
	lightManager->SetDirLightActive(0, false);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	for (int i = 0; i < LightManager::PointLightNum; i++)
	{
		lightManager->SetPointLightActive(i, true);
		lightManager->SetPointLightAtten(i, { 0.005f,0.005f,0.005f });
		if (i == 0)
		{
			lightManager->SetPointLightColor(i, { 0.8f,0.8f,0.01f });
		}
		else
		{
			lightManager->SetPointLightColor(i, { 0.3f,0.5f,0.8f });
		}
	}
	//カメラ
	camera = std::make_unique<Camera>();
	//camera->Initialize();
	//薬莢
	cartridgeEffectM = std::make_unique<CartridgeEffectManager>();
	//cartridgeEffectM->Initialize();
	//プレイヤー弾
	playerBulletM = std::make_unique<PlayerBulletManager>();
	//playerBulletM.get()->Initialize(model[2], cartridgeEffectM.get(), camera.get());
	//player
	player = std::make_unique<Player>();
	//player.get()->Initialize(model[2], model[3], playerBulletM.get(), &debugText, camera.get());
	//敵
	enemyM = std::make_unique<EnemyManager>();
	//enemyM.get()->Initialize(model[2], player.get());
	//当たり判定クラス
	colliderM = std::make_unique<ColliderManager>();
	//colliderM.get()->Initialize();
	//アイテム
	itemM = std::make_unique<ItemManager>();
	//itemM.get()->Initialize(model[2]);
	//
	breakEffectM = std::make_unique<BreakEffectManager>();
	//breakEffectM.get()->Initialize();
	//ステージ
	stage = std::make_unique<Stage>();
	//stage.get()->Initialize(model[2], enemyM.get(), itemM.get(), lightManager, breakEffectM.get());
	//stage.get()->GenerateStage();
	//
	//ParticleManager::GetInstance()->Initialize();
	//ReloadEffectManager::GetInstance().Initialize();


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

	}

	state->Update();


#ifdef _DEBUG
	//if (KeyboardInput::GetInstance().KeyTrigger(DIK_E)) ChangeState(new SceneTitle);

	/*{
		cameraWorldMat.rot.y += (KeyboardInput::GetInstance().KeyPush(DIK_A) - KeyboardInput::GetInstance().KeyPush(DIK_D)) * 0.05f;

		cameraWorldMat.rot.x += (KeyboardInput::GetInstance().KeyPush(DIK_W) - KeyboardInput::GetInstance().KeyPush(DIK_S)) * 0.05f;
		cameraWorldMat.rot.x = min(cameraWorldMat.rot.x, pi / 2.0f);
		cameraWorldMat.rot.x = max(cameraWorldMat.rot.x, -pi / 2.0f);

		cameraWorldMat.SetWorld();
		viewMat.eye = cameraPos;
		Vec3xM4(viewMat.eye, cameraWorldMat.matWorld, 0);
		viewMat.SetMat();
	}*/
#endif 

	//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	draw[1].DrawModel(draw[1].worldMat, &camera->viewMat, &camera->projectionMat, model[1]);

	state->Draw();

	//imgui
	imGuiManager->Draw();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

	debugText.DrawAll(debugTextHandle);
}