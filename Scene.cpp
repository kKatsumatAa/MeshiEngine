#include "Scene.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"

void SceneState::SetScene(Scene* scene)
{
	//stateではなくSceneクラスのインスタンス//
	this->scene = scene;
}



//---------------------------------------------------------------------------------------
void SceneBasic::Initialize()
{
	scene->draw[0].effectFlags.isFog = false;
	scene->draw[1].effectFlags.isFog = false;
	scene->draw[2].effectFlags.isFog = false;
}

void SceneBasic::Update()
{


	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene1phong);
	}
}

void SceneBasic::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void SceneBasic::DrawSprite()
{
	scene->debugText.Print("[Basic]", 10, 10);
}

//---------------------------------------------------------------------------------------
void Scene1phong::Initialize()
{

}

void Scene1phong::Update()
{

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene2);
	}
}

void Scene1phong::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
	scene->draw[3].DrawModel(scene->draw[3].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[3]);
}

void Scene1phong::DrawSprite()
{
	scene->debugText.Print("[Phong&Smoosing]", 10, 10);
}

//---------------------------------------------------------------------------------------
void Scene2::Initialize()
{
}

void Scene2::Update()
{
	//ライト
			//光線方向初期化
	static XMVECTOR lightDir = { 0,1,5,0 };

	if (KeyboardInput::GetInstance().KeyPush(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
	if (KeyboardInput::GetInstance().KeyPush(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
	if (KeyboardInput::GetInstance().KeyPush(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
	if (KeyboardInput::GetInstance().KeyPush(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

	scene->lightManager->SetDirLightDir(0, lightDir);

	std::ostringstream debugstr;
	debugstr << "lightDirFactor("
		<< std::fixed << std::setprecision(2)
		<< lightDir.m128_f32[0] << ","
		<< lightDir.m128_f32[1] << ","
		<< lightDir.m128_f32[2] << ")";
	scene->debugText.Print(debugstr.str(), 50, 50);

	debugstr.str("");
	debugstr.clear();

	const XMFLOAT3& cameraPos =
	{ scene->camera->viewMat.eye.x,scene->camera->viewMat.eye.y,scene->camera->viewMat.eye.z };
	debugstr << "cameraPos("
		<< std::fixed << std::setprecision(2)
		<< lightDir.m128_f32[0] << ","
		<< lightDir.m128_f32[1] << ","
		<< lightDir.m128_f32[2] << ")";
	scene->debugText.Print(debugstr.str(), 50, 70);

	scene->lightManager->Update();


	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene3);
	}
}

void Scene2::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
	scene->draw[3].DrawModel(scene->draw[3].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[3]);
}

void Scene2::DrawSprite()
{
	scene->debugText.Print("ARROW:pos", 10, 25);
	scene->debugText.Print("[LightSeparate]", 10, 10);
}


//----------------------------------------------------------------------------------------
void Scene3::Initialize()
{
}

void Scene3::Update()
{
	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene4);
	}
}

void Scene3::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[4]);
}

void Scene3::DrawSprite()
{
	scene->debugText.Print("[Phong]", 10, 10);
}
//---------------
void Scene4::Initialize()
{
	Vec3 dir = { -1.0f,0,0.0f };
	Vec3 dir2 = { 1.0f,0,0.0f };
	dir.Normalized();
	dir2.Normalized();
	scene->lightManager->SetDirLightDir(0, { dir.x,dir.y,dir.z });
	scene->lightManager->SetDirLightDir(1, { dir2.x,dir2.y,dir2.z });
}

void Scene4::Update()
{
	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene5);
	}
}

void Scene4::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[4]);
}

void Scene4::DrawSprite()
{
	scene->debugText.Print("[Lights]", 10, 10);
}

//---------------
void Scene5::Initialize()
{
	scene->lightManager->SetDirLightActive(0, false);
	scene->lightManager->SetDirLightActive(1, false);

	scene->lightManager->SetPointLightActive(0, true);

}

void Scene5::Update()
{
	//点光源
	scene->lightManager->SetPointLightPos(0, XMFLOAT3(pointLightPos));
	scene->lightManager->SetPointLightColor(0, XMFLOAT3(pointLightColor));
	scene->lightManager->SetPointLightAtten(0, XMFLOAT3(pointLightAtten));

	static bool a = true;
	ImGui::Begin("PointLight", &a, ImGuiWindowFlags_MenuBar);
	ImGui::ColorEdit3("pointLightColor", pointLightColor, ImGuiColorEditFlags_Float);
	ImGui::InputFloat3("pointLightPos", pointLightPos);
	ImGui::InputFloat3("pointLightAtten", pointLightAtten);
	ImGui::End();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene6);
	}
}

void Scene5::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void Scene5::DrawSprite()
{
	scene->debugText.Print("[PointLight]", 10, 10);
}

//---------------
void Scene6::Initialize()
{
	//点光源
	for (int i = 0; i < 6; i++)
	{
		scene->lightManager->SetPointLightActive(i, false);
	}
	//スポットライト
	scene->lightManager->SetSpotLightActive(0, true);
}

void Scene6::Update()
{
	scene->lightManager->SetSpotLightDir(0,
		XMVECTOR({ spotLightDir[0], spotLightDir[1], spotLightDir[2] }));
	scene->lightManager->SetSpotLightPos(0, XMFLOAT3(spotLightPos));
	scene->lightManager->SetSpotLightColor(0, XMFLOAT3(spotLightColor));
	scene->lightManager->SetSpotLightAtten(0, XMFLOAT3(spotLightAtten));
	scene->lightManager->SetSpotLightFactorAngle(0, XMFLOAT2(spotLightFactorAngle));

	static bool a = true;
	ImGui::Begin("spotLight", &a, ImGuiWindowFlags_MenuBar);
	ImGui::InputFloat3("spotLightDir", spotLightDir);
	ImGui::ColorEdit3("spotLightColor", spotLightColor, ImGuiColorEditFlags_Float);
	ImGui::InputFloat3("spotLightPos", spotLightPos);
	ImGui::InputFloat3("spotLightAtten", spotLightAtten);
	ImGui::InputFloat2("spotLightFactorAngle", spotLightFactorAngle);
	ImGui::End();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene7);
	}
}

