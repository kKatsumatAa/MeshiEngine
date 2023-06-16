#pragma once
#include"Camera.h"

class CameraManager;

class UsingCameraState
{
protected:
	CameraManager* pCameraM_ = nullptr;

public:
	void SetCameraM(CameraManager* pCameraM);
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
	Camera* pUsingCamera_;
	//�I����ʂŎg��
	std::unique_ptr<Camera> stageSelectCamera_;
	//�Q�[�������C���Ŏg���J����
	std::unique_ptr<Camera> gameMainCamera_;
	//�Q�[�����A��]���Ɏg���J����
	std::unique_ptr<Camera> gameTurnCamera_;
	//�S�[�����o���Ɏg���J����
	std::unique_ptr<Camera> goalEffectCamera_;

	//���o�Ȃǂ��I�������ɂ��̃J�����ɐ؂�ւ��邽�߂̃|�C���^
	Camera* pAfterCamera_ = nullptr;
	//�I�������A�J�����ɐ؂�ւ���܂ł̃t���[����
	int32_t afterCount_ = 0;

	//lerp�p
	int32_t lerpCount_ = 0;
	int32_t lerpCountMax_ = 0;
	Vec3 startEye_, endEye_, startTarget_, endTarget_, startUp_, endUp_;

	bool isLerpMoving_ = false;
	bool isLerpEnd_ = false;

public://�֐�
	CameraManager();
	~CameraManager();

	void ChangeUsingCameraState(std::unique_ptr<UsingCameraState> state);

	void Initialize();

	void Update();

	//�ړ��̐��`�⊮���n�߂�
	void BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget,
		const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* pAfterCamera = nullptr, int32_t afterCount = 0);


	CameraManager& operator=(const CameraManager& obj);
};