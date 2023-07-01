#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"


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

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	//ゲームスピード
	BulletManager::GetInstance().Draw();

	LevelManager::GetInstance().Draw();
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
