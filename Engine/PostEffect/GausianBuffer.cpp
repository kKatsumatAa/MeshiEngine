#include "GausianBuffer.h"
#include "Util.h"
#include<d3d12.h>

GausianBuffer::GausianBuffer()
{


}

void GausianBuffer::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE& peraHandle, ID3D12Device& device,
	const D3D12_DESCRIPTOR_HEAP_DESC& heapDesc)
{
	//ガウシアン用のバッファ
	{
		HRESULT result;

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};

		weights_ = std::move(GetGaussianWeights(8, 10.0f));

		//リソース設定
		ResourceProperties(cbResourceDesc,
			(uint32_t)AligmentSize(sizeof(weights_[0]) * (uint32_t)weights_.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT));
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &buff_);

		//定数バッファのマッピング
		float* mappedWeight = nullptr;
		result = buff_->Map(0, nullptr, (void**)&mappedWeight);

		std::copy(weights_.begin(), weights_.end(), mappedWeight);

		buff_->Unmap(0, nullptr);
	}

	//ボケ定数バッファビュー設定
	peraHandle.ptr += device.GetDescriptorHandleIncrementSize(heapDesc.Type);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = buff_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (uint32_t)buff_->GetDesc().Width;

	device.CreateConstantBufferView(&cbvDesc, peraHandle);
}


std::vector<float> GetGaussianWeights(int32_t count, float s)
{
	std::vector<float> weights(count); // ウェイト 配列 返却 用 
	float x = 0.0f;
	float total = 0.0f;
	for (auto& wgt : weights)
	{
		wgt = expf(-0.5f * (x * x) / s);
		total += wgt * 2.0f;
		x += 1.0f;
	}
	total = total * 2.0f - 1.0f;

	// 足し て 1 に なる よう に する 
	for (auto& wgt : weights)
	{
		wgt /= total;
	}

	return weights;
}