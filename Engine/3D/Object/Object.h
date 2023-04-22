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
	Sprite* sprite;

	//ライト
	static LightManager* lightManager;

public://変数
	WorldMat* worldMat = new WorldMat();
	ViewMat* view;
	ProjectionMat* projection;
	bool isWireFrame = 0;
	//画面効果用
	static EffectConstBuffer effectFlags;
	//画面効果用
	static ComPtr <ID3D12Resource> effectFlagsBuff;
	static EffectConstBuffer* mapEffectFlagsBuff;

	//ボーンの最大数
	static const int MAX_BONES = 32;
	//定数バッファ（スキン）
	ComPtr<ID3D12Resource> constBuffSkin = nullptr;

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
	BaseCollider* collider = nullptr;


private:
	//--------------------
	void Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
		Model* model = nullptr, ModelFBX* fbx = nullptr, const bool& primitiveMode = true);

	//行列送信
	void SendingMat(int indexNum);

	//ボーンのデータ転送
	void SendingBoneData(ModelFBX* model);

	//ルートシグネチャ系のコマンド
	void SetRootPipe(ID3D12PipelineState* pipelineState, int pipelineNum, ID3D12RootSignature* rootSignature);
	//マテリアル、ライト、テクスチャ系のコマンド
	void SetMaterialLightMTexSkin(UINT64 textureHandle_, ConstBuffTransform cbt);
	void SetMaterialLightMTexSkinModel(UINT64 textureHandle_, ConstBuffTransform cbt, Material* material);


public:
	//コンストラクタ
	Object();

	//デストラクタ
	virtual ~Object();

	//初期化
	virtual bool Initialize();

	virtual void Update();

	virtual void Draw();

	//ワールド行列の取得
	const M4 GetMatWorld() { return worldMat->matWorld; }

	//コライダーのセット
	void SetCollider(BaseCollider* collider);

	//判定を有効にするか
	void SetIsValid(bool isValid);
	//スプライトかどうか
	void SetIs2D(bool is2D);

	//衝突時コールバック関数
	virtual void OnCollision(const CollisionInfo& info) {}

	//-------------

	static void DrawPera();
	static void DrawPera2();

	void DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
		WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f },
		const UINT64 textureHandle = NULL, const int& pipelineNum = 0);

	void DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f },
		const UINT64 textureHandle = NULL, const int& pipelineNum = 0);

	void DrawBoxSprite(const Vec3& pos, const float& scale, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }
		, const UINT64 textureHandle = NULL, const Vec2& ancorUV = { 0,0 }, const bool& isReverseX = false, const bool& isReverseY = false,
		float rotation = 0.0f, const int& pipelineNum = 0);

	void DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const UINT64 textureHandle = NULL, bool isPosLeftTop = true,
		const bool& isReverseX = false, const bool& isReverseY = false, float rotation = 0.0f, const int& pipelineNum = 0);

	void DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);

	void DrawLine(/*const Vec3& pos1, const Vec3& pos2, */WorldMat* world, ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color
		= { 1.0f,1.0f,1.0f,1.0f }, const UINT64 textureHandle = NULL);

	void DrawCircle(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);

	void DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);

	void DrawModel(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		Model* model, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const int& pipelineNum = 0);

	void DrawFBX(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		ModelFBX* modelFbx, XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }, const int& pipelineNum = 0);

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
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum = NULL);

void Blend(const D3D12_BLEND_OP& blendMode,
	const bool& Inversion = 0, const bool& Translucent = 0);

void SetNormDigitalMat(XMMATRIX& mat);

void Error(const bool& filed);