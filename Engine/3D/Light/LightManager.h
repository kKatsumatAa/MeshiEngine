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
	void Initialize();

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

	//
	void DrawImGui();

	/// <summary>
	/// 標準のライト設定
	/// </summary>
	void DefaultLightSetting();

	/// <summary>
	/// 環境光のライト色をセット
	/// </summary>
	/// <param name="color">ライト色</param>
	void SetAmbientColor(const XMFLOAT3& color);

	void SetDiffuseColor(const XMFLOAT3& color);

	void SetSpecularColor(const XMFLOAT3& color);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	static std::unique_ptr<LightManager> Create();

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

	///ポイントライト用
	void SetPointLightActive(int32_t index, bool active);
	void SetPointLightPos(int32_t index, const XMFLOAT3& pos);
	void SetPointLightColor(int32_t index, const XMFLOAT3& color);
	void SetPointLightAtten(int32_t index, const XMFLOAT3& atten);

	///スポットライト用
	void SetSpotLightActive(int32_t index, bool active);
	//ライトの方向をセット
	void SetSpotLightDir(int32_t index, const XMVECTOR& lightdir);
	void SetSpotLightPos(int32_t index, const XMFLOAT3& lightpos);
	void SetSpotLightColor(int32_t index, const XMFLOAT3& lightcolor);
	//減衰係数をセット
	void SetSpotLightAtten(int32_t index, const XMFLOAT3& lightAtten);
	//減衰開始、終了角度
	void SetSpotLightFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle);

	///丸影用
	void SetCircleShadowActive(int32_t index, bool active);
	//影の発生元のオブジェクトの座標
	void SetCircleShadowCasterPos(int32_t index, const XMFLOAT3& casterPos);
	//ライトの方向をセット
	void SetCircleShadowDir(int32_t index, const XMVECTOR& lightdir);
	//影からライトまでの距離
	void SetCircleShadowDistanceCasterLight(int32_t index, float distanceCasterLight);
	//減衰係数をセット
	void SetCircleShadowAtten(int32_t index, const XMFLOAT3& lightAtten);
	//減衰開始、終了角度
	void SetCircleShadowFactorAngle(int32_t index, const XMFLOAT2& lightFactorAngle);

public:
	//使用されてないポイントライトのインデックス
	bool GetDoNotUseDirLightIndex(int32_t& index);
	bool GetDoNotUsePointLightIndex(int32_t& index);
	bool GetDoNotUseSpotLightIndex(int32_t& index);
};

