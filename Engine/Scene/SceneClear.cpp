#include "SceneClear.h"
#include "SceneManager.h"


void SceneClear::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneClear::Initialize()
{
	sceneM_->StopWaveAllScene();
	ParticleManager::GetInstance()->ClearParticles();
}

void SceneClear::Update()
{
	//ƒV[ƒ“‘JˆÚ
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
	{
		sceneM_->SetNextScene("TITLE");
	}
}

void SceneClear::Draw()
{
}

void SceneClear::DrawSprite()
{
}

void SceneClear::DrawImgui()
{
}

