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

	//�񓯊�����(�X�e�[�W�쐬���ɂ����[�h��ʏo���I��)
	async_.StartAsyncFunction([=]() { Load(); });
}

void SceneLoad::Update(PostPera* postPera)
{


	////�V�[���J��
	//if (async.GetLockFlag())
	//{
	//	async.EndThread();

	//	//�X�e�[�W���I�������
	//	sceneM->ChangeState(new Scene5);
	//}
}

void SceneLoad::Draw()
{
}

void SceneLoad::DrawSprite()
{
	count_++;
	loadObj_.DrawBoxSprite({ 0,0 + sinf(count_ * 0.1f) * 3.0f }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, sceneM_->texhandle_[3]);
}

void SceneLoad::DrawImgui()
{
}

void SceneLoad::DrawPostEffect()
{
}

void SceneLoad::DrawPostEffect2()
{
}
