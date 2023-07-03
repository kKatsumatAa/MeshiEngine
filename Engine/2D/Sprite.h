#pragma once
#include"SpriteCommon.h"
#include "Util.h"


class Sprite
{
private:
	

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
	void Initialize();
	void SpriteDraw();

	void Update(const Vec2& pos,  float scale,
		const XMFLOAT4& color, uint64_t textureHandle, const Vec2& ancorUV,
		 bool isReverseX, bool isReverseY, float rotation,
		ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

	void UpdateClipping(const Vec2& leftTop,  float scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const XMFLOAT4& color, uint64_t textureHandle, bool isPosLeftTop,
		 bool isReverseX, bool isReverseY, float rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);
};

//共通の処理
void SpriteCommonBeginDraw(RootPipe* pipelineSet);



//void SpriteCommonCreate(SpriteSet* spriteSet);