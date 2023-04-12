#include "MyGame.h"

void MyGame::Initialize()
{
	//�R���\�[���ւ̕�������
	OutputDebugStringA("Hello,DirectX!!\n");

	//FBX
	FbxLoader::GetInstance()->Initialize();

	//������
	WindowsApp::GetInstance();
	DirectXWrapper::GetInstance().Initialize();

	DrawInitialize();	

	//�L�[�{�[�h���͏�����
	KeyboardInput::GetInstance();

	//�V�[��
	scene = new Scene();
	scene->Initialize();
}

void MyGame::Finalize()
{
	delete scene;

	//FBX
	FbxLoader::GetInstance()->Finalize();


	//�E�B���h�E�N���X��o�^����
	WindowsApp::GetInstance().UnregisterClassA();
}

bool MyGame::Update()
{
	if (WindowsApp::GetInstance().MessegeRoop(msg))
	{
		return true;
	}

	//���t���[�������@��������//
	//�L�[�{�[�h���̎擾�J�n
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();

	//�X�V����
	scene->Update();


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
