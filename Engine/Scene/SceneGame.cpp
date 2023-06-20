#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	sceneM_->draw_[5].PlayReverseAnimation(sceneM_->modelFBX_, true);
}

void SceneGame::Update()
{
	LevelManager::GetInstance().Update();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	LevelManager::GetInstance().Draw();

	sceneM_->draw_[6].DrawModel(sceneM_->model_[6]);

	//最後に描画しないと映らない
	//ParticleManager::GetInstance()->Draw(sceneM->texhandle[1]);

	sceneM_->draw_[5].DrawFBX(sceneM_->modelFBX_, nullptr, { 10.5f,10.5f,10.5f,10.0f });
}

void SceneGame::DrawSprite()
{
	sceneM_->debugText_.Print("[1]", 10, 10);
}

void SceneGame::DrawImgui()
{
}

void SceneGame::DrawPostEffect()
{
	//postPera.Draw();
}

void SceneGame::DrawPostEffect2()
{
	//postPera.Draw2All();
}
