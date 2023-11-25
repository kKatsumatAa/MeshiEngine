#include "StageManager.h"
#include "StageState.h"
#include "PostEffectManager.h"


StageManager& StageManager::GetInstance()
{
	static StageManager sInst;
	return sInst;
}

void StageManager::LoadStage(int32_t stageIndex)
{
	//プレイヤーui
	PlayerUI::GetInstance().Initialize();

	//json
	JsonLevelLoader::Getinstance().Initialize();
	//レベルマネージャー
	LevelManager::GetInstance().StaticInitialize();
	LevelManager::GetInstance().LoadLevelData(stageIndex);

	//チュートリアルも読み込み
	Tutorial::GetInstance().LoadTutorialData(stageIndex);

	//初期化
	StageManager::Initialize();
}

void StageManager::Initialize()
{
	//カメラをセット
	CameraManager::GetInstance().SetUsingCamera("playerCamera");
	//ライトカメラ
	CameraManager::GetInstance().SetUsingLightCamera("lightCamera");
	//ゲームスピード
	GameVelocityManager::GetInstance().Initialize();
	//弾
	BulletManager::GetInstance().Initialize();

	//敵マネージャー
	EnemyManager::GetInstance().Initialize();

	//フラグ
	isClear_ = false;
	isGameOver_ = false;

	//海用
	seaDistance_ = { 0,SEA_DICTANCE_TMP_,0 };

	//
	ChangeState("BEGINING");

	//更新
	Update();
}


void StageManager::ApproachLava()
{
	//位置
	float slowParam = 1.0f - GameVelocityManager::GetInstance().GetVelocity();
	if (ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "player").size())
	{
		auto player = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "player")[0];
		//距離を減らしていく
		seaDistance_ -= Vec3(0, slowParam, 0) / StageManager::GetInstance().LAVA_APPROACH_TIME_;
		seaDistance_.y = max(seaDistance_.y, -player->GetScale().y * 2.0f);
	}
	//使用されてるカメラに距離を足して海までの距離とする
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaCameraPos =
		CameraManager::GetInstance().GetCamera()->GetEye() + seaDistance_;
	//強さも変える
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaTimerExtend = 2.5f * (1.0f - seaDistance_.y / SEA_DICTANCE_TMP_);
}

void StageManager::ChangeState(const std::string& name)
{
	state_.reset();
	state_ = std::move(StageState::GetState(name));
	state_->Initialize();
}

void StageManager::Update()
{
	state_->Update();
}

void StageManager::DrawShadow()
{
	state_->DrawShadow();
}

void StageManager::Draw()
{
	state_->Draw();
}

void StageManager::DrawSprite()
{
	state_->DrawSprite();
}

void StageManager::DrawImGui()
{
	state_->DrawImgui();
}
