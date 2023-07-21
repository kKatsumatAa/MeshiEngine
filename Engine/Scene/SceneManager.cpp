#include "SceneManager.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "FbxLoader.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"


//---------------------------------------------------------------------------------------
//デストラクタ
SceneManager::~SceneManager()
{
	state_->Finalize();
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
	//Sound::GetInstance().StopWave("Stage_BGM.wav");
}

void SceneManager::Initialize()
{
	//Sound::GetInstance().LoadWave("Stage_BGM.wav", false);

	{
		TextureManager::LoadGraph(L"loading.png", texhandle_[3]);
		//白い画像
		TextureManager::LoadGraph(L"white.png", TextureManager::GetInstance().sWhiteTexHandle_);

		//画像
		TextureManager::LoadGraph(L"ascii.png", debugTextHandle_);

		TextureManager::LoadGraph(L"effect1.png", texhandle_[1]);
	}

	//インスタンス生成
	lightManager_ = std::move(LightManager::Create());
	//ライト色を設定
	lightManager_->SetDirLightColor(0, { 0.6f,0.6f,0.6f });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Object::SetLight(lightManager_.get());
	lightManager_->SetDirLightActive(0, true);
	lightManager_->SetDirLightActive(1, false);
	lightManager_->SetDirLightActive(2, false);
	Vec3 v = { 0, -1.0f, 1.0f };
	v.Normalized();
	lightManager_->SetDirLightDir(0, { v.x_,v.y_,v.z_ });
	//点光源
	for (int32_t i = 0; i < 6; i++)
	{
		lightManager_->SetPointLightActive(i, false);
	}
	//丸影
	lightManager_->SetCircleShadowActive(0, false);
}

void SceneManager::Update()
{
	lightManager_->Update();

	state_->Update();

	lightManager_->SetAmbientColor({ ambientColor_[0],ambientColor_[1], ambientColor_[2] });
	lightManager_->SetDiffuseColor({ diffuseColor_[0],diffuseColor_[1], diffuseColor_[2] });
	lightManager_->SetSpecularColor({ specularColor_[0],specularColor_[1], specularColor_[2] });
}

void SceneManager::Draw()
{
	state_->Draw();

	ParticleManager::GetInstance()->Draw(/*texhandle_[1]*/);
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


