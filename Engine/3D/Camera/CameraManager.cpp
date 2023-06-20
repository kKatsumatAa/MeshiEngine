#include "CameraManager.h"

CameraManager::CameraManager()
{
	AddCamera("debugCamera");
	GetCamera("debugCamera")->Initialize();

	//��������J�����̃|�C���^�����(MyGame�œ���Ă���)
}

CameraManager::~CameraManager()
{
	cameraAndNames_.clear();
}

void CameraManager::ChangeUsingCameraState(std::unique_ptr<UsingCameraState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetCameraM(this);
}

void CameraManager::Initialize()
{
	lerpCount_ = 0;
	lerpCountMax_ = 0;
	afterCount_ = 0;
	afterCamera_ = nullptr;
	isLerpMoving_ = false;
	isLerpEnd_ = false;

	ChangeUsingCameraState(std::make_unique<UsingCameraNormalState>());
}

void CameraManager::Update()
{
#ifdef _DEBUG
	{
		Camera* debugCamera = GetCamera("debugCamera");

		if (KeyboardInput::GetInstance().KeyTrigger(DIK_C))
		{
			//�f�o�b�O�J�����ɕύX
			if (usingCamera_ != debugCamera)
			{
				afterCamera_ = usingCamera_;
				usingCamera_ = debugCamera;
			}
			else
			{
				usingCamera_ = afterCamera_;
			}
		}

		//�f�o�b�O�J�������L���̎�
		if (usingCamera_ == debugCamera)
		{
			Vec3 rotMove = {
				(KeyboardInput::GetInstance().KeyPush(DIK_W) - KeyboardInput::GetInstance().KeyPush(DIK_S)) * 0.05f,
				(KeyboardInput::GetInstance().KeyPush(DIK_A) - KeyboardInput::GetInstance().KeyPush(DIK_D)) * 0.05f,
				0
			};

			debugWorldMat_.rot_ += rotMove;
			debugWorldMat_.rot_.x_ = min(debugWorldMat_.rot_.x_, PI / 2.0f);
			debugWorldMat_.rot_.x_ = max(debugWorldMat_.rot_.x_, -PI / 2.0f);

			debugWorldMat_.SetWorld();
			debugCamera->viewMat_.eye_ = CAMERA_POS_;
			Vec3xM4(debugCamera->viewMat_.eye_, debugWorldMat_.matWorld_, 0);
			debugCamera->UpdateViewMatrix();
		}
	}
#endif

	if (state_.get())
	{
		state_->Update();
	}

	usingCamera_->Update();
}

void CameraManager::AddCamera(std::string cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//���łɓ������O�̃J��������������G���[
	if (it != cameraAndNames_.end())
	{
		return;
		//assert(false);
	}

	//�Ȃ���Γǂݍ���
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Initialize();

	//�ۑ����Ă���
	cameraAndNames_.insert(std::make_pair(cameraName, std::move(camera)));
}

Camera* CameraManager::GetCamera(std::string cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//�w�肵�����O�̃J����������΃|�C���^��Ԃ�
	if (it != cameraAndNames_.end())
	{
		return it->second.get();
	}

	//�Ȃ����nullptr
	assert(false);
	return nullptr;
}

void CameraManager::SetUsingCamera(std::string cameraName)
{
	//�t�@�C��������T��
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//�w�肵�����O�̃J����������΃|�C���^usingCamera_�Ƀ|�C���^�Z�b�g
	if (it != cameraAndNames_.end())
	{
		usingCamera_ = it->second.get();
		return;
	}

	//�Ȃ����
	assert(false);
}

void CameraManager::BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* afterCamera, int32_t afterCount)
{
	startEye_ = startEye;
	endEye_ = endEye;
	startTarget_ = startTarget;
	endTarget_ = endTarget;
	startUp_ = startUp;
	endUp_ = endUp;
	lerpCountMax_ = time;
	lerpCount_ = 0;
	afterCamera_ = afterCamera;
	afterCount_ = afterCount;
	isLerpMoving_ = true;
	isLerpEnd_ = false;

	ChangeUsingCameraState(std::make_unique<UsingCameraLerpMoveState>());

	Update();
}

//----------------------------------------------------------------------------------------------------------------------------------------
void UsingCameraState::SetCameraM(CameraManager* cameraM)
{
	cameraM_ = cameraM;
}


//-------------------------------------------------------------------------------------------------------
void UsingCameraNormalState::Update()
{
	if (cameraM_->isLerpMoving_)
	{
		cameraM_->ChangeUsingCameraState(std::make_unique<UsingCameraLerpMoveState>());
	}
}

//-------------------------------------------------------------------------------------------------------
void UsingCameraLerpMoveState::Update()
{
	float t = (float)cameraM_->lerpCount_ / (float)cameraM_->lerpCountMax_;

	if (cameraM_->lerpCount_ < cameraM_->lerpCountMax_)
	{
		cameraM_->lerpCount_++;
	}

	cameraM_->usingCamera_->SetEye(LerpVec3(cameraM_->startEye_, cameraM_->endEye_, EaseOut(t)));
	cameraM_->usingCamera_->SetUp(LerpVec3(cameraM_->startUp_, cameraM_->endUp_, EaseOut(t)));
	cameraM_->usingCamera_->SetTarget(LerpVec3(cameraM_->startTarget_, cameraM_->endTarget_, EaseOut(t)));

	//�I������1�t���[���O�ɉ��o�������p
	if (cameraM_->lerpCount_ >= cameraM_->lerpCountMax_ - 1)
	{
		cameraM_->isLerpEnd_ = true;
	}

	if (cameraM_->lerpCount_ >= cameraM_->lerpCountMax_)
	{
		cameraM_->afterCount_--;
		cameraM_->isLerpMoving_ = false;

		if (cameraM_->afterCount_ <= 0)
		{

			if (cameraM_->afterCamera_)
			{
				cameraM_->usingCamera_ = cameraM_->afterCamera_;
			}



			cameraM_->ChangeUsingCameraState(std::make_unique<UsingCameraNormalState>());
		}
	}
}
