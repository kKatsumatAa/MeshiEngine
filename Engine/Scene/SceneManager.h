#pragma once
#include"Collision.h"
#include"DebugText.h"
#include"ImGuiManager.h"
#include"ParticleManager.h"
#include"Camera.h"
#include"Async.h"
#include "PadInput.h"
#include "SceneState.h"
#include "AbstractSceneFactory.h"
#include "ModelManager.h"
#include "LevelManager.h"
#include "MouseInput.h"
#include "PostEffectManager.h"



class CollisionManager;
class Player;

class SceneManager final
{
private:
	//�V�[���̏�ԁi�s���j
	std::unique_ptr<SceneState> state_;
	std::unique_ptr<SceneState> nextScene_;
	//�V�[���t�@�N�g���[�i�|�C���^���؂��j
	AbstractSceneFactory* sceneFactory_ = nullptr;

private:
	uint64_t texhandle_[10];

	//�����̉摜
	uint64_t textureNumHundle_[12];

	//���C�g
	std::unique_ptr<LightManager> lightManager_;

	float ambientColor_[3] = { 1,1,1 };
	float diffuseColor_[3] = { 1,1,1 };
	float specularColor_[3] = { 1,1,1 };

	std::unique_ptr<Camera> camera_;

	//�Q�[�����I��������
	bool isEscapingGame_ = false;

private:
	~SceneManager();
	SceneManager() { ; }

public:
	static SceneManager& GetInstance() { static SceneManager sInst; return sInst; }
	//�R�s�[�R���X�g���N�^�𖳌�
	SceneManager(const SceneManager& obj) = delete;
	//������Z�q��
	SceneManager& operator=(const SceneManager& obj) = delete;

	//�V�[���X�e�[�g�ύX
	void ChangeScene();

public:
	//���̃V�[���Z�b�g
	void SetNextScene(std::string sceneName);

	//�V�[���t�@�N�g���[�̃Z�b�^�[
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

	void Initialize();
	void Update();
	void Draw();
	void DrawSprite();

	void DrawImgui();

	void StopWaveAllScene();

public:
	void SetIsEscapingGame(bool is) { isEscapingGame_ = is; }
	bool GetIsEscapingGame() { return isEscapingGame_; }

};