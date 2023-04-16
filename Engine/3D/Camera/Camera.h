#pragma once
#include"ViewMat.h"
#include"ProjectionMat.h"
#include"Shake.h"

#include <DirectXMath.h>

/// <summary>
/// カメラ基本機能
/// </summary>
class Camera
{
protected: // エイリアス
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	Camera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Camera() = default;

	virtual void Initialize();

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// 射影行列を更新
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// ビルボード行列の取得
	/// </summary>
	/// <returns>ビルボード行列</returns>
	inline const XMMATRIX& GetBillboardMatrix() {
		return matBillboard;
	}

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	inline const Vec3& GetEye() {
		return viewMat.eye;
	}

	/// <summary>
	/// 視点座標の設定
	/// </summary>
	/// <param name="eye">座標</param>
	inline void SetEye(Vec3 eye) {
		this->viewMat.eye = eye; viewDirty = true;
	}

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	inline const Vec3& GetTarget() {
		return viewMat.target;
	}

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="target">座標</param>
	inline void SetTarget(Vec3 target) {
		this->viewMat.target = target; viewDirty = true;
	}

	/// <summary>
	/// 上方向ベクトルの取得
	/// </summary>
	/// <returns>上方向ベクトル</returns>
	inline const Vec3& GetUp() {
		return viewMat.up;
	}

	/// <summary>
	/// 上方向ベクトルの設定
	/// </summary>
	/// <param name="up">上方向ベクトル</param>
	inline void SetUp(Vec3 up) {
		this->viewMat.up = up; viewDirty = true;
	}

	/// <summary>
	/// ベクトルによる視点移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveEyeVector(const Vec3& move);
	void MoveEyeVector(const XMVECTOR& move);

	//
	void CameraShake(int time, float length);

	/// <summary>
	/// ベクトルによる移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveVector(const Vec3& move);
	void MoveVector(const XMVECTOR& move);

protected: // メンバ変数
	Shake shake;

	// ビルボード行列
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y軸回りビルボード行列
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	//合成
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	// ビュー行列ダーティフラグ
	bool viewDirty = false;
	// 射影行列ダーティフラグ
	bool projectionDirty = false;

public:
	ViewMat viewMat;
	ProjectionMat projectionMat;
};



