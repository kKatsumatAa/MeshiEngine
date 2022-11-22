#pragma once
#include"SpriteCommon.h"
#include "Util.h"

//リソース設定
extern D3D12_RESOURCE_DESC resDesc;
//設定をもとにSRV用デスクリプタヒープを生成
extern ComPtr < ID3D12DescriptorHeap> srvHeap;
extern D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

static const int srvCount = 512;
extern ComPtr<ID3D12Resource> texBuff[srvCount];

//SRVの最大個数
const size_t kMaxSRVCount = 2056;
//デスクリプタヒープの設定
static D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;



class Sprite
{
private:
	

public:
	VertexSprite vertices[4] = {
		{{0.0f,100.0f,0.0f},{0.0f,1.0f}},
		{{0.0f,0.0f,0.0f},{0.0f,0.0f}},
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
		{{100.0f,0.0f,0.0f},{1.0f,0.0f}}
	};
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

public:
	void Initialize();
	void SpriteDraw();

	void Update(const Vec3& pos, const float& scale,
		XMFLOAT4 color, const UINT64 textureHandle, const Vec2& ancorUV,
		const bool& isReverseX,const bool& isReverseY, float rotation,
		ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);

	void UpdateClipping(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
		XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop,
		const bool& isReverseX,const bool& isReverseY, float rotation, ConstBuffTransform* cbt, ConstBufferDataMaterial* constMapMaterial);
};

//共通の処理
void SpriteCommonBeginDraw(PipeLineSet* pipelineSet);

void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, const UINT& size);

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource,
	ID3D12Resource** buff);

void LoadGraph(const wchar_t* name, UINT64& textureHandle);

//void SpriteCommonCreate(SpriteSet* spriteSet);