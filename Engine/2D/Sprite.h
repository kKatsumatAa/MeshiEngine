#pragma once
#include"IObject.h"
#include"SpriteCommon.h"
#include "Util.h"
#include "Camera2D.h"


class Sprite : public IObject
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
	//�v�C��
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

private:
	//
	static RootPipe spritePipelineSet_;

private:
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

	void Initialize(std::unique_ptr<WorldMat> worldMat = nullptr);
	void SpriteDraw();

private:
	void Update(Camera2D* camera, const Vec2& pos, const Vec2& scale,
		const Vec4& color, uint64_t textureHandle, const Vec2& ancorUV,
		bool isReverseX, bool isReverseY, const Vec3& rotation,
		ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

	void UpdateClipping(Camera2D* camera, const Vec2& leftTop, const Vec2& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		const Vec4& color, uint64_t textureHandle, bool isPosLeftTop,
		bool isReverseX, bool isReverseY, const Vec3& rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

public:
	void DrawBoxSprite(Camera2D* camera, uint64_t textureHandle = NULL, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }
		, const Vec2& ancorUV = { 0,0 }, bool isReverseX = false, bool isReverseY = false,
		int32_t pipelineNum = 0);

	void DrawClippingBoxSprite(Camera2D* camera, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		uint64_t textureHandle = NULL, const Vec4& color = { 1.0f,1.0f,1.0f,1.0f }, bool isPosLeftTop = true,
		bool isReverseX = false, bool isReverseY = false, int32_t pipelineNum = 0);

private:
	DirectX::XMMATRIX GetCameraMatrix(Camera2D* camera);

	void CalcAndSetMat(ConstBuffTransform* cbt, WorldMat& worldMat, Camera2D* camera);
};

//���ʂ̏���
void SpriteCommonBeginDraw(RootPipe* pipelineSet);
