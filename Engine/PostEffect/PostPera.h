#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include "GlassFilter.h"
#include <functional>


//画面効果用のフラグ
struct EffectConstBuffer
{
	//課題用マルチテクスチャ組み合わせ
	uint32_t isMultiTex = false;
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
	//クロスフィルタ
	uint32_t isCrossFilter = false;
	//時間
	uint32_t time = 0;
	//ブルームの強さ
	float bloomPow = 3.0f;
};

struct PeraVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class PostPera
{
private:
	PeraVertex pVertices_[4] = {
		{{-1.0f, -1.0f,0},{0,1.0f}},//左下
		{{-1.0f, +1.0f,0},{0,0}},//左上
		{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
		{{+1.0f, +1.0f,0},{1.0f,0}}//右上
	};

	ComPtr<ID3D12Resource> peraVB_;
	D3D12_VERTEX_BUFFER_VIEW peraVBV_;

	ComPtr<ID3DBlob> vs_;
	ComPtr<ID3DBlob> ps_;
	ComPtr<ID3DBlob> errBlob_;

	ComPtr<ID3D12RootSignature> peraRS_;
	ComPtr<ID3D12PipelineState> peraPipeline_ = NULL;
	ComPtr<ID3D12PipelineState> peraPipeline2_ = NULL;

	//画面効果用
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	//マッピング用
	EffectConstBuffer* mapEffectFlagsBuff_;

	//ポストエフェクト用
	D3D12_CPU_DESCRIPTOR_HANDLE peraSRHandle_;
	ComPtr<ID3D12Resource> peraResources_[2];
	ComPtr<ID3D12Resource> peraResource2_;
	ComPtr<ID3D12DescriptorHeap> peraRTVHeap_;//レンダーターゲット用
	ComPtr<ID3D12DescriptorHeap> peraSRVHeap_;//テクスチャ用
	bool isPeraClear_ = false;

	GausianBuffer gaussianBuff_;

	//法線マップ用
	ComPtr<ID3D12Resource>effectTexBuffer_;

	//ブルームバッファー
	ComPtr<ID3D12Resource>bloomBuffers_[3];
	//画面全体ぼかし用パイプライン(ブルーム)
	ComPtr<ID3D12PipelineState> blurPipeline_;

	//ブルームの最大強さ
	const float BLOOM_MAX_POW_ = 10.0f;

public:
	//画面効果用
	EffectConstBuffer effectFlags_;



private://関数
	void InitializeBuffRTV(const wchar_t* normalImageFileName);
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

	//全てのビュー等をセット
	void SetHeapAllBuffView(bool isPost2 = false);

public:
	void Initialize(const wchar_t* normalImageFileName, float vertexExtend = 1.0f);

	void Update();

	//縮小バッファ書き込み
	void DrawShrinkTextureForBlur();

	//エフェクトかけ終わって実際に描画する
	void Draw2();

	//一枚目と二枚目を描画する(引数の描画関数を一枚目に描画)
	void DrawToPostpera(const std::function<void()>& f);
};

