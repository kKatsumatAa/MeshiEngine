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

	//initialize�̓x,���������Ⴄ�Ƃ��������Ȃ�
	{
		objAndModels_.clear();

		//���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
		for (auto& objData : JsonLevelLoader::Getinstance().levelData_->objects)
		{
			//�t�@�C��������o�^�ς݃��f��������
			Model* model = ModelManager::GetInstance().LoadModel(objData->fileName);
			//���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr <Object> newObj = std::make_unique<Object>();
			//worldmat
			newObj->SetWorldMat_(*objData->worldMat.get());

			//�Z�b�g�œo�^
			objAndModels_.insert(std::make_pair(std::move(newObj), model));
		}
	}

}

void SceneGame::Update()
{
	//ParticleManager::GetInstance()->GenerateRandomParticle(2, 30, 1.0f, { 0,30,0 }, 100.0f, 0);
	//ParticleManager::GetInstance()->Update(&sceneM->camera->viewMat_, &sceneM->camera->projectionMat_);

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
	for (std::map<std::unique_ptr<Object>, std::unique_ptr<Model>>::iterator it = objAndModels_.begin(); it != objAndModels_.end(); it++)
	{
		Object* obj = it->first.get();
		Model* model = it->second.get();

		obj->DrawModel(&sceneM_->camera_->viewMat_, &sceneM_->camera_->projectionMat_, model);
	}

	sceneM_->draw_[6].DrawSphere(&sceneM_->camera_->viewMat_, &sceneM_->camera_->projectionMat_);

	//�Ō�ɕ`�悵�Ȃ��Ɖf��Ȃ�
	//ParticleManager::GetInstance()->Draw(sceneM->texhandle[1]);

	sceneM_->draw_[5].DrawFBX(&sceneM_->camera_->viewMat_,&sceneM_->camera_->projectionMat_,sceneM_->modelFBX_,{10.5f,10.5f,10.5f,10.0f});
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
