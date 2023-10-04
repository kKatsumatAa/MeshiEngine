#pragma once
#include"IObject.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "ModelManager.h"


//演出用のフラグ
struct EffectOConstBuffer
{
	//フォグ
	uint32_t isFog = false;
	//トゥーン
	uint32_t isToon = false;
	//リムライト
	uint32_t isRimLight = false;
	float pad1;
	//リムの色
	DirectX::XMFLOAT4 rimColor = { 1.0f,1.0f,1.0f,0 };
	//疑似シルエット
	uint32_t isSilhouette = false;
	//ディゾルブ
	uint32_t isDissolve = false;
	//ディゾルブ割合
	float dissolveT = 0;
	//スペキュラマップ
	uint32_t isSpecularMap = false;
	//ノーマルマップ
	uint32_t isNormalMap = false;
	//時間
	uint32_t time = 0;
};

//3Dオブジェクトの親クラス--------------------------------------------
class IObject3D : public IObject
{
protected:
	//パイプラインなどの設定
	static RootPipe pipelineSetM_[2];

private:
	//演出用バッファ
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	EffectOConstBuffer* mapEffectFlagsBuff_;

protected:
	//モデルのポインタ
	IModel* model_ = nullptr;

private:
	//親子関係を結ぶモデルのノード
	const Node* parentNode_ = nullptr;
	ModelFBX* parentNodeModel_ = nullptr;

protected:
	//ディゾルブの画像ハンドル
	uint64_t dissolveTextureHandle_ = NULL;
	//スペキュラマップの画像
	uint64_t specularMapTextureHandle_ = NULL;
	//ノーマルマップの画像
	uint64_t normalMapTextureHandle_ = NULL;
	//演出フラグ
	EffectOConstBuffer effectFlags_;

protected:
	//メッシュのオフセット
	Mesh::PolygonOffset meshOffsetData_;

private:
	//ライト
	static LightManager* sLightManager_;

private:
	//初期の正面ベクトル
	Vec3 frontVecTmp_ = { 0,0,-1.0f };
	//正面ベクトル
	Vec3 frontVec_ = frontVecTmp_;


public://関数---------------------------------------------------------
	virtual ~IObject3D() override;
	IObject3D();

protected:
	//継承コンストラクタ
	virtual void Construct()override;

public:
	static void CommonInitialize();

	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr, IModel* model = nullptr);

public:
	//更新
	virtual void Update() override;
	//描画
	virtual void Draw()override;
	//
	void DrawModel(Camera* camera = nullptr, bool isWireFrame = false);

	//演出系のアップデート
	void EffectUpdate();
	//imgui
	virtual void DrawImGui(std::function<void()>imguiF = NULL)override;

protected:
	//モデル描画の内部処理
	virtual void DrawModelInternal(const RootPipe& pipelineSet, int32_t pipelineNum) { ; }

protected:
	//行列マッピング
	void MatMap(Camera* camera, IModel* model = nullptr);

	//マテリアル、ライト、テクスチャ系のコマンド
	void SetMaterialLightMTex(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
		uint64_t normalMapTex, bool setTex = true);

public:
	//モデルの部位と親子関係を持たせる
	void ParentFbxNode(IObject* obj, IModel* model, const std::string& nodeName);
	//親ノードを解除
	void ResetParentFbxNode();

public:
	//ディゾルブ画像ハンドル
	void SetDissolveTexHandle(uint64_t dissolveTextureHandle) { dissolveTextureHandle_ = dissolveTextureHandle; }
	//ディゾルブ割合
	void SetDissolveT(float t) { effectFlags_.dissolveT = t; }
	void SetisDissolve(bool isDissolve) { effectFlags_.isDissolve = isDissolve; }

	//スペキュラマップ
	void SetIsSpecularMap(bool isSpecularMap) { effectFlags_.isSpecularMap = isSpecularMap; }
	void SetSpecularMapTexHandle(uint64_t specularMapTextureHandle) { specularMapTextureHandle_ = specularMapTextureHandle; }

	//ノーマルマップ
	void SetIsNormalMap(bool isNormalMap) { effectFlags_.isNormalMap = isNormalMap; }
	void SetNormalMapTexHandle(uint64_t normalMapTextureHandle) { normalMapTextureHandle_ = normalMapTextureHandle; }

	//シルエット
	void SetIsSilhouette(bool is) { effectFlags_.isSilhouette = is; }

public:
	//モデルのポインタ
	virtual void SetModel(IModel* model);
	IModel* GetModel() { return model_; }

	//正面ベクトル
	//オブジェクトの角度で回転させた正面ベクトルをゲット
	const Vec3& GetFrontVec();
	void CalcFrontVec();
	const Vec3& GetFrontVecTmp() { return frontVecTmp_; }
	inline void SetFrontVecTmp(const Vec3& vec) { frontVecTmp_ = vec; }
	inline void SetFrontVec(const Vec3& vec) { frontVec_ = vec; }

public:
	//メッシュのオフセット用
	void SetMeshPolygonOffsetData(const Mesh::PolygonOffset& offsetData) { meshOffsetData_ = offsetData; }

public:
	/// <summary>
/// ライトのセット
/// </summary>
/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { sLightManager_ = lightManager; }

};