#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "Material.h"
#include <vector>
#include <unordered_map>
#include "Camera.h"
#include "ConstBuffTransform.h"
#include "WorldMat.h"


struct Node
{
	//名前
	std::string name = {};
	//ローカルスケール
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//ローカル回転角
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//ローカル移動
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//ローカル変形行列
	DirectX::XMMATRIX transform = {};
	//グローバル変形行列（親の影響も含めた）
	DirectX::XMMATRIX globalTransform = {};
	//親ノード
	Node* parent = nullptr;
};


/// <summary>
/// 形状データ
/// </summary>
class Mesh
{
public:
	//フレンドクラス
	friend class FbxLoader;

private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;


public://定数
	//ボーンインデックス（影響を受けるボーン）の最大数
	static const int32_t S_MAX_BONE_INDICES_ = 4;//hlslのfloat4に対応するため"4"

public://サブクラス
	//頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		Vec3 pos;//座標
		Vec3 normal;//法線ベクトル
		Vec2 uv;//uv座標
		uint32_t boneIndex[S_MAX_BONE_INDICES_] = { 0 };//影響を受けるボーン　番号
		float boneWeight[S_MAX_BONE_INDICES_] = { 1.0f,0,0,0 };//ボーン　重み
		Vec4 tangent;//法線の接線
	};

	//ポリゴンの座標のオフセット
	struct PolygonOffset
	{
		float ratio = 0;//メッシュのどのくらいのポリゴンに適用するか
		float length = 0;//どのくらい加算するのか(長さ)
		float interval = 0;//実行間隔
	};

public: // 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* sDevice_;

public: // メンバ関数
	Mesh();

	//頂点マッピング
	void MappingVertices(std::vector<VertexPosNormalUvSkin>* vertices = nullptr);

	/// <summary>
	/// 名前を取得
	/// </summary>
	/// <returns>名前</returns>
	const std::string& GetName() { return name_; }

	/// <summary>
	/// 名前をセット
	/// </summary>
	/// <param name="name_">名前</param>
	void SetName(const std::string& name);

	/// <summary>
	/// 頂点データの追加
	/// </summary>
	/// <param name="vertex">頂点データ</param>
	void AddVertex(const VertexPosNormalUvSkin& vertex);

	/// <summary>
	/// 頂点インデックスの追加
	/// </summary>
	/// <param name="index">インデックス</param>
	void AddIndex(uint16_t index);

	void PopIndex();

	/// <summary>
	/// 頂点データの数を取得
	/// </summary>
	/// <returns>頂点データの数</returns>
	inline size_t GetVertexCount() { return vertices_.size(); }

	/// <summary>
	/// エッジ平滑化データの追加
	/// </summary>
	/// <param name="indexPosition">座標インデックス</param>
	/// <param name="indexVertex">頂点インデックス</param>
	void AddSmoothData(uint16_t indexPosition, uint16_t indexVertex);


	/// <summary>
	/// 平滑化された頂点法線の計算
	/// </summary>
	void CalculateSmoothedVertexNormals();

	//メッシュの接線を計算
	void CalculateTangent();

	/// <summary>
	/// マテリアルの取得
	/// </summary>
	/// <returns>マテリアル</returns>
	Material* GetMaterial() { return material_; }

	/// <summary>
	/// マテリアルの割り当て
	/// </summary>
	/// <param name="material">マテリアル</param>
	void SetMaterial(Material* material);

	/// <summary>
	/// バッファの生成
	/// </summary>
	void CreateBuffers();

	//行列の情報を転送
	void SendingMat(Vec3 materialExtend, const ConstBuffTransform& cbt);

	//加算座標を頂点座標に足す
	void SendingVetices();


	/// <summary>
	/// 頂点バッファ取得
	/// </summary>
	/// <returns>頂点バッファ</returns>
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// <summary>
	/// インデックスバッファ取得
	/// </summary>
	/// <returns>インデックスバッファ</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="cmdList">命令発行先コマンドリスト</param>
	void Draw(Vec3 materialExtend, const ConstBuffTransform& cbt,
		const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex);

public:
	/// <summary>
	/// 頂点配列を取得
	/// </summary>
	/// <returns></returns>
	inline const std::vector<VertexPosNormalUvSkin>& GetVertices() { return vertices_; }

	/// <summary>
	/// インデックス配列を取得
	/// </summary>
	/// <returns></returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private: // メンバ変数
	// 名前
	std::string name_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// 頂点データ配列
	std::vector<VertexPosNormalUvSkin> vertices_;
	//オフセット適用後
	std::vector<VertexPosNormalUvSkin> offsetVertices_;
	// 頂点インデックス配列
	std::vector<uint16_t> indices_;
	// 頂点法線スムージング用データ
	std::unordered_map<uint16_t, std::vector<uint16_t>> smoothData_;
	// マテリアル
	Material* material_ = nullptr;

	//メッシュを持つノード(fbx)
	Node* meshNode_ = nullptr;
	//
	int32_t timer_ = 0;

	//グローバル変形行列（親の影響も含めた）
	DirectX::XMMATRIX globalTransform_ = {};

	ConstBuffTransform cbt_;

	PolygonOffset polygonOffsetData_;

public:
	//getter
//モデルの変形行列を取得
	const XMMATRIX& GetMeshFBXTransform() { return meshNode_->globalTransform; }

	const Node& GetMeshNode() { return *meshNode_; }

	//setter
	void SetMeshNode(Node* node) { meshNode_ = node; }

	//getter
	//ポリゴン数を取得
	int32_t GetPolygonCount() { return (int32_t)indices_.size() / 3; }
	Vec3 GetPolygonNormal(int32_t index);
	//メッシュの加算座標をランダムで出す
	void SetPolygonOffsetData(const Mesh::PolygonOffset& polygonOffsetData);
};
