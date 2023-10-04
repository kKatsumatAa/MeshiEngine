#include"IObject.h"
#include"CollisionManager.h"
#include "CameraManager.h"
#include "ObjectManager.h"

using namespace DirectX;

//���[�g�p�����[�^�̐ݒ�
D3D12_ROOT_PARAMETER IObject::rootParams_[RootParamNum::count] = {};
D3D12_GRAPHICS_PIPELINE_STATE_DESC IObject::pipelineDesc_ = {};

//--------------------------------------------------------------------
IObject::~IObject()
{
	constBuffMaterial_.Reset();

	//object���ɏ�����̂ł���Ȃ�����
	if (collider_.get())
	{
		CollisionManager::GetInstance()->RemoveCollider(collider_.get());
	}
}


IObject::IObject()
{
	Construct();
}

void IObject::Construct()
{
	HRESULT result = S_OK;

	//�}�e���A���o�b�t�@�i�F�j
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((uint32_t)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
	//�萔�o�b�t�@�̐���
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial_);
	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_);//�}�b�s���O
	assert(SUCCEEDED(result));
}

//------------------------------------------------------------------------
void IObject::CommonInitialize()
{
	//���[�g�p�����[�^�̐ݒ�
	//�萔�o�b�t�@0�ԁi�F�j
	rootParams_[COLOR].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[COLOR].Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�(b0)
	rootParams_[COLOR].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[COLOR].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0�ԁi�e�N�X�`���j
	rootParams_[TEX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//�f�X�N���v�^
	rootParams_[TEX].DescriptorTable.pDescriptorRanges = &TextureManager::GetDescRange();//�f�X�N���v�^�����W
	rootParams_[TEX].DescriptorTable.NumDescriptorRanges = 1;//�V��
	rootParams_[TEX].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@1��(�s��)
	rootParams_[MATRIX].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[MATRIX].Descriptor.ShaderRegister = 1;//�萔�o�b�t�@�ԍ�(b1)
	rootParams_[MATRIX].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[MATRIX].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//���b�V�����Ƃ̍s��2��
	rootParams_[MESH_MAT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[MESH_MAT].Descriptor.ShaderRegister = 2;//�萔�o�b�t�@�ԍ�(b2)
	rootParams_[MESH_MAT].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[MESH_MAT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@3�ԁi�}�e���A���j
	rootParams_[MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[MATERIAL].Descriptor.ShaderRegister = 3;//�萔�o�b�t�@�ԍ�(b3)
	rootParams_[MATERIAL].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@4��(���C�g�p)
	rootParams_[LIGHT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[LIGHT].Descriptor.ShaderRegister = 4;//�萔�o�b�t�@�ԍ�(b4)
	rootParams_[LIGHT].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[LIGHT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@5��(���ʃt���O)
	rootParams_[EFFECT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[EFFECT].Descriptor.ShaderRegister = 5;//�萔�o�b�t�@�ԍ�(b5)
	rootParams_[EFFECT].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[EFFECT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@6�ԁi�X�L�j���O�p�j
	rootParams_[SKIN].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams_[SKIN].Descriptor.ShaderRegister = 6;//�萔�o�b�t�@�ԍ�(b6)
	rootParams_[SKIN].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams_[SKIN].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^1�ԁi�f�B�]���u�p�e�N�X�`���j
	rootParams_[DISSOLVE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//�f�X�N���v�^
	D3D12_DESCRIPTOR_RANGE dissolveDHRange = TextureManager::GetDescRange();//�f�X�N���v�^�����W
	dissolveDHRange.BaseShaderRegister++;
	rootParams_[DISSOLVE].DescriptorTable.pDescriptorRanges = &dissolveDHRange;//�f�X�N���v�^�����W
	rootParams_[DISSOLVE].DescriptorTable.NumDescriptorRanges = 1;//�V��
	rootParams_[DISSOLVE].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^2�ԁi�X�y�L�����}�b�v�p�e�N�X�`���j
	rootParams_[SPECULAR_MAP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//�f�X�N���v�^
	D3D12_DESCRIPTOR_RANGE dissolveDHRange2 = TextureManager::GetDescRange();//�f�X�N���v�^�����W
	dissolveDHRange2.BaseShaderRegister += 2;
	rootParams_[SPECULAR_MAP].DescriptorTable.pDescriptorRanges = &dissolveDHRange2;//�f�X�N���v�^�����W
	rootParams_[SPECULAR_MAP].DescriptorTable.NumDescriptorRanges = 1;//�V��
	rootParams_[SPECULAR_MAP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^3�ԁi�m�[�}���}�b�v�p�e�N�X�`���j
	rootParams_[NORM_MAP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//�f�X�N���v�^
	D3D12_DESCRIPTOR_RANGE dissolveDHRange3 = TextureManager::GetDescRange();//�f�X�N���v�^�����W;
	dissolveDHRange3.BaseShaderRegister += 3;
	rootParams_[NORM_MAP].DescriptorTable.pDescriptorRanges = &dissolveDHRange3;//�f�X�N���v�^�����W
	rootParams_[NORM_MAP].DescriptorTable.NumDescriptorRanges = 1;//�V��
	rootParams_[NORM_MAP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����
}

//-------------------------------------------------------------------------
void IObject::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (worldMat == nullptr)
	{
		worldMat = std::move(worldMat_);
	}
}

//---------------------------------------------------------------------
void IObject::SetColliderIsValid(bool isValid)
{
	if (collider_)
	{
		collider_->SetIsValid(isValid);
	}
}

void IObject::SetCollider(std::unique_ptr<BaseCollider> collider)
{
	collider->SetObject(this);
	collider_ = std::move(collider);
	//�R���W�����}�l�[�W���[�ɓo�^
	CollisionManager::GetInstance()->AddCollider(collider_.get());
	//�s��,�R���C�_�[�̍X�V
	WorldMatColliderUpdate();
}

void IObject::SetIsAlive(bool isAlive)
{
	isAlive_ = isAlive;
	//�����蔻��̕�������
	if (collider_)
	{
		collider_->SetIsValid(false);
	}
}

void IObject::SetIsValid(bool isValid)
{
	isValid_ = isValid;
	SetColliderIsValid(isValid);

	if (isValid == true)
	{
		worldMat_->CalcAllTreeMat();
	}
}

//----------------------------------------------------------------------
void IObject::WorldMatColliderUpdate()
{
	//�s��X�V�i���[���h���W�n�ɂ��ē����蔻����s���j
	worldMat_->CalcAllTreeMat();
	//�����蔻��X�V
	if (collider_.get())
	{
		collider_->Update();
	}
}

void IObject::Update()
{
	WorldMatColliderUpdate();
}

//----------------------------------------------------------------------
void IObject::DrawImGui(std::function<void()>imguiF)
{
	std::string str = "NO_NAME";
	if (objName_.size())
	{
		str = objName_;
	}

	ImGui::Begin(str.c_str());

	//�����t���O
	ImGui::Checkbox("isAlive: ", &isAlive_);
	if (isAlive_ == false && collider_)
	{
		collider_->SetIsValid(false);
	}

	//�g�����X�Ȃ�
	if (ImGui::TreeNode("TransScaleRot")) {

		ImGui::DragFloat3("Trans: ", &worldMat_->trans_.x);
		ImGui::DragFloat3("Scale: ", &worldMat_->scale_.x);
		ImGui::DragFloat3("Rot: ", &worldMat_->rot_.x);

		ImGui::TreePop();
	}

	//�h���N���X���Ƃ�
	if (imguiF)
	{
		imguiF();
	}

	ImGui::End();
}

//---------------------------------------------------------------------------------------------------------------------
void IObject::SetRootPipe(ID3D12PipelineState* pipelineState, int32_t pipelineNum, ID3D12RootSignature* rootSignature)
{
	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->SetPipelineState(&pipelineState[pipelineNum]);

	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature);
}


