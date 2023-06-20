#include "MyGame.h"
#include"SceneFactory.h"


void MyGame::Initialize()
{
	//���N���X�̏���������
	Framework::Initialize();

	//�Q�[���ŗL�̏�����

	//�V�[���t�@�N�g���[�𐶐����A�}�l�[�W���[�ɃZ�b�g
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneM_->SetSceneFactory(sceneFactory_.get());
	sceneM_->ChangeScene("GAME");

	CameraManager::GetInstance().AddCamera("mainCamera");
	CameraManager::GetInstance().SetUsingCamera("mainCamera");
}

void MyGame::Finalize()
{
	//�Q�[���ŗL�̏I������


	//���N���X�̏I������
	Framework::Finalize();
}

bool MyGame::Update()
{
	//���N���X�̍X�V����
	if (Framework::Update())
	{
		return true;
	}

	//�Q�[���ŗL�̍X�V����


	return false;
}

void MyGame::Draw()
{
	//1���ڂɕ`��
	{
		std::function<void()>f = [=]() {sceneM_->Draw(); };
		//�ꖇ�ڂɕ`�挋�ʁA�񖇖ڂ��`�悷��
		postPera_[0]->DrawToPostpera(f);
	}

	//�u���[���p
	{
		postPera_[0]->DrawShrinkTextureForBlur();
	}

	//���ۂɕ`��
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDraw();

		//���ۂɕ`��
		postPera_[0]->Draw2();

		sceneM_->DrawSprite();

		imguiM_->Draw();

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
