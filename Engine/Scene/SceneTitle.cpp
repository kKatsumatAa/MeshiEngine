#include "SceneTitle.h"
#include "SceneManager.h"


void SceneTitle::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneTitle::Initialize()
{
	sceneM_->StopWaveAllScene();
	ParticleManager::GetInstance()->ClearParticles();
}

void SceneTitle::Update(PostPera* postPera)
{
	//�V�[���J��
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
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

