#pragma once
#include"ConstBuffTransform.h"
#include"TextureManager.h"
#include "WorldMat.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "RootPipe.h"


struct ConstBufferDataMaterial
{
	Vec4 color;//�F(RGBA)
};
struct SpriteBuff
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//���_�o�b�t�@
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@�̐���
};
struct VertexSprite
{
	DirectX::XMFLOAT3 pos;//xyz���W
	DirectX::XMFLOAT2 uv;//uv���W
};

class SpriteCommon
{
private:

public:
};

