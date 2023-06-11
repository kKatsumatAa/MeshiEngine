#include "GausianBuffer.h"
#include "Util.h"
#include<d3d12.h>


GausianBuffer::GausianBuffer()
{


}

void GausianBuffer::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE& peraHandle, ID3D12Device& device,
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc)
{
	//ガウシアン用のバッファ
	{
		HRESULT result;

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};

		weights = std::move(GetGaussianWeights(8, 10.0f));

		//リソース設定
		ResourceProperties(cbResourceDesc,
			(UINT)AligmentSize(sizeof(weights[0]) * weights.size(), 256));
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &buff);

		//定数バッファのマッピング
		float* mappedWeight = nullptr;
		result = buff->Map(0, nullptr, (void**)&mappedWeight);

		std::copy(weights.begin(), weights.end(), mappedWeight);

		buff->Unmap(0, nullptr);
	}

	//ボケ定数バッファビュー設定
	peraHandle.ptr += device.GetDescriptorHandleIncrementSize(heapDesc.Type);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = buff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)buff->GetDesc().Width;

	device.CreateConstantBufferView(&cbvDesc, peraHandle);
}


std::vector<float> GetGaussianWeights(size_t count, float s)
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