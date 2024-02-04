#include "TextureManager.h"
#include <d3dx12.h>
#include "Util.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Util;

//格納先
const std::string TextureManager::S_DIRECTORY_PATH_ = "Resources/image/";
const std::string TextureManager::S_DEFAULT_TEX_FILE_NAME_ = "white.dds";
const std::string TextureManager::S_DEFAULT_TEX_FILE_FULL_PATH_ = TextureManager::S_DIRECTORY_PATH_ + TextureManager::S_DEFAULT_TEX_FILE_NAME_;
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

//{
//
//	void afWriteBuffer(const ComPtr<ID3D12Resource> res, const void* buf, int size)
//	{
//		void* p;
//		D3D12_RANGE readRange = {};
//		res->Map(0, &readRange, &p);
//		memcpy(p, buf, size);
//		D3D12_RANGE wroteRange = { 0, (SIZE_T)size };
//		id->Unmap(0, &wroteRange);
//	}
//
//	ComPtr<ID3D12Resource> afCreateBuffer(int size, const void* buf)
//	{
//		D3D12_HEAP_PROPERTIES prop = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
//		D3D12_RESOURCE_DESC desc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, (UINT64)size, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
//		UBOID o;
//		deviceMan.GetDevice()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&o));
//		afWriteBuffer(o, buf, size);
//		return o;
//	}
//
//	void UploadTexture(ComPtr<ID3D12Resource> tex, const void* buf)
//	{
//		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
//		UINT64 uploadSize, rowSizeInBytes;
//		const D3D12_RESOURCE_DESC destDesc = tex->GetDesc();
//		deviceMan.GetDevice()->GetCopyableFootprints(&destDesc, 0, 1, 0, &footprint, nullptr, &rowSizeInBytes, &uploadSize);
//
//		assert(rowSizeInBytes == footprint.Footprint.RowPitch); // It is safe in most case!
//
//		ComPtr<ID3D12Resource> uploadBuf = afCreateBuffer((int)uploadSize, buf);
//		D3D12_TEXTURE_COPY_LOCATION uploadBufLocation = { uploadBuf.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, footprint }, nativeBufLocation = { tex.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };
//		ID3D12GraphicsCommandList* list = deviceMan.GetCommandList();
//
//		D3D12_RESOURCE_BARRIER transition1 = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { tex.Get(), 0, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
//		list->ResourceBarrier(1, &transition1);
//		list->CopyTextureRegion(&nativeBufLocation, 0, 0, 0, &uploadBufLocation, nullptr);
//		D3D12_RESOURCE_BARRIER transition2 = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { tex.Get(), 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
//		list->ResourceBarrier(1, &transition2);
//
//		deviceMan.AddIntermediateCommandlistDependentResource(uploadBuf); // keep until the command queue has been flushed
//	}
//
//	//------------------------------------------------------------------------------------------------------------------------------------------------------------
//	struct DDSHeader {
//		uint32_t h3[3];
//		int h, w;
//		uint32_t h2[2];
//		int mipCnt;
//		uint32_t h13[13];
//		uint32_t fourcc, bitsPerPixel, rMask, gMask, bMask, aMask, caps1, caps2;
//		bool IsCubeMap() const { return caps2 == 0xFE00; }
//		int GetArraySize() const { return IsCubeMap() ? 6 : 1; }
//		int GetMipCnt() const { return std::max(mipCnt, 1); }
//	};
//
//	struct TexDesc {
//		IVec2 size;
//		int arraySize = 1;
//		bool isCubeMap = false;
//	};
//
//	struct AFTexSubresourceData
//	{
//		const void* ptr;
//		uint32_t pitch;
//		uint32_t pitchSlice;
//	};
//
//	static ComPtr<ID3D12Resource> LoadDDSTexture(const char* name, TexDesc& texDesc)
//	{
//		int size;
//		void* img = LoadFile(name, &size);
//		if (!img) {
//			aflog("LoadDDSTexture failed! %s", name);
//			return nullptr;
//		}
//		const DDSHeader* hdr = (DDSHeader*)img;
//
//		AFDTFormat format = AFDT_INVALID;
//		int(*pitchCalcurator)(int, int) = nullptr;
//		switch (hdr->fourcc) {
//		case 0x31545844: //'1TXD':
//			format = AFDT_BC1_UNORM;
//			pitchCalcurator = [](int w, int h) { return ((w + 3) / 4) * ((h + 3) / 4) * 8; };
//			break;
//		case 0x33545844: //'3TXD':
//			format = AFDT_BC2_UNORM;
//			pitchCalcurator = [](int w, int h) { return ((w + 3) / 4) * ((h + 3) / 4) * 16; };
//			break;
//		case 0x35545844: //'5TXD':
//			format = AFDT_BC3_UNORM;
//			pitchCalcurator = [](int w, int h) { return ((w + 3) / 4) * ((h + 3) / 4) * 16; };
//			break;
//		default:
//			ArrangeRawDDS(img, size);
//			format = AFDT_R8G8B8A8_UNORM;
//			pitchCalcurator = [](int w, int h) { return w * h * 4; };
//			break;
//		}
//		texDesc.size.x = hdr->w;
//		texDesc.size.y = hdr->h;
//		texDesc.arraySize = hdr->GetArraySize();
//		texDesc.isCubeMap = hdr->IsCubeMap();
//
//		int arraySize = hdr->GetArraySize();
//		int mipCnt = hdr->GetMipCnt();
//		std::vector<AFTexSubresourceData> r;
//		int offset = 128;
//		for (int a = 0; a < arraySize; a++) {
//			for (int m = 0; m < mipCnt; m++) {
//				int w = std::max(1, hdr->w >> m);
//				int h = std::max(1, hdr->h >> m);
//				int size = pitchCalcurator(w, h);
//				r.push_back({ (char*)img + offset, (uint32_t)pitchCalcurator(w, 1), (uint32_t)size });
//				offset += size;
//			}
//		}
//
//		ComPtr<ID3D12Resource> tex = afCreateTexture2D(format, texDesc, mipCnt, &r[0]);
//		assert(tex);
//		free(img);
//		return tex;
//	}
//
//	static const D3D12_HEAP_PROPERTIES defaultHeapProperties = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
//
//	ComPtr<ID3D12Resource> afCreateTexture2D(AFDTFormat format, const struct TexDesc& desc, int mipCount, const AFTexSubresourceData datas[])
//	{
//		D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, (UINT64)desc.size.x, (UINT)desc.size.y, (UINT16)desc.arraySize, (UINT16)mipCount, format, {1, 0} };
//		ComPtr<ID3D12Resource> tex;
//		HRESULT hr = deviceMan.GetDevice()->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&tex));
//		afWriteTexture(tex, desc, mipCount, datas);
//		return tex;
//	}
//
//	void afWriteTexture(ComPtr<ID3D12Resource> tex, const TexDesc& desc, int mipCount, const AFTexSubresourceData datas[])
//	{
//		const int maxSubresources = 100;
//		const UINT subResources = mipCount * desc.arraySize;
//		assert(subResources <= maxSubresources);
//		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprints[maxSubresources];
//		UINT64 rowSizeInBytes[maxSubresources], uploadSize;
//		UINT numRows[maxSubresources];
//		D3D12_RESOURCE_BARRIER transitions1[maxSubresources], transitions2[maxSubresources];
//		deviceMan.GetDevice()->GetCopyableFootprints(&tex->GetDesc(), 0, subResources, 0, footprints, numRows, rowSizeInBytes, &uploadSize);
//		ComPtr<ID3D12Resource> uploadBuf = afCreateBuffer((int)uploadSize);
//		assert(uploadBuf);
//		uploadBuf->SetName(__FUNCTIONW__ L" intermediate buffer");
//		D3D12_RANGE readRange = {};
//		BYTE* ptr;
//		HRESULT hr = uploadBuf->Map(0, &readRange, (void**)&ptr);
//		assert(ptr);
//		for (UINT i = 0; i < subResources; i++)
//		{
//			transitions1[i] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE,{ tex.Get(), i, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST } };
//			transitions2[i] = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE,{ tex.Get(), i, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } };
//		}
//		ID3D12GraphicsCommandList* list = deviceMan.GetCommandList();
//		list->ResourceBarrier(subResources, transitions1);
//		for (UINT i = 0; i < subResources; i++)
//		{
//			for (UINT row = 0; row < numRows[i]; row++) {
//				memcpy(ptr + footprints[i].Offset + footprints[i].Footprint.RowPitch * row, (BYTE*)datas[i].ptr + datas[i].pitch * row, datas[i].pitch);
//			}
//			D3D12_TEXTURE_COPY_LOCATION uploadBufLocation = { uploadBuf.Get(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, footprints[i] };
//			D3D12_TEXTURE_COPY_LOCATION nativeBufLocation = { tex.Get(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, i };
//			list->CopyTextureRegion(&nativeBufLocation, 0, 0, 0, &uploadBufLocation, nullptr);
//		}
//		list->ResourceBarrier(subResources, transitions2);
//		uploadBuf->Unmap(0, nullptr);
//		deviceMan.AddIntermediateCommandlistDependentResource(uploadBuf);
//		deviceMan.AddIntermediateCommandlistDependentResource(tex);
//	}
//}

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
		fileName = S_DIRECTORY_PATH_ + fileName;
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

	//読み込んだディフューズテクスチャをSRGBとして扱う
	//sMetadata_.format = MakeSRGB(sMetadata_.format);


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
	UINT numRows[100];
	DirectXWrapper::GetInstance().GetDevice()->GetCopyableFootprints(
		&textureResourceDesc, 0, 1, 0, &footprint, &numRows[0], nullptr, &total_bytes);

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


	/*//	転送
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
	DirectXWrapper::GetInstance().GetTexUploadBuffP()->Unmap(0, nullptr);	//	unmap*/


	//転送----------------------------------------
	BYTE* ptr;
	DirectXWrapper::GetInstance().GetTexUploadBuffP()->Map(0, nullptr, (void**)&ptr);
	//uint32_t imgCount = (uint32_t)sScratchImage_.GetImageCount();

	//uint32_t uploadPtrAdd = 0;

	////イメージごとの処理
	//for (uint32_t i = 0; i < imgCount; i++)
	//{
	//	auto img = sScratchImage_.GetImage(i, 0, 0);

	//	//イメージごとに高さ分コピー
	//	for (uint32_t h = 0; h < footprint.Footprint.Height; h++)
	//	{
	//		UINT8* pScan = ptr + footprint.Offset + uploadPtrAdd;

	//		//コピー
	//		auto copySize = sizeof(uint32_t) * img->width;
	//		memcpy(pScan, img->pixels + h * sizeof(uint32_t) * img->width, copySize);

	//		uploadPtrAdd += footprint.Footprint.RowPitch;
	//	}
	//}

	auto img = sScratchImage_.GetImage(0, 0, 0);

	memcpy(ptr, img->pixels, img->slicePitch);

	DirectXWrapper::GetInstance().GetTexUploadBuffP()->Unmap(0, nullptr);


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

std::string TextureManager::GetTextureName(uint64_t texHandle)
{
	for (auto& texData : sTextureDatas_)
	{
		if (texData.second == texHandle)
		{
			return texData.first;
		}
	}

	return S_DEFAULT_TEX_FILE_NAME_;
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
