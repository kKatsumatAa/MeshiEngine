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

	static int32_t sCount_;

	//テクスチャデータの連想配列
	static std::map < std::string, uint64_t> sTextureDatas_;

	TextureManager();

	~TextureManager();


public:
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

public:
	//コピーコンストラクタを無効
	TextureManager(const TextureManager& obj) = delete;
	//代入演算子も
	TextureManager& operator=(const TextureManager& obj) = delete;

	static TextureManager& GetInstance();

	//デスクリプタヒープ初期化
	static void InitializeDescriptorHeap();
	static uint64_t LoadGraph(const wchar_t* name, ID3D12Resource** texBuff = nullptr,
		D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr, D3D12_CPU_DESCRIPTOR_HANDLE* srvHandle = nullptr);

	static void AddSrvHandleCount() { sCount_++; }

	//テクスチャハンドルが何も入ってなかったら白い画像のハンドル入れる
	static void CheckTexHandle(uint64_t& texHandle);
};

