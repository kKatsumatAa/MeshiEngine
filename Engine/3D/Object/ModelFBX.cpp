#include "ModelFBX.h"
#include "DirectXWrapper.h"

ModelFBX::~ModelFBX()
{
	//fbxシーンの解放（FBXLoaderのFinalize()の前じゃないとエラー出る）
	fbxScene_->Destroy();
}

void ModelFBX::CreateBuffers()
{
	HRESULT result;//

	//頂点データ全体のサイズ
	uint32_t sizeVB =
		static_cast<uint32_t>(sizeof(VertexPosNormalUvSkin) * vertices_.size());
	//頂点バッファ生成
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//頂点バッファの生成
	BuffProperties(heapProp, resDesc, vertBuff_.GetAddressOf());

	//頂点バッファへのデータ転送
	VertexPosNormalUvSkin* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	//頂点インデックス全体のサイズ
	uint32_t sizeIB = static_cast<uint32_t>(sizeof(uint16_t) * indices_.size());
	//生成
	ResourceProperties(resDesc, sizeIB);
	BuffProperties(heapProp, resDesc, indexBuff_.GetAddressOf());

	//インデックスバッファへのデータ転送
	uint16_t* indexmap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexmap);
	if (SUCCEEDED(result))
	{
		std::copy(indices_.begin(), indices_.end(), indexmap);
		indexBuff_->Unmap(0, nullptr);
	}

	//インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeIB;
	ibView_.Format = DXGI_FORMAT_R16_UINT;
}

void ModelFBX::Draw(const std::function<void()>& setRootParam, const std::function<void()>& setMaterialLightTex)
{
	// パイプラインステートとルートシグネチャの設定コマンド
	setRootParam();

	DirectXWrapper::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectXWrapper::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	DirectXWrapper::GetInstance().GetCommandList()->IASetIndexBuffer(&ibView_);

	setMaterialLightTex();

	// 描画コマンド
	DirectXWrapper::GetInstance().GetCommandList()->DrawIndexedInstanced((uint32_t)indices_.size(), 1, 0, 0, 0);
}
