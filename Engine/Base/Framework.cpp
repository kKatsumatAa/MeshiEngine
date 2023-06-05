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

	DrawInitialize();

	//�|�X�g�G�t�F�N�g//
	postPera[0] = std::make_unique<PostPera>();
	postPera[1] = std::make_unique<PostPera>();
	postPera[0]->Initialize(L"Resources/image/normalImage.jpg");
	postPera[1]->Initialize(L"Resources/image/normalImage.jpg");

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

	imguiM = new ImGuiManager();
	imguiM->Initialize();
}

void Framework::Finalize()
{
	//�V�[���t�@�N�g�����
	delete sceneFactory_;

	//FBX
	FbxLoader::GetInstance()->Finalize();

	//�E�B���h�E�N���X��o�^����
	WindowsApp::GetInstance().UnregisterClassA();

	imguiM->Finalize();
	delete imguiM;
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

	//imgui
	imguiM->Begin();

	//�|�X�g�G�t�F�N�g(imgui)
	for (std::unique_ptr<PostPera>& postpera : postPera)
	{
		postpera->Update();
	}
	//(imgui)
	Object::StaticUpdate();
	//(imgui)
	sceneM->DrawImgui();

	imguiM->End();

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
