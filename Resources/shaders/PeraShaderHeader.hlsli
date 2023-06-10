


cbuffer ConstBufferEffectFlags : register(b0)
{
	//ぼかし
	uint isGaussian;
	//ガウシアンぼかし
	uint isGaussian2;
	//エンボス
	uint isEmboss;
	//シャープネス
	uint isSharpness;
	//諧調
	uint isGradation;
	//アウトライン
	uint isOutLine;
	//ビネット
	uint isVignette;
	//樽状湾曲
	uint isBarrelCurve;
	//走査線
	uint isScanningLine;
	//グレースケール
	uint isGrayScale;
	//ガラスフィルター
	uint isGlassFilter;
	//モザイク
	uint isMosaic;
	//ネガポジ反転
	uint isNega;
	//ネガポジ反転
	uint isRGBShift;
	//時間
	uint time;
}

cbuffer PostEffect : register(b1)
{
	float4 bkweights[2];
}

//vsからpsに送る
struct Output
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};

//一枚目の一つ目(通常カラー)
Texture2D<float4> tex0 : register(t0);
//一枚目の二つ目(違うカラー（法線←まだ）)
Texture2D<float4> tex1 : register(t1);
//一枚目の三つ目(高輝度用)
Texture2D<float4> tex2 : register(t2);
//ガラスフィルター
Texture2D<float4> effectTex : register(t3);
