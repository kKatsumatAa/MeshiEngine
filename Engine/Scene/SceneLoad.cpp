#include "SceneLoad.h"
#include "SceneManager.h"


//--------------------------------------------------------------------------------------
void SceneLoad::Load()
{

}

void SceneLoad::Finalize()
{
}

void SceneLoad::Initialize()
{
	////�񓯊�����(�X�e�[�W�쐬���ɂ����[�h��ʏo���I��)
	//async_.StartAsyncFunction([=]() { sceneM_->GetLoadFunc(); });
}

void SceneLoad::Update()
{
	////�V�[���J��
	//if (async_.GetLockFlag())
	//{
	//	async_.EndThread();

	//	//���[�h�I�������
	//	sceneM_->ChangeScene(sceneM_->GetAfterLoadNextSceneName());
	//}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
}

void SceneLoad::DrawImgui()
{
}
