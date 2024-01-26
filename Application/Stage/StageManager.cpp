#include "StageManager.h"
#include "StageState.h"
#include "PostEffectManager.h"
#include "Player.h"

using namespace Util;


StageManager& StageManager::GetInstance()
{
	static StageManager sInst;
	return sInst;
}

void StageManager::LoadStage(int32_t stageIndex, const std::string& stateName)
{
	stageNum_ = stageIndex;

	//プレイヤーui
	PlayerUI::GetInstance().Initialize();

	//json
	JsonLevelLoader::Getinstance().Initialize();
	//レベルマネージャー
	LevelManager::GetInstance().LoadLevelData(stageIndex);

	//チュートリアルも読み込み
	Tutorial::GetInstance().LoadTutorialData(stageIndex);

	//初期化
	StageManager::Initialize(stateName);
}

void StageManager::LoadStage(const std::string& stateName)
{
	LoadStage(stageNum_, stateName);
}

void StageManager::Initialize(const std::string& stateName)
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
	seaMaxAfterTime_ = SEA_MAX_AFTER_TIME_;

	//ステート
	ChangeState(stateName);

	//更新
	Update();
}

void StageManager::InitializeReplay()
{
	//リプレイ
	replay_ = std::make_unique<Replay>();
	replay_->Initialize();
}


void StageManager::ApproachLava()
{
	//位置
	float slowParam = GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_ - GameVelocityManager::GetInstance().GetVelocity();
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
	float t = SEA_DISTANCE_RATIO_MAX_ - seaDistance_.y / SEA_DICTANCE_TMP_;
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaTimerExtend = POST_SEA_TIME_EXTEND_RATE_ * t;

	if (t >= SEA_DISTANCE_RATIO_MAX_)
	{
		AfterLavaMaxUpdate();
	}
}

void StageManager::AfterLavaMaxUpdate()
{
	seaMaxAfterTime_ = max(--seaMaxAfterTime_, 0);

	float t = SEA_AFTER_TIME_RATIO_MAX_ - (float)seaMaxAfterTime_ / (float)SEA_MAX_AFTER_TIME_;

	//ステージや壁等のオブジェクト取得
	auto stages = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);

	//徐々にディゾルブ
	for (auto landShape : stages)
	{
		landShape->SetDissolveRatio(EaseIn(t));

		//時間終わったら消す
		if (t >= ERASE_LAND_SHAPE_SEA_TIME_RATIO_)
		{
			landShape->SetIsAlive(false);
		}
	}
}

void StageManager::ChangeState(const std::string& name)
{
	if (name.size())
	{
		state_.reset();
		state_ = std::move(StageState::GetState(name));
		state_->Initialize();
	}
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
	ImGui::Begin("stage");
	ImGui::DragFloat("seaDistance", &seaDistance_.y, IMGUI_SEA_DIST_DRAG_SPEED_);
	ImGui::End();

	state_->DrawImgui();
}
