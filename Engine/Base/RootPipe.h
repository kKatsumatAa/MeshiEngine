#pragma once
#include"DirectXWrapper.h"

struct RootPipe
{
private:
public:
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;

private:

public:
	void CreateBlob(const wchar_t* vsName, const wchar_t* psName, const wchar_t* gsName = nullptr, 
		std::string vEPName = "main", std::string pEPName = "main", std::string gEPName = "main");
};
