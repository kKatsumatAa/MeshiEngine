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
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// ���_�o�b�t�@�r���[�̍쐬
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

//���ʂ̏���
void SpriteCommonBeginDraw(RootPipe* pipelineSet);



//void SpriteCommonCreate(SpriteSet* spriteSet);