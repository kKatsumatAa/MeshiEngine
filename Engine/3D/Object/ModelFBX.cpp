#include "ModelFBX.h"
#include "DirectXWrapper.h"

ModelFBX::~ModelFBX()
{
	//fbxシーンの解放（エラー出る）
	//fbxScene->Destroy();
}

void ModelFBX::CreateBuffers()
{
	HRESULT result;//

	//頂点データ全体のサイズ
	uint32_t sizeVB =
		static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices.size());
	//頂点バッファ生成
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//頂点バッファの生成
	BuffProperties(heapProp, resDesc, vertBuff.GetAddressOf());

	//頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//頂点インデックス全体のサイズ
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(unsigned short) * indices.size());
	//生成
	ResourceProperties(resDesc, sizeIB);
	BuffProperties(heapProp, resDesc, indexBuff.GetAddressOf());

	//インデックスバッファへのデータ転送
	unsigned short* indexmap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexmap);
	if (SUCCEEDED(result))
	{
		std::copy(indices.begin(), indices.end(), indexmap);
		indexBuff->Unmap(0, nullptr);
	}

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.SizeInBytes = sizeIB;
	ibView.Format = DXGI_FORMAT_R16_UINT;
}

void ModelFBX::Draw(std::function<void()>setRootParam, std::function<void()>setMaterialLightTex)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	setRootParam();

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &this->vbView);

	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView);

	setMaterialLightTex();

	// 描画コマンド
	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indices.size(), 1, 0, 0, 0);
}
