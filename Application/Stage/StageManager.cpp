#include "StageManager.h"
#include "StageState.h"


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
	//ゲームスピード
	GameVelocityManager::GetInstance().Initialize();
	//弾
	BulletManager::GetInstance().Initialize();

	//敵マネージャー
	EnemyManager::GetInstance().Initialize();

	//フラグ
	isClear_ = false;
	isGameOver_ = false;

	//
	ChangeState("BEGINING");
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
