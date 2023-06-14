#include "ModelFBX.h"
#include "DirectXWrapper.h"

ModelFBX::~ModelFBX()
{
	//fbx�V�[���̉���i�G���[�o��j
	//fbxScene->Destroy();
}

void ModelFBX::CreateBuffers()
{
	HRESULT result;//

	//���_�f�[�^�S�̂̃T�C�Y
	uint32_t sizeVB =
		static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices.size());
	//���_�o�b�t�@����
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//���_�o�b�t�@�̐���
	BuffProperties(heapProp, resDesc, vertBuff.GetAddressOf());

	//���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//���_�C���f�b�N�X�S�̂̃T�C�Y
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(unsigned short) * indices.size());
	//����
	ResourceProperties(resDesc, sizeIB);
	BuffProperties(heapProp, resDesc, indexBuff.GetAddressOf());

	//�C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexmap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexmap);
	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexmap);
		indexBuff->Unmap(0, nullptr);
	}

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeIB;
	ibView.Format = DXGI_FORMAT_R16_UINT;
}

void ModelFBX::Draw(std::function<void()>setRootParam, std::function<void()>setMaterialLightTex)
{
	// �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	setRootParam();

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &this->vbView);

	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView);

	setMaterialLightTex();

	// �`��R�}���h
	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indices.size(), 1, 0, 0, 0);
}
