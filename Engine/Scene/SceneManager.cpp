#include "SceneManager.h"
#include <sstream>
#include <iomanip>
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "FbxLoader.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"
#include "ClearEffectState.h"
#include "SceneTransitionManager.h"
#include "SceneTransitionEffectState.h"
#include "StageSelect.h"


//---------------------------------------------------------------------------------------
//�f�X�g���N�^
SceneManager::~SceneManager()
{
	state_->Finalize();
}

void SceneManager::SetNextScene(std::string sceneName)
{
	nextScene_ = sceneFactory_->CreateScene(sceneName);

	//�J�ډ��o�J�n
	ChangeScene();
}


void SceneManager::ChangeScene()
{
	if (nextScene_)
	{
		//�V�[���J�ډ��o���ɔ񓯊��Ń��[�h
		SceneTransitionManager::GetInstance().BeginSceneTransition([=]()
			{
				if (state_) {
					state_->Finalize();
					state_.reset();
				}
				//�V�[���t�@�N�g���[�ŃV�[������
				state_ = std::move(nextScene_);
				state_->SetSceneManager(this);
				state_->Initialize();
				//�摜�A�b�v���[�h
				DirectXWrapper::GetInstance().UpLoadTexture();
				//�X�V����
				UpdateInternal();
			});
	}
}

void SceneManager::StopWaveAllScene()
{
}

void SceneManager::UpdateInternal()
{
	//obj�}�l�[�W��
	ObjectManager::GetInstance().Update();

	//�I�u�W�F�}�l�[�W���[�̃f�o�b�O�p�A�b�v�f�[�g
	ObjectManager::GetInstance().DebugUpdate();

	//�J�����}�l�[�W���[
	CameraManager::GetInstance().Update();
}


//---------------------------------------------------------------------------------------------------
void SceneManager::Initialize()
{
	TextureManager::Initialize();

	texhandle_[1] = TextureManager::LoadGraph("effect1.png");

	//�C���X�^���X����
	lightManager_ = std::move(LightManager::Create());
	//���C�g�F��ݒ�
	lightManager_->SetDirLightColor(0, { 0.6f,0.6f,0.6f });
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g(�S�̂ň�����L)
	Object::SetLight(lightManager_.get());
	//���x���}�l�[�W���[�ɂ�
	LevelManager::GetInstance().SetLightManager(lightManager_.get());

	//�V�[���J�ڃ}�l�[�W��
	SceneTransitionManager::GetInstance().Initialize();

	//�摜�A�b�v���[�h
	DirectXWrapper::GetInstance().UpLoadTexture();
}

void SceneManager::Update()
{
	//�V�[���J�ڃ}�l�[�W��
	SceneTransitionManager::GetInstance().Update();

	//�V�[���J�ډ��o�I�������
	if (!SceneTransitionManager::GetInstance().GetIsDoingEffect() && state_)
	{
		UpdateInternal();
		state_->Update();
	}

	lightManager_->SetAmbientColor({ ambientColor_[0],ambientColor_[1], ambientColor_[2] });
	lightManager_->SetDiffuseColor({ diffuseColor_[0],diffuseColor_[1], diffuseColor_[2] });
	lightManager_->SetSpecularColor({ specularColor_[0],specularColor_[1], specularColor_[2] });

	lightManager_->Update();
}

void SceneManager::Draw()
{
	//���[�h���ĂȂ���Ε`�悾��
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->Draw();
		//obj�}�l�[�W��
		ObjectManager::GetInstance().Draw();
		//�p�[�e�B�N��
		ParticleManager::GetInstance()->Draw();
	}
}

void SceneManager::DrawSprite()
{
	//���[�h���ĂȂ���Ε`�悾��
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->DrawSprite();
	}

	//�V�[���J�ڃ}�l�[�W��
	SceneTransitionManager::GetInstance().Draw();
}

void SceneManager::DrawImgui()
{
	ImGui::Begin("LightColor");

	ImGui::SliderFloat3("ambientColor", ambientColor_, 0, 1.0f);
	ImGui::SliderFloat3("diffuseColor", diffuseColor_, 0, 1.0f);
	ImGui::SliderFloat3("specularColor", specularColor_, 0, 10.0f);

	ImGui::End();

	//���[�h���ĂȂ����
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->DrawImgui();
		LevelManager::GetInstance().DrawImGui();
		//obj�}�l�[�W��
		ObjectManager::GetInstance().DrawImGui();
		//light
		lightManager_->DrawImGui();
	}
}


