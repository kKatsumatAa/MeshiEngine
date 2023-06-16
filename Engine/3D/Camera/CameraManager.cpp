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
	pUsingCamera_ = gameMainCamera_.get();
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
	pAfterCamera_ = nullptr;
	isLerpMoving_ = false;
	isLerpEnd_ = false;

	ChangeUsingCameraState(std::make_unique<UsingCameraNormalState>());
}

void CameraManager::Update()
{
	state_->Update();

	pUsingCamera_->Update();

	stageSelectCamera_->Update();
	//ゲーム中メインで使うカメラ
	gameMainCamera_->Update();
	//ゲーム中、回転時に使うカメラ
	gameTurnCamera_->Update();
	//ゴール演出時に使うカメラ
	goalEffectCamera_->Update();
}

void CameraManager::BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget,
	const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* pAfterCamera, int32_t afterCount)
{
	startEye_ = startEye;
	endEye_ = endEye;
	startTarget_ = startTarget;
	endTarget_ = endTarget;
	startUp_ = startUp;
	endUp_ = endUp;
	lerpCountMax_ = time;
	lerpCount_ = 0;
	pAfterCamera_ = pAfterCamera;
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
	pUsingCamera_ = obj.pUsingCamera_;
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
	if (obj.pAfterCamera_) { pAfterCamera_ = obj.pAfterCamera_; }
	afterCount_ = obj.afterCount_;
	isLerpMoving_ = obj.isLerpMoving_;

	return *this;
}


//----------------------------------------------------------------------------------------------------------------------------------------
void UsingCameraState::SetCameraM(CameraManager* pCameraM)
{
	pCameraM_ = pCameraM;
}


//-------------------------------------------------------------------------------------------------------
void UsingCameraNormalState::Update()
{
	if (pCameraM_->isLerpMoving_)
	{
		pCameraM_->ChangeUsingCameraState(std::make_unique<UsingCameraLerpMoveState>());
	}
}

//-------------------------------------------------------------------------------------------------------
void UsingCameraLerpMoveState::Update()
{
	float t = (float)pCameraM_->lerpCount_ / (float)pCameraM_->lerpCountMax_;

	if (pCameraM_->lerpCount_ < pCameraM_->lerpCountMax_)
	{
		pCameraM_->lerpCount_++;
	}

	pCameraM_->pUsingCamera_->SetEye(LerpVec3(pCameraM_->startEye_, pCameraM_->endEye_, EaseOut(t)));
	pCameraM_->pUsingCamera_->SetUp(LerpVec3(pCameraM_->startUp_, pCameraM_->endUp_, EaseOut(t)));
	pCameraM_->pUsingCamera_->SetTarget(LerpVec3(pCameraM_->startTarget_, pCameraM_->endTarget_, EaseOut(t)));

	//終了する1フレーム前に演出等入れる用
	if (pCameraM_->lerpCount_ >= pCameraM_->lerpCountMax_ - 1)
	{
		pCameraM_->isLerpEnd_ = true;
	}

	if (pCameraM_->lerpCount_ >= pCameraM_->lerpCountMax_)
	{
		pCameraM_->afterCount_--;
		pCameraM_->isLerpMoving_ = false;

		if (pCameraM_->afterCount_ <= 0)
		{

			if (pCameraM_->pAfterCamera_)
			{
				pCameraM_->pUsingCamera_ = pCameraM_->pAfterCamera_;
			}



			pCameraM_->ChangeUsingCameraState(std::make_unique<UsingCameraNormalState>());
		}
	}
}
