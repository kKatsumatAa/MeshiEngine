/*
* @file ModelFBX.h
* @brief Fbxのモデル
*/

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
		double seconds;
		//キーフレームに対応した全てのノードの情報
		std::vector<NodeKeyData> nodeKeys;

		template<class T>
		void serialize(T& archive)
		{
			archive(seconds, nodeKeys);
		}
	};

	struct Animation
	{
		std::string name;
		//アニメーションの長さ
		double secondsLength;
		//fps
		double frameRate;
		//開始時間
		double startTime;
		//終了時間
		double endTime;
		//1フレームごとに加算する時間
		double addTime;
		//キーフレームごとのノード全ての情報の配列
		std::vector<Keyframe> keyframes;

		template<class T>
		void serialize(T& archive)
		{
			archive(name, secondsLength, frameRate, keyframes);
		}
	};


	//変数---------------------------------
private:
	std::vector<DirectX::XMMATRIX> offsetTransforms_;
	//ボーンのノードのインデックス
	std::vector<uint64_t> boneNodeIndices_;
	std::vector<Animation>	animationDatas_;

private:
	//モデル名
	std::string name_;
	//ノード配列
	std::vector<Node> nodes_;


	//関数---------------------------------
public:
	//デストラクタ
	~ModelFBX();

private:
	//画像読み込み
	void LoadTextures()override;
	//初期化
	void Initialize()override;

public:
	//ノード配列のポインタ取得
	std::vector<Node>* GetNodes() { return &nodes_; }
	//ノードを名前で指定して取得
	const Node* GetNode(const std::string& name);

	//ボーンのインデックスを名前で取得
	uint64_t GetBoneIndex(const std::string& name) const;

	//アニメーションの番号を取得
	uint32_t GetAnimIndex(const std::string& animName);

	//オフセット行列の配列取得
	std::vector<DirectX::XMMATRIX>& GetOffsetTransforms() { return offsetTransforms_; }
	//ボーンのノードのインデックス配列取得
	std::vector<uint64_t>& GetBoneNodeIndices() { return boneNodeIndices_; }
	//アニメーション配列を取得
	std::vector<Animation>& GetAnimations() { return animationDatas_; }
};

