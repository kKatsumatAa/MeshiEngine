#include "SceneGame.h"
#include "SceneManager.h"
#include "JsonLevelLoader.h"


void SceneGame::Finalize()
{
}

//---------------------------------------------------------------------------------------
void SceneGame::Initialize()
{
	//Sound::GetInstance().PlayWave("Stage_BGM.wav", 0.4f, true);

	sceneM->draw[5].PlayReverseAnimation(sceneM->modelFBX, true);

	//ポストエフェクト初期化
	//postPera.Initialize(L"Resources/image/normalImage.jpg");

	//initializeの度,毎回やっちゃうとおかしくなる
	{
		objAndModels.clear();

		//レベルデータからオブジェクトを生成、配置
		for (auto& objData : JsonLevelLoader::Getinstance().levelData->objects)
		{
			//ファイル名から登録済みモデルを検索
			Model* model = ModelManager::GetInstance().LoadModel(objData.fileName);
			//モデルを指定して3Dオブジェクトを生成
			Object* newObj = new Object;
			//worldmat
			newObj->worldMat = objData.worldMat;

			//セットで登録
			objAndModels.insert(std::make_pair(newObj, model));
		}
	}

}

void SceneGame::Update()
{
	//ParticleManager::GetInstance()->GenerateRandomParticle(2, 30, 1.0f, { 0,30,0 }, 100.0f, 0);
	//ParticleManager::GetInstance()->Update(&sceneM->camera->viewMat, &sceneM->camera->projectionMat);

	//ImGui::ShowDemoWindow();

	//postPera.Update();

	//シーン遷移
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_SPACE) || PadInput::GetInstance().GetTriggerButton(GAMEPAD_A))
	{
		//sceneM->ChangeScene("TITLE");
	}
}

void SceneGame::Draw()
{
	for (std::map<Object*, Model*>::iterator it = objAndModels.begin(); it != objAndModels.end(); it++)
	{
		Object* obj = it->first;
		Model* model = it->second;

		obj->DrawModel(obj->worldMat, &sceneM->camera->viewMat, &sceneM->camera->projectionMat, model);
	}

	//最後に描画しないと映らない
	//ParticleManager::GetInstance()->Draw(sceneM->texhandle[1]);
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
