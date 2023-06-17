#include "CameraManager.h"

CameraManager::CameraManager()
{
	stageSelectCamera_ = std::make_unique<Camera>();
	gameMainCamera_ = std::make_unique<Camera>();
	gameTurnCamera_ = std::make_unique<Camera>();
	goalEffectCamera_ = std::make_unique<Camera>();

	stageSelectCamera_->Initialize();
	gameMainCamera_->Initialize();
	gameTurnCamera_->Initialize();
	goalEffectCamera_->Initialize();

	//何かしらカメラのポインタ入れる
	usingCamera_ = gameMainCamera_.get();
}

CameraManager::~CameraManager()
{
	stageSelectCamera_.reset();
	gameMainCamera_.reset();
	gameTurnCamera_.reset();
	goalEffectCamera_.reset();
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
	state_->Update();

	usingCamera_->Update();

	stageSelectCamera_->Update();
	//ゲーム中メインで使うカメラ
	gameMainCamera_->Update();
	//ゲーム中、回転時に使うカメラ
	gameTurnCamera_->Update();
	//ゴール演出時に使うカメラ
	goalEffectCamera_->Update();
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

CameraManager& CameraManager::operator=(const CameraManager& obj)
{
	Initialize();

	*state_ = *obj.state_;//ステートなど、ポインタは削除される可能性があるので中身のみコピー
	usingCamera_ = obj.usingCamera_;
	*stageSelectCamera_.get() = *obj.stageSelectCamera_.get();
	*gameMainCamera_.get() = *obj.gameMainCamera_.get();
	*gameTurnCamera_.get() = *obj.gameTurnCamera_.get();
	*goalEffectCamera_.get() = *obj.goalEffectCamera_.get();
	startEye_ = obj.startEye_;
	endEye_ = obj.endEye_;
	startTarget_ = obj.startTarget_;
	endTarget_ = obj.endTarget_;
	startUp_ = obj.startUp_;
	endUp_ = obj.endUp_;
	lerpCountMax_ = obj.lerpCountMax_;
	lerpCount_ = obj.lerpCount_;
	if (obj.afterCamera_) { afterCamera_ = obj.afterCamera_; }
	afterCount_ = obj.afterCount_;
	isLerpMoving_ = obj.isLerpMoving_;

	return *this;
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

	//終了する1フレーム前に演出等入れる用
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
