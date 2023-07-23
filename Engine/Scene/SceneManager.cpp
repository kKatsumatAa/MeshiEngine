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

//---------------------------------------------------------------------------------------
//�f�X�g���N�^
SceneManager::~SceneManager()
{
	state_->Finalize();
}

void SceneManager::SetNextScene(std::string sceneName)
{
	nextScene_ = sceneFactory_->CreateScene(sceneName);
	//���[�f�B���O�t���O���I�t�ɂ���
	if (nextScene_)
	{
		async_.SetIsLoading(false);
	}
}


void SceneManager::ChangeScene()
{
	if (nextScene_)
	{
		//�񓯊��ŏ������֐������s����
		async_.StartAsyncFunction([=]()
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
	//Sound::GetInstance().StopWave("Stage_BGM.wav");
}


//---------------------------------------------------------------------------------------------------
void SceneManager::Initialize()
{
	//Sound::GetInstance().LoadWave("Stage_BGM.wav", false);

	{
		//�摜
		TextureManager::LoadGraph(L"ascii.png", debugTextHandle_);

		//�����摜
		TextureManager::LoadGraph(L"white.png", TextureManager::GetInstance().sWhiteTexHandle_);

		TextureManager::LoadGraph(L"loading.png", loadTexHandle_);

		TextureManager::LoadGraph(L"effect1.png", texhandle_[1]);
	}

	//�C���X�^���X����
	lightManager_ = std::move(LightManager::Create());
	//���C�g�F��ݒ�
	lightManager_->SetDirLightColor(0, { 0.6f,0.6f,0.6f });
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g(�S�̂ň�����L)
	Object::SetLight(lightManager_.get());
	lightManager_->SetDirLightActive(0, true);
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
	//�ۉe
	lightManager_->SetCircleShadowActive(0, false);

	//�摜�A�b�v���[�h
	DirectXWrapper::GetInstance().UpLoadTexture();
}

void SceneManager::Update()
{
	//���̃V�[�����Z�b�g����Ă���΃V�[���؂�ւ��i�񓯊��j
	ChangeScene();

	//���[�h�I������u��
	if (async_.GetFinishedAsync())
	{
		async_.EndThread();
	}
	//���[�h�I�������
	if (!async_.GetIsLoading())
	{
		state_->Update();
	}

	lightManager_->SetAmbientColor({ ambientColor_[0],ambientColor_[1], ambientColor_[2] });
	lightManager_->SetDiffuseColor({ diffuseColor_[0],diffuseColor_[1], diffuseColor_[2] });
	lightManager_->SetSpecularColor({ specularColor_[0],specularColor_[1], specularColor_[2] });

	lightManager_->Update();
}

void SceneManager::Draw()
{
	//���[�h���ĂȂ����
	if (!async_.GetIsLoading())
	{
		state_->Draw();
	}

	ParticleManager::GetInstance()->Draw(/*texhandle_[1]*/);
}

void SceneManager::DrawSprite()
{
	//���[�h���ĂȂ����
	if (!async_.GetIsLoading())
	{
		state_->DrawSprite();
	}
	//���[�h���Ȃ�
	else
	{
		loadCount_++;
		loadObj_.DrawBoxSprite({ 0,0 + sinf(loadCount_ * 0.1f) * 3.0f }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, loadTexHandle_);
	}

	debugText_.DrawAll(debugTextHandle_);
}

void SceneManager::DrawImgui()
{
	ImGui::Begin("LightColor");

	ImGui::SliderFloat3("ambientColor", ambientColor_, 0, 1.0f);
	ImGui::SliderFloat3("diffuseColor", diffuseColor_, 0, 1.0f);
	ImGui::SliderFloat3("specularColor", specularColor_, 0, 1.0f);

	ImGui::End();

	//���[�h���ĂȂ����
	if (!async_.GetIsLoading())
	{
		state_->DrawImgui();
	}
}


