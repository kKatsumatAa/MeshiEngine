/*
* @file PostPera.h
* @brief ポストエフェクト一回分
*/

#pragma once
#include"TextureManager.h"
#include "DirectXWrapper.h"
#include <functional>
#include "Vec4.h"
#include "Vec2.h"
#include "Vec3.h"


struct PeraVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class PostPera
{
private:
	//SRV系のシェーダーレジスタ
	enum BaseShaderRegisterTexNum
	{
		R_COLOR,
		R_COLOR2,
		R_HIGH_LUMI,
		R_SHRINK,
		R_SHRINK2,
		R_DEPTH_FIELD,
		R_GLASS,
		R_DEPTH_TEX,

		R_T_COUNT
	};
	//cBuff系のシェーダーレジスタ
	enum BaseShaderRegisterCBuffNum
	{
		R_GAUSS_PARAM,
		R_EFFECT_FLAG,
	};

	enum RootParamIndex
	{
		COLOR,
		COLOR2,
		HIGH_LUMI,
		SHRINK,
		SHRINK2,
		DEPTH_FIELD,
		GAUSS_PARAM,
		EFFECT_FLAG,
		GLASS,
		DEPTH_TEX,

		ROOT_PARAM_COUNT//要素数
	};

private:
	//Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static const int8_t S_VERTEX_COUNT_ = 4;
	static const int8_t S_SHRINK_BUFF_NUM_ = 3;
	static const int8_t S_SHRINK_ROOP_NUM_ = 8;
	static const int8_t S_PERA_1_RTV_NUM_ = 3;
	static const int8_t S_PERA_2_RTV_NUM_ = 3;
	const float SHRINK_RATE_ = 0.5f;
public:
	static const float S_BLOOM_POW_;
	//ブルームの最大強さ
	static const float S_BLOOM_MAX_POW_;
	//ラジアルの最大強さ
	static const float S_RADIAL_MAX_POW_;
	//グレースケール強さ
	static const float S_GRAY_SCALE_POW_MAX_;
	//湾曲強さ
	static const float S_BARREL_POW_MAX_;
	//RGBシフト強さ
	static const float S_RGB_SHIFT_POW_MAX_;
	//ノイズ強さ
	static const float S_NOISE_POW_MAX_;
	//被写界深度強さ
	static const float S_DEPTH_FIELD_POW_MAX_;
private:
	const float IMGUI_SEA_DIR_ROT_DRAG_SPEED_ = 0.0001f;
	const float IMGUI_SEA_RESOULUTION_DRAG_SPEED_ = 0.05f;
	const float IMGUI_SEA_DIR_ROT_EXTEND_DRAG_SPEED_ = 0.01f;
	const float IMGUI_SEA_COLOR_DRAG_SPEED_ = 0.05f;
	const float IMGUI_SEA_TIME_EXTEND_DRAG_SPEED_ = 0.05f;
private:
	static const int8_t S_RTV_NUM_ = 7;
	static const int8_t S_MIP_LEVEL_ = 1;
	static const int8_t S_INPUT_LAYOUT_NUM_ = 2;
	static const int8_t S_SHRINK_SHIFT_WIDTH_RATE_ = 1;

