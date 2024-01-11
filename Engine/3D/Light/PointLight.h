/*
* @file PointLight.h
* @brief 点光源
*/

#pragma once
#include<DirectXMath.h>
#include <wrl.h>


class PointLight
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using Comptr = Microsoft::WRL::ComPtr<T>;
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
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		uint32_t active;
	};

public:
	const float IMGUI_LIGHT_ATTEN_MIN_ = 0;
	const float IMGUI_LIGHT_ATTEN_MAX_ = 10.0f;

private://メンバ変数
	//ライト座標（ワールド座標系）
	XMFLOAT3 lightpos_ = { 0,0,0 };
	//ライト色
	XMFLOAT3 lightcolor_ = { 1.0f,1.0f,1.0f };
	//ライト距離減衰係数
	XMFLOAT3 lightatten_ = { 1.0f,1.0f,1.0f };
	//有効フラグ
	bool active_ = false;



public://メンバ関数
	//位置セット
	void SetLightPos(const XMFLOAT3& lightpos) { lightpos_ = lightpos; }
	//位置取得
	const XMFLOAT3& GetLightPos() { return lightpos_; }
	//色セット
	void SetLightColor(const XMFLOAT3& lightcolor) { lightcolor_ = lightcolor; }
	//色取得
	const XMFLOAT3& GetLightColor() { return lightcolor_; }
	//減衰セット
	void SetLightAtten(const XMFLOAT3& lightatten) { lightatten_ = lightatten; }
	//減衰取得
	const XMFLOAT3& GetLightAtten() { return lightatten_; }
	//アクティブセット
	void SetActive(bool active) { active_ = active; }
	//アクティブ取得
	bool GetActive() { return active_; }

	//ImGuiの描画
	void DrawImGui(int32_t index);
};

