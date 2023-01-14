#include "PostPera.h"
#include <d3dx12.h>

void PostPera::Initialize()
{
	//�o�b�t�@
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_peraVB.ReleaseAndGetAddressOf())
	);

	//�r���[
	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	//�}�b�v
	PeraVertex* mappedPera = nullptr;
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	std::copy(std::begin(pv), std::end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

	//rootsig
	GenerateRSPL();
}

void PostPera::GenerateRSPL()
{
	D3D12_INPUT_ELEMENT_DESC layout[2] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	gpsDesc.InputLayout.NumElements = _countof(layout);
	gpsDesc.InputLayout.pInputElementDescs = layout;


	//pipeline
	//���_
	HRESULT result = D3DCompileFromFile(
		L"Resources/shaders/PeraVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS",
		"vs_5_0",
		0,
		0,
		vs.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));
	//�s�N�Z��
	result = D3DCompileFromFile(
		L"Resources/shaders/PeraPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS",
		"ps_5_0",
		0,
		0,
		ps.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	gpsDesc.VS.pShaderBytecode = vs->GetBufferPointer();
	gpsDesc.VS.BytecodeLength = vs->GetBufferSize();
	gpsDesc.PS.pShaderBytecode = ps->GetBufferPointer();
	gpsDesc.PS.BytecodeLength = ps->GetBufferSize();

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 0;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob>rsBlob;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rsBlob.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	result = Directx::GetInstance().GetDevice()->CreateRootSignature(
		0,
		rsBlob.Get()->GetBufferPointer(),
		rsBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(_peraRS.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//�p�C�v���C���X�e�[�g
	gpsDesc.pRootSignature = _peraRS.Get();
	result = Directx::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline.GetAddressOf())
	);
	assert(SUCCEEDED(result));
}

void PostPera::Draw()
{
	Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	Directx::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline.Get());
	Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);
	Directx::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
