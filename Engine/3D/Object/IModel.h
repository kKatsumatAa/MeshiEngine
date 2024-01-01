/*
* @file IModel.h
* @brief モデル基底クラス
*/

#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>
#include "Material.h"
#include "Mesh.h"
#include <unordered_map>


class IModel
{
protected:// エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImg = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;

public://定数
	//参照先のパス
	static const std::string S_BASE_DIRECTORY_;

public:
	const float IMGUI_SCALE_EXTEND_MAX_ = 10.0f;
	const float IMGUI_SCALE_EXTEND_MIN_ = 0.0f;
	const float IMGUI_MATERIAL_EXTEND_DRAG_SPEED_ = 1.0f;
	const float IMGUI_MATERIAL_EXTEND_MAX_ = 100.0f;
	const float IMGUI_MATERIAL_EXTEND_MIN_ = 0;

public:
	static const int8_t S_MESH_VERTEX_NORMAL_NUM_ = 3;
	static const int8_t S_INDEX_OFFSET_VERTEX_4_OF_2_ = -1;
	static const int8_t S_INDEX_OFFSET_VERTEX_4_OF_4_ = -3;
	static const int8_t S_MESH_VERTEX_NUM_MAX_ = 4;
	static const int8_t S_MESH_VERTEX_NUM_NORMAL_ = 3;

protected:
	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<std::unique_ptr<Mesh>> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, std::unique_ptr<Material>> materials_;
	// デフォルトマテリアル
	std::unique_ptr<Material> defaultMaterial_;

	//fbxか
	bool isFbx_ = false;

	//スケールの倍率(判定などには影響しない)
	float scaleExtend_ = 1.0f;

	Vec3 materialExtend_ = { 1.0f,1.0f,1.0f };


public:
	//コンストラクタ
	IModel() { ; }
	//デストラクタ
	virtual ~IModel();

protected:
	//materialにテクスチャを読み込む
	virtual void LoadTexturesInternal(const std::string& directoryPath);

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

public:
	/// <summary>
	/// マテリアル登録
	/// </summary>
	void AddMaterial(std::unique_ptr<Material> material);

	//テクスチャ読み込み
	virtual void LoadTextures() = 0;

public: // メンバ関数
	static void StaticInitialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList">命令発行先コマンドリスト</param>
	virtual void Draw(const D3D_PRIMITIVE_TOPOLOGY& topology, const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
		, const ConstBuffTransform& cbt, bool isShadow);

	//ImGuiの描画
	virtual void DrawImGui();

public:
	/// <summary>
	/// メッシュコンテナを取得
	/// </summary>
	/// <returns></returns>
	inline const std::vector<std::unique_ptr<Mesh>>& GetMeshes() { return meshes_; }

public:
	//fbxモデルか
	bool GetIsFbx() { return isFbx_; }

	//スケールの倍率セット
	void SetScaleExtend(float extend) { scaleExtend_ = extend; }
	//スケールの倍率取得
	float GetScaleExtend()const { return scaleExtend_; }
	//マテリアルの倍率取得
	Vec3 GetMaterialExtend() { return materialExtend_; }
	//マテリアルの倍率セット
	void SetMaterialExtend(const Vec3& materialExtend) { materialExtend_ = materialExtend; }
	//ポリゴンの座標に加算する座標を計算するための情報をセット
	void SetPolygonOffsetData(const Mesh::PolygonOffset& polygonOffsetData);
};