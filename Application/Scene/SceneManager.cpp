#include "SceneManager.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "FbxLoader.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"
#include "ClearEffectState.h"
#include "SceneTransitionManager.h"
#include "SceneTransitionEffectState.h"
#include "StageSelect.h"


//---------------------------------------------------------------------------------------
//デストラクタ
SceneManager::~SceneManager()
{
	state_->Finalize();
}

void SceneManager::SetNextScene(std::string sceneName)
{
	nextScene_ = sceneFactory_->CreateScene(sceneName);

	//遷移演出開始
	ChangeScene();
}


void SceneManager::ChangeScene()
{
	if (nextScene_)
	{
		//シーン遷移演出時に非同期でロード
		SceneTransitionManager::GetInstance().BeginSceneTransition([=]()
			{
				if (state_) {
					state_->Finalize();
					state_.reset();
				}
				//シーンファクトリーでシーン生成
				state_ = std::move(nextScene_);
				state_->SetSceneManager(this);
				state_->Initialize();
				//画像アップロード
				DirectXWrapper::GetInstance().UpLoadTexture();
				//更新処理
				UpdateInternal();
			}
		);
	}
}

void SceneManager::StopWaveAllScene()
{
}

void SceneManager::UpdateInternal()
{
	//objマネージャ
	ObjectManager::GetInstance().Update();

	//オブジェマネージャーのデバッグ用アップデート
	ObjectManager::GetInstance().DebugUpdate();

	//カメラマネージャー
	CameraManager::GetInstance().Update();
}


//---------------------------------------------------------------------------------------------------
void SceneManager::Initialize()
{
	TextureManager::Initialize();

	texhandle_[1] = TextureManager::LoadGraph("effect1.png");

	//インスタンス生成
	lightManager_ = std::move(LightManager::Create());
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	IObject3D::SetLight(lightManager_.get());
	//レベルマネージャーにも
	LevelManager::GetInstance().SetLightManager(lightManager_.get());
	//パーティクルマネージャにも
	ParticleManager::GetInstance()->SetLightManager(lightManager_.get());

	//シーン遷移マネージャ
	SceneTransitionManager::GetInstance().Initialize();

	//画像アップロード
	DirectXWrapper::GetInstance().UpLoadTexture();
}

void SceneManager::Update()
{
	//シーン遷移マネージャ
	SceneTransitionManager::GetInstance().Update();

	//シーン遷移演出終わったら
	if (!SceneTransitionManager::GetInstance().GetIsDoingEffect() && state_)
	{
		UpdateInternal();
		state_->Update();
	}

	lightManager_->Update();
}

void SceneManager::DrawShadow()
{
	//ロードしてなければ描画だけ
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		//objマネージャ
		ObjectManager::GetInstance().DrawShadow();
		state_->DrawShadow();
	}
}

void SceneManager::Draw()
{
	//ロードしてなければ描画だけ
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->Draw();
		//objマネージャ
		ObjectManager::GetInstance().Draw();
		//パーティクル
		ParticleManager::GetInstance()->Draw();
	}
}

void SceneManager::DrawSprite()
{
	//ロードしてなければ描画だけ
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->DrawSprite();
	}

	//シーン遷移マネージャ
	SceneTransitionManager::GetInstance().Draw();
}

void SceneManager::DrawImgui()
{
	//ロードしてなければ
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->DrawImgui();
		//objマネージャ
		ObjectManager::GetInstance().DrawImGui();
		//light
		lightManager_->DrawImGui();
	}
}


