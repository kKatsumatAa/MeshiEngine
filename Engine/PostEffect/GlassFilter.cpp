#include "GlassFilter.h"


//bool GlassFilter::CreateEffectBufferAndView()
//{
//	//法線マップをロード
//	LoadPictureFromFile(L"Resources/image/normalImage.jpg", _effectTexBuffer);
//
//
//	//ポストエフェクト用のディスクリプタヒープを生成
//	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
//
//	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	heapDesc.NumDescriptors = 1;
//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	auto result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_effectSRVHeap));
//	assert(SUCCEEDED(result));
//	//ポストエフェクト用テクスチャビューを作る
//	auto desc = _effectTexBuffer->GetDesc();
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Format = desc.Format;
//	srvDesc.Texture2D.MipLevels = 1;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(_effectTexBuffer.Get(), &srvDesc, _effectSRVHeap->GetCPUDescriptorHandleForHeapStart());
//
//
//	return true;
//}
