#include "PostPera.h"
#include <d3dx12.h>

void PostPera::Initialize(const wchar_t* nomalImageFileName)
{
	//ガラスフィルター
	DirectXWrapper::GetInstance().GlassFilterBuffGenerate(nomalImageFileName);

	//定数バッファ0番(画面効果フラグ)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	//画面効果用
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
		//リソース設定
		ResourceProperties(cbResourceDesc,
			((UINT)sizeof(EffectConstBuffer) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff);
		//定数バッファのマッピング
		DirectXWrapper::GetInstance().result = effectFlagsBuff->Map(0, nullptr, (void**)&mapEffectFlagsBuff);//マッピング
		assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
	}


	//バッファ
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_peraVB.ReleaseAndGetAddressOf())
	);

	//ビュー
	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	//マップ
	PeraVertex* mappedPera = nullptr;
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	std::copy(std::begin(pv), std::end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

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
	//頂点
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
	//ピクセル
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
	gpsDesc.NumRenderTargets = 2;//peraResource1は二つレンダーターゲット
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = _countof(rootParams);//ルートパラメータ数;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootsig

	D3D12_DESCRIPTOR_RANGE range[3] = {};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[0].BaseShaderRegister = 0;//0
	range[0].NumDescriptors = 1;
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[1].BaseShaderRegister = 1;//フラグを送るのがb0なので1
	range[1].NumDescriptors = 1;
	//ガラス
	range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[2].BaseShaderRegister = 1;//1
	range[2].NumDescriptors = 1;

	//実際に使うルートパラメータ
	D3D12_ROOT_PARAMETER rp[4] = {};
	//SRV
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	//ボケ定数用
	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;

	rp[2] = rootParams[0];//フラグのやつ
	//ガラスフィルタ
	rp[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[3].DescriptorTable.pDescriptorRanges = &range[2];
	rp[3].DescriptorTable.NumDescriptorRanges = 1;


	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0); //s0

	//デスクリプタテーブル+フラグのやつ
	rsDesc.NumParameters = _countof(rp);
	rsDesc.pParameters = rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sampler;


	ComPtr<ID3DBlob>rsBlob;
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rsBlob.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(
		0,
		rsBlob.Get()->GetBufferPointer(),
		rsBlob.Get()->GetBufferSize(),
		IID_PPV_ARGS(_peraRS.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	//パイプラインステート
	//一枚目
	gpsDesc.pRootSignature = _peraRS.Get();
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline.GetAddressOf())
	);
	assert(SUCCEEDED(result));


	//二枚目
	result = D3DCompileFromFile(
		L"Resources/shaders/PeraPixelShader2.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS2",
		"ps_5_0",
		0,
		0,
		ps.ReleaseAndGetAddressOf(),
		errBlob.ReleaseAndGetAddressOf()
	);
	assert(SUCCEEDED(result));

	//vs等は同じのを使って、psのみ二枚目
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
	//2枚目用パイプライン生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline2.ReleaseAndGetAddressOf())
	);
}

void PostPera::Draw(EffectConstBuffer effectFlags)
{
	this->mapEffectFlagsBuff->isEmboss = effectFlags.isEmboss;
	this->mapEffectFlagsBuff->isGaussian = effectFlags.isGaussian;
	this->mapEffectFlagsBuff->isGaussian2 = effectFlags.isGaussian2;
	this->mapEffectFlagsBuff->isGradation = effectFlags.isGradation;
	this->mapEffectFlagsBuff->isOutLine = effectFlags.isOutLine;
	this->mapEffectFlagsBuff->isSharpness = effectFlags.isSharpness;
	this->mapEffectFlagsBuff->isVignette = effectFlags.isVignette;
	this->mapEffectFlagsBuff->isBarrelCurve = effectFlags.isBarrelCurve;
	this->mapEffectFlagsBuff->isScanningLine = effectFlags.isScanningLine;
	this->mapEffectFlagsBuff->isGrayScale = effectFlags.isGrayScale;
	this->mapEffectFlagsBuff->isGlassFilter = effectFlags.isGlassFilter;
	this->mapEffectFlagsBuff->time++;


	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline.Get());
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, DirectXWrapper::GetInstance().GetPeraSRVHeap().GetAddressOf());

	auto peraHandle = DirectXWrapper::GetInstance().GetPeraSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	//パラメーター0番とヒープを関連付ける
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);

	//ボケ定数(一枚目の二つと二枚目の一つを飛ばす)
	for (int i = 0; i < 3; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, peraHandle);

	//ガラスフィルター
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(3, peraHandle);

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);

	//ポストエフェクトフラグなのでrp[2]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, effectFlagsBuff->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

void PostPera::Draw2()
{
	//DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline2.Get());
	//ヒープをセット
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, DirectXWrapper::GetInstance().GetPeraSRVHeap().GetAddressOf());

	auto peraHandle = DirectXWrapper::GetInstance().GetPeraSRVHeap()->GetGPUDescriptorHandleForHeapStart();

	//二枚目
	for (int i = 0; i < 2; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);//SRVなのでrp[0]

	//図形とか
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);

	//ポストエフェクトフラグなのでrp[2]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, effectFlagsBuff->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
