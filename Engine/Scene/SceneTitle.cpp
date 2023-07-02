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

	TextureManager::LoadGraph(L"Resources/image/title.png", titleTex_);
}

void SceneTitle::Update(PostPera* postPera)
{
	//������
	postPera[0].effectFlags_.isScanningLine = true;

	//�^�C�g�����
	if (t_ < 1.0f)
	{
		t_ = (float)titleTime_ / (float)TITLE_TIME_MAX_;

		titleScale_ = LerpVec3({ TITLE_SCALE_MAX_,0,0 }, { 1.0f,0,0 }, EaseOut(t_)).x_;

		titleTime_++;
	}

	//�V�[���J��
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || KeyboardInput::GetInstance().KeyTrigger(DIK_R))
	{
		postPera[0].effectFlags_.isScanningLine = false;

		sceneM_->ChangeScene("GAME");
	}
}

void SceneTitle::Draw()
{
	titleObj_.DrawBoxSprite({ WindowsApp::GetInstance().WINDOW_WIDTH_ / 2.0f,WindowsApp::GetInstance().WINDOW_HEIGHT_ / 2.0f },
		titleScale_, { 1.0f,1.0f,1.0f,1.0f }, titleTex_, { 0.5f,0.5f });
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

