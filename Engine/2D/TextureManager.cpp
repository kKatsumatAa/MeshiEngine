#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"
using namespace DirectX;
using namespace Microsoft::WRL;


//格納先
const std::string TextureManager::sDirectoryPath_ = "Resources/image/";

const std::string TextureManager::S_DEFAULT_TEX_FILE_NAME_ = "white.png";

int32_t TextureManager::sCount_ = 0;
//リソース設定
D3D12_RESOURCE_DESC TextureManager::sResDesc_;
//設定をもとにSRV用デスクリプタヒープを生成
ComPtr < ID3D12DescriptorHeap> TextureManager::sSrvHeap_;
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::sSrvHandle_;

ComPtr<ID3D12Resource> TextureManager::sTexBuff_[S_SRV_COUNT_];

//SRVの最大個数
const int32_t TextureManager::S_K_MAX_SRV_COUNT_ = 2056 * 2 * 2 * 2 * 2;
//デスクリプタヒープの設定
D3D12_DESCRIPTOR_HEAP_DESC TextureManager::sSrvHeapDesc_;

D3D12_DESCRIPTOR_RANGE TextureManager::sDescriptorRange_;

uint64_t TextureManager::sWhiteTexHandle_ = 114514;

std::map < std::string, uint64_t> TextureManager::sTextureDatas_;

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager sInstance;

	return sInstance;
}

void TextureManager::InitializeDescriptorHeap()
{
	HRESULT result = {};

	//設定をもとにSRV用デスクリプタヒープを生成
	sSrvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	sSrvHeapDesc_.NumDescriptors = S_K_MAX_SRV_COUNT_;
	sSrvHeapDesc_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	//descは設定
	result = DirectXWrapper::GetInstance().GetDevice()->CreateDescriptorHeap(&sSrvHeapDesc_, IID_PPV_ARGS(sSrvHeap_.GetAddressOf()));
	assert(SUCCEEDED(result));


	//デスクリプタレンジの設定
	sDescriptorRange_.NumDescriptors = 1;   //一度の描画に使うテクスチャの枚数
	sDescriptorRange_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	sDescriptorRange_.BaseShaderRegister = 0;  //テクスチャレジスタ0番(t0)
	sDescriptorRange_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart();
}

void TextureManager::Initialize()
{
	//白い画像
	TextureManager::GetInstance().sWhiteTexHandle_ = TextureManager::LoadGraph("white.png");
}


uint64_t TextureManager::LoadGraph(const char* name, ID3D12Resource** texBuff,
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE* srvHandle)
{
	assert(sCount_ <= S_SRV_COUNT_ - 1);

	HRESULT result = {};

	//バッファがセットされていたらそこに作成する
	ID3D12Resource** texBuffL = texBuff;
	if (texBuffL == nullptr)
	{
		texBuffL = &sTexBuff_[sCount_];
	}

	//読み込まれているかどうか
	std::string fileName = name;

	//格納先が指定されてなければimage内にあるので
	if (fileName.find("Resources") == std::string::npos)
	{
		fileName = sDirectoryPath_ + fileName;
	}

	//wchar_tに戻す
	wchar_t nameWc[128] = {};
	ConstCharToWcharT(fileName.c_str(), nameWc);

	//srvHandleが指定されていたら新たにsrv作る前提なのでhandleは別のものにするため
	if (texBuff == nullptr && srvDesc == nullptr && srvHandle == nullptr)
	{
		//ファイル名から探す
		std::map<std::string, uint64_t>::iterator it = sTextureDatas_.find(fileName);
		//すでに読み込まれていたらそのハンドルを返す
		if (it != sTextureDatas_.end())
		{
			return it->second;
		}
	}

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WICのテクスチャのロード
	result = LoadFromWICFile(
		nameWc,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	//失敗したら白い画像読み込ませて抜ける
	if (!SUCCEEDED(result))
	{
		return LoadGraph(S_DEFAULT_TEX_FILE_NAME_.c_str());
	}

	ScratchImage mipChain{};
	//mipmap生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);


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
			IID_PPV_ARGS(texBuffL));
	assert(SUCCEEDED(result));

	//	FootPrint取得
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 total_bytes = 0;
	DirectXWrapper::GetInstance().GetDevice()->GetCopyableFootprints(
		&textureResourceDesc, 0, 1, 0, &footprint, nullptr, nullptr, &total_bytes);

#pragma region Upload
	D3D12_RESOURCE_DESC uploadResDesc{};

	//テクスチャをアップロードするバッファのリストに先に積んでおく
	DirectXWrapper::GetInstance().UploatBuffEmplaceBack();

	uploadResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadResDesc.Width = total_bytes;
	uploadResDesc.Height = 1;
	uploadResDesc.DepthOrArraySize = 1;
	uploadResDesc.MipLevels = 1;
	uploadResDesc.SampleDesc.Count = 1;
	uploadResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES uploadHeap{};
	uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

	DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&uploadResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//	CPUから書き込み可能、GPUは読み取り専用
		nullptr,
		IID_PPV_ARGS(DirectXWrapper::GetInstance().GetTexUploadBuffPP()));

