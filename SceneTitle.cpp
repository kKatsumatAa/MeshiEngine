#include "SceneTitle.h"
#include "SceneManager.h"


void SceneTitle::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneTitle::Initialize()
{
	sceneM->StopWaveAllScene();
}

void SceneTitle::Update()
{


	//ƒV[ƒ“‘JˆÚ
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		sceneM->ChangeState(new SceneGame);
	}
}

void SceneTitle::Draw()
{
}

void SceneTitle::DrawSprite()
{
	sceneM->draw[0].DrawBoxSprite({ 0,WindowsApp::GetInstance().window_height / 2.0f,0 },
		0.5f, { 1.0f,1.0f,1.0f,1.0f }, sceneM->texhandle[2]);
	sceneM->draw[1].DrawBoxSprite({ WindowsApp::GetInstance().window_width - 300,
		WindowsApp::GetInstance().window_height / 2.0f,0 },
		1.0f, { 1.0f,1.0f,1.0f,1.0f }, sceneM->texhandle[1]);

	sceneM->debugText.Print("[Basic]", 10, 10);
}

