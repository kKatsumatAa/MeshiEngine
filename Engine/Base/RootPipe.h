#pragma once
#include"DirectXWrapper.h"

struct RootPipe
{
private:
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob; // ジオメトリシェーダオブジェクト
	ComPtr<ID3DBlob> hsBlob; // ハルシェーダオブジェクト
	ComPtr<ID3DBlob> dsBlob; // ドメインシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;

private:

public:
	void CreateBlob(const char* vsName, const char* psName, const char* gsName = nullptr
		, const char* hsName = nullptr, const char* dsName = nullptr,
		std::string vEPName = "main", std::string pEPName = "main", std::string gEPName = "main"
		, std::string hEPName = "main", std::string dEPName = "main");
};
