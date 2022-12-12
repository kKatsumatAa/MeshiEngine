#pragma once
#include "DirectX.h"
#include "PointLight.h"

/// <summary>
/// 照明(様々なライトを作るときはポリモーフィズムで継承するといい)
/// </summary>
class Light
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

private://静的メンバ変数
	static ID3D12Device* device_;

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ライト光線方向
	XMVECTOR lightdir = { 1,0,0,0 };
	//ライト色
	XMFLOAT3 lightColor = { 1,1,1 };
	//ダーティフラグ
	bool dirty = false;

private://定数
	//点光源の数
	static const int PointLightNum = 3;

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR lightv;     //ライトの方向を表すベクトル
		XMFLOAT3 lightColor; //ライトの色
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
	/// インスタンス生成
	/// </summary>
	static Light* Create();

	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir"></param>
	void SetLightDir(const XMVECTOR& lightdir);

	/// <summary>
	/// ライトの色をセット
	/// </summary>
	/// <param name="lightcolor"></param>
	void SetLightColor(const XMFLOAT3& lightcolor);
};

