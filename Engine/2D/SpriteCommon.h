#pragma once
#include"ConstBuffTransform.h"
#include"TextureManager.h"
#include "WorldMat.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "RootPipe.h"
using namespace Microsoft::WRL;//

struct ConstBufferDataMaterial
{
	XMFLOAT4 color;//�F(RGBA)
};
struct SpriteBuff
{
	//���_�o�b�t�@
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@�̐���
};
struct VertexSprite
{
	XMFLOAT3 pos;//xyz���W
	XMFLOAT2 uv;//uv���W
};

class SpriteCommon
{
private:

public:
};

