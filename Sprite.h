#pragma once
#include"ConstBuffTransform.h"
#include"DirectX.h"
#include <wrl.h>
#include "WorldMat.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
using namespace Microsoft::WRL;

struct SpriteSet
{
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト

	ProjectionMat projectionMat;
	WorldMat worldMat;
};
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;//色(RGBA)
};
struct SpriteBuff
{
	//頂点バッファ
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
		//定数バッファの生成
};
struct VertexSprite
{
	XMFLOAT3 pos;//xyz座標
	XMFLOAT2 uv;//uv座標
};



class Sprite
{
private:
	

public:
	VertexSprite vertices[4] = {
		{{0.0f,100.0f,0.0f},{0.0f,1.0f}},
		{{0.0f,0.0f,0.0f},{0.0f,0.0f}},
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
		{{100.0f,0.0f,0.0f},{1.0f,0.0f}}
	};
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	
	//回転用
	float rotation = 0.0f;

public:
	void CreateSprite(D3D12_RESOURCE_DESC resDesc);
	void SpriteCommonBeginDraw(SpriteSet* pipelineSet);
	void SpriteDraw();
};

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);

void SpriteCommonCreate(SpriteSet* spriteSet);