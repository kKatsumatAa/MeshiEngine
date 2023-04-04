#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>
#include <DirectXTex.h>


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
private:
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//フレンドクラス
	friend class FbxLoader;

public://サブクラス
	//頂点データ構造体
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;//座標
		DirectX::XMFLOAT3 normal;//法線ベクトル
		DirectX::XMFLOAT2 uv;//uv座標
	};

#pragma region 変数
private:
	//モデル名
	std::string name;
	//ノード行列
	std::vector<Node> nodes;
	//メッシュを持つノード
	Node* mashNode = nullptr;
	//頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	//頂点インデックス配列
	std::vector<unsigned short> indices;

	// アンビエント影響度
	XMFLOAT3 ambient;	
	// ディフューズ影響度
	XMFLOAT3 diffuse;	
	//テクスチャメタデータ
	DirectX::TexMetadata metaData = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg = {};
	

public:

#pragma endregion

#pragma region 関数
private:

public:
#pragma endregion
};

