#pragma once
#include"IModel.h"
#include"Vec3.h"
#include"Vec4.h"


class ModelFBX :public IModel
{
public:
	//フレンドクラス
	friend class FbxLoader;

private:
	//参照先のパス(fbxかobjか)
	static const std::string S_TYPE_DIRECTORY_;

public://サブクラス
	//キーフレームごとの情報（スケールなど）
	struct NodeKeyData
	{
		XMFLOAT3 scale;
		XMFLOAT4 rotate;
		XMFLOAT3 trans;
		template<class T>
		void serialize(T& archive)
		{
			archive(scale, rotate, trans);
		}
	};

	//キーフレームごとのノード全ての情報
	struct Keyframe
	{
		//開始から何秒か
		float seconds;
		//キーフレームに対応した全てのノードの情報
		std::vector<NodeKeyData> nodeKeys;

		template<class T>
		void serialize(T& archive)
		{
			archive(seconds, nodeKeys);
		}
	};

	//スキニング行列に必要な行列セット
	struct SkinningMatSet
	{
		//メッシュノードのグローバルトランスフォーム
		DirectX::XMMATRIX globalTransform;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitTransform;
	};

	//アニメーション情報
	struct Animation
	{
		std::string name;
		//アニメーションの長さ
		float secondsLength;
		//fps
		float frameRate;
		//開始時間
		float startTime;
		//終了時間
		float endTime;
		//1フレームごとに加算する時間
		float addTime;
		//キーフレームごとのノード全ての情報の配列
		std::vector<Keyframe> keyframes;

		template<class T>
		void serialize(T& archive)
		{
			archive(name, secondsLength, frameRate, keyframes);
		}
	};


private:
	//スキニング行列に必要な行列のセットの配列
	std::vector<SkinningMatSet> skinningMatSets_;
	//ボーンのノードのインデックス
	std::vector<uint64_t> boneNodeIndices_;
	std::vector<Animation>	animationClips_;
	//ボーンのインデックスなどを決めるために使う

#pragma region 変数
private:
	//モデル名
	std::string name_;
	//ノード配列
	std::vector<Node> nodes_;
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
	const std::vector<Node>& GetNodes() { return nodes_; }
	//ノードを名前で指定して取得
	const Node* GetNode(const std::string& name);

	FbxScene* GetFbxScene() { return fbxScene_; }

	uint64_t GetBoneIndex(const std::string& name) const;


	std::vector<SkinningMatSet>& GetSkinningMatSets() { return skinningMatSets_; }
	std::vector<uint64_t>& GetBoneNodeIndices() { return boneNodeIndices_; }
	std::vector<Animation>& GetAnimations() { return animationClips_; }

#pragma endregion
};

