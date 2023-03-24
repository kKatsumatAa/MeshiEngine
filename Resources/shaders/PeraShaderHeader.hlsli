


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
	//走査線
	uint isScanningLine;
	//グレースケール
	uint isGrayScale;
	//ガラスフィルター
	uint isGlassFilter;
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
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};


//ブルーム用
struct PixelOutput//(sv_targetは8つまで指定できる)
{
	float4 col : SV_TARGET0;    //カラー値を出力
	float4 normal : SV_TARGET1; //法線を出力
};