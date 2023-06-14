#include "CameraManager.h"

CameraManager::CameraManager()
{
	stageSelectCamera = std::make_unique<Camera>();
	gameMainCamera = std::make_unique<Camera>();
	gameTurnCamera = std::make_unique<Camera>();
	goalEffectCamera = std::make_unique<Camera>();

	stageSelectCamera->Initialize();
	gameMainCamera->Initialize();
	gameTurnCamera->Initialize();
	goalEffectCamera->Initialize();

	//何かしらカメラのポインタ入れる
	usingCamera = gameMainCamera.get();
}

CameraManager::~CameraManager()
{
	stageSelectCamera.reset();
	gameMainCamera.reset();
	gameTurnCamera.reset();
	goalEffectCamera.reset();
}

void CameraManager::ChangeUsingCameraState(std::unique_ptr<UsingCameraState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetCameraM(this);
}

void CameraManager::Initialize()
{
	lerpCount = 0;
	lerpCountMax = 0;
	afterCount = 0;
	afterCamera = nullptr;
	isLerpMoving = false;
	isLerpEnd = false;

	ChangeUsingCameraState(std::make_unique<UsingCameraNormalState>());
}

void CameraManager::Update()
{
	state_->Update();

	usingCamera->Update();

	stageSelectCamera->Update();
	//ゲーム中メインで使うカメラ
	gameMainCamera->Update();
	//ゲーム中、回転時に使うカメラ
	gameTurnCamera->Update();
	//ゴール演出時に使うカメラ
	goalEffectCamera->Update();
}

void CameraManager::BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* afterCamera, int32_t afterCount)
{
	this->startEye = startEye;
	this->endEye = endEye;
	this->startTarget = startTarget;
	this->endTarget = endTarget;
	this->startUp = startUp;
	this->endUp = endUp;
	this->lerpCountMax = time;
	this->lerpCount = 0;
	this->afterCamera = afterCamera;
	this->afterCount = afterCount;
	isLerpMoving = true;
	isLerpEnd = false;

	ChangeUsingCameraState(std::make_unique<UsingCameraLerpMoveState>());

	Update();
}

CameraManager& CameraManager::operator=(const CameraManager& obj)
{
	this->Initialize();

	*this->state_ = *obj.state_;//ステートなど、ポインタは削除される可能性があるので中身のみコピー
	this->usingCamera = obj.usingCamera;
	*this->stageSelectCamera.get() = *obj.stageSelectCamera.get();
	*this->gameMainCamera.get() = *obj.gameMainCamera.get();
	*this->gameTurnCamera.get() = *obj.gameTurnCamera.get();
	*this->goalEffectCamera.get() = *obj.goalEffectCamera.get();
	this->startEye = obj.startEye;
	this->endEye = obj.endEye;
	this->startTarget = obj.startTarget;
	this->endTarget = obj.endTarget;
	this->startUp = obj.startUp;
	this->endUp = obj.endUp;
	this->lerpCountMax = obj.lerpCountMax;
	this->lerpCount = obj.lerpCount;
	if (obj.afterCamera) { this->afterCamera = obj.afterCamera; }
	this->afterCount = obj.afterCount;
	this->isLerpMoving = obj.isLerpMoving;

	return *this;
}


//----------------------------------------------------------------------------------------------------------------------------------------
void UsingCameraState::SetCameraM(CameraManager* cameraM)
{
	this->cameraM = cameraM;
}


//-------------------------------------------------------------------------------------------------------
void UsingCameraNormalState::Update()
{
	if (cameraM->isLerpMoving)
	{
		cameraM->ChangeUsingCameraState(std::make_unique<UsingCameraLerpMoveState>());
	}
}

//-------------------------------------------------------------------------------------------------------
void UsingCameraLerpMoveState::Update()
{
	float t = (float)cameraM->lerpCount / (float)cameraM->lerpCountMax;

	if (cameraM->lerpCount < cameraM->lerpCountMax)
	{
		cameraM->lerpCount++;
	}

	cameraM->usingCamera->SetEye(LerpVec3(cameraM->startEye, cameraM->endEye, EaseOut(t)));
	cameraM->usingCamera->SetUp(LerpVec3(cameraM->startUp, cameraM->endUp, EaseOut(t)));
	cameraM->usingCamera->SetTarget(LerpVec3(cameraM->startTarget, cameraM->endTarget, EaseOut(t)));

	//終了する1フレーム前に演出等入れる用
	if (cameraM->lerpCount >= cameraM->lerpCountMax - 1)
	{
		cameraM->isLerpEnd = true;
	}

	if (cameraM->lerpCount >= cameraM->lerpCountMax)
	{
		cameraM->afterCount--;
		cameraM->isLerpMoving = false;

		if (cameraM->afterCount <= 0)
		{

			if (cameraM->afterCamera)
			{
				cameraM->usingCamera = cameraM->afterCamera;
			}



			cameraM->ChangeUsingCameraState(std::make_unique<UsingCameraNormalState>());
		}
	}
}
