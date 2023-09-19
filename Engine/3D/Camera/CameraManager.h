#pragma once
#include"Camera.h"
#include"Camera2D.h"
#include"WorldMat.h"



class CameraManager
{
private:
	//デバッグ用
	WorldMat debugWorldMat_;
	Vec3 cameraPos_ = { 0,0,-100 };
	bool isDebugCamera_ = false;

	//カメラと名前をセットで持っておく
	std::map< std::string, std::unique_ptr<Camera>> cameraAndNames_;
	//2D用のカメラ
	std::map< std::string, std::unique_ptr<Camera2D>> camera2DAndNames_;
	//カメラを順に切り替えるためのイテレータ
	std::map< std::string, std::unique_ptr<Camera>>::iterator cameraItr_;
	std::map< std::string, std::unique_ptr<Camera2D>>::iterator camera2DItr_;

public:
	//ここにいろんなカメラのポインタを入れて、描画時に使う
	Camera* usingCamera_;
	Camera2D* usingCamera2D_;

	//演出などが終わった後にそのカメラに切り替えるためのポインタ
	Camera* afterCamera_ = nullptr;

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

	void Initialize();

	void Update();

	void ImguiUpdate();

public:
	//カメラを新たに追加する
	void AddCamera(const std::string& cameraName);
	void AddCamera2D(const std::string& cameraName);
	//名前を指定してカメラを取得
	Camera* GetCamera(const std::string& cameraName);
	//usingCameraにセットされてるカメラ取得
	Camera* GetCamera();
	//名前を指定してカメラを取得
	Camera2D* GetCamera2D(const std::string& cameraName);
	//usingCameraにセットされてるカメラ取得
	Camera2D* GetCamera2D();
	//カメラを使用しているカメラにセット
	void SetUsingCamera(const std::string& cameraName);
	void SetUsingCamera2D(const std::string& cameraName);

public:
	bool GetIsDebugCamera() { return isDebugCamera_; }
};