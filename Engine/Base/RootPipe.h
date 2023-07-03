#pragma once
#include"DirectXWrapper.h"

struct RootPipe
{
private:
public:
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> gsBlob; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;

private:

public:
	void CreateBlob(const wchar_t* vsName, const wchar_t* psName, const wchar_t* gsName = nullptr, 
		std::string vEPName = "main", std::string pEPName = "main", std::string gEPName = "main");
};
