#include "MyGame.h"
#include"SceneFactory.h"
#include"GameVelocityManager.h"


void MyGame::Initialize()
{
	//���N���X�̏���������
	Framework::Initialize();

	//�Q�[���ŗL�̏�����
	CameraManager::GetInstance().Initialize();
	CameraManager::GetInstance().AddCamera("camera");
	CameraManager::GetInstance().SetUsingCamera("camera");

	//�V�[���t�@�N�g���[�𐶐����A�}�l�[�W���[�ɃZ�b�g
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneM_->SetSceneFactory(sceneFactory_.get());
	sceneM_->Initialize();
	sceneM_->SetNextScene("TITLE");
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
	ParticleManager::GetInstance()->Update(GameVelocityManager::GetInstance().GetVelocity());
}

void MyGame::Draw()
{
	//�|�X�g�y���ɕ`�悷��
	{
		std::function<void()>f = [=]() {sceneM_->Draw(); };
		PostEffectManager::GetInstance().BeforeDraw(f);
	}

	//���ۂɕ`��
	{
		//���ۂɕ`��----------------
		DirectXWrapper::GetInstance().PreDraw();

		//�|�X�g�y�����ۂɕ`��
		PostEffectManager::GetInstance().DrawDisplay();

		sceneM_->DrawSprite();

#ifdef _DEBUG
		{
			//imgui
			imguiM_->Begin();


			//(imgui)
			sceneM_->DrawImgui();
			//�|�X�g�G�t�F�N�g
			PostEffectManager::GetInstance().ImGuiUpdate();


			imguiM_->End();

			imguiM_->Draw();
		}
#endif // DEBUG

		// 4.�`��R�}���h�����܂� //
		DirectXWrapper::GetInstance().PostDraw();
	}
}
