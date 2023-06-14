#pragma once

#include "Util.h"
#include "Primitive.h"
#include "LightManager.h"
#include"CollisionInfo.h"
#include "PostPera.h"
#include <FbxLoader.h>

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
	ConstBuffTransform cbt;//ここをどうにかすれば、インスタンス一つでも色々描画


	//定数バッファの生成
	ComPtr < ID3D12Resource> constBuffMaterial = nullptr;
	//定数バッファ用データ構造体（マテリアル）
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//スプライト
	std::unique_ptr<Sprite> sprite_;

	//ライト
	static LightManager* lightManager;

	//ボーンの最大数
	static const int32_t MAX_BONES = 32;
	//定数バッファ（スキン）
	ComPtr<ID3D12Resource> constBuffSkin = nullptr;

	//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//アニメーション現在時間
	FbxTime currentTime;
	//アニメーションフラグ
	bool isPlay = false;
	//ループ
	bool isLoop = false;
	//逆再生
	bool isReverse = false;

	//
	static float rimColorF3[3];

	std::unique_ptr<WorldMat> worldMat_ = std::make_unique<WorldMat>();

public://変数
	ViewMat* view;
	ProjectionMat* projection;
	bool isWireFrame = 0;
	//画面効果用
	static EffectOConstBuffer effectFlags;
	//画面効果用
	static ComPtr <ID3D12Resource> effectFlagsBuff;
	static EffectOConstBuffer* mapEffectFlagsBuff;

public:
	//定数バッファ用データ構造体（スキニング）
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

protected://継承先まで公開
	//クラス名(デバッグ用)
	const char* name = nullptr;
	//コライダー
	std::unique_ptr<BaseCollider> collider_ = nullptr;


private:
	//--------------------
	void Update(const int32_t& indexNum, const int32_t& pipelineNum, const uint64_t textureHandle, const ConstBuffTransform& constBuffTransform,
		Model* model = nullptr, ModelFBX* fbx = nullptr, const bool& primitiveMode = true);

	//行列送信
	void SendingMat(int32_t indexNum);

	//ボーンのデータ転送
	void SendingBoneData(ModelFBX* model);

	//ルートシグネチャ系のコマンド
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//マテリアル、ライト、テクスチャ系のコマンド
	void SetMaterialLightMTexSkin(uint64_t textureHandle_, ConstBuffTransform cbt);
	void SetMaterialLightMTexSkinModel(uint64_t textureHandle_, ConstBuffTransform cbt, Material* material);


public:
	//コンストラクタ
	Object();

	//デストラクタ
	virtual ~Object();

	//位置
	void SetTrans(const Vec3& pos) { worldMat_->trans = pos; }
	void SetTransX(float pos) { worldMat_->trans.x = pos; }
	void SetTransY(float pos) { worldMat_->trans.y = pos; }
	void SetTransZ(float pos) { worldMat_->trans.z = pos; }
	const Vec3& GetTrans() { return worldMat_->trans; }
	//スケール
	void SetScale(const Vec3& scale) { worldMat_->scale = scale; }
	void SetScaleX(float scale) { worldMat_->scale.x = scale; }
	void SetScaleY(float scale) { worldMat_->scale.y = scale; }
	void SetScaleZ(float scale) { worldMat_->scale.z = scale; }
	const Vec3& GetScale() { return worldMat_->scale; }
	//回転
	void SetRot(const Vec3& rot) { worldMat_->rot = rot; }
	void SetRotX(float rot) { worldMat_->rot.x = rot; }
	void SetRotY(float rot) { worldMat_->rot.y = rot; }
	void SetRotZ(float rot) { worldMat_->rot.z = rot; }
	const Vec3& GetRot() { return worldMat_->rot; }
	//行列を更新
	void CulcWorldMat() { worldMat_->SetWorld(); }
	//ワールド行列の中身コピー
	void SetWorldMat_(WorldMat worldMat) { *worldMat_ = worldMat; }

	//初期化
	virtual bool Initialize();

	virtual void Update();

	static void StaticUpdate();

	virtual void Draw();

	//ワールド行列の取得
	const M4& GetMatWorld() { return worldMat_->matWorld; }

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
		ViewMat* view, ProjectionMat* projection, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f },
		const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawBox(ViewMat* view, ProjectionMat* projection, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f },
		const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawBoxSprite(const Vec3& pos, const float& scale, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }
		, const uint64_t textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, const bool& isReverseX = false, const bool& isReverseY = false,
		float rotation = 0.0f, const int32_t& pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, bool isPosLeftTop = true,
		const bool& isReverseX = false, const bool& isReverseY = false, float rotation = 0.0f, const int32_t& pipelineNum = 0);

	void DrawCube3D(ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawLine(/*const Vec3& pos1, const Vec3& pos2, */ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color
		= { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL);

	void DrawCircle(ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawSphere(ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const uint64_t textureHandle = NULL, const int32_t& pipelineNum = 0);

	void DrawModel(ViewMat* view, ProjectionMat* projection,
		Model* model, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const int32_t& pipelineNum = 0);

	void DrawFBX(ViewMat* view, ProjectionMat* projection,
		ModelFBX* modelFbx, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const int32_t& pipelineNum = 0);

	//色を返す
	XMFLOAT4 GetColor() { return constMapMaterial->color; }
	/// <summary>
	/// ライトのセット
	/// </summary>
	/// <param name="light"></param>
	static void SetLight(LightManager* lightManager) { Object::lightManager = lightManager; }

private:
	void constBuffTransfer(const XMFLOAT4& plusRGBA);
};

//優先して最初の方に初期化
void DrawInitialize();

//
void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int32_t& indexNum = NULL);

void Blend(const D3D12_BLEND_OP& blendMode,
	const bool& Inversion = 0, const bool& Translucent = 0);

void SetNormDigitalMat(XMMATRIX& mat);

void Error(const bool& filed);