void Scene6::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void Scene6::DrawSprite()
{
	scene->debugText.Print("[SpotLight]", 10, 10);
}

//---------------
void Scene7::Initialize()
{
	scene->lightManager->SetDirLightActive(0, true);
	scene->lightManager->SetDirLightActive(1, false);
	scene->lightManager->SetDirLightActive(2, false);
	//点光源
	for (int i = 0; i < 6; i++)
	{
		scene->lightManager->SetPointLightActive(i, false);
	}
	//丸影
	scene->lightManager->SetCircleShadowActive(0, true);
}

void Scene7::Update()
{
	//丸影
	scene->lightManager->SetCircleShadowDir(0,
		XMVECTOR({ circleShadowDir[0], circleShadowDir[1], circleShadowDir[2],0 }));
	scene->lightManager->SetCircleShadowCasterPos(0,
		XMFLOAT3({ fighterPos[0],fighterPos[1],fighterPos[2] }));
	scene->lightManager->SetCircleShadowAtten(0, XMFLOAT3(circleShadowAtten));
	scene->lightManager->SetCircleShadowFactorAngle(0, XMFLOAT2(circleShadowFactorAngle));
	scene->lightManager->SetCircleShadowDistanceCasterLight(0, circleShadowDistance);

	static bool a = true;
	ImGui::Begin("circleShadow", &a, ImGuiWindowFlags_MenuBar);

	ImGui::InputFloat3("circleShadowDir", circleShadowDir);
	ImGui::InputFloat3("circleShadowAtten", circleShadowAtten);
	ImGui::InputFloat2("circleShadowFactorAngle", circleShadowFactorAngle);
	ImGui::InputFloat3("fihgterPos", fighterPos);
	ImGui::InputFloat("distanceLight", &circleShadowDistance);


	ImGui::End();

	scene->draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };
	scene->draw[2].worldMat->SetWorld();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new Scene8);
	}
}

void Scene7::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void Scene7::DrawSprite()
{
	scene->debugText.Print("[CircleShadow]", 10, 10);
}

//----------------------------------------------------
void Scene8::Initialize()
{
	scene->lightManager->SetDirLightActive(0, true);
	scene->lightManager->SetDirLightActive(1, true);
	scene->lightManager->SetDirLightActive(2, false);
	scene->lightManager->SetDirLightDir(0, { 0, 0, 1.0 });
	scene->lightManager->SetDirLightDir(1, { 0, -1.0, 0 });
	//点光源
	for (int i = 0; i < 6; i++)
	{
		scene->lightManager->SetPointLightActive(i, false);
	}
	//丸影
	scene->lightManager->SetCircleShadowActive(0, false);
	scene->lightManager->SetSpotLightActive(0, false);

	scene->draw[0].effectFlags.isFog = true;
	scene->draw[1].effectFlags.isFog = true;
	scene->draw[2].effectFlags.isFog = true;
}

void Scene8::Update()
{
	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE))
	{
		scene->ChangeState(new SceneBasic);
	}
}

void Scene8::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		scene->draw[i].DrawModel(scene->draw[i].worldMat, &scene->camera->viewMat,
			&scene->camera->projectionMat, scene->model[i]);
	}
	scene->draw[2].DrawModel(scene->draw[2].worldMat, &scene->camera->viewMat,
		&scene->camera->projectionMat, scene->model[2]);
}

void Scene8::DrawSprite()
{
	scene->debugText.Print("[Fog]", 10, 10);
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

	{

	}

	//model
	Model::StaticInitialize();

	model[0] = Model::LoadFromOBJ("skydome", true, true);
	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[0].worldMat->SetWorld();
	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 0.0f, -10.0f, 0 };
	draw[1].worldMat->SetWorld();
	model[2] = Model::LoadFromOBJ("player");
	draw[2].worldMat->scale = { 5,5,5 };
	draw[2].worldMat->rot.y = { -pi / 2.0f };
	draw[2].worldMat->trans = { fighterPos[0],fighterPos[1],fighterPos[2] };
	draw[2].worldMat->SetWorld();
	model[3] = Model::LoadFromOBJ("player", true);
	draw[3].worldMat->scale = { 5,5,5 };
	draw[3].worldMat->rot.y = { -pi / 2.0f };
	draw[3].worldMat->trans = { 15.0f,0,0 };
	draw[3].worldMat->SetWorld();
	model[4] = Model::LoadFromOBJ("MiG-25PD", true);

	//球
	draw[4].worldMat->scale = { 5.0f,5.0f,5.0f };
	draw[4].worldMat->trans = { -20.0f,0,-10.0f };
	draw[4].worldMat->SetWorld();


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

	//コライダー
	collisionManager = CollisionManager::GetInstance();
	objPlayer = Player::Create();
	//objPlayer->Initialize();

	draw[4].SetCollider(new SphereCollider);
	/*draw[4].SetIsValid(false);*/

	ParticleManager::GetInstance()->Initialize();

	//ステート変更
	ChangeState(new SceneBasic);
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
	draw[5].DrawPera();
}

void Scene::DrawSprite()
{
	state->DrawSprite();

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}


