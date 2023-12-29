/*
* @file Sprite.h
* @brief 画像を表示
*/

#pragma once
#include"IObject.h"
#include"SpriteCommon.h"
#include "Util.h"
#include "Camera2D.h"


class Sprite : public IObject
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//要修正
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

protected:
	//inoutLayout
	static D3D12_INPUT_ELEMENT_DESC sInputLayoutSprite_[2];

private:
	//
	static RootPipe spritePipelineSet_;
	//テクスチャハンドル
	uint64_t texHandle_ = NULL;

public:
	const Vec2 TMP_ANCHOR_UV_ = { 0.5f,0.5f };
	const float ANCHOR_UV_MAX_ = 1.0f;
	const float TRANS_OFFSET_SCALE_RATE_ = 0.5f;

private:
	VertexSprite vertices_[4] = {
		{{0.0f,100.0f,0.0f},{0.0f,1.0f}},
		{{0.0f,0.0f,0.0f},{0.0f,0.0f}},
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
		{{100.0f,0.0f,0.0f},{1.0f,0.0f}}
	};
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

public:
	//デストラクタ
	virtual ~Sprite();
	//コンストラクタ
	Sprite();

public:
	//スプライト全体共通の初期化
	static void CommonInitialize();
	//初期化
	void Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	//更新
	virtual void Update()override;
	//描画
	virtual void Draw()override;
private:
	//描画コマンドなど
	void SpriteDrawCommand();

private:
	//位置などセットして描画(内部処理)
	void DrawUpdate(Camera2D* camera, const Vec2& pos, const Vec2& scale,
		uint64_t textureHandle, const Vec2& ancorUV,
		bool isReverseX, bool isReverseY, const Vec3& rotation,
		ConstBuffTransform* cbt);

	//切り抜いて描画（内部処理）
	void UpdateClipping(Camera2D* camera, const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		uint64_t textureHandle, bool isPosLeftTop,
		bool isReverseX, bool isReverseY, const Vec3& rotation, ConstBuffTransform* cbt);

public:
	//スプライト描画
	void DrawBoxSprite(Camera2D* camera
		, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false);
	//スプライト描画（切り抜き）
	void DrawClippingBoxSprite(Camera2D* camera, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false);

public:
	//画像のハンドルセット
	void SetTexHandle(uint64_t texHandle) { texHandle_ = texHandle; }

private:
	//カメラから行列を取得
	DirectX::XMMATRIX GetCameraMatrix(Camera2D* camera);
	//ワールド行列やカメラ行列を計算しセット
	void CalcAndSetMat(ConstBuffTransform* cbt, WorldMat& worldMat, Camera2D* camera);
};
