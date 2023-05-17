#include "PostPera.h"
#include <d3dx12.h>
#include "ImGuiManager.h"


void PostPera::Initialize(const wchar_t* nomalImageFileName)
{
	//rtv�ȂǍ��
	InitializeBuffRTV();

	//�K���X�t�B���^�[
	GlassFilterBuffGenerate(nomalImageFileName);

	//�萔�o�b�t�@0��(��ʌ��ʃt���O)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	//��ʌ��ʗp
	{
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
		//���\�[�X�ݒ�
		ResourceProperties(cbResourceDesc,
			((UINT)sizeof(EffectConstBuffer) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
		//�萔�o�b�t�@�̐���
		BuffProperties(cbHeapProp, cbResourceDesc, &effectFlagsBuff);
		//�萔�o�b�t�@�̃}�b�s���O
		DirectXWrapper::GetInstance().result = effectFlagsBuff->Map(0, nullptr, (void**)&mapEffectFlagsBuff);//�}�b�s���O
		assert(SUCCEEDED(DirectXWrapper::GetInstance().result));
	}


	//�o�b�t�@
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

	//�r���[
	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	//�}�b�v
	PeraVertex* mappedPera = nullptr;
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	std::copy(std::begin(pv), std::end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

	GenerateRSPL();
}

//directx�̃o�b�N�o�b�t�@�Ȃǐ���������ɌĂяo��
void PostPera::InitializeBuffRTV()
{
	// �쐬 �ς� �� �q�[�v ��� �� �g�� �� ���� 1 �� ���
	auto heapDesc = DirectXWrapper::GetInstance().GetRtvheap()->GetDesc();
	// �g�� �� ���� �o�b�N�o�b�t�@�[ �� ��� �� ���p ���� 
	auto& bbuff = DirectXWrapper::GetInstance().GetBackBuffer()[0];
	auto resDesc = bbuff->GetDesc();

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//�����_�����O���̃N���A�n�Ɠ����l
	float clsClr[4] = { DirectXWrapper::GetInstance().GetClearColor()[0],DirectXWrapper::GetInstance().GetClearColor()[1],
		DirectXWrapper::GetInstance().GetClearColor()[2],DirectXWrapper::GetInstance().GetClearColor()[3] };
	D3D12_CLEAR_VALUE clearValue =
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clsClr);

	HRESULT result;

	//���\�[�X�쐬
	//�ꖇ��
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			//D3D12_RESOURCE_STATE_RENDER_TARGET�ł͂Ȃ�
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
		);
		assert(SUCCEEDED(result));
	}

	//�񖇖�
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		//D3D12_RESOURCE_STATE_RENDER_TARGET�ł͂Ȃ�
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(_peraResource2.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	// RTV �p �q�[�v �� ��� 
	heapDesc.NumDescriptors = 3;//�񖇕�(�ꖇ�ڂ̓��+�񖇖ڂ̈��)
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_peraRTVHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//rtv�����
	D3D12_CPU_DESCRIPTOR_HANDLE handleRH = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	//1����
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
			res.Get(),
			&rtvDesc,
			handleRH
		);
		//�C���N�������g
		handleRH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//�񖇖�
	//handleRH.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DirectXWrapper::GetInstance().GetDevice()->CreateRenderTargetView(
		_peraResource2.Get(),
		&rtvDesc,
		handleRH
	);


	//SRV�p�q�[�v�����
	heapDesc.NumDescriptors = 5;//�K�E�V�A���p�����[�^�A�ꖇ�ڂ̓�A�񖇖ڂ̈��,�K���X�p��4��
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(_peraSRVHeap.ReleaseAndGetAddressOf())
	);
	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// �V�F�[�_�[���\�[�X�r���[�i SRV�j �� ��� 
	peraHandle = _peraSRVHeap->GetCPUDescriptorHandleForHeapStart();
	//1����
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
			res.Get(),
			&srvDesc,
			peraHandle
		);

		//�C���N�������g
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//�񖇖�(3��)
	//�C���N�������g
	//peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(
		_peraResource2.Get(),
		&srvDesc,
		peraHandle
	);

	//�K�E�V�A���p�����[�^(4��)
	gausianBuff.Initialize(peraHandle, *DirectXWrapper::GetInstance().GetDevice(), heapDesc);
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
	gpsDesc.NumRenderTargets = 2;//peraResource1�͓�����_�[�^�[�Q�b�g
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = _countof(rootParams);//���[�g�p�����[�^��;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//rootsig

	D3D12_DESCRIPTOR_RANGE range[3] = {};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[0].BaseShaderRegister = 0;//0
	range[0].NumDescriptors = 1;
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;//b
	range[1].BaseShaderRegister = 1;//�t���O�𑗂�̂�b0�Ȃ̂�1
	range[1].NumDescriptors = 1;
	//�K���X
	range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//t
	range[2].BaseShaderRegister = 1;//1
	range[2].NumDescriptors = 1;

	//���ۂɎg�����[�g�p�����[�^
	D3D12_ROOT_PARAMETER rp[4] = {};
	//SRV
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;
	//�{�P�萔�p
	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;

	rp[2] = rootParams[0];//�t���O�̂��
	//�K���X�t�B���^
	rp[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[3].DescriptorTable.pDescriptorRanges = &range[2];
	rp[3].DescriptorTable.NumDescriptorRanges = 1;


	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0); //s0

	//�f�X�N���v�^�e�[�u��+�t���O�̂��
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

	//�p�C�v���C���X�e�[�g
	//�ꖇ��
	gpsDesc.pRootSignature = _peraRS.Get();
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline.GetAddressOf())
	);
	assert(SUCCEEDED(result));


	//�񖇖�
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

	//vs���͓����̂��g���āAps�̂ݓ񖇖�
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps.Get());
	//2���ڗp�p�C�v���C������
	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(
		&gpsDesc,
		IID_PPV_ARGS(_peraPipeline2.ReleaseAndGetAddressOf())
	);
}


