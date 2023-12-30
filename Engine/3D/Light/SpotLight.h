/*
* @file SpotLight.h
* @brief スポットライト
*/

#pragma once
#include<DirectXMath.h>


class SpotLight
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
		XMVECTOR lightv;
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		float pad3;
		XMFLOAT2 lightfactoranglecos;
		uint32_t active;
		float pad4;
	};

public:
	const float IMGUI_LIGHT_ATTEN_MIN_ = 0;
	const float IMGUI_LIGHT_ATTEN_MAX_ = 3.0f;
	const float IMGUI_LIGHT_DIR_MIN_ = -1.0f;
	const float IMGUI_LIGHT_DIR_MAX_ = 1.0f;

private://メンバ変数
	//ライトの方向
	XMVECTOR lightdir_ = { 1.0f,0,0 };
	//ライト座標（ワールド座標系）
	XMFLOAT3 lightpos_ = { 0,0,0 };
	//ライト色
	XMFLOAT3 lightcolor_ = { 1,1,1 };
	//ライト距離減衰係数
	XMFLOAT3 lightAtten_ = { 1.0f,1.0f,1.0f };
	//ライト減衰角度（開始角度、終了角度）
	XMFLOAT2 lightFactorAngleCos_ = { 0.5f,0.2f };
	//有効フラグ
	bool active_ = false;

public://メンバ関数

	//方向セット
	inline void SetLightDir(const XMVECTOR& lightdir) {
		lightdir_ = DirectX::XMVector3Normalize(lightdir);
	};
	//方向取得
	inline const XMVECTOR& GetLightDir() {
		return lightdir_;
	};
	//位置セット
	inline void SetLightPos(const XMFLOAT3& lightpos) {
		lightpos_ = lightpos;
	};
	//位置取得
	inline const XMFLOAT3& GetLightPos() {
		return lightpos_;
	};
	//色セット
	inline void SetLightColor(const XMFLOAT3& lightcolor) {
		lightcolor_ = lightcolor;
	};
	//色取得
	inline const XMFLOAT3& GetLightColor() {
		return lightcolor_;
	};
	//減衰セット
	inline void SetLightAtten(const XMFLOAT3& lightAtten) {
		lightAtten_ = lightAtten;
	};
	//減衰取得
	inline const XMFLOAT3& GetLightAtten() {
		return lightAtten_;
	};
	//角度減衰セット
	inline void SetLightFactorAngleCos(const XMFLOAT2& lightFactoeAngleCos) {
		lightFactorAngleCos_.x = cosf(DirectX::XMConvertToRadians(lightFactoeAngleCos.x));
		lightFactorAngleCos_.y = cosf(DirectX::XMConvertToRadians(lightFactoeAngleCos.y));
	};
	//角度減衰取得
	inline const XMFLOAT2& GetLightFactorAngleCos() {
		return lightFactorAngleCos_;
	};
	//アクティブセット
	inline void SetActive(bool active) {
		active_ = active;
	};
	//アクティブ取得
	inline bool GetActive() {
		return active_;
	};

	//ImGuiの描画
	void DrawImGui(int32_t index);
};

