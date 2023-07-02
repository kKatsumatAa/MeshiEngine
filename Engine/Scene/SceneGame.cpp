#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//json
	JsonLevelLoader::Getinstance().Initialize();
	//レベルマネージャー
	LevelManager::GetInstance().StaticInitialize();
	LevelManager::GetInstance().LoadLevelData("level");

	//カメラをセット
	CameraManager::GetInstance().SetUsingCamera("playerCamera");

	//ゲームスピード
	GameVelocityManager::GetInstance().Initialize();

	//弾
	BulletManager::GetInstance().Initialize();

	//
	ClearEffect::GetInstance().Initialize();
}

void SceneGame::Update(PostPera* postPera)
{

#ifdef _DEBUG
	//リセット
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		LevelManager::GetInstance().LoadLevelData("level");
		ParticleManager::GetInstance()->ClearParticles();
		//弾
		BulletManager::GetInstance().Initialize();
		//ゲームスピード
		GameVelocityManager::GetInstance().Initialize();
	}
#endif

	//レベルデータで読み込んだオブジェクト等
	LevelManager::GetInstance().Update();

	//弾
	BulletManager::GetInstance().Update();

	//判定
	CollisionManager::GetInstance()->CheckAllCollisions();

	//ゲームスピード
	GameVelocityManager::GetInstance().Update();

	ClearEffect::GetInstance().Update();

	//シーン遷移
	if (LevelManager::GetInstance().GetGameOver())
	{
		sceneM_->ChangeScene("TITLE");
	}
	else if (LevelManager::GetInstance().GetGameClear())
	{
		clearCoolTime_++;

		ClearEffect::GetInstance().Update();

		//連打して飛ばさないように
		if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) && clearCoolTime_ >= CLEAR_COOL_TIME_MAX_)
		{
			sceneM_->ChangeScene("TITLE");
		}
	}
}

void SceneGame::Draw()
{
	//ゲームスピード
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();

	if (LevelManager::GetInstance().GetGameClear())
	{
		ClearEffect::GetInstance().Draw();
	}
}

void SceneGame::DrawSprite()
{

}

void SceneGame::DrawImgui()
{
	GameVelocityManager::GetInstance().UpdateImGui();
}

void SceneGame::DrawPostEffect()
{
	//postPera.Draw();
}

void SceneGame::DrawPostEffect2()
{
	//postPera.Draw2All();
}
