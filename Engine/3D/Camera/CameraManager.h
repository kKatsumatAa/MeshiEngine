#pragma once
#include"Camera.h"
#include"WorldMat.h"

class CameraManager;

class UsingCameraState
{
protected:
	CameraManager* cameraM_ = nullptr;

public:
	virtual ~UsingCameraState() { ; }

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

	//�f�o�b�O�p
	WorldMat debugWorldMat_;
	Vec3 cameraPos_ = { 0,0,-100 };
	bool isDebugCamera_ = false;

	//�J�����Ɩ��O���Z�b�g�Ŏ����Ă���
	std::map< std::string, std::unique_ptr<Camera>> cameraAndNames_;
	//�J���������ɐ؂�ւ��邽�߂̃C�e���[�^
	std::map< std::string, std::unique_ptr<Camera>>::iterator cameraItr_;

public:
	//�����ɂ����ȃJ�����̃|�C���^�����āA�`�掞�Ɏg��
	Camera* usingCamera_;

	//���o�Ȃǂ��I�������ɂ��̃J�����ɐ؂�ւ��邽�߂̃|�C���^
	Camera* afterCamera_ = nullptr;
	//�I�������A�J�����ɐ؂�ւ���܂ł̃t���[����
	int32_t afterCount_ = 0;

	//lerp�p
	int32_t lerpCount_ = 0;
	int32_t lerpCountMax_ = 0;
	Vec3 startEye_, endEye_, startTarget_, endTarget_, startUp_, endUp_;

	bool isLerpMoving_ = false;
	bool isLerpEnd_ = false;

private://�֐�
	CameraManager();
	~CameraManager();

private:
	//�J���������ɐ؂�ւ���
	void ChangeCamera();

public:
	CameraManager(const CameraManager& other) = delete;
	CameraManager& operator=(const CameraManager& other) = delete;

	static CameraManager& GetInstance() { static CameraManager sInst; return sInst; }

	void ChangeUsingCameraState(std::unique_ptr<UsingCameraState> state);

	void Initialize();

	void Update();

	void ImguiUpdate();

public:
	//�ړ��̐��`�⊮���n�߂�
	void BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* afterCamera = nullptr, int32_t afterCount = 0);

	//�J������V���ɒǉ�����
	void AddCamera(std::string cameraName);
	//���O���w�肵�ăJ�������擾
	Camera* GetCamera(std::string cameraName);
	//usingCamera�ɃZ�b�g����Ă�J�����擾
	Camera* GetCamera();
	//�J�������g�p���Ă���J�����ɃZ�b�g
	void SetUsingCamera(std::string cameraName);

public:
	bool GetIsDebugCamera() { return isDebugCamera_; }
};