#include"PeraShaderHeader.hlsli"

//サンプラー
SamplerState smp : register(s0);

//メインテクスチャをぼかす
float4 BlurPS(Output input) : SV_TARGET
{
	float w,h,miplevels;
	tex2.GetDimensions(0, w, h, miplevels);
	float dx = 2.0f / w;
	float dy = 2.0f / h;
	float4 ret;
	ret = Get5x5GaussianBlur(tex2, smp, input.uv, dx, dy, float4(0, 0, 1, 1));
	return ret;
}
