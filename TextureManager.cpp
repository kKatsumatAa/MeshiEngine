#include "TextureManager.h"

int TextureManager::count = 0;
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

UINT64 TextureManager::whiteTexHandle = NULL;

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
	srvHeapDesc.NumDescriptors = kMaxSRVCount + 2;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	//descは設定
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
	assert(SUCCEEDED(Directx::GetInstance().result));


	//デスクリプタレンジの設定
	descriptorRange.NumDescriptors = 100;   //一度の描画に使うテクスチャの枚数
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;  //テクスチャレジスタ0番(t0)
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//白い画像
	if (whiteTexHandle == NULL)
	{
		LoadGraph(L"Resources/image/white.png", whiteTexHandle);
	}
}


void TextureManager::LoadGraph(const wchar_t* name, UINT64& textureHandle)
{
	assert(count <= srvCount - 1);

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	Directx::GetInstance().result = LoadFromWICFile(
		name,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(Directx::GetInstance().result));

	ScratchImage mipChain{};
	//mipmap生成
	Directx::GetInstance().result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(Directx::GetInstance().result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);


	HRESULT result;

	//アップロード用

	const Image* img = scratchImg.GetImage(0, 0, 0); // 生 データ 抽出

	//中間バッファーとしてのアップロードヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	//マップ可能にするため、UPLOADにする
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//アップロード用に使用すること前提なのでUNKNOWNでよい
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	uploadHeapProp.CreationNodeMask = 0; // 単一 アダプター の ため 0
	uploadHeapProp.VisibleNodeMask = 0; // 単一 アダプター の ため 0

	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Format = DXGI_FORMAT_UNKNOWN;//単なるデータの塊なのでUNKNOWN
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;//単なるバッファーとして指定
	resDesc.Width = AligmentSize(img->slicePitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)
		* img->height;//データサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;//連続したデータ
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;//特にフラグなし
	resDesc.SampleDesc.Count = 1;//通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Quality = 0;

	// 中間 バッファー 作成 
	ComPtr <ID3D12Resource> uploadbuff = nullptr;
	result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,//特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadbuff)
	);


	//コピー先用
		// テクスチャ の ため の ヒープ 設定 
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;//テクスチャ用
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;//単一アダプターのため0
	texHeapProp.VisibleNodeMask = 0;//単一アダプターのため0

	//リソース設定（変数は使いまわし）
	resDesc.Format = metadata.format;
	resDesc.Width = metadata.width;//幅
	resDesc.Height = metadata.height;//高さ
	resDesc.DepthOrArraySize = metadata.arraySize;//2Dで配列でもないので1
	resDesc.MipLevels = metadata.mipLevels;//ミップマップしないのでミップ数は1つ
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	// テクスチャバッファー の 作成 
	//ID3D12Resource* texbuff = nullptr;
	result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&texHeapProp, D3D12_HEAP_FLAG_NONE,//特に指定なし
		&resDesc, D3D12_RESOURCE_STATE_COPY_DEST,//コピー先
		nullptr, IID_PPV_ARGS(&texBuff[count]));

	//マップ
	uint8_t* mapforImg = nullptr;//image->pixelsと同じ型にする
	result = uploadbuff->Map(0, nullptr, (void**)&mapforImg);//マップ

	uint8_t* srcAddress = img->pixels;
	auto rowPitch = AligmentSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	for (int y = 0; y < img->height; ++y) {
		std::copy_n(srcAddress, rowPitch, mapforImg);//コピー
		//1行ごとのつじつまを合わせる
		srcAddress += img->rowPitch;
		mapforImg += rowPitch;
	}
	uploadbuff->Unmap(0, nullptr);//アンマップ

	//
	D3D12_TEXTURE_COPY_LOCATION src = {};
	//コピー元（アップロード側）設定
	src.pResource = uploadbuff.Get();//中間バッファー
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;//フットプリント指定
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = metadata.width;
	src.PlacedFootprint.Footprint.Height = metadata.height;
	src.PlacedFootprint.Footprint.Depth = metadata.depth;
	//src.PlacedFootprint.Footprint.RowPitch = img->rowPitch;
	src.PlacedFootprint.Footprint.Format = img->format;
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	//コピー先設定
	dst.pResource = texBuff[count].Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	//256バイトアライメント
	src.PlacedFootprint.Footprint.RowPitch =
		AligmentSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	//
	Directx::GetInstance().GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	//バリアとフェンス
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = texBuff[count].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;//ここが重要
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;//ここも重要

	{
		//SRVヒープの先頭ハンドルを取得
		if (count == 0) { srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart(); }
		else { srvHandle.ptr += Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(srvHeapDesc.Type); }

		//04_03
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = 1;
		//ハンドルのさす位置にシェーダーリソースビュー作成
		Directx::GetInstance().GetDevice()->CreateShaderResourceView(TextureManager::GetInstance().texBuff[count].Get(), &srvDesc, srvHandle);

		//04_02(画像貼る用のアドレスを引数に)
		//SRVヒープのハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		textureHandle = srvGpuHandle.ptr + (Directx::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(TextureManager::GetInstance().srvHeapDesc.Type) * count);


		count++;

		////元データ解放
		scratchImg.Release();
	}

	Directx::GetInstance().GetCommandList()->ResourceBarrier(1, &BarrierDesc);
	Directx::GetInstance().GetCommandList()->Close();

	//コマンドリストの実行
	ID3D12CommandList* cmdlists[] = { Directx::GetInstance().GetCommandList() };
	Directx::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, cmdlists);

	//コマンド閉じる
	Directx::GetInstance().GetCommandQueue()->Signal(Directx::GetInstance().GetFence(),
		++Directx::GetInstance().GetFenceVal());

	if (Directx::GetInstance().GetFence()->GetCompletedValue() != Directx::GetInstance().GetFenceVal())
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		Directx::GetInstance().GetFence()->SetEventOnCompletion(Directx::GetInstance().GetFenceVal(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//バッファ解放
	uploadbuff->Release();

	//コマンドリセット
	Directx::GetInstance().CommandReset();
}