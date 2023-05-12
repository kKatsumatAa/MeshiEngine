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
		postPera->PreDraw();

		// 4.�`��R�}���h��������@//-----------
		sceneM->Draw();

		//
		postPera->PostDraw();
	}

	//�}���`�p�X
	{
		//2����--------------------
		postPera->Draw2All();
	}

	//���ۂɕ`��
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDraw();

		//
		//sceneM->DrawPostEffect2();
		//���ۂɕ`��
		postPera->Draw2();

		sceneM->DrawSprite();

		imguiM->Draw();

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
