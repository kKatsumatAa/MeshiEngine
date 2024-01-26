#include "GausianBuffer.h"
#include "Util.h"
#include<d3d12.h>

using namespace Util;

const float GausianBuffer::S_GAUSSIAN_WEIGHT_EXP_RATE_ = -0.5f;
const float GausianBuffer::S_GAUSSIAN_ADD_WEIGHT_ = 1.0f;
const float GausianBuffer::S_GAUSSIAN_WEIGHT_RATE_ = 2.0f;


//----------------------------------------------
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

		weights_ = std::move(GetGaussianWeights(S_GAUSSIAN_WEIGHT_COUNT_, GAUSSIAN_WEIGHT_SIGMA_));

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
		wgt = expf(GausianBuffer::S_GAUSSIAN_WEIGHT_EXP_RATE_ * (x * x) / s);
		total += wgt * GausianBuffer::S_GAUSSIAN_WEIGHT_RATE_;
		x += GausianBuffer::S_GAUSSIAN_ADD_WEIGHT_;
	}
	total = total * GausianBuffer::S_GAUSSIAN_WEIGHT_RATE_ - GausianBuffer::S_GAUSSIAN_ADD_WEIGHT_;

	// 足し て 1 に なる よう に する 
	for (auto& wgt : weights)
	{
		wgt /= total;
	}

	return weights;
}