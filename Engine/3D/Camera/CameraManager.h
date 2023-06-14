#pragma once
#include"Camera.h"

class CameraManager;

class UsingCameraState
{
protected:
	CameraManager* cameraM = nullptr;

public:
	void SetCameraM(CameraManager* cameraM);
	virtual void Update() = 0;
};

class UsingCameraNormalState : public UsingCameraState
{
private:


public:
	void Update() override;
};

class UsingCameraLerpMoveState : public UsingCameraState
{
private:

public:
	void Update() override;
};


class CameraManager
{
private:
	std::unique_ptr<UsingCameraState> state_;

public:
	//�����ɂ����ȃJ�����̃|�C���^�����āA�`�掞�Ɏg��
	Camera* usingCamera;
	//�I����ʂŎg��
	std::unique_ptr<Camera> stageSelectCamera;
	//�Q�[�������C���Ŏg���J����
	std::unique_ptr<Camera> gameMainCamera;
	//�Q�[�����A��]���Ɏg���J����
	std::unique_ptr<Camera> gameTurnCamera;
	//�S�[�����o���Ɏg���J����
	std::unique_ptr<Camera> goalEffectCamera;

	//���o�Ȃǂ��I�������ɂ��̃J�����ɐ؂�ւ��邽�߂̃|�C���^
	Camera* afterCamera = nullptr;
	//�I�������A�J�����ɐ؂�ւ���܂ł̃t���[����
	int32_t afterCount = 0;

	//lerp�p
	int32_t lerpCount = 0;
	int32_t lerpCountMax = 0;
	Vec3 startEye, endEye, startTarget, endTarget, startUp, endUp;

	bool isLerpMoving = false;
	bool isLerpEnd = false;

public://�֐�
	CameraManager();
	~CameraManager();

	void ChangeUsingCameraState(std::unique_ptr<UsingCameraState> state);

	void Initialize();

	void Update();

	//�ړ��̐��`�⊮���n�߂�
	void BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* afterCamera = nullptr, int32_t afterCount = 0);


	CameraManager& operator=(const CameraManager& obj);
};