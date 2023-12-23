/*
* @file CameraManager.h
* @brief カメラを配列で管理（取得したり追加）
*/

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
	Camera* usingLightCamera_;
	Camera2D* usingCamera2D_;

	//演出などが終わった後にそのカメラに切り替えるためのポインタ
	Camera* afterCamera_ = nullptr;

private://関数
	//コンストラクタ
	CameraManager();
	//デストラクタ
	~CameraManager();

private:
	//カメラを順に切り替える
	void ChangeCamera();

public:
	//コピーコンストラクタ禁止
	CameraManager(const CameraManager& other) = delete;
	//コピーコンストラクタ禁止
	CameraManager& operator=(const CameraManager& other) = delete;

	//インスタンス取得
	static CameraManager& GetInstance() { static CameraManager sInst; return sInst; }

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//ImGuiの描画
	void ImguiUpdate();

public:
	//カメラを新たに追加する(3D)
	void AddCamera(const std::string& cameraName);
	//カメラを新たに追加する(2D)
	void AddCamera2D(const std::string& cameraName);

	//名前を指定してカメラを取得
	Camera* GetCamera(const std::string& cameraName);
	//usingCameraにセットされてるカメラ取得
	Camera* GetCamera();
	//usingLightCameraにセットされてるカメラ取得
	Camera* GetUsingLightCamera();
	//名前を指定してカメラを取得(2D)
	Camera2D* GetCamera2D(const std::string& cameraName);
	//usingCameraにセットされてるカメラ取得(2D)
	Camera2D* GetCamera2D();

	//使用するカメラにセット
	void SetUsingCamera(const std::string& cameraName);
	//シャドウマップ用のライトとして使うカメラをセット
	void SetUsingLightCamera(const std::string& cameraName);
	//使用カメラにセット（2D）
	void SetUsingCamera2D(const std::string& cameraName);

public:
	//デバッグ用のカメラを使用してるか
	bool GetIsDebugCamera() { return isDebugCamera_; }
};