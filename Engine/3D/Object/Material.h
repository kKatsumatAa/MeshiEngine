#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include "DirectXWrapper.h"

/// <summary>
/// マテリアル
/// </summary>
class Material
{
public:
	//フレンドクラス
	friend class FbxLoader;

private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	struct ConstBufferDataMaterial2
	{
		XMFLOAT3 ambient;
		float pad1;       //パディング
		XMFLOAT3 diffuse;
		float pad2;       //パディング
		XMFLOAT3 specular;
		float alpha;
	};

public: // 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// マテリアル生成
	/// </summary>
	/// <returns>生成されたマテリアル</returns>
	static std::unique_ptr<Material> Create();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* sDevice_;

public:
	std::string name_;	// マテリアル名
	XMFLOAT3 ambient_;	// アンビエント影響度
	XMFLOAT3 diffuse_;	// ディフューズ影響度
	XMFLOAT3 specular_;	// スペキュラー影響度
	float alpha_;		// アルファ
	std::string textureFilename_;	// テクスチャファイル名
	uint64_t textureHandle_;//画像ハンドル

public:
	~Material();
	// コンストラクタ
	Material() {
		ambient_ = { 0.3f, 0.3f, 0.3f };
		diffuse_ = { 0.0f, 0.0f, 0.0f };
		specular_ = { 0.0f, 0.0f, 0.0f };
		alpha_ = 1.0f;
		textureHandle_ = NULL;
	}

	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }

	/// テクスチャ読み込み
	/// </summary>
	/// <param name="directoryPath">読み込みディレクトリパス</param>
	/// <param name="cpuHandle">CPUデスクリプタハンドル</param>
	/// <param name="gpuHandle">GPUデスクリプタハンドル</param>
	void LoadTexture(const std::string& directoryPath);


	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff_;

private:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstantBuffer();
};

