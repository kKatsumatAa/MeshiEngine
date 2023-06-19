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
	camera_.reset();
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
		TextureManager::LoadGraph(L"Resources/image/loading.png", texhandle_[3]);
		//白い画像
		TextureManager::LoadGraph(L"Resources/image/white.png", TextureManager::GetInstance().sWhiteTexHandle_);

		//画像
		TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle_);

		TextureManager::LoadGraph(L"Resources/image/effect1.png", texhandle_[1]);
		TextureManager::LoadGraph(L"Resources/image/a.png", texhandle_[2]);
		TextureManager::LoadGraph(L"Resources/image/test.jpg", texhandle_[4]);
	}

	//fbx読み込み
	modelFBX_ = ModelManager::GetInstance().LoadModelFBX("boneTest");

	model_[6] = ModelManager::GetInstance().LoadModel("chr_sword");

	draw_[5].SetScale({ 1.3f,1.3f,1.3f });
	draw_[5].SetTrans({ fighterPos_[0],fighterPos_[1] - 1.0f,fighterPos_[2] });
	draw_[5].SetRot({ -3.14f / 2.0f,0,0 });
	draw_[5].CulcWorldMat();
	
	draw_[6].SetScale({ 2.3f,2.3f,2.3f });
	draw_[6].SetTrans({ -20,0,0 });
	draw_[6].SetRot({ 0,0,0 });
	draw_[6].CulcWorldMat();
	
	//インスタンス生成
	lightManager_ = std::move(LightManager::Create());
	//ライト色を設定
	lightManager_->SetDirLightColor(0, { 1,1,1 });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Object::SetLight(lightManager_.get());
	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightActive(1, false);
	lightManager_->SetDirLightActive(2, false);
	lightManager_->SetDirLightDir(0, { 0, 0, 1.0 });
	//点光源
	for (int32_t i = 0; i < 6; i++)
	{
		lightManager_->SetPointLightActive(i, false);
	}
	//丸影
	lightManager_->SetCircleShadowActive(0, false);

	//カメラ
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	Sound::GetInstance().PlayWave("Stage_BGM.wav");
}

void SceneManager::Update()
{
	lightManager_->Update();

	state_->Update();

	//#ifdef _DEBUG
	{
		cameraWorldMat_.rot_.y_ += (KeyboardInput::GetInstance().KeyPush(DIK_A) - KeyboardInput::GetInstance().KeyPush(DIK_D)) * 0.05f;

		cameraWorldMat_.rot_.x_ += (KeyboardInput::GetInstance().KeyPush(DIK_W) - KeyboardInput::GetInstance().KeyPush(DIK_S)) * 0.05f;
		cameraWorldMat_.rot_.x_ = min(cameraWorldMat_.rot_.x_, PI / 2.0f);
		cameraWorldMat_.rot_.x_ = max(cameraWorldMat_.rot_.x_, -PI / 2.0f);

		cameraWorldMat_.SetWorld();
		camera_->viewMat_.eye_ = CAMERA_POS_;
		Vec3xM4(camera_->viewMat_.eye_, cameraWorldMat_.matWorld_, 0);
		camera_->UpdateViewMatrix();
	}
	//#endif

	lightManager_->SetAmbientColor({ ambientColor_[0],ambientColor_[1], ambientColor_[2] });
	lightManager_->SetDiffuseColor({ diffuseColor_[0],diffuseColor_[1], diffuseColor_[2] });
	lightManager_->SetSpecularColor({ specularColor_[0],specularColor_[1], specularColor_[2] });
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

	debugText_.DrawAll(debugTextHandle_);
}

void SceneManager::DrawImgui()
{
	ImGui::Begin("LightColor");

	ImGui::SliderFloat3("ambientColor", ambientColor_, 0, 1.0f);
	ImGui::SliderFloat3("diffuseColor", diffuseColor_, 0, 1.0f);
	ImGui::SliderFloat3("specularColor", specularColor_, 0, 1.0f);

	ImGui::End();

	state_->DrawImgui();
}


