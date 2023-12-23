/*
* @file CircleShadow.h
* @brief 丸影
*/

#pragma once
#include<DirectXMath.h>


class CircleShadow
{
private://エイリアス
		//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public://サブクラス

	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR dir;
		XMFLOAT3 casterPos;
		float distanceCasterLight;
		XMFLOAT3 atten;
		float pad1;
		XMFLOAT2 factorAngleCos;
		uint32_t active;
		float pad2;
	};

private://メンバ変数
	//方向（単位ベクトル）
	XMVECTOR dir_ = { 1.0f,0,0 };
	//キャスターとライトの距離
	float distanceCasterLight_ = 100.0f;
	//キャスター座標（ワールド座標系）
	XMFLOAT3 casterPos_ = { 0,0,0 };
	//距離減衰係数
	XMFLOAT3 atten_ = { 0.5f,0.6f,0.0f };
	//減衰角度
	XMFLOAT2 factorAngleCos_ = { 0.2f,0.5f };
	//有効フラグ
	bool active_ = false;


public://メンバ関数
	//方向セット
	inline void SetDir(const XMVECTOR& dir) { dir_ = DirectX::XMVector3Normalize(dir); }
	//方向取得
	inline const XMVECTOR& GetDir() { return dir_; }

	//キャスター座標セット
	inline void SetCasterPos(const XMFLOAT3& casterPos) { casterPos_ = casterPos; }
	//キャスター座標取得
	inline const XMFLOAT3& GetCasterPos() { return casterPos_; }

	//キャスターとライトの距離セット
	inline void SetDistanceCasterLight(float distanceCasterLight) { distanceCasterLight_ = distanceCasterLight; }
	//キャスターとライトの距離取得
	inline float GetDistanceCasterLight() { return distanceCasterLight_; }

	//減衰セット
	inline void SetAtten(const XMFLOAT3& atten) { atten_ = atten; }
	//減衰取得
	inline const XMFLOAT3& GetAtten() { return atten_; }

	//減衰角度セット
	inline void SetFactorAngleCos(const XMFLOAT2& FactorAngleCos) {
		factorAngleCos_.x = cosf(DirectX::XMConvertToRadians(FactorAngleCos.x));
		factorAngleCos_.y = cosf(DirectX::XMConvertToRadians(FactorAngleCos.y));
	};
	//減衰角度取得
	inline const XMFLOAT2& GetFactorAngleCos() {
		return factorAngleCos_;
	};

	//ライトのアクティブセット
	inline void SetLightActive(bool active) {
		active_ = active;
	};
	//ライトのアクティブ取得
	inline bool GetLightActive() {
		return active_;
	};
};

