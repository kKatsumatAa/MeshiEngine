#pragma once

#include "Util.h"
#include "Primitive.h"
#include "LightManager.h"
#include"CollisionInfo.h"
#include "PostPera.h"
#include <FbxLoader.h>
#include "Camera.h"
#include "RootPipe.h"

class BaseCollider;


/// <summary>
/// 頂点インデックス用
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

class Object
{
protected://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

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

public:
	//ボーンの最大数
	static const int32_t S_MAX_BONES_ = 90;

	//定数バッファ用データ構造体（スキニング）
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

private:
	//リソース設定
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt_;//ここをどうにかすれば、インスタンス一つでも色々描画

	//図形のクラス
	static Primitive primitive_;
	//
	static RootPipe objPipeLineSet_[3];
	//
	static RootPipe spritePipelineSet_;
	//al4_02_02
	static RootPipe pipelineSetM_;
	//FBX用
	static RootPipe pipelineSetFBX_;
	//ルートパラメータの設定
	static D3D12_ROOT_PARAMETER rootParams_[11];
	// グラフィックスパイプライン設定
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_;

	// 2.描画先の変更
		// レンダーターゲットビューのハンドルを取得
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

	//定数バッファの生成
	ComPtr < ID3D12Resource> constBuffMaterial_ = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

	//スプライト
	std::unique_ptr<Sprite> sprite_;

	//ライト
	static LightManager* sLightManager_;

	//定数バッファ（スキン）
	ComPtr<ID3D12Resource> constBuffSkin_ = nullptr;
	ConstBufferDataSkin* constMapSkin = nullptr;

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

	//画面効果用
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	EffectOConstBuffer* mapEffectFlagsBuff_;

	Object* parentObj_ = nullptr;

public://変数
	bool isWireFrame_ = 0;
	//画面効果用
	EffectOConstBuffer effectFlags_;

protected://継承先まで公開
	//クラス名(デバッグ用)
	std::string objName_;
	//コライダー
	std::unique_ptr<BaseCollider> collider_ = nullptr;


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
	void PlayAnimationInternal(FbxTime& sTime, FbxTime& eTime,
		bool isLoop = false, bool isReverse = false);
	//アニメーションリセット
	void AnimationReset(FbxTime& sTime, FbxTime& eTime);

	//ボーンのアニメーション処理
	void CalcBoneDataInternal(ModelFBX* model);
	//ボーンの行列を計算
	XMMATRIX GetCalcSkinMat(const std::vector<ModelFBX::Bone>& bones, int32_t index);

	//
	static void PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe, int32_t indexNum = NULL);

	static void Blend(const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//コンストラクタ
	Object();

	//デストラクタ
	virtual ~Object();

	//位置
	void SetTrans(const Vec3& pos) { worldMat_->trans_ = pos; }
	void SetTransX(float pos) { worldMat_->trans_.x_ = pos; }
	void SetTransY(float pos) { worldMat_->trans_.y_ = pos; }
	void SetTransZ(float pos) { worldMat_->trans_.z_ = pos; }
	Vec3 GetTrans() { return worldMat_->trans_; }
	//スケール
	void SetScale(const Vec3& scale) { worldMat_->scale_ = scale; }
	void SetScaleX(float scale) { worldMat_->scale_.x_ = scale; }
	void SetScaleY(float scale) { worldMat_->scale_.y_ = scale; }
	void SetScaleZ(float scale) { worldMat_->scale_.z_ = scale; }
	const Vec3& GetScale() { return worldMat_->scale_; }
	//回転
	void SetRot(const Vec3& rot) {
		worldMat_->rot_ = rot;
	}
	void SetRotX(float rot) { worldMat_->rot_.x_ = rot; }
	void SetRotY(float rot) { worldMat_->rot_.y_ = rot; }
	void SetRotZ(float rot) { worldMat_->rot_.z_ = rot; }
	const Vec3& GetRot() { return worldMat_->rot_; }
	//行列を更新
	void CalcWorldMat() { worldMat_->CalcWorldMat(); }
	void CalcRotMat() { worldMat_->CalcRotMat(); }
	void CalcTransMat() { worldMat_->CalcTransMat(); }
	void CalcScaleMat() { worldMat_->CalcScaleMat(); }
	//ワールド行列の中身コピー
	void SetWorldMat(std::unique_ptr<WorldMat> worldMat) { worldMat_ = std::move(worldMat); }
	//親
	void SetParent(Object* obj) { worldMat_->parent_ = obj->GetWorldMat(); }
	void SetParent(WorldMat* worldMat) { worldMat_->parent_ = worldMat; }
	WorldMat* GetParent() { return worldMat_->parent_; }
	//
	WorldMat* GetWorldMat() { return worldMat_.get(); }
	//親子関係を考慮した位置をゲット
	Vec3 GetWorldTrans() { return worldMat_->GetWorldTrans(); }

	//オブジェクト名前
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }
	//コライダー
	BaseCollider* GetCollider() { return collider_.get(); }

	//モデルのポインタ
	void SetModel(IModel* model) { model_ = model; }
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
	virtual bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

	virtual void Update();

	virtual void Draw();

	//演出系のアップデート
	void EffectUpdate();
	//行列を更新、それに伴いコライダーも
	void WorldMatColliderUpdate();

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

	//アニメーション開始
	void PlayAnimation(bool isLoop = false);
	void PlayReverseAnimation(bool isLoop = false);
	//アニメーションフラグ
	void SetIsPlayAnimation(bool isPlay) { isPlay_ = isPlay; }
	void SetIsLoopAnimation(bool isLoop) { isLoop_ = isLoop; }
	void SetIsReverseAnimation(bool isReverse) { isReverse_ = isReverse; }
	//アニメーションスピード
	void SetAnimationSpeed(float speed) { animationSpeed_ = speed; }
	//モデルの部位と親子関係を持たせる
	void ParentFbxNode(Object* obj, IModel* model, const std::string& nodeName);
	//ボーンを得る
	const XMMATRIX* GetModelBones()const { return constMapSkin->bones; }
	//ボーンのデータ転送
	void MappingBoneData(ModelFBX* model);

	//フォグとかのフラグ
	void SetIsSilhouette(bool is) { effectFlags_.isSilhouette = is; }

	//-------------

	void DrawTriangle(Camera* camera = nullptr, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBoxSprite(const Vec2& pos, const Vec2& scale, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }
		, uint64_t textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, float rotation = 0.0f, int32_t pipelineNum = 0);

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