#pragma once
#include "DirectXWrapper.h"
#include <map>


class TextureManager
{
private:
	static int32_t count;

	//テクスチャデータの連想配列
	static std::map < std::string, uint64_t> textureDatas_;

	TextureManager();

	~TextureManager();


public:
	static uint64_t whiteTexHandle;


	//リソース設定
	static D3D12_RESOURCE_DESC resDesc;
	//設定をもとにSRV用デスクリプタヒープを生成
	static ComPtr < ID3D12DescriptorHeap> srvHeap;
	static D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

	static const int32_t srvCount = 512;
	static ComPtr<ID3D12Resource> texBuff[srvCount];

	//SRVの最大個数
	static const int32_t kMaxSRVCount;
	//デスクリプタヒープの設定
	static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;

	//デスクリプタレンジの設定
	static D3D12_DESCRIPTOR_RANGE descriptorRange;

public:
	//コピーコンストラクタを無効
	TextureManager(const TextureManager& obj) = delete;
	//代入演算子も
	TextureManager& operator=(const TextureManager& obj) = delete;

	static TextureManager& GetInstance();

	//デスクリプタヒープ初期化
	static void InitializeDescriptorHeap();
	static void LoadGraph(const wchar_t* name, uint64_t& textureHandle);

	static void AddSrvHandleCount() { count++; }
};

