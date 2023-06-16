#include "Material.h"
#include <DirectXTex.h>
#include <cassert>
#include "TextureManager.h"
#include "Util.h"

using namespace DirectX;
using namespace std;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Material::pSDevice_ = nullptr;

void Material::StaticInitialize(ID3D12Device* pDevice)
{
	// �ď������`�F�b�N
	assert(!Material::pSDevice_);

	Material::pSDevice_ = pDevice;
}

std::unique_ptr<Material> Material::Create()
{
	std::unique_ptr<Material> instance = std::make_unique<Material>();

	instance->Initialize();

	return std::move(instance);
}

void Material::Initialize()
{
	// �萔�o�b�t�@�̐���
	CreateConstantBuffer();
}

void Material::CreateConstantBuffer()
{
	// �萔�o�b�t�@�̐���
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((uint32_t)sizeof(Material::ConstBufferDataMaterial2) + 0xff) & ~0xff/*256�o�C�g�A���C�����g*/);
	//�萔�o�b�t�@�̐���
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuff_);
}

Material::~Material()
{

}

void Material::LoadTexture(const std::string& directoryPath, const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuHandle, const CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
	// �e�N�X�`���Ȃ�
	if (textureFilename_.size() == 0) {
		textureFilename_ = "image/white.png";
	}

	cpuDescHandleSRV_ = cpuHandle;
	gpuDescHandleSRV_ = gpuHandle;

	HRESULT result = S_FALSE;

	// WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �t�@�C���p�X������
	string filepath = directoryPath + textureFilename_;

	const char* P_NAME = filepath.c_str();
	wchar_t wchar[128];
	ConstCharToWcharT(P_NAME, wchar);
	TextureManager::LoadGraph(wchar, textureHandle_);
}

void Material::Update()
{
	HRESULT result;
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataMaterial2* pConstMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&pConstMap);
	if (SUCCEEDED(result)) {
		pConstMap->ambient = ambient_;
		pConstMap->diffuse = diffuse_;
		pConstMap->specular = specular_;
		pConstMap->alpha = alpha_;
		constBuff_->Unmap(0, nullptr);
	}
}

const CD3DX12_GPU_DESCRIPTOR_HANDLE& Material::GetGpuHandle()
{
	gpuDescHandleSRV_.ptr = textureHandle_;
	return gpuDescHandleSRV_;
}
