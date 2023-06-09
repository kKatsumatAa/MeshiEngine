


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


//ブルーム用
struct PixelOutput//(sv_targetは8つまで指定できる)
{
	float4 col : SV_TARGET0;    //カラー値を出力
	float4 normal : SV_TARGET1; //法線を出力
};


//一枚目の一つ目
Texture2D<float4> tex0 : register(t0);
//一枚目の二つ目
Texture2D<float4> tex1 : register(t1);
//ガラスフィルター
Texture2D<float4> effectTex : register(t2);
//法線(?)
Texture2D<float4> texNormal : register(t3);
//深度（？）
Texture2D<float4> distTex : register(t4);
