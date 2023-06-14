#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"
using namespace DirectX;

int32_t TextureManager::count = 0;
//リソース設定
D3D12_RESOURCE_DESC TextureManager::resDesc;
//設定をもとにSRV用デスクリプタヒープを生成
ComPtr < ID3D12DescriptorHeap> TextureManager::srvHeap;
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::srvHandle;

ComPtr<ID3D12Resource> TextureManager::texBuff[srvCount];

//SRVの最大個数
const size_t TextureManager::kMaxSRVCount = 2056;
//デスクリプタヒープの設定
D3D12_DESCRIPTOR_HEAP_DESC TextureManager::srvHeapDesc;

D3D12_DESCRIPTOR_RANGE TextureManager::descriptorRange;

uint64_t TextureManager::whiteTexHandle = 114514;

std::map < std::string, uint64_t> TextureManager::textureDatas_;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager instance;

	return instance;
}

void TextureManager::InitializeDescriptorHeap()
{
	//設定をもとにSRV用デスクリプタヒープを生成
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	//descは設定
	DirectXWrapper::GetInstance().result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));


	//デスクリプタレンジの設定
	descriptorRange.NumDescriptors = 100;   //一度の描画に使うテクスチャの枚数
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //テクスチャレジスタ0番(t0)
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
}


void TextureManager::LoadGraph(const wchar_t* name, uint64_t& textureHandle)
{
	assert(count <= srvCount - 1);

	//読み込まれているかどうか
	char namec[128] = {};
	ConstWCharTToChar(name, namec);

	std::string fileName = namec;
	{
		//ファイル名から探す
		std::map<std::string, uint64_t>::iterator it = textureDatas_.find(fileName);
		//すでに読み込まれていたらそのハンドルを返す
		if (it != textureDatas_.end())
		{
			textureHandle = it->second;
			return;
		}
	}

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	DirectXWrapper::GetInstance().result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(DirectXWrapper::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap生成
	DirectXWrapper::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(DirectXWrapper::GetInstance().result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);


	HRESULT result;

	//コピー先用
	// ヒープの設定
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(uint64_t)metadata.width,
			(uint32_t)metadata.height,
			(uint16_t)metadata.arraySize,
			(uint16_t)metadata.mipLevels,
			1);

	// テクスチャバッファの生成
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&texBuff[count]));
	assert(SUCCEEDED(result));


	//アップロードバッファ
	// ヒープの設定
	uint64_t uploadSize = GetRequiredIntermediateSize(texBuff[count].Get(), 0, (uint32_t)metadata.mipLevels);

	D3D12_HEAP_PROPERTIES uploadHeapProp{};
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC resDesc =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuff;

	// テクスチャバッファの生成
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&uploadHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuff));
	assert(SUCCEEDED(result));


	//サブリソース生成
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (size_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// 全ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = img->rowPitch;
		subResourcesDatas[i].SlicePitch = img->slicePitch;
	}

	//サブリソースを転送
	UpdateSubresources(
		DirectXWrapper::GetInstance().GetCommandList(),
		texBuff[count].Get(),
		uploadBuff.Get(),
		0,
		0,
		(uint32_t)metadata.mipLevels,
		subResourcesDatas.data()
	);

	//SRV
	{
		//SRVヒープの先頭ハンドルを取得
		if (count == 0) { srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart(); }
		else { srvHandle.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type); }

		//04_03
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;
		//ハンドルのさす位置にシェーダーリソースビュー作成
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(TextureManager::GetInstance().texBuff[count].Get(), &srvDesc, srvHandle);

		//04_02(画像貼る用のアドレスを引数に)
		//SRVヒープのハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		textureHandle = srvGpuHandle.ptr + (DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetInstance().srvHeapDesc.Type) * count);

		{
			//名前とデータを紐づけて保存
			//サウンドデータを連想配列に格納(複製してセットでマップに格納)
			textureDatas_.insert(std::make_pair(fileName, textureHandle));
		}

		//元データ解放
		scratchImg.Release();
	}

	//バリアとフェンス
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = texBuff[count].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;//ここが重要
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;//ここも重要

	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &BarrierDesc);
	DirectXWrapper::GetInstance().GetCommandList()->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { DirectXWrapper::GetInstance().GetCommandList() };
	DirectXWrapper::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, cmdlists);

	//コマンド閉じる
	DirectXWrapper::GetInstance().GetCommandQueue()->Signal(DirectXWrapper::GetInstance().GetFence(),
		++DirectXWrapper::GetInstance().GetFenceVal());

	if (DirectXWrapper::GetInstance().GetFence()->GetCompletedValue() != DirectXWrapper::GetInstance().GetFenceVal())
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		DirectXWrapper::GetInstance().GetFence()->SetEventOnCompletion(DirectXWrapper::GetInstance().GetFenceVal(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//バッファ解放
	/*uploadBuff->Release();
	uploadBuff.Reset();*/

	//コマンドリセット
	DirectXWrapper::GetInstance().CommandReset();


	//バッファ用のカウント
	count++;
}