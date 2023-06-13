#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>
#include "Material.h"


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

class ModelFBX
{
private:// エイリアス
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


public:
	//フレンドクラス
	friend class FbxLoader;

public://定数
	//ボーンインデックス（影響を受けるボーン）の最大数
	static const int MAX_BONE_INDICES = 4;//hlslのfloat4に対応するため"4"

public://サブクラス
	//頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		DirectX::XMFLOAT3 pos;//座標
		DirectX::XMFLOAT3 normal;//法線ベクトル
		DirectX::XMFLOAT2 uv;//uv座標
		UINT boneIndex[MAX_BONE_INDICES] = { 0 };//影響を受けるボーン　番号
		float boneWeight[MAX_BONE_INDICES] = { 1.0f,0,0,0 };//ボーン　重み
	};

	//ボーン構造体
	struct Bone
	{
		//名前
		std::string name;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//クラスター(FBX側のボーン情報)
		FbxCluster* fbxCluster;
		//コンストラクタ
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};

#pragma region 変数
private:
	//モデル名
	std::string name;
	//ノード行列
	std::vector<Node> nodes;
	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	std::vector<VertexPosNormalUvSkin> vertices;
	//頂点インデックス配列
	std::vector<unsigned short> indices;

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	//ボーン配列
	std::vector<Bone> bones;

	//FBXシーン
	FbxScene* fbxScene = nullptr;

public:
	//アンビエントなど
	std::unique_ptr<Material> material;


#pragma endregion

#pragma region 関数
private:


public:
	~ModelFBX();

	//モデルの変形行列を取得
	const XMMATRIX& GetModelFBXTransform() { return meshNode->globalTransform; }

	//各種バッファ生成
	void CreateBuffers();

	//描画
	void Draw(std::function<void()>setRootParam, std::function<void()>setMaterialLightTex);

public:
	//getter
	std::vector<Bone>& GetBones() { return bones; }

	FbxScene* GetFbxScene() { return fbxScene; }

#pragma endregion
};