//---------------------------------------------------------------------------------------------------------------------
void IObject::PipeLineSetting(const D3D12_FILL_MODE& fillMode, RootPipe& rootPipe,
	const std::string& vSName, const std::string& pSName,
	D3D12_INPUT_ELEMENT_DESC* inputLayout, uint32_t inputLCount,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE priTopoType, bool isSprite)
{
	HRESULT result = {};

	rootPipe.CreateBlob(vSName.c_str(), pSName.c_str());

	// �V�F�[�_�[�̐ݒ�
	pipelineDesc_.VS.pShaderBytecode = rootPipe.vsBlob->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = rootPipe.vsBlob->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = rootPipe.psBlob->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = rootPipe.psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	if (isSprite)
	{
		pipelineDesc_.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
	{
		pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �w�ʃJ�����O
	}

	pipelineDesc_.RasterizerState.FillMode = fillMode; // �|���S�����h��Ԃ�
	pipelineDesc_.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	Blend(D3D12_BLEND_OP_ADD, false, true);

	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc_.InputLayout.NumElements = inputLCount;

	// �}�`�̌`��ݒ�
	pipelineDesc_.PrimitiveTopologyType = priTopoType;

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
	if (isSprite)
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

void IObject::Blend(const D3D12_BLEND_OP& blendMode, bool Inversion, bool Translucent)
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