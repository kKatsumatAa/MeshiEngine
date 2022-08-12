#pragma once

#include "Util.h"
#include "Sprite.h"


/// <summary>
/// ���_�C���f�b�N�X�p
/// </summary>
enum indices
{
	TRIANGLE,
	BOX,
	CUBE,
	LINE,
	CIRCLE,
	SPHERE,
	SPRITE
};
struct Vertex
{
	XMFLOAT3 pos;   //xyz���W
	XMFLOAT3 normal;//�@���x�N�g��
	XMFLOAT2 uv;    //uv���W
};
//���_�f�[�^

class Draw
{
private:
	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC resDesc{};
	ConstBuffTransform cbt;//�������ǂ��ɂ�����΁A�C���X�^���X��ł��F�X�`��
	
	//���_�o�b�t�@�̐���
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//04_01
		//���_�f�[�^�\����
	
	
		//�萔�o�b�t�@�̐���
	ComPtr < ID3D12Resource> constBuffMaterial = nullptr;
	//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	
	
	//04_02
	
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle2;
	int count2=0;
	
	////sprite�p
	//���_�o�b�t�@�̐���
	ComPtr < ID3D12Resource> vertBuffS = nullptr;
	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbViewS{};
	

private:
	//--------------------
	void Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
		const bool& primitiveMode= true);

public://�ϐ�
	WorldMat* worldMat = new WorldMat();
	ViewMat* view = new ViewMat();
	ProjectionMat* projection = new ProjectionMat();
	bool isWireFrame = 0;

public:
	

	//
	Draw();

	void DrawTriangle(XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,
		WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color={NULL,NULL,NULL,NULL},
		const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection, /*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4,*/
		XMFLOAT4 color = { NULL,NULL,NULL,NULL },
		const UINT64 textureHandle = NULL, const int& pipelineNum=0);
	void DrawBoxSprite(const Vec3& pos, const float& scale, XMFLOAT4 color = {NULL,NULL,NULL,NULL}
		, const UINT64 textureHandle = NULL,const Vec2& ancorUV={0,0}, float rotation = 0.0f, const int& pipelineNum = 0);
	void DrawClippingBoxSprite(const Vec3& leftTop, const float& scale,const XMFLOAT2& UVleftTop,const XMFLOAT2& UVlength,
		XMFLOAT4 color = {NULL,NULL,NULL,NULL}, float rotation = 0.0f, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { NULL,NULL,NULL,NULL }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawLine(XMFLOAT3 pos1, XMFLOAT3 pos2, WorldMat* world, ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color
	, const UINT64 textureHandle=NULL, const int& pipelineNum=0);
	void DrawCircle(float radius, WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { NULL,NULL,NULL,NULL }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	void DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
		XMFLOAT4 color = { NULL,NULL,NULL,NULL }, const UINT64 textureHandle = NULL, const int& pipelineNum = 0);
	
private:
	void constBuffTransfer(const XMFLOAT4& plusRGBA);
};

void LoadGraph(const wchar_t* name, UINT64& textureHandle);

void DrawInitialize();


//
void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum = NULL);

void Blend(const D3D12_BLEND_OP& blendMode,
	const bool& Inversion = 0, const bool& Translucent = 0);





void SetNormDigitalMat(XMMATRIX& mat);

void Error(const bool& filed);