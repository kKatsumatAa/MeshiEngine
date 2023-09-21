#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include <functional>
#include "Vec4.h"
#include "Vec2.h"




struct PeraVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class PostPera
{
private:
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static const float S_BLOOM_POW_;

	//ブルームの最大強さ
	static const float S_BLOOM_MAX_POW_;
	//ラジアルの最大強さ
	static const float S_RADIAL_MAX_POW_;


private:
	//画面効果用のフラグ
	struct EffectConstBuffer
	{
		//色
		Vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		//課題用マルチテクスチャ組み合わせ
		uint32_t isMultiTex = false;
		//ぼかし
		uint32_t isGaussian = false;
		//ガウシアンぼかし
		uint32_t isGaussian2 = false;
		//ラジアルブラー
		uint32_t isRadialBlur = false;
		//ラジアル強さ
		float radialPow = 1.0f;
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
		float barrelCurvePow = 1.0f;
		//走査線
		uint32_t isScanningLine = false;
		//グレースケール
		uint32_t isGrayScale = false;
		float grayScalePow = 1.0f;
		//ガラスフィルター
		uint32_t isGlassFilter = false;
		//モザイク
		uint32_t isMosaic = false;
		//ネガポジ
		uint32_t isNega = false;
		//RGBずらし
		uint32_t isRGBShift = false;
		float RGBShiftPow = 0.05f;
		//ブルーム
		uint32_t isBloom = false;
		//クロスフィルタ
		uint32_t isCrossFilter = false;
		//ブルームの強さ
		float bloomPow = PostPera::S_BLOOM_POW_;
		//ノイズ
		uint32_t isNoise = false;
		//被写界深度
		uint32_t isDepthField = false;
		//フォーカス合わせる深度値
		float focusDepth = 0.05f;
		//フォーカスのスムースステップの幅の下限
		float nFocusWidth = 0.005f;
		//フォーカスのスムースステップの幅の上限
		float focusDiffPow = 0.01f;
		//時間
		uint32_t time = 0;
	};

private:
	//ペラの拡大率
	float pera1Extend_ = 1.0f;
	float peraGExtend_ = 1.0f;
	float pera2Extend_ = 1.0f;
	//一枚目の頂点
	PeraVertex p1Vertices_[4] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//左下
			{{-1.0f, +1.0f,0},{0,0}},//左上
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
			{{+1.0f, +1.0f,0},{1.0f,0}}//右上
	};
	//ガウシアン用の頂点
	PeraVertex pGVertices_[4] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//左下
			{{-1.0f, +1.0f,0},{0,0}},//左上
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
			{{+1.0f, +1.0f,0},{1.0f,0}}//右上
	};
	//二枚目の頂点
	PeraVertex p2Vertices_[4] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//左下
			{{-1.0f, +1.0f,0},{0,0}},//左上
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
			{{+1.0f, +1.0f,0},{1.0f,0}}//右上
	};

	ComPtr<ID3D12Resource> pera1VB_;
	D3D12_VERTEX_BUFFER_VIEW pera1VBV_;
	ComPtr<ID3D12Resource> peraGVB_;
	D3D12_VERTEX_BUFFER_VIEW peraGVBV_;
	ComPtr<ID3D12Resource> pera2VB_;
	D3D12_VERTEX_BUFFER_VIEW pera2VBV_;



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

	//被写界深度バッファ
	ComPtr<ID3D12Resource>dofBuffer_;

	//テクスチャの大きさ
	float peraExtend_ = 1.0f;

public:
	//画面効果用
	EffectConstBuffer effectFlags_;


private://関数
	//頂点バッファ作成
	void CreateVertexBuff(PeraVertex* vertex, int32_t vertexCount, ID3D12Resource** vB, D3D12_VERTEX_BUFFER_VIEW* vBV);
	//頂点バッファマッピング
	void MappingVertexBuff(PeraVertex* vertex, int32_t vertexCount, float peraExtend, ID3D12Resource* vB);

	void InitializeBuffRTV(const char* normalImageFileName);
	void GenerateRSPL();
	void GlassFilterBuffGenerate(const char* fileName);
	//ガラス用のバッファとビュー作成
	bool CreateEffectBufferAndView(const char* fileName);
	//ブルーム用のバッファ作成
	bool CreateBloomBuffer(const D3D12_RESOURCE_DESC& resDesc,
		const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue);
	bool CreateBloomBuffView();
	//被写界深度用バッファ作成
	bool CreateDofBuffer(const D3D12_RESOURCE_DESC& resDesc,
		const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue);
	//被写界深度用SRV
	bool CreateDofSRV();

	//深度値テクスチャ用SRV
	bool CreateDepthTexBuffAndSRV();

	//一枚目に描画
	void PreDraw(bool isFirstPostPera);
	void PostDraw();

	//二枚目に描画
	void PreDraw2();
	void PostDraw2();
	//二回目のエフェクトかける
	void Draw();

	//ヒープとヒープ上に作ったバッファビュー等をセット
	void SetHeapAllBuffView(bool isPost2 = false, bool isPost1 = false);

public:
	void Initialize(const char* normalImageFileName);

	void Update();
	void ImGuiUpdate(int32_t num);

	//二回目のエフェクトかける用の描画
	void DrawToBlurAll();

	//縮小バッファ書き込み
	void DrawShrinkTextureForBlur();

	//エフェクトかけ終わって実際に描画する
	void Draw2();

	//一枚目と二枚目を描画する(引数の描画関数を一枚目に描画)
	void DrawToPostpera(const std::function<void()>& f, bool isFirstPostPera = false, const std::function<void()>& drawSpriteF = nullptr);

public:
	void SetPera1Extend(float extend) { pera1Extend_ = extend; }
	void SetPeraGExtend(float extend) { peraGExtend_ = extend; }
	void SetPera2Extend(float extend) { pera2Extend_ = extend; }
};