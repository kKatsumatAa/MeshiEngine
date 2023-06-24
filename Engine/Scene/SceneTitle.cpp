#include "SceneTitle.h"
#include "SceneManager.h"


void SceneTitle::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneTitle::Initialize()
{
	sceneM_->StopWaveAllScene();
}

void SceneTitle::Update(PostPera* postPera)
{


	//�V�[���J��
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		sceneM_->ChangeScene("GAME");
	}
}

void SceneTitle::Draw()
{
}

void SceneTitle::DrawSprite()
{
}

void SceneTitle::DrawImgui()
{
}

void SceneTitle::DrawPostEffect()
{
}

void SceneTitle::DrawPostEffect2()
{
}

