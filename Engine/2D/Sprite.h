#pragma once
#include"SpriteCommon.h"
#include "Util.h"
#include "Camera2D.h"


class Sprite
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

	void Initialize();
	void SpriteDraw();

	void Update(Camera2D* camera, const Vec2& pos, const Vec2& scale,
		const Vec4& color, uint64_t textureHandle, const Vec2& ancorUV,
		bool isReverseX, bool isReverseY, const Vec3& rotation,
		ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

	void UpdateClipping(Camera2D* camera, const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const Vec4& color, uint64_t textureHandle, bool isPosLeftTop,
		bool isReverseX, bool isReverseY, const Vec3& rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

private:
	DirectX::XMMATRIX GetCameraMatrix(Camera2D* camera);

	void CalcAndSetMat(ConstBuffTransform* cbt, WorldMat& worldMat, Camera2D* camera);
};

//共通の処理
void SpriteCommonBeginDraw(RootPipe* pipelineSet);
