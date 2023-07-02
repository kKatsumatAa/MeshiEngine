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

	postPera_[1]->effectFlags_.isBarrelCurve = true;
	postPera_[0]->effectFlags_.isBloom = true;
	postPera_[0]->effectFlags_.isVignette = true;
}

void MyGame::Finalize()
{
	//�Q�[���ŗL�̏I������


	//���N���X�̏I������
	Framework::Finalize();
}

void MyGame::Update()
{
	//���N���X�̍X�V����
	Framework::Update();

	//�Q�[���ŗL�̍X�V����
		//�p�[�e�B�N��
	ParticleManager::GetInstance()->Update();
}

void MyGame::Draw()
{
	//1���ڂɕ`��
	{
		std::function<void()>f = [=]() {sceneM_->Draw(); };
		postPera_[0]->DrawToPostpera(f);
		postPera_[0]->DrawShrinkTextureForBlur();
		//�ꖇ�ڂɕ`�挋�ʁA�񖇖ڂ��`�悷��
		std::function<void()>f2 = [=]() { postPera_[0]->Draw2();  };
		postPera_[1]->DrawToPostpera(f2);
	}

	//�u���[���p
	{
		postPera_[1]->DrawShrinkTextureForBlur();
	}

	//���ۂɕ`��
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDraw();

		//���ۂɕ`��
		postPera_[1]->Draw2();

		sceneM_->DrawSprite();

		imguiM_->Draw();

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
