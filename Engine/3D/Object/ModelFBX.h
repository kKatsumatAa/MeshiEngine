#pragma once
#include"IModel.h"


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

class ModelFBX :public IModel
{
public:
	//フレンドクラス
	friend class FbxLoader;

private:
	//参照先のパス(fbxかobjか)
	static const std::string S_TYPE_DIRECTORY_;

public://サブクラス
	//ボーン構造体
	struct Bone
	{
		//名前
		std::string name_;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//ノードのグローバルトランスフォーム
		DirectX::XMMATRIX globalTransform;
		//クラスター(FBX側のボーン情報)
		FbxCluster* fbxCluster;
		//コンストラクタ
		Bone(const std::string& name)
		{
			name_ = name;
		}
	};

#pragma region 変数
private:
	//モデル名
	std::string name_;
	//ノード配列
	std::vector<Node> nodes_;
	//メッシュを持つノード
	Node* meshNode_ = nullptr;
	//ボーン配列
	std::vector<Bone> bones_;
	//FBXシーン
	FbxScene* fbxScene_ = nullptr;


#pragma endregion

#pragma region 関数
private:
	void LoadTextures()override;

	void Initialize()override;

public:
	~ModelFBX();

	void Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex) override;

public:
	//getter
		//モデルの変形行列を取得
	const XMMATRIX& GetModelFBXTransform() { return meshNode_->globalTransform; }

	std::vector<Bone>& GetBones() { return bones_; }

	FbxScene* GetFbxScene() { return fbxScene_; }

	const Node& GetMeshNode() { return *meshNode_; }

#pragma endregion
};

