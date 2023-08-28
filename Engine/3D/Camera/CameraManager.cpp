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

		//最後まで行ったら
		if (cameraItr_ == cameraAndNames_.end())
		{
			cameraItr_ = cameraAndNames_.begin();
		}

		//カメラポインタをセット
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

	//カメラを順に切り替えるためのイテレータ
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
			//デバッグカメラに変更
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

		//デバッグカメラが有効の時
		if (usingCamera_ == debugCamera)
		{

			if (MouseInput::GetInstance().GetClick(CLICK_RIGHT))
			{
				Vec2 vel = MouseInput::GetInstance().GetCursorVelocity();

				//回転
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
	//カメラ切り替え
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
	//ファイル名から探す
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//すでに同じ名前のカメラがあったらエラー
	if (it != cameraAndNames_.end())
	{
		return;
		//assert(false);
	}

	//なければ読み込み
	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Initialize();

	//保存しておく
	cameraAndNames_.insert(std::make_pair(cameraName, std::move(camera)));
}

Camera* CameraManager::GetCamera(std::string cameraName)
{
	//ファイル名から探す
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//指定した名前のカメラがあればポインタを返す
	if (it != cameraAndNames_.end())
	{
		return it->second.get();
	}

	//なければnullptr
	assert(false);
	return nullptr;
}

Camera* CameraManager::GetCamera()
{
	return usingCamera_;
}

void CameraManager::SetUsingCamera(std::string cameraName)
{
	//ファイル名から探す
	std::map< std::string, std::unique_ptr<Camera>>::iterator it = cameraAndNames_.find(cameraName);
	//指定した名前のカメラがあればポインタusingCamera_にポインタセット
	if (it != cameraAndNames_.end())
	{
		usingCamera_ = it->second.get();
		return;
	}

	//なければ
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
