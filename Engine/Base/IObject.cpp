#include"IObject.h"


void IObject::SetColliderIsValid(bool isValid)
{
	if (collider_)
	{
		collider_->SetIsValid(isValid);
	}
}

void IObject::InitializeCommon(std::unique_ptr<WorldMat> worldMat)
{
	worldMat_ = std::move(worldMat);
}

//---------------------------------------------------------------------------------------------------------------------
void IObject::PipeLineState(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe)
{
	HRESULT result = {};

	if (indexNum == SPRITE)
	{
		rootPipe.CreateBlob("Resources/shaders/SpriteVS.hlsl", "Resources/shaders/SpritePS.hlsl");
	}
	else if (indexNum == OBJ || indexNum == FBX)
	{
		rootPipe.CreateBlob("Resources/shaders/OBJVertexShader.hlsl", "Resources/shaders/OBJPixelShader.hlsl");
	}
	else
	{
		rootPipe.CreateBlob("Resources/shaders/BasicVS.hlsl", "Resources/shaders/BasicPS.hlsl");
	}

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc_.VS.pShaderBytecode = rootPipe.vsBlob->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = rootPipe.vsBlob->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = rootPipe.psBlob->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = rootPipe.psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	if (indexNum == SPRITE)
	{
		pipelineDesc_.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �w�ʃJ�����O

	pipelineDesc_.RasterizerState.FillMode = fillMode; // �|���S�����h��Ԃ�
	pipelineDesc_.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	Blend(D3D12_BLEND_OP_ADD, false, true);

	// ���_���C�A�E�g�̐ݒ�
	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[7] = {
	{//xyz���W
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)

		{//�@���x�N�g��
	 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)

	{//uv���W
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)

	{//�e�����󂯂�{�[���ԍ�
	 "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�{�[���̃X�L���E�F�C�g�i4�j
	 "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},

	{//�ڐ�
	 "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)

	{//�]�@��
	 "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)

	};

	//sprite
	D3D12_INPUT_ELEMENT_DESC inputLayoutSprite[2] = {
		{//xyz���W
		 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		 D3D12_APPEND_ALIGNED_ELEMENT,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1�s�ŏ������ق������₷��)

		{//uv���W
		 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		 D3D12_APPEND_ALIGNED_ELEMENT,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		} // (1�s�ŏ������ق������₷��)
	};
	if (indexNum == SPRITE)
	{
		pipelineDesc_.InputLayout.pInputElementDescs = inputLayoutSprite;
		pipelineDesc_.InputLayout.NumElements = _countof(inputLayoutSprite);
	}
	else
	{
		pipelineDesc_.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc_.InputLayout.NumElements = _countof(inputLayout);
	}

	// �}�`�̌`��ݒ�
	if (indexNum == LINE)
	{
		pipelineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}
	else
	{
		pipelineDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}

	// ���̑��̐ݒ�
	pipelineDesc_.NumRenderTargets = 3; // �`��Ώۂ�3�i�|�X�g�G�t�F�N�g�̈ꖇ�ڂ�3�j
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc_.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc_.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc_.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//04_02
	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //���s���J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    //�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                  //�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;                               //�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_�[����̂ݎg�p�\

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams_;//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams_);//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		rootSigBlob.ReleaseAndGetAddressOf(), rootPipe.errorBlob.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(result));
	result = DirectXWrapper::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootPipe.rootSignature));
	assert(SUCCEEDED(result));
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc_.pRootSignature = rootPipe.rootSignature.Get();

	//06_01
	//�f�v�X�X�e���V���X�e�[�g
	pipelineDesc_.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();

	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
	if (indexNum == SPRITE)
	{
		pipelineDesc_.DepthStencilState.DepthEnable = false;
		pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}
	else
	{
		pipelineDesc_.DepthStencilState.DepthEnable = true;//�[�x�e�X�g���s��
		pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//��������΍��i
	}
	pipelineDesc_.DSVFormat = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g

	result = DirectXWrapper::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_, IID_PPV_ARGS(rootPipe.pipelineState.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void Object::Blend(const D3D12_BLEND_OP& blendMode, bool Inversion, bool Translucent)
{
	//���ʐݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc_.BlendState.RenderTarget[0];
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA�S�Ẵ`�����l����`��
	blendDesc.BlendEnable = true;//�u�����h��L��
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//���Z
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�f�X�g�̒l��0%�g��
	//��������
	blendDesc.BlendOp = blendMode;
	if (Inversion)//���]
	{
		blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-�f�X�g�J���[�̒l
		blendDesc.DestBlend = D3D12_BLEND_ZERO;//�g��Ȃ�
	}
	else if (Translucent)//������
	{
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-�f�X�g�J���[�̒l
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l
	}
	else
	{
		blendDesc.SrcBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
		blendDesc.DestBlend = D3D12_BLEND_ONE;//�f�X�g�̒l��100%�g��
	}

	//�|�X�g�G�t�F�N�g�̈ꖇ�ڂ̓�ڗp��
	pipelineDesc_.BlendState.RenderTarget[1] = blendDesc;
}


void SetNormDigitalMat(XMMATRIX& mat)
{
	mat.r[0].m128_f32[0] = 2.0f / WindowsApp::GetInstance().WINDOW_WIDTH_;
	mat.r[1].m128_f32[1] = -2.0f / WindowsApp::GetInstance().WINDOW_HEIGHT_;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
}

void Error(bool filed, ID3DBlob* errorBlob)
{
	if (filed)
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}