void PostPera::GlassFilterBuffGenerate(const wchar_t* fileName)
{
	//�K���X�t�B���^�[�i4�ځj
	//�C���N�������g
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	this->CreateEffectBufferAndView(fileName);
}

bool PostPera::CreateEffectBufferAndView(const wchar_t* fileName)
{
	//�@���}�b�v�����[�h
	LoadPictureFromFile(fileName, this->_effectTexBuffer);

	//�|�X�g�G�t�F�N�g�p�e�N�X�`���r���[�����
	auto desc = _effectTexBuffer->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(_effectTexBuffer.Get(), &srvDesc, this->peraHandle);

	return true;
}

void PostPera::Update()
{
	ImGui::Begin("PostEffect");
	ImGui::SliderInt("GrayScale", (int*)&effectFlags.isGrayScale, 0, 1);
	ImGui::SliderInt("Emboss", (int*)&effectFlags.isEmboss, 0, 1);
	ImGui::SliderInt("Gaussian", (int*)&effectFlags.isGaussian, 0, 1);
	ImGui::SliderInt("GlassFilter", (int*)&effectFlags.isGlassFilter, 0, 1);
	ImGui::SliderInt("ScanningLine", (int*)&effectFlags.isScanningLine, 0, 1);
	ImGui::SliderInt("BarrelCurve", (int*)&effectFlags.isBarrelCurve, 0, 1);
	ImGui::SliderInt("Gradation", (int*)&effectFlags.isGradation, 0, 1);
	ImGui::SliderInt("Outline", (int*)&effectFlags.isOutLine, 0, 1);
	ImGui::SliderInt("Sharpness", (int*)&effectFlags.isSharpness, 0, 1);
	ImGui::SliderInt("Vignette", (int*)&effectFlags.isVignette, 0, 1);
	ImGui::SliderInt("Mosaic", (int*)&effectFlags.isMosaic, 0, 1);
	ImGui::SliderInt("NegaPosi", (int*)&effectFlags.isNega, 0, 1);
	ImGui::SliderInt("RGBShift", (int*)&effectFlags.isRGBShift, 0, 1);
	ImGui::End();

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
	this->mapEffectFlagsBuff->isMosaic = effectFlags.isMosaic;
	this->mapEffectFlagsBuff->isNega = effectFlags.isNega;
	this->mapEffectFlagsBuff->isRGBShift = effectFlags.isRGBShift;
	this->mapEffectFlagsBuff->time++;
}

//----------------------------------------------------------------------------------------------------------------------------------
//�ꖇ�ڂ�`��
void PostPera::Draw()
{
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline.Get());
	//�q�[�v���Z�b�g
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, _peraSRVHeap.GetAddressOf());

	auto peraHandle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
	//�p�����[�^�[0�Ԃƃq�[�v���֘A�t����
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);

	//�{�P�萔(�ꖇ�ڂ̓�Ɠ񖇖ڂ̈���΂�)
	for (int i = 0; i < 3; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, peraHandle);

	//�K���X�t�B���^�[
	peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(3, peraHandle);

	//�}�`�Ƃ�
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);

	//�|�X�g�G�t�F�N�g�t���O�Ȃ̂�rp[2]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, effectFlagsBuff->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}

//���ۂɕ`��
void PostPera::Draw2()
{
	//DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(_peraRS.Get());
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(_peraPipeline2.Get());
	//�q�[�v���Z�b�g
	DirectXWrapper::GetInstance().GetCommandList()
		->SetDescriptorHeaps(1, _peraSRVHeap.GetAddressOf());

	auto peraHandle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();

	//�񖇖�
	for (int i = 0; i < 2; i++)
	{
		peraHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, peraHandle);//SRV�Ȃ̂�rp[0]

	//�}�`�Ƃ�
	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &_peraVBV);

	//�|�X�g�G�t�F�N�g�t���O�Ȃ̂�rp[2]
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, effectFlagsBuff->GetGPUVirtualAddress());

	DirectXWrapper::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------
