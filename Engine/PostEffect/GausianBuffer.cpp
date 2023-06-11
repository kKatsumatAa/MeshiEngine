#include "GausianBuffer.h"
#include "Util.h"
#include<d3d12.h>


GausianBuffer::GausianBuffer()
{


}

void GausianBuffer::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE& peraHandle, ID3D12Device& device,
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc)
{
	//�K�E�V�A���p�̃o�b�t�@
	{
		HRESULT result;

		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};

		weights = std::move(GetGaussianWeights(8, 10.0f));

		//���\�[�X�ݒ�
		ResourceProperties(cbResourceDesc,
			(UINT)AligmentSize(sizeof(weights[0]) * weights.size(), 256));
		//�萔�o�b�t�@�̐���
		BuffProperties(cbHeapProp, cbResourceDesc, &buff);

		//�萔�o�b�t�@�̃}�b�s���O
		float* mappedWeight = nullptr;
		result = buff->Map(0, nullptr, (void**)&mappedWeight);

		std::copy(weights.begin(), weights.end(), mappedWeight);

		buff->Unmap(0, nullptr);
	}

	//�{�P�萔�o�b�t�@�r���[�ݒ�
	peraHandle.ptr += device.GetDescriptorHandleIncrementSize(heapDesc.Type);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = buff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)buff->GetDesc().Width;

	device.CreateConstantBufferView(&cbvDesc, peraHandle);
}


std::vector<float> GetGaussianWeights(size_t count, float s)
{
	std::vector<float> weights(count); // �E�F�C�g �z�� �ԋp �p 
	float x = 0.0f;
	float total = 0.0f;
	for (auto& wgt : weights)
	{
		wgt = expf(-0.5f * (x * x) / s);
		total += wgt * 2.0f;
		x += 1.0f;
	}
	total = total * 2.0f - 1.0f;

	// ���� �� 1 �� �Ȃ� �悤 �� ���� 
	for (auto& wgt : weights)
	{
		wgt /= total;
	}

	return weights;
}