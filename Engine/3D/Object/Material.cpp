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
ID3D12Device* Material::sDevice_ = nullptr;

void Material::StaticInitialize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!Material::sDevice_);

	Material::sDevice_ = device;
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

void Material::LoadTexture(const std::string& directoryPath)
{
	// �t�@�C���p�X������
	string filepath = directoryPath + textureFilename_;
	// �e�N�X�`���Ȃ�
	if (textureFilename_.size() == 0) {
		filepath = "white.png";
	}

	textureHandle_ = TextureManager::LoadGraph(filepath.c_str());
}

void Material::Update()
{
	HRESULT result;
	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataMaterial2* constMap = nullptr;
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = ambient_;
		constMap->diffuse = diffuse_;
		constMap->specular = specular_;
		constMap->alpha = alpha_;
		constBuff_->Unmap(0, nullptr);
	}
}
