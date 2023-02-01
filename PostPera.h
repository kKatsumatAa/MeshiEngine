#pragma once
#include"TextureManager.h"
#include "Directx.h"


//画面効果用のフラグ
struct EffectConstBuffer
{
	//フォグ
	unsigned int isFog = false;
	//ぼかし
	unsigned int isGaussian = false;
	//ガウシアンぼかし
	unsigned int isGaussian2 = false;
	//エンボス
	unsigned int isEmboss = false;
	//シャープネス
	unsigned int isSharpness = false;
	//諧調
	unsigned int isGradation = false;
	//アウトライン
	unsigned int isOutLine = false;
	//ビネット
	unsigned int isVignette = false;
	//樽状湾曲
	unsigned int isBarrelCurve = false;
};

struct PeraVertex
{
	XMFLOAT3 pos;
	float pad1 = 1.0f;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pv[4] = {
		{{-1, -1,0.1f},1,{0,1}},//左下
		{{-1, 1,0.1f},1,{0,0}},//左上
		{{1, -1,0.1f},1,{1,1}},//右下
		{{1, 1,0.1f},1,{1,0}}//右上
	};

	ComPtr<ID3D12Resource> _peraVB;
	D3D12_VERTEX_BUFFER_VIEW _peraVBV;

	ComPtr<ID3DBlob> vs;
	ComPtr<ID3DBlob> ps;
	ComPtr<ID3DBlob> errBlob;

	ComPtr<ID3D12RootSignature> _peraRS;
	ComPtr<ID3D12PipelineState> _peraPipeline = NULL;

	//画面効果用
	ComPtr <ID3D12Resource> effectFlagsBuff;
	EffectConstBuffer* mapEffectFlagsBuff;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[1] = {};

public:
	void Initialize();
	void GenerateRSPL();
	void Draw(EffectConstBuffer effectFlags);
};

