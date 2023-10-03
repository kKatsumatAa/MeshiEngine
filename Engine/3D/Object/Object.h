#pragma once
#include "IObject3D.h"
#include "Primitive.h"


/// <summary>
/// オブジェクトのタイプ
/// </summary>
enum objType
{
	TRIANGLE,
	BOX,
	CUBE,
	LINE,
	CIRCLE,
	SPHERE,
	SPRITE,
	OBJ,
	FBX
};

//オブジェクトクラス
class Object : public IObject3D
{


public:
	enum ROOTPARAM_NUM
	{
		COLOR,
		TEX,
		MATRIX,
		MESH_MAT,
		MATERIAL,
		LIGHT,
		EFFECT,
		SKIN,
		DISSOLVE,
		SPECULAR_MAP,
		NORM_MAP
	};



private:
	//図形のクラス
	static Primitive primitive_;
	//
	static RootPipe objPipeLineSet_[3];
	//al4_02_02
	static RootPipe pipelineSetM_;

	//ルートパラメータの設定
	static D3D12_ROOT_PARAMETER rootParams_[11];

	//定数バッファの生成
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

	//スプライト
	std::unique_ptr<Sprite> sprite_ = nullptr;

	//ライト
	static LightManager* sLightManager_;

private:
	//親子関係を結ぶモデルのノード
	const Node* parentNode_ = nullptr;
	ModelFBX* parentNodeModel_ = nullptr;

	//生きてるフラグ
	bool isAlive_ = true;
	//
	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();
	//初期の正面ベクトル
	Vec3 frontVec_ = { 0,0,-1.0f };
	Vec3 frontVecTmp_ = { 0,0,-1.0f };

	//仮
	Vec3 velocity_ = { 0,0,0 };

	//モデルのポインタ
	IModel* model_ = nullptr;

	//ディゾルブの画像ハンドル
	uint64_t dissolveTextureHandle_ = NULL;
	//スペキュラマップの画像
	uint64_t specularMapTextureHandle_ = NULL;
	//ノーマルマップの画像
	uint64_t normalMapTextureHandle_ = NULL;

	//描画、更新するかどうか
	bool isValid_ = true;

	//メッシュのオフセット
	Mesh::PolygonOffset meshOffsetData_;


private:
	//--------------------
	void Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
		Camera* camera, IModel* model = nullptr, bool primitiveMode = true);

	//行列送信
	void SendingMat(int32_t indexNum, Camera* camera, IModel* model = nullptr);

	void SendingMat(int32_t indexNum, Camera* camera, const XMMATRIX* mat);

	//ルートシグネチャ系のコマンド
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//マテリアル、ライト、テクスチャ系のコマンド
	void SetMaterialLightMTexSkin(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
		uint64_t normalMapTex, bool setTex = true);
	void SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
		uint64_t normalMapTexHandle);

	//アニメーション開始
	void PlayAnimationInternal(int32_t animeIndex,
		bool isLoop = false, bool isReverse = false);
	//アニメーションリセット
	void AnimationReset(int32_t animeIndex);

	//アニメーションで変化したノードやボーンの処理
	void CalcNodeMatBoneMatInternal(ModelFBX* model);
	//ボーンの行列を計算
	XMMATRIX GetCalcSkinMat(IModel* model, int32_t index);

	static void Blend(const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//コンストラクタ
	Object();

	//デストラクタ
	virtual ~Object();

	//モデルのポインタ
	void SetModel(IModel* model);
	IModel* GetModel() { return model_; }

	//生きてるか
	void SetIsAlive(bool isAlive);
	bool GetIsAlive() { return isAlive_; }

	//描画、更新するかどうか
	virtual void SetIsValid(bool isValid);
	bool GetIsValid() { return isValid_; }

	//メッシュのオフセット用
	void SetMeshPolygonOffsetData(const Mesh::PolygonOffset& offsetData) { meshOffsetData_ = offsetData; }

	//正面ベクトル
	//オブジェクトの角度で回転させた正面ベクトルをゲット
	const Vec3& GetFrontVec();
	void CalcFrontVec();
	const Vec3& GetFrontVecTmp() { return frontVecTmp_; }
	inline void SetFrontVecTmp(const Vec3& vec) { frontVecTmp_ = vec; }
	inline void SetFrontVec(const Vec3& vec) { frontVec_ = vec; }

	//スピード（当たり判定に使う）
	inline void SetVelocity(const Vec3& vec) { velocity_ = vec; }
	inline const Vec3& GetVelocity() { return velocity_; }

	//クォータニオン系
	Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }
	void SetIsUseQuaternionMatRot(bool is) { worldMat_->SetIsUseQuaMatRot(is); }
	void SetMatRot(const M4& m) { worldMat_->SetRotMat(m); }

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

	//--------------------------------------------------------
	//初期化
	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr, IModel* model = nullptr);

	virtual void Update();

	virtual void Draw();

	//演出系のアップデート
	void EffectUpdate();
	//行列を更新、それに伴いコライダーも
	void WorldMatColliderUpdate();
	//ノードの行列をアニメーションに合わせて変更
	void BlendAnimationUpdate();
	//fbxのノードの行列更新
	void UpdateFBXNodeMat();
	//アニメーションのアップデート
	void AnimationUpdate();

	static void StaticUpdate();
	//--------------------------------------------------------

	//ワールド行列の取得
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//コライダーのセット
	void SetCollider(std::unique_ptr<BaseCollider> collider);

	//判定を有効にするか
	void SetColliderIsValid(bool isValid);
	//スプライトかどうか
	void SetIs2D(bool is2D);

	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) {}



	//フォグとかのフラグ
	void SetIsSilhouette(bool is) { effectFlags_.isSilhouette = is; }

	//-------------

	void DrawTriangle(Camera* camera = nullptr, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBoxSprite(Camera2D* camera, uint64_t textureHandle = NULL, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }
		, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(Camera2D* camera, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		uint64_t textureHandle = NULL, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, int32_t pipelineNum = 0);

	void DrawCube3D(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawLine(Camera* camera = nullptr, const Vec4& color
		= { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL);

	void DrawCircle(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawSphere(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawModel(IModel* model, Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, int32_t pipelineNum = 0);

	//デバッグ用の表示
	virtual void DrawImGui(std::function<void()>imguiF = NULL);

	//色を返す
	const Vec4& GetColor() { return constMapMaterial_->color; }
	/// <summary>
	/// ライトのセット
	/// </summary>
	/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::sLightManager_ = lightManager; }

	//
	//優先して最初の方に初期化
	static void DrawInitialize();

};
void SetNormDigitalMat(DirectX::XMMATRIX& mat);

void Error(bool filed, ID3DBlob* errorBlob);