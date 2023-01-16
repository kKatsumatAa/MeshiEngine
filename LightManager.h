#pragma once
#include "DirectX.h"
#include "PointLight.h"
#include "DirLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"


class LightManager
{
public:
	//点光源の数
	static const int PointLightNum = 6;

private://エイリアス
//Microsoft::WRL::を省略
	template<class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private://定数

	//平行光源
	static const int DirLightNum = 3;

	//スポットライトの数
	static const int SpotLightNum = 3;

	//丸影の数
	static const int CircleShadowNum = 1;

private://静的メンバ変数
	static ID3D12Device* device_;

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ダーティフラグ
	bool dirty = false;
	// 環境光の色
	XMFLOAT3 ambientColor = { 1,1,1 };
	//点光源の配列
	PointLight pointLights[PointLightNum];
	// 平行光源の配列
	DirLight dirLights[DirLightNum];
	//スポットライトの配列
	SpotLight spotLights[SpotLightNum];
	//丸影の配列
	CircleShadow circleShadows[CircleShadowNum];

public://サブクラス
	//定数バッファ用データ構造体(平行光源、点光源の配列をまとめて送る)
	struct ConstBufferData
	{
		// 環境光の色
		XMFLOAT3 ambientColor;
		float pad1;

		// 平行光源の配列
		DirLight::ConstBufferData dirLights[DirLightNum];
		//点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
		//スポットライト用
		SpotLight::ConstBufferData spotLights[SpotLightNum];
		//丸影用
		CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
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

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画（描画はせず、定数バッファビューのセットのみ行う）
	/// </summary>
	/// <param name="rootParamaterIndex"></param>
	void Draw(UINT rootParamaterIndex);

	/// <summary>
	/// 標準のライト設定
	/// </summary>
	void DefaultLightSetting();

	/// <summary>
	/// 環境光のライト色をセット
	/// </summary>
	/// <param name="color">ライト色</param>
	void SetAmbientColor(const XMFLOAT3& color);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	static LightManager* Create();

	/// <summary>
		/// 平行光源の有効フラグをセット
		/// </summary>
		/// <param name="index">ライト番号</param>
		/// <param name="active">有効フラグ</param>
	void SetDirLightActive(int index, bool active);

	/// <summary>
	/// 平行光源のライト方向をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightdir">ライト方向</param>
	void SetDirLightDir(int index, const XMVECTOR& lightdir);

	/// <summary>
	/// 平行光源のライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);

///ポイントライト用
	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const XMFLOAT3& pos);
	void SetPointLightColor(int index, const XMFLOAT3& color);
	void SetPointLightAtten(int index, const XMFLOAT3& atten);

///スポットライト用
	void SetSpotLightActive(int index, bool active);
	//ライトの方向をセット
	void SetSpotLightDir(int index, const XMVECTOR& lightdir);
	void SetSpotLightPos(int index, const XMFLOAT3& lightpos);
	void SetSpotLightColor(int index, const XMFLOAT3& lightcolor);
	//減衰係数をセット
	void SetSpotLightAtten(int index, const XMFLOAT3& lightAtten);
	//減衰開始、終了角度
	void SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle);

///丸影用
	void SetCircleShadowActive(int index, bool active);
	//影の発生元のオブジェクトの座標
	void SetCircleShadowCasterPos(int index, const XMFLOAT3& casterPos);
	//ライトの方向をセット
	void SetCircleShadowDir(int index, const XMVECTOR& lightdir);
	//影からライトまでの距離
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	//減衰係数をセット
	void SetCircleShadowAtten(int index, const XMFLOAT3& lightAtten);
	//減衰開始、終了角度
	void SetCircleShadowFactorAngle(int index, const XMFLOAT2& lightFactorAngle);
};

