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
	~Sprite();
	Sprite();

protected:
	//継承コンストラクタ
	virtual void Construct()override;

public:
	//スプライト全体共通の初期化
	static void CommonInitialize();
	//初期化
	void Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);

public:
	virtual void Update()override;
	virtual void Draw()override;
private:
	//描画コマンドなど
	void SpriteDraw();

private:
	void DrawUpdate(Camera2D* camera, const Vec2& pos, const Vec2& scale,
		 uint64_t textureHandle, const Vec2& ancorUV,
		bool isReverseX, bool isReverseY, const Vec3& rotation,
		ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

	void UpdateClipping(Camera2D* camera, const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		 uint64_t textureHandle, bool isPosLeftTop,
		bool isReverseX, bool isReverseY, const Vec3& rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

public:
	void DrawBoxSprite(Camera2D* camera
		, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false);

	void DrawClippingBoxSprite(Camera2D* camera, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		 bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false);

public:
	void SetTexHandle(uint64_t texHandle) { texHandle_ = texHandle; }

private:
	DirectX::XMMATRIX GetCameraMatrix(Camera2D* camera);

	void CalcAndSetMat(ConstBuffTransform* cbt, WorldMat& worldMat, Camera2D* camera);
};

//共通の処理
void SpriteCommonBeginDraw(RootPipe* pipelineSet);
