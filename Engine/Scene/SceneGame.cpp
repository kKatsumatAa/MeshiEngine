#include "SceneGame.h"
#include "SceneManager.h"


void SceneGame::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	Sound::GetInstance().PlayWave("Stage_BGM.wav", 0.4f, true);

	sceneM->draw[5].PlayReverseAnimation(sceneM->modelFBX, true);

	//ポストエフェクト初期化
	//postPera.Initialize(L"Resources/image/normalImage.jpg");
}

void SceneGame::Update()
{
	//ImGui::ShowDemoWindow();

	//postPera.Update();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	for (int i = 0; i < 2; i++)
	{
		sceneM->draw[i].DrawModel(sceneM->draw[i].worldMat, &sceneM->camera->viewMat,
			&sceneM->camera->projectionMat, sceneM->model[i]);
	}
	sceneM->draw[2].DrawModel(sceneM->draw[2].worldMat, &sceneM->camera->viewMat,
		&sceneM->camera->projectionMat, sceneM->model[2]);
	//sceneM->draw[3].DrawSphere(sceneM->draw[3].worldMat, &sceneM->camera->viewMat,
	//	&sceneM->camera->projectionMat, { 1.0f,0.5f,0.2f,0.8f });
	//sceneM->draw[4].DrawCube3D(sceneM->draw[4].worldMat, &sceneM->camera->viewMat,
	//	&sceneM->camera->projectionMat, { 1.0f,0.2f,0.7f,1.0f });

	sceneM->draw[5].DrawFBX(sceneM->draw[5].worldMat, &sceneM->camera->viewMat,
		&sceneM->camera->projectionMat, sceneM->modelFBX, { 1.0f,0.2f,0.7f,1.0f });
}

void SceneGame::DrawSprite()
{
	sceneM->debugText.Print("[1]", 10, 10);
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
