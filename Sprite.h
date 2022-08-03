#pragma once
#include"ConstBuffTransform.h"
#include"DirectX.h"
#include <wrl.h>
using namespace Microsoft::WRL;

struct PipelineSet
{
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
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
	//定数バッファの生成
	ComPtr < ID3D12Resource> constBuffMaterial = nullptr;
	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	ConstBuffTransform cbt;
	PipelineSet pipelineSet;
		// 頂点レイアウトスプライト用
	D3D12_INPUT_ELEMENT_DESC inputLayoutSprite[2] = {
	{//xyz座標
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

	{//uv座標
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	} // (1行で書いたほうが見やすい)
	};

	

	void CreateSprite(D3D12_RESOURCE_DESC resDesc);
	void SpriteCommonBeginDraw();
	void SpriteDraw();
};

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);