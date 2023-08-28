#include "CameraManager.h"
#include "MouseInput.h"
#include "ImGuiManager.h"

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
	cameraAndNames_.clear();
}

void CameraManager::ChangeCamera()
{
	if (KeyboardInput::GetInstance().KeyTrigger(DIK_V))
	{
		cameraItr_++;

		//�Ō�܂ōs������
		if (cameraItr_ == cameraAndNames_.end())
		{
			cameraItr_ = cameraAndNames_.begin();
		}

		//�J�����|�C���^���Z�b�g
		usingCamera_ = cameraItr_->second.get();
	}
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

	cameraAndNames_.clear();

#ifdef _DEBUG

	AddCamera("debugCamera");
	GetCamera("debugCamera")->Initialize();

#endif

	//�J���������ɐ؂�ւ��邽�߂̃C�e���[�^
	cameraItr_ = cameraAndNames_.begin();

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
				isDebugCamera_ = true;
				afterCamera_ = usingCamera_;
				usingCamera_ = debugCamera;
			}
			else if (afterCamera_)
			{
				isDebugCamera_ = false;
				usingCamera_ = afterCamera_;
			}
		}

		//�f�o�b�O�J�������L���̎�
		if (usingCamera_ == debugCamera)
		{

			if (MouseInput::GetInstance().GetClick(CLICK_RIGHT))
			{
				Vec2 vel = MouseInput::GetInstance().GetCursorVelocity();

				//��]
				Vec3 rotMove = {
					vel.y_ * 0.01f,
					vel.x_ * 0.01f,
					0
				};

				debugWorldMat_.rot_ += rotMove;
				debugWorldMat_.rot_.x_ = min(debugWorldMat_.rot_.x_, PI / 2.0f);
				debugWorldMat_.rot_.x_ = max(debugWorldMat_.rot_.x_, -PI / 2.0f);
			}

			cameraPos_.z_ = min(cameraPos_.z_ + (float)MouseInput::GetInstance().GetWheelAmountOfRot() * 0.02f, -1.0f);
			cameraPos_.z_ = max(cameraPos_.z_ + (float)MouseInput::GetInstance().GetWheelAmountOfRot() * 0.02f, -1000.0f);

			debugWorldMat_.CalcWorldMat();
			Vec3 pos = cameraPos_;
			Vec3xM4(pos, debugWorldMat_.matWorld_, 0);
			debugCamera->SetEye(debugCamera->GetTarget() + pos);

			debugCamera->UpdateViewMatrix();
		}
	}
#endif
	//�J�����؂�ւ�
	ChangeCamera();


	if (state_.get())
	{
		state_->Update();
	}

	usingCamera_->Update();
}

void CameraManager::ImguiUpdate()
{
	ImGui::Begin("Camera");
	//ImGui::char
	ImGui::Begin("Camera");
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

Camera* CameraManager::GetCamera()
{
	return usingCamera_;
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
