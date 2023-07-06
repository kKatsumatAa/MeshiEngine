#include "GausianBuffer.h"
#include "Util.h"
#include<d3d12.h>

GausianBuffer::GausianBuffer()
{


}

void GausianBuffer::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE& peraHandle, ID3D12Device& device,
	const D3D12_DESCRIPTOR_HEAP_DESC& heapDesc)
{
	//�K�E�V�A���p�̃o�b�t�@
	{
		HRESULT result;

		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};

		weights_ = std::move(GetGaussianWeights(8, 10.0f));

		//���\�[�X�ݒ�
		ResourceProperties(cbResourceDesc,
			(uint32_t)AligmentSize(sizeof(weights_[0]) * (uint32_t)weights_.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		//�萔�o�b�t�@�̐���
		BuffProperties(cbHeapProp, cbResourceDesc, &buff_);

		//�萔�o�b�t�@�̃}�b�s���O
		float* mappedWeight = nullptr;
		result = buff_->Map(0, nullptr, (void**)&mappedWeight);

		std::copy(weights_.begin(), weights_.end(), mappedWeight);

		buff_->Unmap(0, nullptr);
	}

	//�{�P�萔�o�b�t�@�r���[�ݒ�
	peraHandle.ptr += device.GetDescriptorHandleIncrementSize(heapDesc.Type);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = buff_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (uint32_t)buff_->GetDesc().Width;

	device.CreateConstantBufferView(&cbvDesc, peraHandle);
}


std::vector<float> GetGaussianWeights(int32_t count, float s)
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