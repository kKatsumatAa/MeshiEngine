#pragma once
#include"IModel.h"


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


public:
	std::vector<Bone>& GetBones() { return bones_; }

	const std::vector<Node>& GetNodes() { return nodes_; }
	//ノードを名前で指定して取得
	const Node* GetNode(const std::string& name);
	//ノードの

	FbxScene* GetFbxScene() { return fbxScene_; }

	int32_t GetBoneIndex(const std::string& name) const;

#pragma endregion
};

