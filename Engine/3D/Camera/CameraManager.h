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

	//デバッグ用
	WorldMat debugWorldMat_;
	Vec3 cameraPos_ = { 0,0,-100 };
	bool isDebugCamera_ = false;

	//カメラと名前をセットで持っておく
	std::map< std::string, std::unique_ptr<Camera>> cameraAndNames_;
	//カメラを順に切り替えるためのイテレータ
	std::map< std::string, std::unique_ptr<Camera>>::iterator cameraItr_;

public:
	//ここにいろんなカメラのポインタを入れて、描画時に使う
	Camera* usingCamera_;

	//演出などが終わった後にそのカメラに切り替えるためのポインタ
	Camera* afterCamera_ = nullptr;
	//終わった後、カメラに切り替えるまでのフレーム数
	int32_t afterCount_ = 0;

	//lerp用
	int32_t lerpCount_ = 0;
	int32_t lerpCountMax_ = 0;
	Vec3 startEye_, endEye_, startTarget_, endTarget_, startUp_, endUp_;

	bool isLerpMoving_ = false;
	bool isLerpEnd_ = false;

private://関数
	CameraManager();
	~CameraManager();

private:
	//カメラを順に切り替える
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
	//移動の線形補完を始める
	void BegineLerpUsingCamera(const Vec3& startEye, const Vec3& endEye, const Vec3& startTarget, const Vec3& endTarget, const Vec3& startUp, const Vec3& endUp, int32_t time, Camera* afterCamera = nullptr, int32_t afterCount = 0);

	//カメラを新たに追加する
	void AddCamera(std::string cameraName);
	//名前を指定してカメラを取得
	Camera* GetCamera(std::string cameraName);
	//usingCameraにセットされてるカメラ取得
	Camera* GetCamera();
	//カメラを使用しているカメラにセット
	void SetUsingCamera(std::string cameraName);

public:
	bool GetIsDebugCamera() { return isDebugCamera_; }
};