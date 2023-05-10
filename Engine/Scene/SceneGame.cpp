#include "SceneGame.h"
#include "SceneManager.h"


void SceneGame::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//Sound::GetInstance().PlayWave("Stage_BGM.wav", 0.4f, true);

	sceneM->draw[5].PlayReverseAnimation(sceneM->modelFBX, true);

	//Object::effectFlags.isGrayScale = true;
}

void SceneGame::Update()
{
	ImGui::ShowDemoWindow();

	ImGui::BeginPopupContextWindow("PostEffect");
	ImGui::SliderInt("GrayScale", (int*)&Object::effectFlags.isGrayScale, 0, 1);
	ImGui::SliderInt("Emboss", (int*)&Object::effectFlags.isEmboss, 0, 1);
	ImGui::SliderInt("Gaussian", (int*)&Object::effectFlags.isGaussian, 0, 1);
	ImGui::SliderInt("GlassFilter", (int*)&Object::effectFlags.isGlassFilter, 0, 1);
	ImGui::SliderInt("ScanningLine", (int*)&Object::effectFlags.isScanningLine, 0, 1);
	ImGui::SliderInt("Fog", (int*)&Object::effectFlags.isFog, 0, 1);
	ImGui::SliderInt("BarrelCurve", (int*)&Object::effectFlags.isBarrelCurve, 0, 1);
	ImGui::SliderInt("Gradation", (int*)&Object::effectFlags.isGradation, 0, 1);
	ImGui::SliderInt("Outline", (int*)&Object::effectFlags.isOutLine, 0, 1);
	ImGui::SliderInt("Sharpness", (int*)&Object::effectFlags.isSharpness, 0, 1);
	ImGui::SliderInt("Vignette", (int*)&Object::effectFlags.isVignette, 0, 1);


	sceneM->imGuiManager->End();

	//ƒV[ƒ“‘JˆÚ
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	/*for (int i = 0; i < 2; i++)
	{
		sceneM->draw[i].DrawModel(sceneM->draw[i].worldMat, &sceneM->camera->viewMat,
			&sceneM->camera->projectionMat, sceneM->model[i]);
	}*/
	sceneM->draw[2].DrawModel(sceneM->draw[2].worldMat, &sceneM->camera->viewMat,
		&sceneM->camera->projectionMat, sceneM->model[3]);
	sceneM->draw[3].DrawSphere(sceneM->draw[3].worldMat, &sceneM->camera->viewMat,
		&sceneM->camera->projectionMat, { 1,1,1,1 }, sceneM->texhandle[4]);
	//sceneM->draw[4].DrawCube3D(sceneM->draw[4].worldMat, &sceneM->camera->viewMat,
	//	&sceneM->camera->projectionMat, { 1.0f,0.2f,0.7f,1.0f });

	//sceneM->draw[5].DrawFBX(sceneM->draw[5].worldMat, &sceneM->camera->viewMat,
	//	&sceneM->camera->projectionMat, sceneM->modelFBX, { 1.0f,0.2f,0.7f,1.0f });
}

void SceneGame::DrawSprite()
{
	sceneM->debugText.Print("[1]", 10, 10);
}