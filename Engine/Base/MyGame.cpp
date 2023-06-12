#include "MyGame.h"
#include"SceneFactory.h"


void MyGame::Initialize()
{
	//���N���X�̏���������
	Framework::Initialize();

	//�Q�[���ŗL�̏�����

	//�V�[���t�@�N�g���[�𐶐����A�}�l�[�W���[�ɃZ�b�g
	sceneFactory_ = new SceneFactory();
	sceneM->SetSceneFactory(sceneFactory_);
	sceneM->ChangeScene("GAME");

	postPera[0]->effectFlags.isBloom = true;
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
		std::function<void()>f = [=]() {sceneM->Draw(); };
		//�ꖇ�ڂɕ`�挋�ʁA�񖇖ڂ��`�悷��
		postPera[0]->DrawToPostpera(f);
		//std::function<void()>f2 = [=]() {postPera[0]->Draw2(); };
		//postPera[1]->DrawToPostpera(f2);
	}

	//�u���[���p
	{
		postPera[0]->DrawShrinkTextureForBlur();
	}

	//���ۂɕ`��
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDraw();

		//
		//sceneM->DrawPostEffect2();
		//���ۂɕ`��
		postPera[0]->Draw2();

		sceneM->DrawSprite();

		imguiM->Draw();

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