//�ꖇ�ڂ̃e�N�X�`���ɕ`��
void PostPera::PreDraw()
{
	//�|�X�g�G�t�F�N�g
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		barrierDesc.Transition.pResource = res.Get(); // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // �\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
			1,
			&barrierDesc
		);
	}

	// 2.�`���̕ύX
	// 1 �p�X �� 
	//auto rtvHeapPointer = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXWrapper::GetInstance().GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	/*DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(
		1, &rtvHeapPointer, false, &dsvHandle
	);*/


	auto rtvHeapPointer = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHeapPointer.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvs[2] = {
		_peraRTVHeap->GetCPUDescriptorHandleForHeapStart(),
		rtvHeapPointer
	};
	DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(2, rtvs, false, &dsvHandle);


	// 3.��ʃN���A R G B A
	//DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rtvHeapPointer, clearColor, 0, nullptr);
	float* clearColor = DirectXWrapper::GetInstance().GetClearColor();
	float clsClr[4] = { clearColor[0],clearColor[1],clearColor[2],clearColor[3] };
	for (auto& rt : rtvs)
	{
		DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rt, clsClr, 0, nullptr);
	}

	//06_01(dsv)
	DirectXWrapper::GetInstance().GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	//���t���[���[�x�o�b�t�@�̒l���`��͈͂ōł���(1.0)�Ƀ��Z�b�g�����

	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	DirectXWrapper::GetInstance().GetCommandList()->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// �V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // �؂蔲�����W��
	scissorRect.right = (LONG)(scissorRect.left + WindowsApp::GetInstance().window_width); // �؂蔲�����W�E
	scissorRect.top = 0; // �؂蔲�����W��
	scissorRect.bottom = (LONG)(scissorRect.top + WindowsApp::GetInstance().window_height); // �؂蔲�����W��
	// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	DirectXWrapper::GetInstance().GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void PostPera::PostDraw()
{
	// 5.���\�[�X�o���A��߂�
	for (ComPtr< ID3D12Resource>& res : _peraResource)
	{
		barrierDesc.Transition.pResource = res.Get(); // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // �\����Ԃ�
		DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &barrierDesc);
	}
}

//-----------------------------
//�񖇖ڂɕ`��
void PostPera::PreDraw2()
{
	//��Ԃ������_�[�^�[�Q�b�g�ɑJ��
	barrierDesc.Transition.pResource = _peraResource2.Get(); // �񖇖ڂ̃��\�[�X���w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // �\����Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(
		1,
		&barrierDesc
	);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	//3��(�ꖇ�ڂ̓�̌�)�Ȃ̂Ői�߂�
	rtvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	rtvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 2 �p�X �� 
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXWrapper::GetInstance().GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	DirectXWrapper::GetInstance().GetCommandList()->OMSetRenderTargets(
		1, &rtvHandle, false, &dsvHandle
	);


	// 3.��ʃN���A R G B A
	float* clearColor = DirectXWrapper::GetInstance().GetClearColor();
	float clsClr[4] = { clearColor[0],clearColor[1],clearColor[2],clearColor[3] };
	DirectXWrapper::GetInstance().GetCommandList()->ClearRenderTargetView(rtvHandle, clsClr, 0, nullptr);
	//06_01(dsv)
	DirectXWrapper::GetInstance().GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);
	//���t���[���[�x�o�b�t�@�̒l���`��͈͂ōł���(1.0)�Ƀ��Z�b�g�����

	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	DirectXWrapper::GetInstance().GetCommandList()->RSSetViewports(1, &WindowsApp::GetInstance().viewport);

	// �V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // �؂蔲�����W��
	scissorRect.right = (LONG)(scissorRect.left + WindowsApp::GetInstance().window_width); // �؂蔲�����W�E
	scissorRect.top = 0; // �؂蔲�����W��
	scissorRect.bottom = (LONG)(scissorRect.top + WindowsApp::GetInstance().window_height); // �؂蔲�����W��
	// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	DirectXWrapper::GetInstance().GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void PostPera::PostDraw2()
{
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // �\����Ԃ�
	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &barrierDesc);
}


//---------------------------------------------------------------------
//�񖇖ڂ̕`����܂Ƃ߂�
void PostPera::Draw2All()
{
	PreDraw2();

	Draw();

	PostDraw2();
}

//�ꖇ�ڂƓ񖇖ڂ�`�悷��(�����̕`��֐����ꖇ�ڂɕ`��)
void PostPera::DrawToPostpera(std::function<void()> f)
{
	//�ꖇ��
	{
		PreDraw();

		//�y���ɃI�u�W�F�N�g�ȂǕ`�悳����
		f();

		PostDraw();
	}

	//------------
	//�񖇖�
	{
		Draw2All();
	}
}