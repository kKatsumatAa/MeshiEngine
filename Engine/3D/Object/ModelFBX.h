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

	//ボーンの最大数
	static const int32_t S_MAX_BONES_ = 90;
	//定数バッファ（スキン）
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;

	//1フレームの時間
	FbxTime frameTime_;
	//アニメーション開始時間
	FbxTime startTime_;
	//アニメーション終了時間
	FbxTime endTime_;
	//アニメーション現在時間
	FbxTime currentTime_;
	//アニメーションフラグ
	bool isPlay_ = false;
	//ループ
	bool isLoop_ = false;
	//逆再生
	bool isReverse_ = false;
	//再生のスピード倍率
	float animationSpeed_ = 1.0f;

public:
	//定数バッファ用データ構造体（スキニング）
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

#pragma endregion

#pragma region 関数
private:
	void LoadTextures()override;

	//
	void SendingBoneData();

	void Initialize()override;

public:
	~ModelFBX();
	ModelFBX();


public:
	std::vector<Bone>& GetBones() { return bones_; }

	FbxScene* GetFbxScene() { return fbxScene_; }

public:
	//アニメーション開始
	virtual void PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
		bool isLoop = false, bool isReverse = false);
	//アニメーションリセット
	virtual void AnimationReset(FbxTime& sTime, FbxTime& eTime);
	//アニメーション開始
	virtual void PlayAnimation(bool isLoop = false);
	virtual void PlayReverseAnimation(bool isLoop = false);
	//アニメーションフラグ
	virtual void SetIsPlayAnimation(bool isPlay) { isPlay_ = isPlay; }
	virtual void SetIsLoopAnimation(bool isLoop) { isLoop_ = isLoop; }
	virtual void SetIsReverseAnimation(bool isReverse) { isReverse_ = isReverse; }

	//アニメーションスピード
	virtual void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }

public:
	void Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex
		, const ConstBuffTransform& cbt) override;

	void DrawImGui(const std::function<void()>& childImguiF = nullptr)override;

#pragma endregion
};

