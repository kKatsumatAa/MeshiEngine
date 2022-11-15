#include "Sprite.h"

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size;						//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resource,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(Directx::GetInstance().result));
}


void Sprite::Initialize()
{
	//sprite�p
	{
		UINT sizeVB;
		D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
		// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
		sizeVB = static_cast<UINT>(sizeof(vertices[0]) * 4.0);
		//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

		ResourceProperties(resDesc, sizeVB);
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		//���_�o�b�t�@�̐���
		BuffProperties(heapProp, resDesc, vertBuff.GetAddressOf());

		// ���_�o�b�t�@�r���[�̍쐬
		// GPU���z�A�h���X
		vbView.BufferLocation = vertBuff.Get()->GetGPUVirtualAddress();
		// ���_�o�b�t�@�̃T�C�Y
		vbView.SizeInBytes = sizeVB;
		// ���_1���̃f�[�^�T�C�Y
		vbView.StrideInBytes = sizeof(vertices[0]);//kokogamatigatteita(primitive.vertices[0]ni!)
	}
}

void Sprite::SpriteCommonBeginDraw(SpriteSet* pipelineSet)
{
	Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineSet->pipelineState.Get());

	Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());

	Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	/*ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
	cmdlist->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);*/
}

void Sprite::SpriteDraw()
{
	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	VertexSprite* vertMap = nullptr;
	Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(Directx::GetInstance().result));
	// �S���_�ɑ΂���
	for (int i = 0; i < 4; i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	//Directx::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
