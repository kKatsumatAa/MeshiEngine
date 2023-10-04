#pragma once
#include"IObject3D.h"
#include <FbxLoader.h>
#include "LightManager.h"
#include "Mesh.h"


class ObjectFBX : public IObject3D
{
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

private:
	// 頂点レイアウトの設定
// 頂点レイアウト
	static D3D12_INPUT_ELEMENT_DESC sInputLayout_[7];

protected:
	//定数バッファ（スキン）
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;
	ConstBufferDataSkin* constMapSkin = nullptr;

private://fbxモデル系
	//アニメーション
	std::vector<AnimationData>animeDatas_;
	//アニメーションのインデックス
	int32_t animeIndex_ = 0;
	std::vector<Node>* nodes_;

public://関数
	ObjectFBX();
	virtual ~ObjectFBX();

public:
	//共通の初期化
	static void CommonInitialize();

private:
	//継承コンストラクタ
	void Construct()override;

private:
	//マテリアル、ボーン行列などをセット
	void SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
		uint64_t normalMapTexHandle);
	//内部描画処理
	void DrawModelInternal(const RootPipe& pipelineSet, int32_t pipelineNum)override;

public:
	virtual void Update()override;
	//imgui
	void DrawImGui(std::function<void()>imguiF = NULL)override;

public:
	void SetModel(IModel* model)override;

public:
	//アニメーション開始
	void PlayAnimation(bool isLoop, int32_t animeIndex = 0);
	void PlayReverseAnimation(bool isLoop, int32_t animeIndex = 0);
	//アニメーションフラグ
	void SetIsPlayAnimation(bool isPlay, int32_t animeIndex = 0) { animeDatas_[animeIndex].isPlay_ = isPlay; }
	void SetIsLoopAnimation(bool isLoop, int32_t animeIndex = 0) { animeDatas_[animeIndex].isLoop_ = isLoop; }
	void SetIsReverseAnimation(bool isReverse, int32_t animeIndex = 0) { animeDatas_[animeIndex].isReverse_ = isReverse; }
	//アニメーションスピード
	void SetAnimationSpeed(float speed) {
		animeDatas_[animeIndex_].animationSpeed_ = speed;
	}
	//ボーンを得る
	const XMMATRIX* GetModelBones()const { return constMapSkin->bones; }
	//オブジェクトクラスが持ってるfbxモデルのノード
	const std::vector<Node>& GetNodes();
	//ボーンのデータ転送
	void MappingBoneData(ModelFBX* model);

public:
	//ノードの行列をアニメーションに合わせて変更
	void BlendAnimationUpdate();
	//fbxのノードの行列更新
	void UpdateFBXNodeMat();
	//アニメーションのアップデート
	void AnimationUpdate();

private:
	//アニメーション開始
	void PlayAnimationInternal(int32_t animeIndex,
		bool isLoop = false, bool isReverse = false);
	//アニメーションリセット
	void AnimationReset(int32_t animeIndex);

	//アニメーションで変化したノードやボーンの処理
	void CalcNodeMatBoneMatInternal(ModelFBX* model);
	//ボーンの行列を計算
	XMMATRIX GetCalcSkinMat(IModel* model, int32_t index);

};