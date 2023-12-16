#include "SceneGame.h"
#include "SceneManager.h"
#include "StageManager.h"
#include "StageState.h"
#include "StageSelect.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//リプレイ
	replay_ = std::make_unique<Replay>();
	replay_->Initialize();

	//ステージ読み込み
	StageManager::GetInstance().LoadStage(StageSelect::GetInstance().GetSelectStageIndex(),replay_.get());

	//クリア演出初期化
	ClearEffect::GetInstance().Initialize();
	//パーティクルの種類をセット
	ParticleManager::GetInstance()->SetAmbient(PARTICLE_AMBIENT_);
	ParticleManager::GetInstance()->SetDiffuse(PARTICLE_DIFFUSE_);
	ParticleManager::GetInstance()->SetSpecular(PARTICLE_SPECULAR_);
	ParticleManager::GetInstance()->ClearParticles();

	//ポストエフェクト
	PostEffectManager::GetInstance().GetPostEffect3()->effectFlags_.isBarrelCurve = false;
}

void SceneGame::Update()
{
	//リセット
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		sceneM_->SetNextScene("GAME");
	}

	//ステージアップデート
	StageManager::GetInstance().Update();

	//ゲームスピード
	GameVelocityManager::GetInstance().Update();

	//シーン遷移
	if (StageManager::GetInstance().GetIsGameOver())
	{
		sceneM_->SetNextScene("GAME");
	}
	else if (StageManager::GetInstance().GetIsGameClear() || KeyboardInput::GetInstance().KeyTrigger(DIK_ESCAPE))
	{
		ParticleManager::GetInstance()->ClearParticles();
		sceneM_->SetNextScene("STAGESELECT");
	}
}

void SceneGame::DrawShadow()
{
	StageManager::GetInstance().DrawShadow();
}

void SceneGame::Draw()
{
	StageManager::GetInstance().Draw();
}

void SceneGame::DrawSprite()
{
	StageManager::GetInstance().DrawSprite();
}

void SceneGame::DrawImgui()
{
	StageManager::GetInstance().DrawImGui();
}
