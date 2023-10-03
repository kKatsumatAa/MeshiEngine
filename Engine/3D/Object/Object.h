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
private:
	//図形のクラス
	static Primitive primitive_;
	//
	static RootPipe objPipeLineSet_[3];
	//al4_02_02
	static RootPipe pipelineSetM_;

	//ルートパラメータの設定
	static D3D12_ROOT_PARAMETER rootParams_[11];

private:
	//--------------------
	void DrawUpdate(int32_t indexNum, int32_t pipelineNum, uint64_t textureHandle, ConstBuffTransform* constBuffTransform,
		Camera* camera, IModel* model = nullptr, bool primitiveMode = true);

	//行列送信
	void SendingMat(int32_t indexNum, Camera* camera, IModel* model = nullptr);

	//ルートシグネチャ系のコマンド
	void SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature);
	//マテリアル、ライト、テクスチャ系のコマンド
	void SetMaterialLightMTexSkin(uint64_t textureHandle, uint64_t dissolveTex, uint64_t specularMapTex,
		uint64_t normalMapTex, bool setTex = true);
	void SetMaterialLightMTexSkinModel(uint64_t dissolveTexHandle, uint64_t specularMapTexhandle,
		uint64_t normalMapTexHandle);

	static void Blend(const D3D12_BLEND_OP& blendMode,
		bool Inversion = 0, bool Translucent = 0);

public:
	//コンストラクタ
	Object();

	//デストラクタ
	virtual ~Object();

public:
	//優先して最初の方に初期化
	static void DrawInitialize();

	//初期化
	bool Initialize(std::unique_ptr<WorldMat> worldMat = nullptr, IModel* model = nullptr);

	void Update();

	void Draw();

public:
	void DrawTriangle(Camera* camera = nullptr, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

	void DrawBox(Camera* camera = nullptr,
		const Vec4& color = { 1.0f,1.0f,1.0f,1.0f },
		uint64_t textureHandle = NULL, int32_t pipelineNum = 0);

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
};
