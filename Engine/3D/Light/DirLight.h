/*
* @file DirLight.h
* @brief 方向ライト
*/

#pragma once
#include<DirectXMath.h>
#include "Vec3.h"


class DirLight
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
		XMVECTOR lightdir;     //ライトの方向を表すベクトル
		XMFLOAT3 lightColor; //ライトの色
		uint32_t active; //有効か
	};


public:
	const float IMGUI_LIGHT_DIR_MIN_ = -1.0f;
	const float IMGUI_LIGHT_DIR_MAX_ = 1.0f;

private://メンバ変数
	//ライト方向
	Vec3 lightdir_ = { 1.0f,0,0 };
	//ライト色
	XMFLOAT3 lightColor_ = { 1,1,1 };
	// 有効フラグ
	bool active_ = false;

public:
	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir_">ライト方向</param>
	void SetLightDir(const XMVECTOR& lightdir);

	/// <summary>
	/// ライト方向を取得
	/// </summary>
	/// <returns>ライト方向</returns>
	XMVECTOR GetLightDir();

	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name="lightcolor">ライト色</param>
	inline void SetLightColor(const XMFLOAT3& lightcolor) { lightColor_ = lightcolor; }

	/// <summary>
	/// ライト色を取得
	/// </summary>
	/// <returns>ライト色</returns>
	inline const XMFLOAT3& GetLightColor() { return lightColor_; }

	/// <summary>
	/// 有効フラグをセット
	/// </summary>
	/// <param name="active">有効フラグ</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// 有効チェック
	/// </summary>
	/// <returns>有効フラグ</returns>
	inline bool GetActive() { return active_; }

	//ImGuiの描画
	void DrawImGui(int32_t index);
};

