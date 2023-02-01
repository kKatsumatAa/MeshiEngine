
Texture2D<float4> tex : register(t0);

SamplerState smp : register(s0);

cbuffer ConstBufferEffectFlags : register(b0)
{
	//フォグ
	uint isFog;
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
}

//vsからpsに送る
struct Output
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};