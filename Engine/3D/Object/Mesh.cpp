#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Mesh::pSDevice_ = nullptr;

void Mesh::StaticInitialize(ID3D12Device* pDevice)
{
	// 再初期化チェック
	assert(!Mesh::pSDevice_);

	Mesh::pSDevice_ = pDevice;

	// マテリアルの静的初期化
	Material::StaticInitialize(pDevice);
}

void Mesh::SetName(const std::string& name)
{
	name_ = name;
}

void Mesh::AddVertex(const VertexPosNormalUvSkin& vertex)
{
	vertices_.emplace_back(vertex);
}

void Mesh::AddIndex(uint16_t index)
{
	indices_.emplace_back(index);
}

void Mesh::PopIndex()
{
	indices_.pop_back();
}

void Mesh::AddSmoothData(uint16_t indexPosition, uint16_t indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr) {
		// 各面用の共通頂点コレクション
		std::vector<uint16_t>& v = itr->second;
		// 全頂点の法線を平均する
		XMVECTOR normal = {};
		for (uint16_t index : v) {
			normal += XMVectorSet(vertices_[index].normal.x, vertices_[index].normal.y, vertices_[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		for (uint16_t index : v) {
			vertices_[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
		}
	}
}

void Mesh::SetMaterial(Material* pMaterial)
{
	pMaterial_ = pMaterial;
}

void Mesh::CreateBuffers()
{
	HRESULT result = {};

	uint32_t sizeVB = static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * indices_.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));


	// 頂点バッファへのデータ転送
	VertexPosNormalUvSkin* pVertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&pVertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), pVertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(VertexPosNormalUvSkin);

	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	BuffProperties(heapProps, resourceDesc, &indexBuff_);

	// インデックスバッファへのデータ転送
	uint16_t* pIndexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&pIndexMap);
	if (SUCCEEDED(result)) {

		std::copy(indices_.begin(), indices_.end(), pIndexMap);

		indexBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;
}

void Mesh::Draw(ID3D12GraphicsCommandList* pCmdList)
{
	// 頂点バッファをセット
	pCmdList->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファをセット
	pCmdList->IASetIndexBuffer(&ibView_);
	// シェーダリソースビューをセット
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = pMaterial_->textureHandle_;
	pCmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// マテリアルの定数バッファをセット
	ID3D12Resource* pConstBuff = pMaterial_->GetConstantBuffer();
	pCmdList->SetGraphicsRootConstantBufferView(3, pConstBuff->GetGPUVirtualAddress());

	// 描画コマンド
	pCmdList->DrawIndexedInstanced((uint32_t)indices_.size(), 1, 0, 0, 0);
}
