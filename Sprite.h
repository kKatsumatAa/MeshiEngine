#pragma once
#include"DirectX.h"
#include <wrl.h>
using namespace Microsoft::WRL;

//struct PipelineSet
//{
//	ComPtr<ID3D12PipelineState> pipelineState;
//	ComPtr<ID3D12RootSignature> rootSignature;
//};

class Sprite
{
private:


public:
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト

	struct Vertex
	{
		XMFLOAT3 pos;//xyz座標
		XMFLOAT2 uv;//uv座標
	};

};

