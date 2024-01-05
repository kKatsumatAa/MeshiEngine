/*
* @file ObjectFBX.h
* @brief fbxのオブジェクトクラス
*/

#pragma once
#include"IObject3D.h"
#include <FbxLoader.h>
#include "LightManager.h"
#include "Mesh.h"
#include "FBXNodeColliders.h"


class ObjectFBX : public IObject3D
{
protected:
	enum PipelineStateNum
	{
		NORMAL,
		WIREFRAME,
		SHADOW,
		HULL_DOMAIN,
		SHADOW_HULL_DOMAIN,

		COUNT//要素数
	};

public:
	//ボーンの最大数
	static const int32_t S_MAX_BONES_ = 90;
	//定数バッファ用データ構造体（スキニング）
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

public:
	//アニメーションデータ
	struct AnimationData
	{
		//アニメーション現在時間
		double currentTime_;
		//アニメーションフラグ
		bool isPlay_ = false;
		//ループ
		bool isLoop_ = false;
		//逆再生
		bool isReverse_ = false;
		//再生のスピード倍率
		float animationSpeed_ = 1.0f;
	};

	static const float NORMAL_ANIM_SPEED_;

private:
	//パイプラインなどの設定
	static RootPipe pipelineSetM_[COUNT];

protected:
	//定数バッファ（スキン）
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;
	ConstBufferDataSkin* constMapSkin_ = nullptr;

protected:
	//ノードごとの当たり判定
	FBXNodeColliders nodeColliders_;
	//ノードごとの当たり判定を描画するかどうか
	bool isValidNodeCollidersDraw_ = false;

private://fbxモデル系
	//アニメーション
	std::vector<AnimationData>animeDatas_;
	//アニメーションのインデックス
	int32_t animeIndex_ = 0;
	std::vector<Node>* nodes_ = nullptr;

public://関数
	//コンストラクタ
	ObjectFBX();
	//デストラクタ
	virtual ~ObjectFBX();

public:
	//共通の初期化
	static void CommonInitialize();

private:
	//マテリアル、ボーン行列などをセット
	void SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
		uint64_t normalMapTexHandle);
	//内部描画処理
	void DrawModelInternal(int32_t pipelineNum)override;

public:
	//更新
	virtual void Update()override;
	//imgui描画
	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	//モデルのポインタセット
	void SetModel(IModel* model)override;

public:
	//ノードごとの当たり判定を描画するかどうかセット
	void SetIsValidNodeCollidersDraw(bool isValid) { isValidNodeCollidersDraw_ = isValid; }
	//ノードごとの当たり判定を描画するかどうか取得
	bool GetIsValidNodeCollidersDraw() { return isValidNodeCollidersDraw_; }

	//ノードごとの当たり判定の判定処理をセット
	void SetNodeCollidersOnCollision(std::function<void(IObject3D* obj, const CollisionInfo& info)>onCollisionF)
	{
		nodeColliders_.SetOnCollisionFunc(onCollisionF);
	}

	//ノードごとの当たり判定初期化
	void InitializeNodeColliders(ModelFBX* model, float colliderScale, uint16_t attribute);
	//ノードごとの当たり判定更新
	void UpdateNodeColliders() { nodeColliders_.Update(worldMat_.get()); }
	//ノードごとの当たり判定描画
	void DrawNodeColliders() { if (isValidNodeCollidersDraw_) { nodeColliders_.Draw(); } }
	//ノードごとの当たり判定の無効化
	void SetIsValidNodeColliders(bool isValidColliders) { nodeColliders_.SetIsValidNodeColliders(isValidColliders); }

	//ノードごとの当たり判定を名前で取得
	IObject3D* GetNodeColliderObj(const std::string& nodeName);
	//ノードの当たり判定の配列
	const std::vector<std::unique_ptr<FBXNodeColliders::ColliderObject>>& GetNodeColliders() { return nodeColliders_.GetColliders(); }

public:
	//アニメーション開始
	void PlayAnimation(bool isLoop, int32_t animeIndex = 0, bool isResetCurrentTime = true);
	//反転アニメーション開始
	void PlayReverseAnimation(bool isLoop, int32_t animeIndex = 0, bool isResetCurrentTime = true);
	//アニメーションリセット
	void AnimationReset(int32_t animeIndex);
	//アニメーションフラグセット
	void SetIsPlayAnimation(bool isPlay, int32_t animeIndex = 0) { animeDatas_[animeIndex].isPlay_ = isPlay; }
	//ループアニメーションフラグセット
	void SetIsLoopAnimation(bool isLoop, int32_t animeIndex = 0) { animeDatas_[animeIndex].isLoop_ = isLoop; }
	//反転アニメーションフラグセット
	void SetIsReverseAnimation(bool isReverse, int32_t animeIndex = 0) { animeDatas_[animeIndex].isReverse_ = isReverse; }
	//アニメーションスピードセット
	void SetAnimationSpeed(float speed, uint32_t animeIndex) {
		animeDatas_[animeIndex].animationSpeed_ = speed;
	}
	//アニメーションスピードセット
	void SetAnimationSpeed(float speed);
	//ボーン行列を得る
	const XMMATRIX* GetModelBones()const { return constMapSkin_->bones; }
	//オブジェクトクラスが持ってるfbxモデルのノードを取得
	const std::vector<Node>& GetNodes();
	//ノードを名前で取得
	Node* GetNode(const std::string& nodeName);
	//ボーンのデータマッピング
	void MappingBoneData(ModelFBX* model);

public:
	//ノードの行列をアニメーションに合わせて変更
	void BlendAnimationUpdate(uint32_t startIndex, uint32_t endIndex, float rate);
	//fbxのノードの行列更新
	void UpdateFBXNodeMat();
	//アニメーションのアップデート
	void AnimationUpdate(uint32_t animeIndex = 0);

public:
	//ノードの足す角度をセット
	void SetNodeAddRot(const std::string& nodeName, const Vec3& addRot);

private:
	//アニメーション開始
	void PlayAnimationInternal(int32_t animeIndex,
		bool isLoop = false, bool isReverse = false, bool isResetCurrentTime = true);
	//アニメーションで変化したノードやボーンの処理
	void CalcNodeMatBoneMatInternal(ModelFBX* model);
	//ボーンの行列を計算
	XMMATRIX GetCalcSkinMat(IModel* model, int32_t index);

public:
	//アニメーションのフラグ等を取得
	const AnimationData& GetAnimData(uint32_t index);
};