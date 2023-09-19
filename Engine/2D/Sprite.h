#pragma once
#include"SpriteCommon.h"
#include "Util.h"
#include "Camera2D.h"


class Sprite
{
private://�G�C���A�X
	//Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
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
	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// ���_�o�b�t�@�r���[�̍쐬
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

//���ʂ̏���
void SpriteCommonBeginDraw(RootPipe* pipelineSet);
