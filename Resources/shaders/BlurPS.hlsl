#include"PeraShaderHeader.hlsli"

//サンプラー
SamplerState smp : register(s0);

//クロスフィルタ用に二枚
struct PSOutput
{
	float4 blur : SV_TARGET0;//高輝度ぼかし
	float4 blur2 : SV_TARGET1;//高輝度ぼかし２
};

//メインテクスチャをぼかす
PSOutput BlurPS(Output input)
{

	PSOutput output;

	float w, h, miplevels;
	tex2.GetDimensions(0, w, h, miplevels);
	float dx = 2.0f / w;
	float dy = 2.0f / h;
	float4 ret;

	if (isBloom)
	{
		output.blur = Get5x5GaussianBlur(tex2, smp, input.uv, dx, dy, float4(0, 0, 1, 1));
	}

	if (isCrossFilter)
	{
		//高輝度ぼかし一枚目にだけ
		output.blur = GaussianAngle(tex2, smp, 30, input.uv);
		//高輝度ぼかし一枚目にだけ
		output.blur2 = GaussianAngle(tex2, smp, -30, input.uv);
	}

	return output;
}
