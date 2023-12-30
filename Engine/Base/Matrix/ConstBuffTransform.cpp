#include "ConstBuffTransform.h"
#include "TextureManager.h"


void ConstBuffTransform::Initialize()
{
	HRESULT result;

	//バッファ作成
	{
		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};

		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + DirectXWrapper::S_BUFFER_ALIGNMENT_) & ~DirectXWrapper::S_BUFFER_ALIGNMENT_/*256バイトアライメント*/;
		cbResourceDesc.Height = S_RESOURCE_DESC_HEIGHT_;
		cbResourceDesc.DepthOrArraySize = S_DEPTH_ARRAY_SIZE_;
		cbResourceDesc.MipLevels = S_MIP_LEVELS_;
		cbResourceDesc.SampleDesc.Count = S_SAMPLE_DESC_COUNT_;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
			&cbHeapProp,//ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc,//リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffTransform_));
		assert(SUCCEEDED(result));

	}
	//定数バッファのマッピング
	result = constBuffTransform_->Map(0, nullptr, (void**)&constMapTransform_);//マッピング
	assert(SUCCEEDED(result));
}

void ConstBuffTransform::DrawCommand(int32_t index)
{
	//定数バッファビュー(CBV)の設定コマンド
	DirectXWrapper::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(index, constBuffTransform_->GetGPUVirtualAddress());
}