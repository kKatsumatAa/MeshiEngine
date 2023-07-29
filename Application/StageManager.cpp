#include "StageManager.h"


StageManager& StageManager::GetInstance()
{
	static StageManager sInst;
	return sInst;
}

void StageManager::LoadStage(int32_t stageIndex)
{
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
}


void StageManager::Update()
{
	//レベルデータで読み込んだオブジェクト等
	LevelManager::GetInstance().Update();

	//弾
	BulletManager::GetInstance().Update();

	//判定
	CollisionManager::GetInstance()->CheckAllCollisions();

	//ゲームスピード
	GameVelocityManager::GetInstance().Update();

	//チュートリアル
	Tutorial::GetInstance().Update();
}

void StageManager::Draw()
{
	//弾
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

	ParticleManager::GetInstance()->Draw();

	//チュートリアル
	Tutorial::GetInstance().Draw();

	//クリア演出
	if (LevelManager::GetInstance().GetGameClear())
	{
		ClearEffect::GetInstance().Draw();
	}
}

void StageManager::DrawSprite()
{
}

void StageManager::DrawImGui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
}
