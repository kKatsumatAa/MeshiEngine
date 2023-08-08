#pragma once
#include"ConstBuffTransform.h"
#include"TextureManager.h"
#include "WorldMat.h"
#include "ViewMat.h"
#include "ProjectionMat.h"
#include "RootPipe.h"


struct ConstBufferDataMaterial
{
	Vec4 color;//色(RGBA)
};
struct SpriteBuff
{
private://エイリアス
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//頂点バッファ
	ComPtr < ID3D12Resource> vertBuff = nullptr;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファの生成
};
struct VertexSprite
{
	DirectX::XMFLOAT3 pos;//xyz座標
	DirectX::XMFLOAT2 uv;//uv座標
};

class SpriteCommon
{
private:

public:
};

