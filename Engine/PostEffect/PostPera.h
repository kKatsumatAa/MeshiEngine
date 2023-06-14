#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include "GlassFilter.h"
#include <functional>


//画面効果用のフラグ
struct EffectConstBuffer
{
	//ぼかし
	uint32_t isGaussian = false;
	//ガウシアンぼかし
	uint32_t isGaussian2 = false;
	//エンボス
	uint32_t isEmboss = false;
	//シャープネス
	uint32_t isSharpness = false;
	//諧調
	uint32_t isGradation = false;
	//アウトライン
	uint32_t isOutLine = false;
	//ビネット
	uint32_t isVignette = false;
	//樽状湾曲
	uint32_t isBarrelCurve = false;
	//走査線
	uint32_t isScanningLine = false;
	//グレースケール
	uint32_t isGrayScale = false;
	//ガラスフィルター
	uint32_t isGlassFilter = false;
	//モザイク
	uint32_t isMosaic = false;
	//ネガポジ
	uint32_t isNega = false;
	//ネガポジ
	uint32_t isRGBShift = false;
	//ブルーム
	uint32_t isBloom = false;
	//時間
	uint32_t time = 0;
};

struct PeraVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pv[4] = {
		{{-1.0f, -1.0f,0},{0,1.0f}},//左下
		{{-1.0f, +1.0f,0},{0,0}},//左上
		{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
		{{+1.0f, +1.0f,0},{1.0f,0}}//右上
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
	//マッピング用
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

	//ブルームバッファー
	std::array<ComPtr<ID3D12Resource>, 2>_bloomBuffer;
	//画面全体ぼかし用パイプライン(ブルーム)
	ComPtr<ID3D12PipelineState> _blurPipeline;

public:
	//画面効果用
	EffectConstBuffer effectFlags;



private://関数
	void InitializeBuffRTV(const wchar_t* nomalImageFileName);
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const wchar_t* fileName);
	//ガラス用のバッファとビュー作成
	bool CreateEffectBufferAndView(const wchar_t* fileName);
	//ブルーム用のバッファ作成
	bool CreateBloomBuffer();
	//ブルーム用のバッファ作成
	bool CreateBloomBuffView();

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

	//縮小バッファ書き込み
	void DrawShrinkTextureForBlur();

	//エフェクトかけ終わって実際に描画する
	void Draw2();

	//一枚目と二枚目を描画する(引数の描画関数を一枚目に描画)
	void DrawToPostpera(const std::function<void()>& f);
};

