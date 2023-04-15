#include "Framework.h"

void Framework::Initialize()
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

	ParticleManager::GetInstance()->Initialize();

	//Light
	LightManager::StaticInitialize();

	//model
	Model::StaticInitialize();

	//�V�[��
	sceneM = &SceneManager::GetInstance();
	sceneM->Initialize();
}

void Framework::Finalize()
{
	//�V�[���t�@�N�g�����
	delete sceneFactory_;

	//FBX
	FbxLoader::GetInstance()->Finalize();

	//�E�B���h�E�N���X��o�^����
	WindowsApp::GetInstance().UnregisterClassA();
}

bool Framework::Update()
{
	if (WindowsApp::GetInstance().MessegeRoop(msg))
	{
		return true;
	}

	//�L�[�{�[�h���̎擾�J�n
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();

	//�X�V����
	sceneM->Update();

    return false;
}

void Framework::Run()
{
    //�Q�[���̏�����
    Initialize();

	//�Q�[�����[�v
	while (true) {

		//�Q�[���̍X�V
		if (Update())
		{
			break;
		}

		//�Q�[���̕`��
		Draw();
	}

	//�Q�[���I��
	Finalize();
}
