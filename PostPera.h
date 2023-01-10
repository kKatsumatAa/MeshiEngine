#pragma once
#include"TextureManager.h"
#include "Directx.h"

struct PeraVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pv[4] = {
		{{-1, -1,0.1f},{0,1}},//ç∂â∫
		{{-1, 1,0.1f},{0,0}},//ç∂è„
		{{-1, -1,0.1f},{1,1}},//âEâ∫
		{{-1, 1,0.1f},{1,0}}//âEè„
	};

	ComPtr<ID3D12Resource> _peraVB;
	D3D12_VERTEX_BUFFER_VIEW _peraVBV;

	ComPtr<ID3DBlob> vs;
	ComPtr<ID3DBlob> ps;
	ComPtr<ID3DBlob> errBlob;

	ComPtr<ID3D12RootSignature> _peraRS;
	ComPtr<ID3D12PipelineState> _peraPipeline=NULL;

public:
	void Initialize();
	void GenerateRSPL();
	void Draw();
};

