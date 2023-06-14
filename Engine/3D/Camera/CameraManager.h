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
	//ここにいろんなカメラのポインタを入れて、描画時に使う
	Camera* usingCamera;
	//選択画面で使う
	std::unique_ptr<Camera> stageSelectCamera;
	//ゲーム中メインで使うカメラ
	std::unique_ptr<Camera> gameMainCamera;
	//ゲーム中、回転時に使うカメラ
	std::unique_ptr<Camera> gameTurnCamera;
	//ゴール演出時に使うカメラ
	std::unique_ptr<Camera> goalEffectCamera;

	//演出などが終わった後にそのカメラに切り替えるためのポインタ
	Camera* afterCamera = nullptr;
	//終わった後、カメラに切り替えるまでのフレーム数
	int32_t afterCount = 0;

	//lerp用
	int32_t lerpCount = 0;
	int32_t lerpCountMax = 0;
	Vec3 startEye, endEye, startTarget, endTarget, startUp, endUp;

	bool isLerpMoving = false;
	bool isLerpEnd = false;

public://関数
	CameraManager();
	~CameraManager();

	void ChangeUsingCameraState(std::unique_ptr<UsingCameraState> state);

	void Initialize();

	void Update();

	//移動の線形補完を始める
	void BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* afterCamera = nullptr, int32_t afterCount = 0);


	CameraManager& operator=(const CameraManager& obj);
};