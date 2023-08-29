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
		return matBillboard_;
	}

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	inline const Vec3& GetEye() const{
		return viewMat_.eye_;
	}

	/// <summary>
	/// 視点座標の設定
	/// </summary>
	/// <param name="eye">座標</param>
	inline void SetEye(const Vec3& eye) {
		viewMat_.eye_ = eye; viewDirty_ = true;
	}

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	inline const Vec3& GetTarget() const {
		return viewMat_.target_;
	}

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="target">座標</param>
	inline void SetTarget(const Vec3& target) {
		viewMat_.target_ = target; viewDirty_ = true;
	}

	/// <summary>
	/// 上方向ベクトルの取得
	/// </summary>
	/// <returns>上方向ベクトル</returns>
	inline const Vec3& GetUp() const {
		return viewMat_.up_;
	}

	/// <summary>
	/// 上方向ベクトルの設定
	/// </summary>
	/// <param name="up">上方向ベクトル</param>
	inline void SetUp(const Vec3& up) {
		viewMat_.up_ = up; viewDirty_ = true;
	}

	/// <summary>
	/// ベクトルによる視点移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveEyeVector(const Vec3& move);
	void MoveEyeVector(const XMVECTOR& move);

	//
	void CameraShake(int32_t time, float length);

	/// <summary>
	/// ベクトルによる移動
	/// </summary>
	/// <param name="move">移動量</param>
	void MoveVector(const Vec3& move);
	void MoveVector(const XMVECTOR& move);

protected: // メンバ変数
	Shake shake_;

	// ビルボード行列
	XMMATRIX matBillboard_ = DirectX::XMMatrixIdentity();
	// Y軸回りビルボード行列
	XMMATRIX matBillboardY_ = DirectX::XMMatrixIdentity();
	//合成
	XMMATRIX matViewProjection_ = DirectX::XMMatrixIdentity();
	// ビュー行列ダーティフラグ
	bool viewDirty_ = false;
	// 射影行列ダーティフラグ
	bool projectionDirty_ = false;

private:
	ViewMat viewMat_;
	ProjectionMat projectionMat_;

public:
	const XMMATRIX& GetViewMat()const { return viewMat_.matView_; }
	const XMMATRIX& GetProjMat()const { return projectionMat_.matProjection_; }
	const XMMATRIX& GetBillboardMat()const { return matBillboard_; }
	const XMMATRIX& GetBillboardMatY()const { return matBillboardY_; }
};



