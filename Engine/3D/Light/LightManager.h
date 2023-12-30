/*
* @file LightManager.h
* @brief ライト全部を管理（方向、ポイント、スポット等）
*/

#pragma once
#include "DirectXWrapper.h"
#include "PointLight.h"
#include "DirLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"


class LightManager
{
public:
	//点光源の数
	static const int32_t S_POINT_LIGHT_NUM_ = 12;

private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private://定数

	//平行光源
	static const int32_t S_DIR_LIGHT_NUM_ = 3;

	//スポットライトの数
	static const int32_t S_SPOT_LIGHT_NUM_ = 3;

	//丸影の数
	static const int32_t S_CIRCLE_SHADOW_NUM_ = 1;

private://静的メンバ変数
	static ID3D12Device* sDevice_;

public:
	const float IMGUI_COLOR_DRAG_SPEED_ = 0.1f;

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff_;
	//ダーティフラグ
	bool dirty_ = false;
	// 環境光の色
	XMFLOAT3 ambientColor_ = { 1,1,1 };
	XMFLOAT3 diffuseColor_ = { 1,1,1 };
	XMFLOAT3 specularColor_ = { 1,1,1 };
	//点光源の配列
	PointLight pointLights_[S_POINT_LIGHT_NUM_];
	// 平行光源の配列
	DirLight dirLights_[S_DIR_LIGHT_NUM_];
	//スポットライトの配列
	SpotLight spotLights_[S_SPOT_LIGHT_NUM_];
	//丸影の配列
	CircleShadow circleShadows_[S_CIRCLE_SHADOW_NUM_];

public://サブクラス
	//定数バッファ用データ構造体(平行光源、点光源の配列をまとめて送る)
	struct ConstBufferData
	{
		// 環境光の色
		XMFLOAT3 ambientColor;
		float pad1;
		// diffuseの色
		XMFLOAT3 diffuseColor;
		float pad2;
		// specularの色
		XMFLOAT3 specularColor;
		float pad3;

		// 平行光源の配列
		DirLight::ConstBufferData dirLights[S_DIR_LIGHT_NUM_];
		//点光源用
		PointLight::ConstBufferData pointLights[S_POINT_LIGHT_NUM_];
		//スポットライト用
		SpotLight::ConstBufferData spotLights[S_SPOT_LIGHT_NUM_];
		//丸影用
		CircleShadow::ConstBufferData circleShadows[S_CIRCLE_SHADOW_NUM_];
	};


private:
	/// <summary>
/// 定数バッファ転送
/// </summary>
	void TransferConstBuffer();

public://静的メンバ関数
	static void StaticInitialize();

public:
	//初期化
	void Initialize();
	//ライトのアクティブ初期化
	void InitializeActive();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画（描画はせず、定数バッファビューのセットのみ行う）
	/// </summary>
	/// <param name="rootParamaterIndex"></param>
	void Draw(uint32_t rootParamaterIndex);

	//ImGuiの描画
	void DrawImGui();

public:
	/// <summary>
	/// 環境光のライト色をセット
	/// </summary>
	/// <param name="color">ライト色</param>
	void SetAmbientColor(const XMFLOAT3& color);
	//拡散反射の色セット
	void SetDiffuseColor(const XMFLOAT3& color);
	//鏡面反射の色セット
	void SetSpecularColor(const XMFLOAT3& color);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	static std::unique_ptr<LightManager> Create();

public://平行光源
	/// <summary>
		/// 平行光源の有効フラグをセット
		/// </summary>
		/// <param name="index">ライト番号</param>
		/// <param name="active">有効フラグ</param>
	void SetDirLightActive(int32_t index, bool active);

	/// <summary>
	/// 平行光源のライト方向をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightdir">ライト方向</param>
	void SetDirLightDir(int32_t index, const XMVECTOR& lightdir);

	/// <summary>
	/// 平行光源のライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetDirLightColor(int32_t index, const XMFLOAT3& lightcolor);

public://ポイントライト
	///ポイントライトのアクティブセット
	void SetPointLightActive(int32_t index, bool active);
	///ポイントライトの位置セット
	void SetPointLightPos(int32_t index, const XMFLOAT3& pos);
	///ポイントライトの色セット
	void SetPointLightColor(int32_t index, const XMFLOAT3& color);
	///ポイントライトの減衰セット
	void SetPointLightAtten(int32_t index, const XMFLOAT3& atten);

public://スポットライト
	//スポットライトのアクティブセット
	void SetSpotLightActive(int32_t index, bool active);
	//スポットライトの方向をセット
	void SetSpotLightDir(int32_t index, const XMVECTOR& lightdir);
	//スポットライトの位置をセット
	void SetSpotLightPos(int32_t index, const XMFLOAT3& lightpos);
	//スポットライトの色をセット
	void SetSpotLightColor(int32_t index, const XMFLOAT3& lightcolor);
	//スポットライトの減衰をセット
	void SetSpotLightAtten(int32_t index, const XMFLOAT3& lightAtten);
	//スポットライトの減衰開始、終了角度セット
	void SetSpotLightFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle);

public://丸影
	///丸影のアクティブセット
	void SetCircleShadowActive(int32_t index, bool active);
	//影の発生元のオブジェクトの座標
	void SetCircleShadowCasterPos(int32_t index, const XMFLOAT3& casterPos);
	//丸影のライトの方向をセット
	void SetCircleShadowDir(int32_t index, const XMVECTOR& lightdir);
	//影からライトまでの距離
	void SetCircleShadowDistanceCasterLight(int32_t index, float distanceCasterLight);
	//丸影の減衰係数をセット
	void SetCircleShadowAtten(int32_t index, const XMFLOAT3& lightAtten);
	//丸影の減衰開始、終了角度セット
	void SetCircleShadowFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle);

public:
	//このインデックスの方向ライトが使用されているか
	bool GetDoNotUseDirLightIndex(int32_t& index);
	//このインデックスのポイントライトが使用されているか
	bool GetDoNotUsePointLightIndex(int32_t& index);
	//このインデックスのスポットライトが使用されているか
	bool GetDoNotUseSpotLightIndex(int32_t& index);
};

