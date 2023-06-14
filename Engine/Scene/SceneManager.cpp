#include "SceneManager.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "FbxLoader.h"
#include "JsonLevelLoader.h"


//---------------------------------------------------------------------------------------
//デストラクタ
SceneManager::~SceneManager()
{
	state_->Finalize();
	camera.reset();
	//音データ解放
}

void SceneManager::ChangeScene(std::string sceneName)
{
	assert(sceneFactory_);

	if (state_) {
		state_->Finalize();
		state_.reset();
	}
	//シーンファクトリーでシーン生成
	state_ = sceneFactory_->CreateScene(sceneName);
	state_->SetScene(this);
	state_->Initialize();
}

void SceneManager::StopWaveAllScene()
{
	Sound::GetInstance().StopWave("Stage_BGM.wav");
}

void SceneManager::Initialize()
{
	Sound::GetInstance().LoadWave("Stage_BGM.wav",false);

	//json
	JsonLevelLoader::Getinstance().Initialize();
	JsonLevelLoader::Getinstance().LoadJsonFile("untitled");

	{
		TextureManager::LoadGraph(L"Resources/image/loading.png", texhandle[3]);
		//白い画像
		TextureManager::LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().whiteTexHandle);

		//画像
		TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);

		TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle[1]);
		TextureManager::LoadGraph(L"Resources/image/a.png", texhandle[2]);
		TextureManager::LoadGraph(L"Resources/image/test.jpg", texhandle[4]);
	}

	//fbx読み込み
	modelFBX = ModelManager::GetInstance().LoadModelFBX("boneTest");

	draw[5].SetScale({ 1.3f,1.3f,1.3f });
	draw[5].SetTrans({ fighterPos[0],fighterPos[1] - 1.0f,fighterPos[2] });
	draw[5].SetRot({ -3.14f / 2.0f,0,0 });
	draw[5].CulcWorldMat();
	
	draw[6].SetScale({ 1.3f,1.3f,1.3f });
	draw[6].SetTrans({ -20,0,0 });
	draw[6].SetRot({ 0,0,0 });
	draw[6].CulcWorldMat();
	
	//インスタンス生成
	lightManager = std::move(LightManager::Create());
	//ライト色を設定
	lightManager->SetDirLightColor(0, { 1,1,1 });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Object::SetLight(lightManager.get());
	lightManager->SetDirLightActive(0, true);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetDirLightDir(0, { 0, 0, 1.0 });
	//点光源
	for (int32_t i = 0; i < 6; i++)
	{
		lightManager->SetPointLightActive(i, false);
	}
	//丸影
	lightManager->SetCircleShadowActive(0, false);

	//カメラ
	camera = std::make_unique<Camera>();
	camera->Initialize();

	Sound::GetInstance().PlayWave("Stage_BGM.wav");
}

void SceneManager::Update()
{
	lightManager->Update();

	state_->Update();

	//#ifdef _DEBUG
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

	lightManager->SetAmbientColor({ ambientColor[0],ambientColor[1], ambientColor[2] });
	lightManager->SetDiffuseColor({ diffuseColor[0],diffuseColor[1], diffuseColor[2] });
	lightManager->SetSpecularColor({ specularColor[0],specularColor[1], specularColor[2] });
}

void SceneManager::Draw()
{
	state_->Draw();
}

void SceneManager::DrawPostEffect()
{
	state_->DrawPostEffect();
}

void SceneManager::DrawPostEffect2()
{
	state_->DrawPostEffect2();
}

void SceneManager::DrawSprite()
{
	state_->DrawSprite();

	debugText.DrawAll(debugTextHandle);
}

void SceneManager::DrawImgui()
{
	ImGui::Begin("LightColor");

	ImGui::SliderFloat3("ambientColor", ambientColor, 0, 1.0f);
	ImGui::SliderFloat3("diffuseColor", diffuseColor, 0, 1.0f);
	ImGui::SliderFloat3("specularColor", specularColor, 0, 1.0f);

	ImGui::End();

	state_->DrawImgui();
}


