#include "Sprite.h"

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size;						//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resource,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(Directx::GetInstance().result));
}

void SpriteCommonCreate(SpriteSet* spriteSet)
{
	


}

//void Sprite::CreateSprite(D3D12_RESOURCE_DESC& resDesc, D3D12_HEAP_PROPERTIES& heapProp)
//{
//	HRESULT result = S_FALSE;
//
//	
//
//	UINT sizeVB;
//
//	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
//	sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
//
//	//頂点バッファの設定		//ヒープ設定
//	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用
//
//	ResourceProperties(resDesc, sizeVB);
//	resDesc.Format = DXGI_FORMAT_UNKNOWN;
//	//頂点バッファの生成
//	BuffProperties(heapProp, resDesc, vertBuff.GetAddressOf());
//
//	// 頂点バッファビューの作成
//	// GPU仮想アドレス
//	vbView.BufferLocation = vertBuff.Get()->GetGPUVirtualAddress();
//	// 頂点バッファのサイズ
//	vbView.SizeInBytes = sizeVB;
//	// 頂点1つ分のデータサイズ
//	vbView.StrideInBytes = sizeof(vertices[0]);
//
//	//-----------------------------------------------------------------
//	//	//定数バッファ生成
//	//resDesc.Width = ((UINT)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
//	//result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
//	//	&heapProp,//ヒープ設定
//	//	D3D12_HEAP_FLAG_NONE,
//	//	&resDesc,//リソース設定
//	//	D3D12_RESOURCE_STATE_GENERIC_READ,
//	//	nullptr,
//	//	IID_PPV_ARGS(constBuffMaterial.GetAddressOf()));
//	//assert(SUCCEEDED(result));
//
//	//// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
//	//result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
//	//assert(SUCCEEDED(result));
//
//	//cbt.Initialize(Directx::GetInstance());
//
//	//cbt.constMapTransform->mat =
//	//	XMMatrixOrthographicOffCenterLH(0.0, WindowsApp::GetInstance().window_width,
//	//		WindowsApp::GetInstance().window_height, 0.0, 0.0f, 1.0f);
//}

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
	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	VertexSprite* vertMap = nullptr;
	Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(Directx::GetInstance().result));
	// 全頂点に対して
	for (int i = 0; i < 4; i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	//Directx::GetInstance().GetCommandList()->DrawInstanced(4, 1, 0, 0);
}
