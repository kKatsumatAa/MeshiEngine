#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include "GlassFilter.h"
#include <functional>


//画面効果用のフラグ
struct EffectConstBuffer
{
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
	//走査線
	unsigned int isScanningLine = false;
	//グレースケール
	unsigned int isGrayScale = false;
	//ガラスフィルター
	unsigned int isGlassFilter = false;
	//モザイク
	unsigned int isMosaic = false;
	//ネガポジ
	unsigned int isNega = false;
	//ネガポジ
	unsigned int isRGBShift = false;
	//時間
	unsigned int time = 0;
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
		{{-1, -1,0.1f},1, {0,1}},//左下
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
	ComPtr<ID3D12PipelineState> _peraPipeline2 = NULL;

	//画面効果用
	ComPtr <ID3D12Resource> effectFlagsBuff;
	EffectConstBuffer* mapEffectFlagsBuff;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[1] = {};

	//ポストエフェクト用
	D3D12_CPU_DESCRIPTOR_HANDLE peraHandle;
	std::array<ComPtr<ID3D12Resource>, 2> _peraResource;
	ComPtr<ID3D12Resource> _peraResource2;
	ComPtr<ID3D12DescriptorHeap> _peraRTVHeap;//レンダーターゲット用
	ComPtr<ID3D12DescriptorHeap> _peraSRVHeap;//テクスチャ用
	bool isPeraClear = false;

	GausianBuffer gausianBuff;

	//ガラス
	ComPtr<ID3D12Resource>_effectTexBuffer;

	D3D12_RESOURCE_BARRIER barrierDesc{};

public:
	//画面効果用
	EffectConstBuffer effectFlags;



private://関数
	void InitializeBuffRTV();
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const wchar_t* fileName);
	bool CreateEffectBufferAndView(const wchar_t* fileName);

	//一枚目に描画
	void PreDraw();
	void PostDraw();

	//二枚目に描画
	void PreDraw2();
	void PostDraw2();
	//二回目のエフェクトかける
	void Draw();

	//二回目のエフェクトかける用の描画
	void Draw2All();

public:
	void Initialize(const wchar_t* nomalImageFileName);

	void Update();

	//エフェクトかけ終わって実際に描画する
	void Draw2();

	//一枚目と二枚目を描画する(引数の描画関数を一枚目に描画)
	void DrawToPostpera(std::function<void()> f);
};

