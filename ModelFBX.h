#pragma once
#include <string>
#include <DirectXMath.h>
#include <vector>


struct Node
{
	//名前
	std::string name;
	//ローカルスケール
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//ローカル回転角
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//ローカル移動
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//ローカル変形行列
	DirectX::XMMATRIX transform;
	//グローバル変形行列（親の影響も含めた）
	DirectX::XMMATRIX globalTransform;
	//親ノード
	Node* parent = nullptr;
};

class ModelFBX
{
public:
	//フレンドクラス
	friend class FbxLoader;

#pragma region 変数
private:
	//モデル名
	std::string name;
	//ノード行列
	std::vector<Node> nodes;
public:

#pragma endregion

#pragma region 関数
private:

public:
#pragma endregion
};

