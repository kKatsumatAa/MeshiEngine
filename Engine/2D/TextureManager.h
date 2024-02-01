/*
* @file TextureManager.h
* @brief 画像を読み込んだり管理
*/

#pragma once
#include "DirectXWrapper.h"
#include <map>


class TextureManager
{
private:
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//格納先
	static const std::string sDirectoryPath_;

	//srvのカウント
	static int32_t sSRVCount_;

	//テクスチャデータの連想配列
	static std::map < std::string, uint64_t> sTextureDatas_;

	TextureManager();

	~TextureManager();


private:
	//テクスチャがない場合の標準テクスチャファイル名
	static const std::string S_DEFAULT_TEX_FILE_NAME_;

	static uint64_t sWhiteTexHandle_;

	//リソース設定
	static D3D12_RESOURCE_DESC sResDesc_;
	//設定をもとにSRV用デスクリプタヒープを生成
	static  ComPtr < ID3D12DescriptorHeap> sSrvHeap_;
	static D3D12_CPU_DESCRIPTOR_HANDLE sSrvHandle_;

	static const int32_t S_SRV_COUNT_ = 512;
	static ComPtr<ID3D12Resource> sTexBuff_[S_SRV_COUNT_];

	//SRVの最大個数
	static const int32_t S_K_MAX_SRV_COUNT_;
	//デスクリプタヒープの設定
	static D3D12_DESCRIPTOR_HEAP_DESC sSrvHeapDesc_;

	//デスクリプタレンジの設定
	static D3D12_DESCRIPTOR_RANGE sDescriptorRange_;

private:
	//画像の情報
	static DirectX::TexMetadata sMetadata_;
	//画像イメージのコンテナ
	static DirectX::ScratchImage sScratchImage_;
private:
	//ファイル名
	static std::string sFileName_;
	//ファイル拡張子
	static std::string sFileExt_;

public:
	//コピーコンストラクタを無効
	TextureManager(const TextureManager& obj) = delete;
	//代入演算子も無効
	TextureManager& operator=(const TextureManager& obj) = delete;

	//インスタンス取得
	static TextureManager& GetInstance();

public:
	//デスクリプタヒープ初期化
	static void InitializeDescriptorHeap();
	//初期化
	static void Initialize();

public:
	//画像の読み込み
	static uint64_t LoadGraph(const char* name, ID3D12Resource** texBuff = nullptr,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr, D3D12_CPU_DESCRIPTOR_HANDLE* srvHandle = nullptr);

	//テクスチャハンドルが何も入ってなかったら白い画像のハンドル入れる
	static void CheckTexHandle(uint64_t& texHandle);

public:
	//デスクリプタヒープのポインタのポインタ取得
	static ID3D12DescriptorHeap** GetDescHeapPP() { return sSrvHeap_.GetAddressOf(); }
	//デスクリプタヒープのポインタ取得
	static ID3D12DescriptorHeap* GetDescHeapP() { return sSrvHeap_.Get(); }
	//デスクリプタの設定のポインタ取得
	static D3D12_DESCRIPTOR_RANGE* GetDescRange() { return &sDescriptorRange_; }
	//白画像のハンドル取得
	static uint64_t GetWhiteTexHandle() { return sWhiteTexHandle_; }
	//テクスチャバッファ取得
	static ComPtr<ID3D12Resource>* GetTexBuff() { return sTexBuff_; }
	//リソース設定取得
	static const D3D12_RESOURCE_DESC& GetResDesc() { return sResDesc_; }
	//デスクリプタヒープの設定取得
	static const D3D12_DESCRIPTOR_HEAP_DESC & GetHeapDesc() { return sSrvHeapDesc_; }

public:
	//SRVのカウントを取得
	static int32_t GetSRVCount() { return sSRVCount_; }
	//SRVのカウントを加算
	static void AddSRVHandleCount() { sSRVCount_++; }

private:
	//ddsかwic読み込み
	static HRESULT LoadFile(wchar_t* nameWc);
	//ファイル拡張子を取得
	static void AcquisitionExt(const std::string& fileName);
};

