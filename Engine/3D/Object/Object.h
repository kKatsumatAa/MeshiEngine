#pragma once

#include "Util.h"
#include "Primitive.h"
#include "LightManager.h"
#include"CollisionInfo.h"
#include "PostPera.h"
#include <FbxLoader.h>
#include "Camera.h"

class BaseCollider;


/// <summary>
/// 頂点インデックス用
/// </summary>
enum indices
{
	TRIANGLE,
	BOX,
	CUBE,
	LINE,
	CIRCLE,
	SPHERE,
	SPRITE,
	MODEL,
	FBX
};

//画面効果用のフラグ
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
	XMFLOAT3 rimColor = { 1.0f,1.0f,1.0f };
	float pad2;
	//時間
	uint32_t time = 0;
};

class Object
{
private:
	//リソース設定
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt_;//ここをどうにかすれば、インスタンス一つでも色々描画


	//定数バッファの生成
	ComPtr < ID3D12Resource> constBuffMaterial_ = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;

	//スプライト
	std::unique_ptr<Sprite> sprite_;

	//ライト
	static LightManager* sLightManager_;

	//ボーンの最大数
	static const int32_t S_MAX_BONES_ = 32;
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

	//
	static float sRimColorF3_[3];

	//生きてるフラグ
	bool isAlive_ = true;
	//
	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();
	//初期の正面ベクトル
	Vec3 frontVec_ = { 0,0,-1.0f };
	Vec3 frontVecTmp_ = { 0,0,-1.0f };

public://変数
	bool isWireFrame_ = 0;
	//画面効果用
	static EffectOConstBuffer sEffectFlags_;
	//画面効果用
	static ComPtr <ID3D12Resource> sEffectFlagsBuff_;
	static EffectOConstBuffer* sMapEffectFlagsBuff_;

public:
	//定数バッファ用データ構造体（スキニング）
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[S_MAX_BONES_];
	};

protected://継承先まで公開
	//クラス名(デバッグ用)
	std::string objName_;
	//コライダー
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//--------------------
	void Update(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, const ConstBuffTransform& constBuffTransform,
		Camera* camera, Model* model = nullptr, ModelFBX* fbx = nullptr, bool primitiveMode = true);

	//行列送信
	void SendingMat(int32_t indexNum, Camera* camera);

	//ボーンのデータ転送
	void SendingBoneData(ModelFBX* model);

	//ルートシグネチャ系のコマンド
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//マテリアル、ライト、テクスチャ系のコマンド
	void SetMaterialLightMTexSkin(uint64_t textureHandle, ConstBuffTransform cbt);
	void SetMaterialLightMTexSkinModel(uint64_t textureHandle, ConstBuffTransform cbt, Material* material);

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
	const Vec3& GetTrans() { return worldMat_->trans_; }
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
	void CulcWorldMat() { worldMat_->CulcWorldMat(); }
	void CulcRotMat() { worldMat_->CulcRotMat(); }
	void CulcTransMat() { worldMat_->CulcTransMat(); }
	void CulcScaleMat() { worldMat_->CulcScaleMat(); }
	//ワールド行列の中身コピー
	void SetWorldMat_(WorldMat worldMat) { *worldMat_ = worldMat; }

	//オブジェクト名前
	void SetObjName(std::string objName) { objName_ = objName; }
	const std::string& GetObjName() { return objName_; }
	//コライダー
	BaseCollider* GetCollider() { return collider_.get(); }

	//生きてるか
	inline void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	inline bool GetIsAlive() { return isAlive_; }

	//正面ベクトル
	//オブジェクトの角度で回転させた正面ベクトルをゲット
	const Vec3& GetFrontVec();
	void CulcFrontVec();
	const Vec3& GetFrontVecTmp() { return frontVecTmp_; }
	inline void SetFrontVecTmp(const Vec3& vec) { frontVecTmp_ = vec; }

	//
	 Quaternion GetQuaternion() { return worldMat_->GetQuaternion(); }

	//初期化
	virtual bool Initialize();

	virtual void Update();

	static void StaticUpdate();

	virtual void Draw();

	//ワールド行列の取得
	const M4& GetMatWorld() { return worldMat_->matWorld_; }

	//コライダーのセット
	void SetCollider(std::unique_ptr<BaseCollider> collider);

	//判定を有効にするか
	void SetIsValid(bool isValid);
	//スプライトかどうか
	void SetIs2D(bool is2D);

	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) {}

	//アニメーション開始
	void PlayAnimationInternal(ModelFBX* model, FbxTime& sTime, FbxTime& eTime,
		bool isLoop = false, bool isReverse = false);
	void PlayAnimation(ModelFBX* model, bool isLoop = false);
	void PlayReverseAnimation(ModelFBX* model, bool isLoop = false);

	//-------------

	void DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
		Camera* camera = nullptr, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBoxSprite(const Vec2& pos, float scale, const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }
		, uint64_t textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec2& leftTop, float scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, float rotation = 0.0f, int32_t pipelineNum = 0);

	void DrawCube3D(Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawLine(/*const Vec3& pos1, const Vec3& pos2, */Camera* camera = nullptr, const XMFLOAT4& color
		= { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL);

	void DrawCircle(Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawSphere(Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawModel(Model* model, Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, int32_t pipelineNum = 0);

	void DrawFBX(ModelFBX* modelFbx, Camera* camera = nullptr,
		const XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f }, int32_t pipelineNum = 0);

	//色を返す
	const XMFLOAT4& GetColor() { return constMapMaterial_->color; }
	/// <summary>
	/// ライトのセット
	/// </summary>
	/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::sLightManager_ = lightManager; }

private:
	void constBuffTransfer(const XMFLOAT4& plusRGBA);
};

//優先して最初の方に初期化
void DrawInitialize();

//
void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, int32_t indexNum = NULL);

void Blend(const D3D12_BLEND_OP& blendMode,
	bool Inversion = 0, bool Translucent = 0);

void SetNormDigitalMat(XMMATRIX& mat);

void Error(bool filed);