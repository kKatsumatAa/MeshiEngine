#pragma once
#include"Camera.h"
#include"Camera2D.h"
#include"WorldMat.h"



class CameraManager
{
private:
	//�f�o�b�O�p
	WorldMat debugWorldMat_;
	Vec3 cameraPos_ = { 0,0,-100 };
	bool isDebugCamera_ = false;

	//�J�����Ɩ��O���Z�b�g�Ŏ����Ă���
	std::map< std::string, std::unique_ptr<Camera>> cameraAndNames_;
	//2D�p�̃J����
	std::map< std::string, std::unique_ptr<Camera2D>> camera2DAndNames_;
	//�J���������ɐ؂�ւ��邽�߂̃C�e���[�^
	std::map< std::string, std::unique_ptr<Camera>>::iterator cameraItr_;
	std::map< std::string, std::unique_ptr<Camera2D>>::iterator camera2DItr_;

public:
	//�����ɂ����ȃJ�����̃|�C���^�����āA�`�掞�Ɏg��
	Camera* usingCamera_;
	Camera2D* usingCamera2D_;

	//���o�Ȃǂ��I�������ɂ��̃J�����ɐ؂�ւ��邽�߂̃|�C���^
	Camera* afterCamera_ = nullptr;

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

	void Initialize();

	void Update();

	void ImguiUpdate();

public:
	//�J������V���ɒǉ�����
	void AddCamera(const std::string& cameraName);
	void AddCamera2D(const std::string& cameraName);
	//���O���w�肵�ăJ�������擾
	Camera* GetCamera(const std::string& cameraName);
	//usingCamera�ɃZ�b�g����Ă�J�����擾
	Camera* GetCamera();
	//���O���w�肵�ăJ�������擾
	Camera2D* GetCamera2D(const std::string& cameraName);
	//usingCamera�ɃZ�b�g����Ă�J�����擾
	Camera2D* GetCamera2D();
	//�J�������g�p���Ă���J�����ɃZ�b�g
	void SetUsingCamera(const std::string& cameraName);
	void SetUsingCamera2D(const std::string& cameraName);

public:
	bool GetIsDebugCamera() { return isDebugCamera_; }
};