#include"PeraShaderHeader.hlsli"

//サンプラー
SamplerState smp : register(s0);

//クロスフィルタ用に二枚+被写界深度用
struct PSOutput
{
	float4 blur : SV_TARGET0;//高輝度ぼかし
	float4 blur2 : SV_TARGET1;//高輝度ぼかし２
	float4 col : SV_TARGET2;//通常ぼかし（被写界深度用）
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
		output.blur = Gaussian2(tex2, smp, input.uv);
	}

	if (isCrossFilter)
	{
		//高輝度ぼかし一枚目にだけ
		output.blur = GaussianAngle(tex2, smp, 45, input.uv);
		output.blur += GaussianAngle(tex2, smp, 90, input.uv);
		//高輝度ぼかし2枚目にだけ
		output.blur2 = GaussianAngle(tex2, smp, -45, input.uv);
		output.blur2 += GaussianAngle(tex2, smp, -90, input.uv);
	}
	//被写界深度用
	if(isDepthField)
    {
        output.col = Gaussian2(tex0, smp, input.uv);
    }

	return output;
}
