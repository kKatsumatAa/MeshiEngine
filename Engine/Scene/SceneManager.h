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



class CollisionManager;
class Player;

class SceneManager final
{
private:
	//�V�[���̏�ԁi�s���j
	SceneState* state = nullptr;
	//�V�[���t�@�N�g���[�i�|�C���^���؂��j
	AbstractSceneFactory* sceneFactory_ = nullptr;

public:
	WorldMat cameraWorldMat;
	const Vec3 cameraPos = { 0,0,-100 };

	Model* model[20];

	Object draw[10];

	ModelFBX* modelFBX;

	DebugText debugText;

	//�f�o�b�O�e�L�X�g
	UINT64 debugTextHandle;
	UINT64 texhandle[10];

	//�摜�p�n���h��
	UINT64 textureHandle[30] = { 0 };

	//�����̉摜
	UINT64 textureNumHundle[12];

	//���C�g
	LightManager* lightManager = nullptr;

	float ambientColor[3] = { 0,0,1 };
	float diffuseColor[3] = { 1,0,0 };
	float specularColor[3] = { 0,1,0 };

	//�ۉe
	float circleShadowDir[3] = { 0,-1,0 };
	float circleShadowAtten[3] = { 0.5f,0.15f,0 };
	float circleShadowFactorAngle[2] = { 0,5.0f };
	float circleShadowDistance = 100.0f;

	float fighterPos[3] = { 1.0f,0,0 };


	std::unique_ptr<Camera> camera;



private:
	~SceneManager();
	SceneManager() { ; }

public:
	static SceneManager& GetInstance() { static SceneManager inst; return inst; }
	//�R�s�[�R���X�g���N�^�𖳌�
	SceneManager(const SceneManager& obj) = delete;
	//������Z�q��
	SceneManager& operator=(const SceneManager& obj) = delete;

public:
	//�V�[���X�e�[�g�ύX
	void ChangeScene(std::string sceneName);

	//�V�[���t�@�N�g���[�̃Z�b�^�[
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory_ = sceneFactory; }

	void Initialize();
	void Update();
	void Draw();
	void DrawPostEffect();
	void DrawPostEffect2();
	void DrawSprite();

	void DrawImgui();

	void StopWaveAllScene();
};