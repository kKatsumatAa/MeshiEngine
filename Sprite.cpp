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
