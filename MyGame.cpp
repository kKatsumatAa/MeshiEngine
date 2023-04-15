#include "MyGame.h"

void MyGame::Initialize()
{
	//���N���X�̏���������
	Framework::Initialize();
	
	//�Q�[���ŗL�̏�����

	//�V�[��
	sceneM->ChangeState(new SceneTitle);

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
	{
		DirectXWrapper::GetInstance().DrawUpdate();

		// 4.�`��R�}���h��������@//-----------
		sceneM->Draw();

		//
		DirectXWrapper::GetInstance().DrawUpdate2();
	}

	//�}���`�p�X
	{
		//2����--------------------
		DirectXWrapper::GetInstance().PreDrawToPera();

		//
		sceneM->DrawPostEffect();

		DirectXWrapper::GetInstance().PostDrawToPera();

	}
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDrawToPera2();

		//
		sceneM->DrawPostEffect2();

		sceneM->DrawSprite();

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDrawToPera2();
	}
}
