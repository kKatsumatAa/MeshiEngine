#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Util;

//格納先
const std::string TextureManager::sDirectoryPath_ = "Resources/image/";
const std::string TextureManager::S_DEFAULT_TEX_FILE_NAME_ = "white.dds";
int32_t TextureManager::sSRVCount_ = 0;
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
//画像の情報
DirectX::TexMetadata TextureManager::sMetadata_;
//画像イメージのコンテナ
DirectX::ScratchImage TextureManager::sScratchImage_;
//ファイル名
std::string TextureManager::sFileName_;
//ファイル拡張子
std::string TextureManager::sFileExt_;

//--------------------------------------------------------------------------
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
	TextureManager::GetInstance().sWhiteTexHandle_ = TextureManager::LoadGraph("white.dds");
}


uint64_t TextureManager::LoadGraph(const char* name, ID3D12Resource** texBuff,
	D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE* srvHandle)
{
	assert(sSRVCount_ <= S_SRV_COUNT_ - 1);

	HRESULT result = {};

	//バッファがセットされていたらそこに作成する
	ID3D12Resource** texBuffL = texBuff;
	if (texBuffL == nullptr)
	{
		texBuffL = &sTexBuff_[sSRVCount_];
	}

	//読み込まれているかどうか
	std::string fileName = name;

	//格納先が指定されてなければimage内にあるので
	if (fileName.find("Resources") == std::string::npos)
	{
		fileName = sDirectoryPath_ + fileName;
	}

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

	//ファイル拡張子を取得
	AcquisitionExt(fileName);

	if (sFileExt_ != "dds")
	{
		//区切り文字'.'が出てくる一番最後の部分を検索
		size_t pos1 = fileName.rfind('.');
		fileName = fileName.substr(0, pos1) + ".dds";
		sFileExt_ = "dds";
	}

	//wchar_tに戻す
	wchar_t nameWc[128] = {};
	ConstCharToWcharT(fileName.c_str(), nameWc);

	//画像ロード
	result = LoadFile(nameWc);

	//失敗したら白い画像読み込ませて抜ける
	if (!SUCCEEDED(result) || result == S_FALSE)
	{
		return LoadGraph(S_DEFAULT_TEX_FILE_NAME_.c_str());
	}

	ScratchImage mipChain{};
	////mipmap生成
	//result = GenerateMipMaps(
	//	sScratchImage_.GetImages(), sScratchImage_.GetImageCount(), sScratchImage_.GetMetadata(),
	//	TEX_FILTER_DEFAULT, 0, mipChain);
	//if (SUCCEEDED(result))
	//{
	//	sScratchImage_ = std::move(mipChain);
	//	sMetadata_ = sScratchImage_.GetMetadata();
	//}
	//読み込んだディフューズテクスチャをSRGBとして扱う
	sMetadata_.format = MakeSRGB(sMetadata_.format);


	//コピー先用-------------------------------------------------------------
	// ヒープの設定
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			sMetadata_.format,
			(uint64_t)sMetadata_.width,
			(uint32_t)sMetadata_.height,
			(uint16_t)sMetadata_.arraySize,
			(uint16_t)sMetadata_.mipLevels,
			1);

	// テクスチャバッファの生成
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,//コピー先用
			nullptr,
			IID_PPV_ARGS(texBuffL));
	assert(SUCCEEDED(result));

	//	FootPrint取得
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 total_bytes = 0;
	DirectXWrapper::GetInstance().GetDevice()->GetCopyableFootprints(
		&textureResourceDesc, 0, 1, 0, &footprint, nullptr, nullptr, &total_bytes);

	// Upload(コピー元)-----------------------------------------------------------------------
	D3D12_RESOURCE_DESC uploadResDesc{};

	//テクスチャをアップロードするバッファのリストに先に積んでおく
	DirectXWrapper::GetInstance().EmplaceBackUploadBuff();

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
		D3D12_RESOURCE_STATE_GENERIC_READ,//	CPUから書き込み可能、GPUは読み取り専用
		nullptr,
		IID_PPV_ARGS(DirectXWrapper::GetInstance().GetTexUploadBuffPP()));

	//転送
	uint8_t* mapforImg = nullptr;
	result = DirectXWrapper::GetInstance().GetTexUploadBuffP()->Map(0, nullptr, (void**)&mapforImg);//マッピング

	const Image* img = sScratchImage_.GetImage(0, 0, 0);
	uint8_t* uploadStart = mapforImg;
	uint8_t* sourceStart = img->pixels;
	//uint32_t sourcePitch = ((uint32_t)img->width * sizeof(uint32_t));

	////	画像の高さ(ピクセル)分コピーする
	//for (uint32_t i = 0; i < footprint.Footprint.Height; i++)
	//{
	//	memcpy(
	//		uploadStart + i * footprint.Footprint.RowPitch,
	//		sourceStart + i * sourcePitch,
	//		sourcePitch
	//	);
	//}

	memcpy(reinterpret_cast<unsigned char*>(uploadStart) + footprint.Offset, sourceStart, (uint64_t)img->width * (uint32_t)img->height);

	DirectXWrapper::GetInstance().GetTexUploadBuffP()->Unmap(0, nullptr);	//	unmap

	//for (size_t i = 0; i < sMetadata_.mipLevels; i++)
	//{
	//	const Image* img = sScratchImage_.GetImage(i, 0, 0);
	//	result = (*texBuffL)->WriteToSubresource(
	//		(UINT)i,
	//		nullptr,
	//		img->pixels,
	//		(UINT)img->rowPitch,
	//		(UINT)img->slicePitch
	//		);
	//}



	// CopyCommand
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

	//ローカル
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
			textureHandle = 0;
		}
		else
		{
			//SRVヒープの先頭ハンドルを取得
			sSrvHandle_ = sSrvHeap_->GetCPUDescriptorHandleForHeapStart();
			sSrvHandle_.ptr += DirectXWrapper::GetInstance().GetDevice()->GetDescriptorHandleIncrementSize(sSrvHeapDesc_.Type) * sSRVCount_;

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
					GetDescriptorHandleIncrementSize(TextureManager::GetInstance().sSrvHeapDesc_.Type) * sSRVCount_);
		}

		//ハンドルのさす位置にシェーダーリソースビュー作成
		DirectXWrapper::GetInstance().GetDevice()->CreateShaderResourceView(*texBuffL, &srvDescL, srvHandleL);

		if (texBuff == nullptr)
		{
			//バッファ用のカウント
			sSRVCount_++;
		}

		{
			//名前とデータを紐づけて保存
			//テクスチャを連想配列に格納(セットでマップに格納)
			sTextureDatas_.insert(std::make_pair(fileName, textureHandle));
		}

		//元データ解放
		sScratchImage_.Release();
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

HRESULT TextureManager::LoadFile(wchar_t* nameWc)
{
	HRESULT result = S_FALSE;

	//ddsなら
	if (sFileExt_ == "dds")
	{
		//DDSのテクスチャのロード
		result = LoadFromDDSFile(
			nameWc,
			DDS_FLAGS_NONE,
			&sMetadata_, sScratchImage_
		);
	}

	return result;
}

void TextureManager::AcquisitionExt(const std::string& fileName)
{
	//拡張子とそれ以外を分ける----------------------
	//区切り文字'.'が出てくる一番最後の部分を検索
	size_t pos1 = fileName.rfind('.');
	//検索がヒットしたら
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の後ろをファイル拡張子として保存
		sFileExt_ = fileName.substr(pos1 + 1, fileName.size() - pos1 - 1);
	}
	else
	{
		sFileExt_ = "";
	}
}
