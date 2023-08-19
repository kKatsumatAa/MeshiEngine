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
			});
	}
}

void SceneManager::StopWaveAllScene()
{
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
	lightManager_->SetDirLightActive(0, false);
	lightManager_->SetDirLightActive(1, false);
	lightManager_->SetDirLightActive(2, false);
	Vec3 v = { 0, -1.0f, 1.0f };
	v.Normalized();
	lightManager_->SetDirLightDir(0, { v.x_,v.y_,v.z_ });
	//�_����
	for (int32_t i = 0; i < 6; i++)
	{
		lightManager_->SetPointLightActive(i, false);
	}
	lightManager_->SetPointLightActive(0, true);

	//�ۉe
	lightManager_->SetCircleShadowActive(0, false);

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
		//obj�}�l�[�W��
		ObjectManager::GetInstance().Update();
		state_->Update();

		//�I�u�W�F�}�l�[�W���[�̃f�o�b�O�p�A�b�v�f�[�g
		ObjectManager::GetInstance().DebugUpdate();
	}

	lightManager_->SetAmbientColor({ ambientColor_[0],ambientColor_[1], ambientColor_[2] });
	lightManager_->SetDiffuseColor({ diffuseColor_[0],diffuseColor_[1], diffuseColor_[2] });
	lightManager_->SetSpecularColor({ specularColor_[0],specularColor_[1], specularColor_[2] });
	Vec3 dir = { DirlightDir_[0],DirlightDir_[1], DirlightDir_[2] };
	lightManager_->SetDirLightDir(0, { dir.GetNormalized().x_ ,dir.GetNormalized().y_ ,dir.GetNormalized().z_ });

	lightManager_->SetPointLightAtten(0, { pointAtten_[0],pointAtten_[1],pointAtten_[2] });
	lightManager_->SetPointLightPos(0, { pointPos_[0],pointPos_[1],pointPos_[2] });

	lightManager_->Update();

	//�J�����}�l�[�W���[
	CameraManager::GetInstance().Update();
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
	ImGui::SliderFloat3("DirlightDir", DirlightDir_, -1.0f, 1.0f);
	ImGui::SliderFloat3("PointAtten", pointAtten_, 0.0f, 3.0f);
	ImGui::SliderFloat3("PointPos", pointPos_, -100.0f, 100.0f);

	ImGui::End();

	//���[�h���ĂȂ����
	if (!SceneTransitionManager::GetInstance().GetIsLoadingOnly() && state_)
	{
		state_->DrawImgui();
		LevelManager::GetInstance().DrawImGui();
		//obj�}�l�[�W��
		ObjectManager::GetInstance().DrawImGui();
	}
}


