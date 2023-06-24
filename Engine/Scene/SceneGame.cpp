#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"


void SceneGame::Finalize()
{

}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	player_ = Player::Create();

	//�J�������Z�b�g
	CameraManager::GetInstance().SetUsingCamera("playerCamera");
}

void SceneGame::Update(PostPera* postPera)
{
	LevelManager::GetInstance().Update();

	//
	player_->Update();

	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	LevelManager::GetInstance().Draw();

	player_->Draw();
}

void SceneGame::DrawSprite()
{
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
