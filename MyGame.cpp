#include "MyGame.h"

void MyGame::Initialize()
{
	//���N���X�̏���������
	Framework::Initialize();
	
	//�Q�[���ŗL�̏�����


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
		scene->Draw();

		//
		DirectXWrapper::GetInstance().DrawUpdate2();
	}

	//�}���`�p�X
	{
		//2����--------------------
		DirectXWrapper::GetInstance().PreDrawToPera();

		//
		scene->DrawPostEffect();

		DirectXWrapper::GetInstance().PostDrawToPera();

	}
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDrawToPera2();

		//
		scene->DrawPostEffect2();

		scene->DrawSprite();

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDrawToPera2();
	}
}
