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

	//�|�X�g�G�t�F�N�g������
	//postPera.Initialize(L"Resources/image/normalImage.jpg");

	//initialize�̓x,���������Ⴄ�Ƃ��������Ȃ�
	{
		objAndModels.clear();

		//���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
		for (auto& objData : JsonLevelLoader::Getinstance().levelData->objects)
		{
			//�t�@�C��������o�^�ς݃��f��������
			Model* model = ModelManager::GetInstance().LoadModel(objData.fileName);
			//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			Object* newObj = new Object;
			//worldmat
			newObj->worldMat = objData.worldMat;

			//�Z�b�g�œo�^
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

	//�V�[���J��
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

	//�Ō�ɕ`�悵�Ȃ��Ɖf��Ȃ�
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
