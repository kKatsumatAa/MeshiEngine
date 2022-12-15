#include "Mesh.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Mesh::device = nullptr;

void Mesh::StaticInitialize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!Mesh::device);

	Mesh::device = device;

	// マテリアルの静的初期化
	Material::StaticInitialize(device);
}

void Mesh::SetName(const std::string& name)
{
	this->name = name;
}

void Mesh::AddVertex(const VertexPosNormalUv& vertex)
{
	vertices.emplace_back(vertex);
}

void Mesh::AddIndex(unsigned short index)
{
	indices.emplace_back(index);
}

void Mesh::PopIndex()
{
	indices.pop_back();
}

void Mesh::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData.begin();
	for (; itr != smoothData.end(); ++itr) {
		// 各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		// 全頂点の法線を平均する
		XMVECTOR normal = {};
		for (unsigned short index : v) {
			normal += XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		for (unsigned short index : v) {
			vertices[index].normal = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2] };
		}
	}
}

void Mesh::SetMaterial(Material* material)
{
	this->material = material;
}

void Mesh::CreateBuffers()
{
	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(Directx::GetInstance().result));


	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	Directx::GetInstance().result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(VertexPosNormalUv);

	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	BuffProperties(heapProps, resourceDesc, &indexBuff);

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {

		std::copy(indices.begin(), indices.end(), indexMap);

		indexBuff->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファをセット
	cmdList->IASetIndexBuffer(&ibView);
	// シェーダリソースビューをセット
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
	srvGpuHandle.ptr = material->textureHandle;
	cmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// マテリアルの定数バッファをセット
	ID3D12Resource* constBuff = material->GetConstantBuffer();
	cmdList->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());

	// 描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}