	const float BLOOM_RATIO_MAX_ = 1.0f;
private:
	static const int8_t S_IMGUI_SLIDER_FLAG_MIN_ = 0;
	static const int8_t S_IMGUI_SLIDER_FLAG_MAX_ = 1;

private:
	//画面効果用のフラグ
	struct EffectConstBuffer
	{
		//色
		Vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		//---
		//課題用マルチテクスチャ組み合わせ
		uint32_t isMultiTex = false;
		//ぼかし
		uint32_t isGaussian = false;
		//ガウシアンぼかし
		uint32_t isGaussian2 = false;
		//ラジアルブラー
		uint32_t isRadialBlur = false;
		//---
		//ラジアル強さ
		float radialPow = 1.0f;
		//エンボス
		uint32_t isEmboss = false;
		//シャープネス
		uint32_t isSharpness = false;
		//諧調
		uint32_t isGradation = false;
		//---
		//アウトライン
		uint32_t isOutLine = false;
		//ビネット
		uint32_t isVignette = false;
		//樽状湾曲
		uint32_t isBarrelCurve = false;
		float barrelCurvePow = 1.0f;
		//---
		//走査線
		uint32_t isScanningLine = false;
		//グレースケール
		uint32_t isGrayScale = false;
		float grayScalePow = 1.0f;
		//ガラスフィルター
		uint32_t isGlassFilter = false;
		//---
		//モザイク
		uint32_t isMosaic = false;
		//ネガポジ
		uint32_t isNega = false;
		//RGBずらし
		uint32_t isRGBShift = false;
		float RGBShiftPow = 0.05f;
		//---
		//ブルーム
		uint32_t isBloom = false;
		//クロスフィルタ
		uint32_t isCrossFilter = false;
		//ブルームの強さ
		float bloomPow = PostPera::S_BLOOM_POW_;
		//ノイズ
		uint32_t isNoise = false;
		//---
		//ノイズ強さ
		float noisePow = 1.0f;
		//被写界深度
		uint32_t isDepthField = false;
		//フォーカス合わせる深度値
		float focusDepth = 0.05f;
		//フォーカスのスムースステップの幅の下限
		float nFocusWidth = 0.005f;
		//---
		//フォーカスのスムースステップの幅の上限
		float focusDiffPow = 0.01f;
		//海
		uint32_t isSea = false;
		uint32_t isSeaImageEffect = false;
		float resoulution = 0.0015f;
		//---
		Vec3 seaDirRot = { 0,0,0 };
		float pad2 = 0;
		//---
		Vec3 seaCameraPos = { 0,10.0f,0 };
		float pad3 = 0;
		//---
		//カメラ回転の倍率
		Vec3 seaDirRotExtend = { 0.57f,0.18f,1.0f };
		float pad4 = 0;
		//---
		//空の色
		Vec3 seaSkyCol = { 0.05f,0.1f,0.05f };
		float seaTimerExtend = 1.0f;
		//---
		// 海の色
		Vec3 seaCol = { 0.1f,0.3f,0.1f };
		//時間
		uint32_t time = 0;
	};

private:
	//ペラの拡大率
	float pera1Extend_ = 1.0f;
	float peraGExtend_ = 1.0f;
	float pera2Extend_ = 1.0f;
	//一枚目の頂点
	PeraVertex p1Vertices_[S_VERTEX_COUNT_] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//左下
			{{-1.0f, +1.0f,0},{0,0}},//左上
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
			{{+1.0f, +1.0f,0},{1.0f,0}}//右上
	};
	//ガウシアン用の頂点
	PeraVertex pGVertices_[S_VERTEX_COUNT_] = {
			{{-1.0f, -1.0f,0},{0,1.0f}},//左下
			{{-1.0f, +1.0f,0},{0,0}},//左上
			{{+1.0f, -1.0f,0},{1.0f,1.0f}},//右下
			{{+1.0f, +1.0f,0},{1.0f,0}}//右上
	};
	//二枚目の頂点
	PeraVertex p2Vertices_[S_VERTEX_COUNT_] = {
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
	//RTVや演出用のバッファを生成
	void InitializeBuffRTV(const char* normalImageFileName);
	//パイプラインステート、ルートパラメータの設定
	void GenerateRSPL();
	//ガラスフィルター用のバッファ生成
	void GlassFilterBuffGenerate(const char* fileName);
	//ガラス用のバッファとビュー作成
	bool CreateEffectBufferAndView(const char* fileName);
	//ブルーム用のバッファ作成
	bool CreateBloomBuffer(const D3D12_RESOURCE_DESC& resDesc,
		const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue);
	//ブルーム用のバッファビューを生成
	bool CreateBloomBuffView();
	//被写界深度用バッファ作成
	bool CreateDofBuffer(const D3D12_RESOURCE_DESC& resDesc,
		const D3D12_HEAP_PROPERTIES& heapProp, D3D12_CLEAR_VALUE clearValue);
	//被写界深度用SRV生成
	bool CreateDofSRV();
	//深度値テクスチャ用SRV生成
	bool CreateDepthTexBuffAndSRV();

public:
	//初期化
	void Initialize(const char* normalImageFileName);
	//更新
	void Update();
	//ImGuiの描画
	void ImGuiUpdate(int32_t num);

private:
	//ヒープとヒープ上に作ったバッファビュー等をセット
	void SetHeapAllBuffView(bool isPost2 = false, bool isPost1 = false);

private://一枚目に描画----------------------------------
	//一枚目への描画の前処理
	void PreDraw(bool isFirstPostPera);
	//一枚目への描画の後処理
	void PostDraw();
public:
	//一枚目への描画(引数の描画関数を一枚目に描画)
	void DrawToPostpera(const std::function<void()>& f, bool isFirstPostPera = false, const std::function<void()>& drawSpriteF = nullptr);

public://ぼかす用のペラに描画---------------------------
	//ブルームや被写界深度などぼかす用のペラに書き込み
	void DrawShrinkTextureForBlur();

private://二枚目に描画-----------------------------------
	//二枚目への描画の前処理
	void PreDraw2();
	//二枚目への描画の後処理
	void PostDraw2();
public:
	//二枚目への描画処理まとめ
	void DrawToBlurAll();

public:
	//一枚目を描画
	void Draw();
	//エフェクトかけ終わって二枚目を実際に描画する
	void Draw2();

public:
	//一枚目の大きさセット
	void SetPera1Extend(float extend) { pera1Extend_ = extend; }
	//ブラー用ペラの大きさセット
	void SetPeraGExtend(float extend) { peraGExtend_ = extend; }
	//二枚目の大きさセット
	void SetPera2Extend(float extend) { pera2Extend_ = extend; }
};