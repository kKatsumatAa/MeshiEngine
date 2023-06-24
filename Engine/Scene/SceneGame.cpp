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

void SceneGame::Update(PostPera* postPera)
{
	/*MouseInput::GetInstance().GetClick(CLICK_LEFT);
	MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT);
	MouseInput::GetInstance().GetTriggerReleaseClick(CLICK_WHEEL);*/


	LevelManager::GetInstance().Update();

	//ƒV[ƒ“‘JˆÚ
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	LevelManager::GetInstance().Draw();

	//sceneM_->draw_[6].DrawModel(sceneM_->model_[6]);

	//ÅŒã‚É•`‰æ‚µ‚È‚¢‚Æ‰f‚ç‚È‚¢
	//ParticleManager::GetInstance()->Draw(sceneM->texhandle[1]);

	Vec2 pos = MouseInput::GetInstance().GetCurcorPos();
	sceneM_->draw_[0].DrawBoxSprite(pos, 0.1f, { 1.0f,1.0f,1.0f,1.0f }, NULL, { 0.5f,0.5f });

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
