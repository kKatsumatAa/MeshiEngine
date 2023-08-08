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

	Sound::Initialize();

	Object::DrawInitialize();

	//�|�X�g�G�t�F�N�g
	PostEffectManager::GetInstance().Initialize("Resources/image/normalImage.jpg");

	//�L�[�{�[�h���͏�����
	KeyboardInput::GetInstance();
	//true�ɂ��邱�ƂŃ}�E�X���ق��̃A�v���Ŏg�p����Ȃ�
	MouseInput::GetInstance().CreateDevice(true);

	//
	ParticleManager::GetInstance()->Initialize();

	//Light
	LightManager::StaticInitialize();

	//model
	Model::StaticInitialize();

	//�V�[��
	sceneM_ = &SceneManager::GetInstance();
	sceneM_->Initialize();

	imguiM_ = std::make_unique<ImGuiManager>();
	imguiM_->Initialize();
}

void Framework::Finalize()
{
	//���f���S�Ă����
	ModelManager::GetInstance().Finalize();

	//FBX
	FbxLoader::GetInstance()->Finalize();

	//�E�B���h�E�N���X��o�^����
	WindowsApp::GetInstance().UnregisterClassA();

	imguiM_->Finalize();

	MouseInput::GetInstance().Finalize();
	PadInput::GetInstance().Finalize();
}

void Framework::Update()
{
	//�I��
	if (WindowsApp::GetInstance().MessegeRoop(msg_) || KeyboardInput::GetInstance().KeyPush(DIK_ESCAPE))
	{
		isEndGame_ = true;
	}

	//�L�[�{�[�h���̎擾�J�n
	KeyboardInput::GetInstance().Update();
	PadInput::GetInstance().Update();
	MouseInput::GetInstance().Update();

	//�X�V����
	sceneM_->Update();

	//�|�X�g�G�t�F�N�g
	PostEffectManager::GetInstance().Update();
}

void Framework::Run()
{
	//�Q�[���̏�����
	Initialize();

	//�Q�[�����[�v
	while (!isEndGame_) {

		//�Q�[���̍X�V
		Update();

		//�Q�[���̕`��
		Draw();
	}

	//�Q�[���I��
	Finalize();
}