#pragma endregion


	//	転送
	uint8_t* mapforImg = nullptr;
	result = DirectXWrapper::GetInstance().GetTexUploadBuffP()->Map(0, nullptr, (void**)&mapforImg);//マッピング

	const Image* img = scratchImg.GetImage(0, 0, 0);
	uint8_t* uploadStart = mapforImg + footprint.Offset;
	uint8_t* sourceStart = img->pixels;
	uint32_t sourcePitch = ((uint32_t)img->width * sizeof(uint32_t));

	//	画像の高さ(ピクセル)分コピーする
	for (uint32_t i = 0; i < footprint.Footprint.Height; i++)
	{
		memcpy(
			uploadStart + i * footprint.Footprint.RowPitch,
			sourceStart + i * sourcePitch,
			sourcePitch
		);
	}
	DirectXWrapper::GetInstance().GetTexUploadBuffP()->Unmap(0, nullptr);	//	unmap

#pragma region CopyCommand
	//	グラフィックボード上のコピー先アドレス
	D3D12_TEXTURE_COPY_LOCATION texCopyDest{};
	texCopyDest.pResource = *texBuffL;
	texCopyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	texCopyDest.SubresourceIndex = 0;
	//	グラフィックボード上のコピー元アドレス
	D3D12_TEXTURE_COPY_LOCATION src{};
	src.pResource = *DirectXWrapper::GetInstance().GetTexUploadBuffPP();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprint;

	//	作成
	DirectXWrapper::GetInstance().GetTexCommandList()->CopyTextureRegion(&texCopyDest, 0, 0, 0, &src, nullptr);

	//	resourceBarrier挿入
	D3D12_RESOURCE_BARRIER copyBarrier{};
	copyBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	copyBarrier.Transition.pResource = *texBuffL;
	copyBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	copyBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	copyBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	DirectXWrapper::GetInstance().GetTexCommandList()->ResourceBarrier(1, &copyBarrier);

	uint64_t textureHandle;

	//SRV
	{
		//04_03
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDescL{};//設定構造体
		//srvハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleL = {};

		//srvが引数に設定されていたら
		if (texBuff && srvDesc && srvHandle)
		{
			srvDescL = *srvDesc;
			srvHandleL = *srvHandle;
			textureHandle = -114514;
		}
		else
		{
			//SRVヒープの先頭ハンドルを取得
			sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart();
			sSrvHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(sSrvHeapDesc_.Type) * sCount_;

			srvDescL.Format = uploadResDesc.Format;
			srvDescL.Shader4ComponentMapping =
				D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDescL.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
			srvDescL.Texture2D.MipLevels = 1;

			srvHandleL = sSrvHandle_;

			//04_02(画像貼る用のアドレスを引数に)
			//SRVヒープのハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = sSrvHeap_->GetGPUDescriptorHandleForHeapStart();
			textureHandle = srvGpuHandle.ptr +
				(DirectXWrapper::GetInstance().GetDevice()->
					GetDescriptorHandleIncrementSize(TextureManager::GetInstance().sSrvHeapDesc_.Type) * sCount_);
		}

		//ハンドルのさす位置にシェーダーリソースビュー作成
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(*texBuffL, &srvDescL, srvHandleL);

		if (texBuff == nullptr)
		{
			//バッファ用のカウント
			sCount_++;
		}

		{
			//名前とデータを紐づけて保存
			//テクスチャを連想配列に格納(セットでマップに格納)
			sTextureDatas_.insert(std::make_pair(fileName, textureHandle));
		}

		//元データ解放
		scratchImg.Release();
	}

	return textureHandle;
}

void TextureManager::CheckTexHandle(uint64_t& texHandle)
{
	if (texHandle == NULL)
	{
		texHandle = sWhiteTexHandle_;
	}
}
